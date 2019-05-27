/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2015-2016, 2019,
 * Technische Universitaet Darmstadt, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 */

#include <config.h>
#include <scorep_system_tree_sequence.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Memory.h>
#include <SCOREP_Hashtab.h>
#include <scorep_ipc.h>
#include <scorep_runtime_management.h>
#include <scorep_type_utils.h>
#include <scorep_environment.h>
#define SCOREP_DEBUG_MODULE_NAME UNIFY
#include <UTILS_Debug.h>

#include <string.h>

/**
 * Contains the data for a system tree sequence node.
 */
struct scorep_system_tree_seq
{
    /* Id of the unified name string of the current system tree node. E.g. a host name. */
    uint32_t name;

    /* Id of this definition */
    uint64_t node_id;

    /* Stores the element type (system tree node, location group, location) */
    scorep_system_tree_seq_type seq_type;

    /* Stores the sub-type of the element */
    uint64_t sub_type;

    /* Stores the system tree domain. Only valid for system tree node. */
    SCOREP_SystemTreeDomain domains;

    /* Number of instances that this level represent */
    uint64_t num_copies;

    /* Number of child nodes */
    uint64_t num_children;

    /* list of child nodes */
    struct scorep_system_tree_seq** children;

    /* parent */
    struct scorep_system_tree_seq* parent;
};

/**
 * Contains data for mappings of one entry.
 */
typedef struct
{
    uint64_t node_id;    /* Index of the sequence node */
    uint64_t index;      /* Index in the number of copies */
} mapping_data;

/**
 * The mapping data for one node in the hierarchical unification tree
 */
typedef struct
{
    mapping_data* data[ 3 ];   /* Matrix with three rows of unequal length of mapping data */
    uint64_t      length[ 3 ]; /* Length of the rows in the data matrix */
} hierarchical_map_data;

/**
 * Data entries in the hash table of struct scorep_system_tree_seq_name.
 * They store the data associated with a system tree node class.
 */
typedef struct
{
    const char* class_name; /* Name of the system tree node class */
    uint64_t    counter;    /* Number of instances of this class so far. */
    uint64_t    string_id;  /* Sequence number of the string handle. */
} system_tree_node_type_counter;

/**
 * Data that is needed to construct names for the nodes from
 * the sequence definitions
 */
struct scorep_system_tree_seq_name
{
    uint64_t        rank_counter; /* Counts the number of ranks */
    SCOREP_Hashtab* node_counter; /* The number of system tree nodes per class */
};

/**
 * Pointer to the root node of the system tree definitions.
 */
static scorep_system_tree_seq* system_tree_root = NULL;

/**
 * Stores the index of this rank in the unified definitions.
 */
static uint64_t unified_rank_index = 0;

/**
 * Stores the mapping of the locations to their index in the
 * new definitions. The i-th value in the returned array is
 * the index of the i-th location in the original definitions.
 * Maps only the locations of the current process.
 */
static uint64_t* local_location_map = NULL;

/**
 * Stores the mapping of the locations to their index in the
 * new definitions. The i-th value in the returned array is
 * the index of the i-th location in the original definitions.
 * Maps the locations of all processes. Only valid on rank 0.
 */
static uint64_t* global_location_map = NULL;

/**
 * Stores the mapping of the ranks to their index in the
 * new definitions. The i-th value in the returned array is
 * the index of the i-th location in the original definitions.
 * Maps the locations of all processes. Only valid on rank 0.
 */
static uint32_t* rank_map = NULL;

/**
 * Contains the number of events for the locations in the
 * depth first order. Only valid on rank 0.
 */
static uint64_t* global_event_numbers = NULL;

/**
 * Stores a Ipc_Group that contains all ranks in the order
 * of a depth-first traversal of the unified definitions.
 */
static SCOREP_Ipc_Group* depth_order_comm = NULL;


#define INVALID_STRING_ID UINT32_MAX

/**
 * The number of elements from scorep_system_tree_seq that are
 * written into the buffer.
 */
#define PACK_BUFFER_FACTOR 5

/**
 * Counter to assign unique node ids
 */
static uint64_t next_node_id = 0;


static int64_t
compare_system_tree_seq( scorep_system_tree_seq* a,
                         scorep_system_tree_seq* b );


/**
 * Initializes the system tree sequence definition structure with the chain
 * from root to locations. We assume that at initialization time, we have only
 * single instances on all system tree levels.
 *
 * @param manager       The used definitions manager object.
 * @param localMapping  (out) The function creates a mapping data structure
 *                      for the locations. This variable returns a pointer
 *                      to the newly created mapping data structure.
 * @param numLocations  (out) The function writes the number of locations
 *                      into the memory location this variable points to.
 * @return This function returns a pointer to the root node of the
 * local system tree information as sequence definition representation.
 */
static scorep_system_tree_seq*
init_system_tree_seq( SCOREP_DefinitionManager* manager,
                      mapping_data**            localMapping,
                      uint64_t*                 numLocations )
{
    uint64_t num_nodes = manager->system_tree_node.counter +
                         manager->location_group.counter +
                         manager->location.counter;

    scorep_system_tree_seq* root = calloc( num_nodes, sizeof( scorep_system_tree_seq ) );
    UTILS_ASSERT( root );

    scorep_system_tree_seq* current        = root;
    scorep_system_tree_seq* location_group = NULL;

    *numLocations = manager->location.counter;
    mapping_data* local_map = malloc( *numLocations * sizeof( mapping_data ) );
    UTILS_ASSERT( local_map );

    /* System tree nodes */
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( manager,
                                                         SystemTreeNode,
                                                         system_tree_node )
    {
        uint32_t local_id = SCOREP_LOCAL_HANDLE_TO_ID( definition->name_handle,
                                                       String );
        current->name     = manager->string.mapping[ local_id ];
        current->node_id  = next_node_id++;
        current->seq_type = SCOREP_SYSTEM_TREE_SEQ_TYPE_SYSTEM_TREE_NODE;

        local_id              = SCOREP_LOCAL_HANDLE_TO_ID( definition->class_handle, String );
        current->sub_type     = manager->string.mapping[ local_id ];
        current->domains      = definition->domains;
        current->num_copies   = 1;
        current->num_children = 0;
        current->children     = NULL;

        if ( definition->parent_handle != SCOREP_INVALID_SYSTEM_TREE_NODE )
        {
            uint64_t index
                = SCOREP_HANDLE_DEREF( definition->parent_handle, SystemTreeNode,
                                       manager->page_manager )->sequence_number;

            current->parent = &root[ index ];
            current->parent->num_children++;
        }
        else
        {
            current->parent = NULL;
        }
        current++;
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();

    /* Location groups */
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( manager,
                                                         LocationGroup,
                                                         location_group )
    {
        current->name         = INVALID_STRING_ID;
        current->node_id      = next_node_id++;
        current->seq_type     = SCOREP_SYSTEM_TREE_SEQ_TYPE_LOCATION_GROUP;
        current->sub_type     = definition->location_group_type;
        current->domains      = 0;
        current->num_copies   = 1;
        current->num_children = 0;
        current->children     = NULL;

        UTILS_ASSERT( definition->parent != SCOREP_INVALID_SYSTEM_TREE_NODE );

        uint64_t index = SCOREP_HANDLE_DEREF( definition->parent,
                                              SystemTreeNode,
                                              manager->page_manager )->sequence_number;

        location_group  = current;
        current->parent = &root[ index ];
        current->parent->num_children++;
        current++;
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();

    /* Locations */
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( manager,
                                                         Location,
                                                         location )
    {
        current->name         = INVALID_STRING_ID;
        current->node_id      = next_node_id++;
        current->seq_type     = SCOREP_SYSTEM_TREE_SEQ_TYPE_LOCATION;
        current->sub_type     = definition->location_type;
        current->domains      = 0;
        current->num_copies   = 1;
        current->num_children = 0;
        current->children     = NULL;
        current->parent       = location_group;
        current->parent->num_children++;

        static uint64_t index = 0;
        local_map[ index ].node_id = current->node_id;
        local_map[ index ].index   = 0;
        index++;
        current++;
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
    *localMapping = local_map;

    /* Create children vector */
    for ( uint64_t i = 0; i < num_nodes; i++ )
    {
        root[ i ].children = calloc( root[ i ].num_children,
                                     sizeof( scorep_system_tree_seq* ) );

        UTILS_ASSERT( ( root[ i ].num_children == 0 ) || root[ i ].children );

        /* Use the num_children field to count the current index for the next child
           at the end it should be the same value as before */
        root[ i ].num_children = 0;
        scorep_system_tree_seq* parent = root[ i ].parent;
        if ( parent != NULL )
        {
            parent->children[ parent->num_children ] = &root[ i ];
            parent->num_children++;
        }
    }

    return root;
}

/**
 * Deallocates the children array in a subtree rooted in @a node.
 *
 * @param node  Root node of a subtree in which all children arrays
 *              are deallocated.
 */
static void
free_system_tree_children( scorep_system_tree_seq* node )
{
    for ( uint64_t i = 0; i < node->num_children; i++ )
    {
        free_system_tree_children( node->children[ i ] );
    }

    free( node->children );
}

/**
 * Deallocates an array of scorep_system_tree_seq objects.
 *
 * @param array  Pointer to the array of scorep_system_tree_seq objects that is deallocated.
 * @param num    Number of elements in @a array.
 */
static void
free_system_tree_seq_array( scorep_system_tree_seq* array )
{
    if ( array == NULL )
    {
        return;
    }

    free_system_tree_children( array );
    free( array );
}

/**
 * Compares the structure of the system trees. They are equal if
 * their types and children match. The number of copies may not
 * match, but the number of copies of the children must match.
 *
 * @param a  First system tree root.
 * @param b  Second system tree root.
 *
 * @returns 0 if @a a and @a b match,
 *          a negative value if @a a < @a b and
 *          a positive value if @a a > @a b.
 */
static inline int64_t
compare_system_tree_structure( scorep_system_tree_seq* a,
                               scorep_system_tree_seq* b )
{
    UTILS_ASSERT( a );
    UTILS_ASSERT( b );
    int64_t diff = a->seq_type - b->seq_type;
    if ( diff != 0 )
    {
        return diff;
    }

    diff = a->sub_type - b->sub_type;
    if ( diff != 0 )
    {
        return diff;
    }

    diff = a->num_children - b->num_children;
    if ( diff != 0 )
    {
        return diff;
    }

    for ( uint64_t i = 0; i < a->num_children; i++ )
    {
        diff = compare_system_tree_seq( a->children[ i ], b->children[ i ] );
        if ( diff != 0 )
        {
            return diff;
        }
    }
    return 0;
}

/**
 * Compares two scorep_system_tree_seq objects and its children recursively.
 * The comparison algorithm assumes that the children of a node
 * are sorted in ascending order. This avoids to consider permutations
 * of children.
 *
 * @param a  Pointer to the scorep_system_tree_seq object that is compared to @a b.
 * @param b  Pointer to the scorep_system_tree_seq object that is compared to @a a.
 *
 * @returns 0 if @a a and @a b match,
 *          a negative value if @a a < @a b and
 *          a positive value if @a a > @a b.
 */
static int64_t
compare_system_tree_seq( scorep_system_tree_seq* a, scorep_system_tree_seq* b )
{
    int64_t diff = a->num_copies - b->num_copies;
    if ( diff != 0 )
    {
        return diff;
    }

    return compare_system_tree_structure( a, b );
}

/**
 * Helper function to print a scorep_system_tree_seq tree recursively.
 * Called by dump_system_tree_seq.
 *
 * @param root  Pointer to the current tree.
 * @param level Number of levels for the indent.
 */
static void
dump_system_tree_seq_rec( scorep_system_tree_seq* root,
                          uint32_t                level )
{
    /* indent */
    for ( uint32_t i = 0; i < level; i++ )
    {
        UTILS_DEBUG( "  " );
    }

    UTILS_DEBUG( "%" PRIu64 "x ", root->num_copies );

    switch ( root->seq_type )
    {
        case SCOREP_SYSTEM_TREE_SEQ_TYPE_SYSTEM_TREE_NODE:
            UTILS_DEBUG( "system tree node %p, subtype %" PRIu64 ", parent %p\n",
                         root, root->sub_type, root->parent );
            break;
        case SCOREP_SYSTEM_TREE_SEQ_TYPE_LOCATION_GROUP:
            UTILS_DEBUG( "location group %p, subtype %" PRIu64 ", parent %p\n",
                         root, root->sub_type, root->parent );
            break;
        case SCOREP_SYSTEM_TREE_SEQ_TYPE_LOCATION:
            UTILS_DEBUG( "location %p, subtype %" PRIu64 ", parent %p\n",
                         root, root->sub_type, root->parent );
            break;
    }

    for ( uint64_t i = 0; i < root->num_children; i++ )
    {
        dump_system_tree_seq_rec( root->children[ i ], level + 1 );
    }
}

/**
 * Prints the structure of a scorep_system_tree_seq tree to the screen.
 *
 * @param root  Pointer to the root node of the scorep_system_tree_seq tree.
 */
static void
dump_system_tree_seq( scorep_system_tree_seq* root )
{
    dump_system_tree_seq_rec( root, 0 );
}

/**
 * Counts the number of scorep_system_tree_seq records that are needed for the
 * system tree rooted in @a root.
 *
 * @param root Pointer to the root object of the system tree.
 */
static uint64_t
count_records( scorep_system_tree_seq* root )
{
    uint64_t count = 1;
    for ( uint64_t i = 0; i < root->num_children; i++ )
    {
        count += count_records( root->children[ i ] );
    }
    return count;
}

static uint64_t
get_buffer_num( uint64_t count )
{
    return PACK_BUFFER_FACTOR * ( count + 1 );
}

/**
 * Helper function to recursively pack scorep_system_tree_seq data into a buffer.
 * This function is called by pack_system_tree_seq.
 *
 * @param buffer  Pointer to the destination buffer, the buffer must be large
 *                enough to write all data into it.
 * @param root    Pointer to the root of the system tree that is written into
 *                the buffer.
 * @param pos     Pointer to a memory location where the next free element
 *                in buffer is stored. After this function pos will contain
 *                the index of the next free element in @a buffer.
 */
static void
pack_system_tree_seq_rec( uint64_t*               buffer,
                          scorep_system_tree_seq* root,
                          uint64_t*               pos )
{
    buffer[ ( *pos )++ ] = root->seq_type;
    buffer[ ( *pos )++ ] = root->sub_type;
    buffer[ ( *pos )++ ] = root->domains;
    buffer[ ( *pos )++ ] = root->num_copies;
    buffer[ ( *pos )++ ] = root->num_children;

    for ( uint64_t i = 0; i < root->num_children; i++ )
    {
        pack_system_tree_seq_rec( buffer, root->children[ i ], pos );
    }
}

/**
 * Packs a system tree in a buffer of unsigned 64 bit integers.
 * The system tree elements are packed in depth-first order.
 *
 * @param root Pointer to the root of the system tree.
 *
 * @returns the buffer with the packed data.
 */
static uint64_t*
pack_system_tree_seq( scorep_system_tree_seq* root )
{
    uint64_t  count  = count_records( root );
    uint64_t  pos    = 1;
    uint64_t* buffer = malloc( sizeof( uint64_t ) * get_buffer_num( count ) );
    UTILS_ASSERT( buffer );
    buffer[ 0 ] = count;
    pack_system_tree_seq_rec( buffer, root, &pos );
    return buffer;
}


/**
 * Helper function to recursively unpack a scorep_system_tree_seq data from a buffer.
 * This function is called by unpack_system_tree_seq.
 *
 * @param array      An array of scorep_system_tree_seq to store the unpacked system
 *                   tree.
 * @param buffer     Pointer to the buffer from which to read the data.
 * @param posArray   Index of the next to write element in @a array.
 * @param posBuffer  Index of the next to read element in @a buffer.
 */
static void
unpack_system_tree_seq_rec( scorep_system_tree_seq* array,
                            uint64_t*               buffer,
                            uint64_t*               posArray,
                            uint64_t*               posBuffer )
{
    scorep_system_tree_seq* current = &array[ *posArray ];
    ( *posArray )++;

    current->node_id      = next_node_id++;
    current->seq_type     = buffer[ ( *posBuffer )++ ];
    current->sub_type     = buffer[ ( *posBuffer )++ ];
    current->domains      = buffer[ ( *posBuffer )++ ];
    current->num_copies   = buffer[ ( *posBuffer )++ ];
    current->num_children = buffer[ ( *posBuffer )++ ];

    current->children = calloc( current->num_children, sizeof( scorep_system_tree_seq* ) );
    UTILS_ASSERT( ( current->num_children == 0 ) || current->children );

    for ( uint64_t i = 0; i < current->num_children; i++ )
    {
        current->children[ i ] = &array[ *posArray ];
        unpack_system_tree_seq_rec( array, buffer, posArray, posBuffer );
    }
}

/**
 * Unpacks a system tree from a buffer and recreates the structure.
 *
 * @param buffer Pointer to the buffer.
 *
 * @returns the unpacked data structure.
 */
static scorep_system_tree_seq*
unpack_system_tree_seq( uint64_t* buffer )
{
    uint64_t                pos_buffer = 1;
    uint64_t                pos_array  = 0;
    uint64_t                count      = buffer[ 0 ];
    scorep_system_tree_seq* array      = calloc( count, sizeof( scorep_system_tree_seq ) );
    UTILS_ASSERT( array );
    unpack_system_tree_seq_rec( array, buffer, &pos_array, &pos_buffer );
    return array;
}

/**
 * Helper function to copy a system tree recursively. Called by copy_system_tree_seq.
 *
 * @param destination Pointer to a array of scorep_system_tree_seq objects to which we
 *                    copy the @a source.
 * @param source      Pointer to the root of a system tree which is copied.
 * @param pos         Pointer to a memory location which stores the next free
 *                    element in @a destination.
 */
static void
copy_system_tree_seq_rec( scorep_system_tree_seq* destination,
                          scorep_system_tree_seq* source,
                          uint64_t*               pos )
{
    scorep_system_tree_seq* current = &destination[ *pos ];

    current->name         = source->name;
    current->node_id      = source->node_id;
    current->seq_type     = source->seq_type;
    current->sub_type     = source->sub_type;
    current->domains      = source->domains;
    current->num_copies   = source->num_copies;
    current->num_children = source->num_children;

    current->children = calloc( current->num_children, sizeof( scorep_system_tree_seq* ) );
    UTILS_ASSERT( ( current->num_children == 0 ) || current->children );
    ( *pos )++;

    for ( uint64_t i = 0; i < current->num_children; i++ )
    {
        current->children[ i ] = &destination[ *pos ];
        copy_system_tree_seq_rec( destination, source->children[ i ], pos );
    }
}

/**
 * Creates a copy of a system tree.
 *
 * @param root Pointer to the root of the system tree.
 *
 * @returns Pointer to the root of the created system tree.
 */
static scorep_system_tree_seq*
copy_system_tree_seq( scorep_system_tree_seq* root )
{
    uint64_t                count = count_records( root );
    uint64_t                pos   = 0;
    scorep_system_tree_seq* copy  = calloc( count, sizeof( scorep_system_tree_seq ) );
    UTILS_ASSERT( copy );
    copy_system_tree_seq_rec( copy, root, &pos );
    return copy;
}

/**
 * Sorts the entries in array in ascendent order using a mergesort algorithm.
 *
 * @param array  An array of scorep_system_tree_seq objects to be sorted. When
 *               the function returns this array contains the sorted objects.
 * @param buffer A buffer that is used during the algorithm. It must provide
 *               enough space to hold a copy of @a array.
 * @param size   Number of elements in @a array.
 */
static void
internal_mergesort( scorep_system_tree_seq** array,
                    scorep_system_tree_seq** buffer,
                    uint64_t                 size )
{
    if ( size < 2 )
    {
        return;
    }

    /* Divide and process recursively */
    uint64_t middle = size / 2;
    internal_mergesort( array, buffer, middle );
    internal_mergesort( &array[ middle ], &buffer[ middle ], size - middle );

    /* Merge */
    uint64_t index_a = 0;
    uint64_t index_b = middle;
    for ( uint64_t i = 0; i < size; i++ )
    {
        if ( index_a >= middle )
        {
            buffer[ i ] = array[ index_b ];
            index_b++;
        }
        else if ( index_b >= size )
        {
            buffer[ i ] = array[ index_a ];
            index_a++;
        }
        else if ( compare_system_tree_seq( array[ index_a ], array[ index_b ] ) > 0 )
        {
            buffer[ i ] = array[ index_b ];
            index_b++;
        }
        else
        {
            buffer[ i ] = array[ index_a ];
            index_a++;
        }
    }

    /* Copy to array */
    for ( uint64_t i = 0; i < size; i++ )
    {
        array[ i ] = buffer[ i ];
    }
}


/**
 * Sorts the children in a system tree. Avoids to consider permutations
 * of children during comparison.
 */
static void
sort_system_tree_seq( scorep_system_tree_seq* root )
{
    /* Make sure sub trees are sorted */
    for ( uint64_t i = 0; i < root->num_children; i++ )
    {
        sort_system_tree_seq( root->children[ i ] );
    }

    /* Sort children */
    if ( root->num_children > 1 )
    {
        scorep_system_tree_seq** buffer = calloc( root->num_children,
                                                  sizeof( scorep_system_tree_seq* ) );
        UTILS_ASSERT( buffer );
        internal_mergesort( root->children, buffer, root->num_children );
        free( buffer );
    }
}

static void
update_local_mapping( mapping_data*           localMappings,
                      uint64_t                numLocations,
                      scorep_system_tree_seq* destination,
                      scorep_system_tree_seq* source )
{
    for ( uint64_t i = 0; i < numLocations; i++ )
    {
        if ( localMappings[ i ].node_id == source->node_id )
        {
            localMappings[ i ].node_id = destination->node_id;
            localMappings[ i ].index  += destination->num_copies - 1;
        }
    }
}

/**
 * Aggregates system tree nodes of the same structure together.
 *
 * @param root  Root of the system tree.
 */
static void
simplify_system_tree_seq( scorep_system_tree_seq* root,
                          mapping_data*           localMapping,
                          uint64_t                numLocations )
{
    UTILS_ASSERT( root );

    /* Make sure children are simplified */
    for ( uint64_t i = 0; i < root->num_children; i++ )
    {
        simplify_system_tree_seq( root->children[ i ], localMapping, numLocations );
    }

    /* Merge children if equal */
    if ( root->num_children > 1 )
    {
        scorep_system_tree_seq** old_children = root->children;
        scorep_system_tree_seq** new_children = calloc( root->num_children,
                                                        sizeof( scorep_system_tree_seq* ) );

        new_children[ 0 ] = root->children[ 0 ];
        uint64_t index = 0;

        for ( uint64_t i = 1; i < root->num_children; i++ )
        {
            if ( compare_system_tree_structure( old_children[ i ],
                                                new_children[ index ] ) == 0 )
            {
                new_children[ index ]->num_copies += old_children[ i ]->num_copies;
                if ( old_children[ i ]->seq_type == SCOREP_SYSTEM_TREE_SEQ_TYPE_LOCATION )
                {
                    update_local_mapping( localMapping, numLocations,
                                          new_children[ index ], old_children[ i ] );
                }
                free_system_tree_children( old_children[ i ] );
            }
            else
            {
                index++;
                new_children[ index ] = old_children[ i ];
            }
        }
        root->num_children = index + 1;
        root->children     = new_children;
        free( old_children );
    }
}

/**
 * Returns the depth of the system tree.
 *
 * @param root  Root of the system tree.
 */
static uint64_t
get_depth( scorep_system_tree_seq* root )
{
    if ( root == NULL )
    {
        return 0;
    }

    if ( root->num_children == 0 )
    {
        return 1;
    }


    uint64_t max = 0;
    for ( uint64_t i = 0; i < root->num_children; i++ )
    {
        uint64_t current = get_depth( root->children[ i ] );
        max = ( current > max ? current : max );
    }
    return max + 1;
}

/**
 * Calculates the global maximum number of levels in the system tree.
 *
 * @param root  Root of the local system tree.
 */
static uint64_t
determine_num_levels( scorep_system_tree_seq* root )
{
    uint64_t local = get_depth( root );
    uint64_t global;
    SCOREP_Ipc_Allreduce( &local, &global, 1, SCOREP_IPC_UINT64_T, SCOREP_IPC_MAX );
    return global;
}

static scorep_system_tree_seq*
get_node_at_level( scorep_system_tree_seq* root,
                   uint64_t                level )
{
    if ( level == 0 )
    {
        if ( root->seq_type == SCOREP_SYSTEM_TREE_SEQ_TYPE_SYSTEM_TREE_NODE )
        {
            return root;
        }
        else
        {
            return NULL;
        }
    }

    if ( root->num_children == 0 )
    {
        return NULL;
    }

    for ( uint64_t i = 0; i < root->num_children; i++ )
    {
        scorep_system_tree_seq* node = get_node_at_level( root->children[ i ], level - 1 );
        if ( node != NULL )
        {
            return node;
        }
    }
    return NULL;
}

/**
 * Creates sub communicators for each system tree node.
 */
static SCOREP_Ipc_Group*
get_level_comm( SCOREP_Ipc_Group*       parentComm,
                scorep_system_tree_seq* node )
{
    /* Uses the unified string handle for the host name as color */
    SCOREP_Ipc_Group* level_comm = parentComm;
    while ( node != NULL )
    {
        int               color    = node->name;
        SCOREP_Ipc_Group* new_comm = SCOREP_IpcGroup_Split( level_comm, color, SCOREP_Ipc_GetRank() );
        if ( level_comm != parentComm )
        {
            SCOREP_IpcGroup_Free( level_comm );
        }
        level_comm = new_comm;
        node       = node->parent;
    }
    return level_comm;
}

/**
 * Constructs a communicator that contains all processes that
 * aggregated the definitions of their system tree node in
 * the current level. Because we aggregate the data to the root node
 * it contains the root nodes.
 * @param currentComm  The communicator that contains all processes
 *                     at the current level.
 * @param rank         The rank id of this process in the node-specific
 *                     subcommunicator of current_comm.
 */
static SCOREP_Ipc_Group*
get_next_parent_comm( SCOREP_Ipc_Group* currentComm,
                      uint32_t          rank )
{
    int color = ( rank == 0 ? 1 : 2 );
    return SCOREP_IpcGroup_Split( currentComm, color,
                                  SCOREP_IpcGroup_GetRank( currentComm ) );
}

/**
 * Merges all children from @a source into @a destination and creates the @a mappings.
 * @param mapping     Returns the mapping data for source.
 * @param destination The sequence definition node into that the children of
 *                    @a source are merged.
 * @param source      The sequence definitions from which the children are merged into
 *                    @a destination.
 */
static void
merge_children( mapping_data*           mapping,
                scorep_system_tree_seq* destination,
                scorep_system_tree_seq* source )
{
    uint64_t num_add = 0;

    /* Try to find for all children a match */

    uint64_t j = 0; /* We want to know the value of j after the loop */
    for ( uint64_t i = 0; i < destination->num_children && j < source->num_children; )
    {
        int64_t diff = compare_system_tree_structure( destination->children[ i ],
                                                      source->children[ j ] );
        if ( diff == 0 ) /* Both match */
        {
            mapping[ j ].node_id                    = destination->children[ i ]->node_id;
            mapping[ j ].index                      = destination->children[ i ]->num_copies;
            destination->children[ i ]->num_copies += source->children[ j ]->num_copies;
            free_system_tree_children( source->children[ j ] );
            source->children[ j ] = NULL;
            j++;
        }
        else if ( diff < 0 ) /* destination is smaller */
        {
            i++;
        }
        else /* source is smaller => does not yet exist */
        {
            num_add++;
            j++;
        }
    }

    num_add += source->num_children - j;

    /* Not all children got a match in destination, thus, create a new
       sorted child vector */
    if ( num_add > 0 )
    {
        num_add += destination->num_children;
        scorep_system_tree_seq** new_children = malloc( num_add * sizeof( scorep_system_tree_seq* ) );
        UTILS_ASSERT( new_children );

        j = 0;
        for ( uint64_t i = 0, k = 0; k < num_add; k++ )
        {
            /* The current source child had a match and was already merged.
               Continue with the next child from source. */
            while ( source->children[ j ] == NULL && j < source->num_children )
            {
                j++;
            }

            /* If all children are merged already simply copy the remaining
               children from the old destination to the new destination. */
            if ( j >= source->num_children )
            {
                new_children[ k ] = destination->children[ i ];
                i++;
            }
            /* All children from the old destination are already in the new copy.
               Copy the remaining children from the source. */
            else if ( i >= destination->num_children )
            {
                new_children[ k ]    = source->children[ j ];
                mapping[ j ].node_id = source->children[ j ]->node_id;
                mapping[ j ].index   = 0;
                j++;
            }
            /* The current child from the old destination is smaller,
               thus, copy it. */
            else if ( compare_system_tree_structure( destination->children[ i ],
                                                     source->children[ j ] ) < 0 )
            {
                new_children[ k ] = destination->children[ i ];
                i++;
            }
            /* The remaining case is that the source is smaller,
               thus, copy it. */
            else
            {
                new_children[ k ]    = source->children[ j ];
                mapping[ j ].node_id = source->children[ j ]->node_id;
                mapping[ j ].index   = 0;
                j++;
            }
            UTILS_ASSERT( new_children[ k ] );
        }

        free( destination->children );
        destination->children     = new_children;
        destination->num_children = num_add;
    }

    free( source->children );
    source->num_children = 0;
    source->children     = NULL;
}

/**
 * Counts the number of system tree elements in a subtree.
 */
static uint64_t
count_nodes( scorep_system_tree_seq* root )
{
    uint64_t count = 1;
    for ( uint64_t i = 0; i < root->num_children; i++ )
    {
        count += count_nodes( root->children[ i ] );
    }
    return count * root->num_copies;
}

/**
 * Returns the system tree definition that belongs to a node
 * with a given index in depth first enumeration.
 * @param root  The root node of the sequence definitions.
 * @param id    The index of the node.
 */
static scorep_system_tree_seq*
get_node_depth_first( scorep_system_tree_seq* root,
                      uint64_t                id )
{
    uint64_t count = count_nodes( root );

    // No node with the id exists
    if ( count <= id )
    {
        return NULL;
    }

    // Number of nodes in one copy of this node.
    count /= root->num_copies;
    // Subtract complete subtrees from copies with lower id.
    id %= count;

    // We found the node
    if ( id == 0 )
    {
        return root;
    }

    // Check children
    for ( uint64_t i = 0; i < root->num_children; i++ )
    {
        uint64_t child_size = count_nodes( root->children[ i ] );
        if ( child_size > id )
        {
            return get_node_depth_first( root->children[ i ], id - 1 );
        }
        id -= child_size;
    }

    // Node node found
    return NULL;
}

/**
 * Convert node_id in mapping to index in the child list.
 * During child merge, we store the node_id to identify nodes.
 * However, when we receive the unified definitions nodes may
 * have a different node id.
 */
static void
update_mappings( scorep_system_tree_seq* node,
                 mapping_data*           mapping,
                 uint64_t                size )
{
    for ( int64_t i = 0; i < size; i++ )
    {
        for ( int64_t j = 0; j < node->num_children; j++ )
        {
            if ( node->children[ j ]->node_id == mapping[ i ].node_id )
            {
                mapping[ i ].node_id = j;
                break;
            }
        }
    }
}

static scorep_system_tree_seq*
distribute_global_system_tree_seq( scorep_system_tree_seq* input )
{
    scorep_system_tree_seq* global_root;
    uint64_t                count = count_records( input );
    uint64_t*               buffer;
    SCOREP_Ipc_Bcast( &count, 1, SCOREP_IPC_UINT64_T, 0 );

    if ( SCOREP_Ipc_GetRank() == 0 )
    {
        buffer      = pack_system_tree_seq( input );
        global_root = input;
    }
    else
    {
        buffer = malloc( get_buffer_num( count ) * sizeof( uint64_t ) );
        UTILS_ASSERT( buffer );
    }

    SCOREP_Ipc_Bcast( buffer, get_buffer_num( count ), SCOREP_IPC_UINT64_T, 0 );

    if ( SCOREP_Ipc_GetRank() > 0 )
    {
        global_root = unpack_system_tree_seq( buffer );
    }
    free( buffer );
    return global_root;
}

/**
 * Resets all parent pointers in the sequence definition structure.
 * It assumes that the children pointers are set correctly.
 * The merging process destroys the parent pointers which makes this
 * reset necessary for further processing.
 * @param root  The root node of the system tree sequence definitions.
 */
static void
restore_parent( scorep_system_tree_seq* root )
{
    UTILS_ASSERT( root );
    for ( uint64_t i = 0; i < root->num_children; i++ )
    {
        UTILS_ASSERT( root->children );
        root->children[ i ]->parent = root;
        restore_parent( root->children[ i ] );
    }
}

/**
 * Receives the data from a child in the hierarchical reduce
 * @param comm   The communicator object that contains all processes that
 *               participate in this level's unification. It consists of all
 *               root processes from the level above.
 * @param sender The rank number of the sending process.
 */
static scorep_system_tree_seq*
receive_definitions( SCOREP_Ipc_Group* comm,
                     uint32_t          sender )
{
    uint64_t num;
    SCOREP_IpcGroup_Recv( comm, &num, 1, SCOREP_IPC_UINT64_T, sender );
    uint64_t* buffer = malloc( num * sizeof( uint64_t ) );
    UTILS_ASSERT( buffer );
    SCOREP_IpcGroup_Recv( comm, buffer, num, SCOREP_IPC_UINT64_T, sender );
    scorep_system_tree_seq* received_data =  unpack_system_tree_seq( buffer );
    free( buffer );
    return received_data;
}

/**
 * Sends the data from a child in the hierarchical reduce
 * @param comm      The communicator object that contains all processes that
 *                  participate in this level's unification. It consists of all
 *                  root processes from the level above.
 * @param receiver  The rank number of the receiving process.
 * @param definitionData  Pointer to the sequence definition root node
 *                  of the sequence definition that are sent.
 */
static void
send_definitions( SCOREP_Ipc_Group*       comm,
                  uint32_t                receiver,
                  scorep_system_tree_seq* definitionData )
{
    uint64_t  buffer_size = get_buffer_num( count_records( definitionData ) );
    uint64_t* buffer      = pack_system_tree_seq( definitionData );
    SCOREP_IpcGroup_Send( comm, &buffer_size, 1, SCOREP_IPC_UINT64_T, receiver );
    SCOREP_IpcGroup_Send( comm, buffer, buffer_size,
                          SCOREP_IPC_UINT64_T, receiver );
    free( buffer );
}

static void
send_offsets( SCOREP_Ipc_Group* comm,
              uint32_t          receiver,
              uint64_t*         offsets,
              uint64_t          length )
{
    SCOREP_IpcGroup_Send( comm, &length, 1, SCOREP_IPC_UINT64_T, receiver );
    SCOREP_IpcGroup_Send( comm, offsets, length,
                          SCOREP_IPC_UINT64_T, receiver );
}

static uint64_t*
receive_offsets( SCOREP_Ipc_Group* comm,
                 uint32_t          sender )
{
    uint64_t num;
    SCOREP_IpcGroup_Recv( comm, &num, 1, SCOREP_IPC_UINT64_T, sender );
    uint64_t* buffer = malloc( num * sizeof( uint64_t ) );
    UTILS_ASSERT( buffer );
    SCOREP_IpcGroup_Recv( comm, buffer, num,
                          SCOREP_IPC_UINT64_T, sender );
    return buffer;
}

/**
 * Performs the forward pass of data from the leaves to the root node for
 * one level in the system tree during unification.
 * We unify from the leaves to the root. Hereby, we assume that the system tree nodes
 * do not branch in the levels below the current unification level, but form
 * a sequential chain.
 * @param comm         The communicator object that contains all processes that
 *                     participate in this level's unification. It consists of all
 *                     root processes from the level above.
 * @param root         Returns a pointer to the root of the unified subtree.
 * @param level        Index which level is unified. The root node has level 0.
 */
static hierarchical_map_data*
hierarchical_reduce( SCOREP_Ipc_Group*        comm,
                     scorep_system_tree_seq** root,
                     uint64_t                 level )
{
    uint32_t size = SCOREP_IpcGroup_GetSize( comm );
    uint32_t rank = SCOREP_IpcGroup_GetRank( comm );

    scorep_system_tree_seq* current          = get_node_at_level( *root, level );
    hierarchical_map_data*  mappings         = calloc( 1, sizeof( hierarchical_map_data ) );
    scorep_system_tree_seq* remote_root[ 2 ] = { NULL, NULL };
    UTILS_ASSERT( mappings );

    /* Initialize own mapping */
    mappings->length[ 0 ] = current->num_children;
    mappings->data[ 0 ]   = calloc( current->num_children, sizeof( mapping_data ) );
    UTILS_ASSERT( mappings->data[ 0 ] );
    for ( int i = 0; i < current->num_children; i++ )
    {
        mappings->data[ 0 ][ i ].node_id = current->children[ i ]->node_id;
        mappings->data[ 0 ][ i ].index   = 0;
    }

    /* Receive data from children */
    for ( uint32_t i = 0; i <= 1; i++ )
    {
        uint32_t sender = 2 * rank + 1 + i;
        if ( sender >= size )
        {
            break;
        }
        remote_root[ i ] = receive_definitions( comm, sender );
        scorep_system_tree_seq* remote_current = get_node_at_level( remote_root[ i ], level );

        mappings->length[ i + 1 ] = remote_current->num_children;
        mappings->data[ i + 1 ]   = calloc( remote_current->num_children, sizeof( mapping_data ) );
        UTILS_ASSERT( mappings->data[ i + 1 ] );

        merge_children( mappings->data[ i + 1 ], current, remote_current );
    }
    // Convert node numbers into indexes in the children array
    for ( uint32_t i = 0; i < 3; i++ )
    {
        if ( mappings->data[ i ] != NULL )
        {
            update_mappings( current, mappings->data[ i ], mappings->length[ i ] );
        }
    }

    /* Send data to parent */
    if ( rank != 0 )
    {
        uint32_t receiver = ( rank + 1 ) / 2 - 1;
        send_definitions( comm, receiver, *root );
    }

    /* Clean up */
    scorep_system_tree_seq* new_root = copy_system_tree_seq( *root );
    free_system_tree_seq_array( *root );
    for ( uint32_t i = 0; i <= 1; i++ )
    {
        free_system_tree_seq_array( remote_root[ i ] );
    }
    *root = new_root;
    return mappings;
}

/**
 * Performs the backward pass of indexes and mapping data from the root to the
 * leaves for one level of the system tree during the unification.
 * @param comm      The communicator object that contains all processes that
 *                  participate in this level's unification. It consists of all
 *                  root processes from the level above.
 * @param root      Returns a pointer to the root of the unified subtree.
 * @param level     Index which level is unified. The root node has level 0.
 * @param mappings  The mapping structures that are created in the
 *                  hierarchical_reduce function.
 * @param parent    Index of the parent node. Need only be a valid value on the
 *                  root rank.
 */
static uint64_t
hierarchical_scatter( SCOREP_Ipc_Group*       comm,
                      scorep_system_tree_seq* root,
                      uint64_t                level,
                      hierarchical_map_data*  mappings,
                      uint64_t                parent )
{
    uint32_t                size    = SCOREP_IpcGroup_GetSize( comm );
    uint32_t                rank    = SCOREP_IpcGroup_GetRank( comm );
    uint64_t*               offsets = NULL;
    scorep_system_tree_seq* parent_node;

    if ( rank == 0 )
    {
        // Initialize offsets
        parent_node = get_node_depth_first( root, parent );
        offsets     = malloc( parent_node->num_children * sizeof( uint64_t ) );
        UTILS_ASSERT( offsets );
        offsets[ 0 ] = parent + 1;

        for ( uint64_t i = 1; i < parent_node->num_children; i++ )
        {
            offsets[ i ] = offsets[ i - 1 ] + count_nodes( parent_node->children[ i - 1 ] );
        }
    }
    else
    {
        uint32_t sender = ( rank + 1 ) / 2 - 1;
        SCOREP_IpcGroup_Recv( comm, &parent, 1, SCOREP_IPC_UINT64_T, sender );
        parent_node = get_node_depth_first( root, parent );
        offsets     = receive_offsets( comm, sender );
    }

    for ( uint32_t i = 0; i <= 1; i++ )
    {
        uint32_t receiver = 2 * rank + 1 + i;
        if ( receiver >= size )
        {
            break;
        }

        uint64_t* child_offsets = malloc( mappings->length[ i + 1 ] * sizeof( uint64_t ) );
        SCOREP_IpcGroup_Send( comm, &parent, 1, SCOREP_IPC_UINT64_T, receiver );
        for ( uint64_t j = 0; j < mappings->length[ i + 1 ]; j++ )
        {
            // We know from the mappings on which position in the offsets
            // the corresponding node was. Thus, we can find the definitions
            // node from the offsets.
            uint64_t                node_id = ( mappings->data[ i + 1 ] )[ j ].node_id;
            scorep_system_tree_seq* current = get_node_depth_first( root, offsets[ node_id ] );
            UTILS_ASSERT( current );
            uint64_t node_count = count_nodes( current ) / current->num_copies;
            child_offsets[ j ] = offsets[ node_id ] + ( mappings->data[ i + 1 ] )[ j ].index * node_count;
        }
        send_offsets( comm, receiver, child_offsets, mappings->length[ i + 1 ] );
        free( child_offsets );
    }

    /* Clean up */
    uint64_t global_index = offsets[ mappings->data[ 0 ][ 0 ].node_id ];
    free( offsets );
    return global_index;
}

/**
 * Unifies a level in the system tree.
 * We unify from the leaves to the root. Hereby, we assume that the system tree nodes
 * do not branch in the levels below the current unification level, but form
 * a sequential chain.
 * @param parentComm   The communicator object that contains all processes that
 *                     participate in this level's unification. It consists of all
 *                     root processes from the level above.
 * @param root         Returns a pointer to the root of the unified subtree.
 * @param level        Index which level is unified. The root node has level 0.
 * @param globalIndex  Returns the index of this process in the unified system tree
 *                     in depth-first enumeration. Requires the index of the parent
 *                     as input value.
 */
static scorep_system_tree_seq*
unify_level_hierarchical( SCOREP_Ipc_Group*        parentComm,
                          scorep_system_tree_seq** root,
                          uint64_t                 level,
                          uint64_t*                globalIndex )
{
    scorep_system_tree_seq* global_root;
    scorep_system_tree_seq* current    = get_node_at_level( *root, level );
    SCOREP_Ipc_Group*       level_comm = get_level_comm( parentComm, current );
    uint32_t                rank       = SCOREP_IpcGroup_GetRank( level_comm );

    /* Downward pass to merge sequence definitions of current level */
    hierarchical_map_data* mappings = hierarchical_reduce( level_comm, root, level );
    restore_parent( *root );
    /* Reduction made current invalid, the data structure may have been reallocated */

    /* process higher levels. */
    if ( level > 0 )
    {
        SCOREP_Ipc_Group* next_parent = get_next_parent_comm( parentComm, rank );
        if ( rank == 0 )
        {
            global_root = unify_level_hierarchical( next_parent, root, level - 1, globalIndex );
            /* If rank is not zero, it is not part of a new communicator */
            SCOREP_IpcGroup_Free( next_parent );
        }
        else
        {
            global_root = distribute_global_system_tree_seq( *root );
        }
    }
    else
    {
        global_root = distribute_global_system_tree_seq( *root );
    }

    /* Distribute back the index information */
    *globalIndex = hierarchical_scatter( level_comm,
                                         global_root,
                                         level, mappings,
                                         *globalIndex );

    /* clean up */
    SCOREP_IpcGroup_Free( level_comm );

    return global_root;
}

static void
enumerate_locations( mapping_data*           localMappings,
                     uint64_t                numLocations,
                     scorep_system_tree_seq* root )
{
    /* Find process */
    while ( ( root->num_children != 0 ) && ( root->seq_type != SCOREP_SYSTEM_TREE_SEQ_TYPE_LOCATION_GROUP ) )
    {
        root = root->children[ 0 ];
    }

    /* Enumerate locations */
    uint64_t sum = 0;
    for ( uint64_t i = 0; i < root->num_children; i++ )
    {
        for ( uint64_t j = 0; j < numLocations; j++ )
        {
            if ( localMappings[ j ].node_id == root->children[ i ]->node_id )
            {
                localMappings[ j ].index += sum;
            }
        }
        sum += root->children[ i ]->num_copies;
    }
}

static uint64_t*
create_local_location_mappings( mapping_data* localMappings,
                                int           numLocations )
{
    /* Create array of local index data */
    uint64_t* local_map = malloc( numLocations * sizeof( uint64_t ) );
    UTILS_ASSERT( local_map );
    for ( uint64_t i = 0; i < numLocations; i++ )
    {
        local_map[ i ] = localMappings[ i ].index;
    }
    return local_map;
}


static uint64_t*
gather_per_location_data( uint64_t* localData,
                          int       numLocations )
{
    /* Calculate global number of locations and receive counts per rank */
    uint32_t all_same         = 1;
    uint64_t global_locations = 0;    // Only valid on rank 0
    int*     recv_counts      = NULL; // Only valid on rank 0
    if ( SCOREP_Ipc_GetRank() == 0 )
    {
        recv_counts = malloc( sizeof( int ) * SCOREP_Ipc_GetSize() );
        UTILS_ASSERT( recv_counts );
    }

    SCOREP_Ipc_Gather( &numLocations, recv_counts, 1,
                       SCOREP_IPC_INT, 0 );

    if ( SCOREP_Ipc_GetRank() == 0 )
    {
        for ( uint64_t i = 0; i < SCOREP_Ipc_GetSize(); i++ )
        {
            if ( recv_counts[ i ] != numLocations )
            {
                all_same = 0;
            }
            global_locations += recv_counts[ i ];
        }

        if ( all_same )
        {
            free( recv_counts ); // Delete memory as early as possible on large systems
        }
    }

    /* Collect data to root */
    SCOREP_Ipc_Bcast( &all_same, 1, SCOREP_IPC_UINT32_T, 0 );

    uint64_t* global_data = NULL; // Only valid on rank 0
    if ( SCOREP_Ipc_GetRank() == 0 )
    {
        global_data = malloc( sizeof( uint64_t ) * global_locations );
        UTILS_ASSERT( global_data );
    }

    if ( all_same )
    {
        SCOREP_Ipc_Gather( localData, global_data, numLocations,
                           SCOREP_IPC_UINT64_T, 0 );
    }
    else
    {
        SCOREP_Ipc_Gatherv( localData, numLocations,
                            global_data, recv_counts,
                            SCOREP_IPC_UINT64_T, 0 );
        if ( SCOREP_Ipc_GetRank() == 0 )
        {
            free( recv_counts );
        }
    }
    return global_data;
}

static uint32_t*
create_rank_mappings( void )
{
    // Create permutated communicator
    SCOREP_Ipc_Group* depth_order_comm = scorep_system_tree_seq_get_ipc_group();

    // Gather data to root
    uint32_t  size;
    uint32_t* rank_mappings = NULL;
    uint32_t  rank          = SCOREP_Ipc_GetRank();
    uint32_t  root;
    if ( rank == 0 )
    {
        size          = SCOREP_Ipc_GetSize();
        rank_mappings = malloc( size * sizeof( uint32_t ) );
        UTILS_ASSERT( rank_mappings );
        root = SCOREP_IpcGroup_GetRank( depth_order_comm );
    }
    SCOREP_Ipc_Bcast( &root, 1, SCOREP_IPC_UINT32_T, 0 );
    SCOREP_IpcGroup_Gather( depth_order_comm,
                            &rank,
                            rank_mappings,
                            1,
                            SCOREP_IPC_UINT32_T,
                            root );
    return rank_mappings;
}

static uint64_t*
get_local_event_numbers( int numLocations )
{
    /* Create array of local index data */
    uint64_t* event_numbers = malloc( numLocations * sizeof( uint64_t ) );
    UTILS_ASSERT( event_numbers );

    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( &scorep_local_definition_manager, Location, location )
    {
        uint32_t index = definition->global_location_id >> 32;
        event_numbers[ local_location_map[ index ] ] = definition->number_of_events;
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();

    return event_numbers;
}

SCOREP_Ipc_Group*
scorep_system_tree_seq_get_ipc_group( void )
{
    // Create permutated communicator
    if ( depth_order_comm == NULL )
    {
        depth_order_comm = SCOREP_IpcGroup_Split( SCOREP_IPC_GROUP_WORLD,
                                                  1,
                                                  unified_rank_index );
    }
    return depth_order_comm;
}

void
scorep_system_tree_seq_free_ipc_group( SCOREP_Ipc_Group* comm )
{
    SCOREP_IpcGroup_Free( depth_order_comm );
    depth_order_comm = NULL;
}

void
scorep_system_tree_seq_unify( void )
{
    mapping_data*           mappings;
    uint64_t                num_locations;
    scorep_system_tree_seq* root = init_system_tree_seq( &scorep_local_definition_manager,
                                                         &mappings, &num_locations );
    sort_system_tree_seq( root );
    simplify_system_tree_seq( root, mappings, num_locations );
    enumerate_locations( mappings, num_locations, root );
    uint64_t levels = determine_num_levels( root );

    scorep_system_tree_seq* unified_root
        = unify_level_hierarchical( SCOREP_IPC_GROUP_WORLD, &root,
                                    levels - 3, &unified_rank_index );
    restore_parent( unified_root );

    /* *INDENT-OFF* */
    UTILS_DEBUG_ONLY(
    if ( SCOREP_Ipc_GetRank() == 0 )
    {
        dump_system_tree_seq( unified_root );
    }
    )
    /* *INDENT-ON* */

    system_tree_root   = root;
    local_location_map = create_local_location_mappings( mappings,
                                                         num_locations );
    if ( SCOREP_Env_DoTracing() )
    {
        global_location_map = gather_per_location_data( local_location_map,
                                                        num_locations );
        uint64_t* event_numbers = get_local_event_numbers( num_locations );
        global_event_numbers = gather_per_location_data( event_numbers,
                                                         num_locations );
        free( event_numbers );
    }
    rank_map = create_rank_mappings();
    free( mappings );
}

void
scorep_system_tree_seq_free( void )
{
    free_system_tree_seq_array( system_tree_root );
    free( local_location_map );
    free( global_location_map );
    free( global_event_numbers );
    free( rank_map );
}

scorep_system_tree_seq*
scorep_system_tree_seq_get_root( void )
{
    return system_tree_root;
}

const uint64_t*
scorep_system_tree_seq_get_local_location_order( void )
{
    return local_location_map;
}

const uint64_t*
scorep_system_tree_seq_get_global_location_order( void )
{
    return global_location_map;
}

const uint32_t*
scorep_system_tree_seq_get_rank_order( void )
{
    return rank_map;
}

const uint64_t*
scorep_system_tree_seq_get_event_numbers( void )
{
    return global_event_numbers;
}

uint64_t
scorep_system_tree_seq_get_number_of_copies( scorep_system_tree_seq* definition )
{
    return definition->num_copies;
}

uint64_t
scorep_system_tree_seq_get_number_of_children( scorep_system_tree_seq* definition )
{
    return definition->num_children;
}

scorep_system_tree_seq*
scorep_system_tree_seq_get_child( scorep_system_tree_seq* definition,
                                  uint64_t                index )
{
    return definition->children[ index ];
}

scorep_system_tree_seq_type
scorep_system_tree_seq_get_type( scorep_system_tree_seq* definition )
{
    return definition->seq_type;
}

uint64_t
scorep_system_tree_seq_get_sub_type( scorep_system_tree_seq* definition )
{
    return definition->sub_type;
}

SCOREP_SystemTreeDomain
scorep_system_tree_seq_get_domains( scorep_system_tree_seq* definition )
{
    return definition->domains;
}

void
scorep_system_tree_seq_traverse_all( scorep_system_tree_seq*            root,
                                     scorep_system_tree_seq_func        func,
                                     void*                              param,
                                     scorep_system_tree_seq_child_param forRoot )
{
    scorep_system_tree_seq_child_param for_children;
    for ( uint64_t i = 0; i < root->num_copies; i++ )
    {
        for_children = func( root, i, param, forRoot );
        for ( uint64_t j = 0; j < root->num_children; j++ )
        {
            scorep_system_tree_seq_traverse_all( root->children[ j ],
                                                 func,
                                                 param,
                                                 for_children );
        }
    }
}

scorep_system_tree_seq_name*
scorep_system_tree_seq_create_name_data( void )
{
    scorep_system_tree_seq_name* name_data = malloc( sizeof( scorep_system_tree_seq_name ) );
    name_data->rank_counter = 0;
    name_data->node_counter = SCOREP_Hashtab_CreateSize( 5,
                                                         SCOREP_Hashtab_HashInt64,
                                                         SCOREP_Hashtab_CompareUint64 );
    return name_data;
}

void
scorep_system_tree_seq_free_name_data( scorep_system_tree_seq_name* nameData )
{
    SCOREP_Hashtab_FreeAll( nameData->node_counter,
                            SCOREP_Hashtab_DeleteNone,
                            SCOREP_Hashtab_DeleteFree );
}

static system_tree_node_type_counter*
get_node_counter( scorep_system_tree_seq_name* nameData,
                  uint64_t                     id )
{
    size_t                hint;
    SCOREP_Hashtab_Entry* entry = SCOREP_Hashtab_Find( nameData->node_counter,
                                                       &id,
                                                       &hint );
    if ( entry != NULL )
    {
        return entry->value.ptr;
    }

    system_tree_node_type_counter* new_type = malloc( sizeof( system_tree_node_type_counter ) );
    new_type->string_id  = id;
    new_type->counter    = 0;
    new_type->class_name = SCOREP_StringHandle_GetById( id );
    SCOREP_Hashtab_InsertPtr( nameData->node_counter,
                              &new_type->string_id,
                              new_type,
                              &hint );

    return new_type;
}

const char*
scorep_system_tree_seq_get_class( scorep_system_tree_seq*      node,
                                  scorep_system_tree_seq_name* nameData )
{
    if ( node->seq_type != SCOREP_SYSTEM_TREE_SEQ_TYPE_SYSTEM_TREE_NODE )
    {
        return NULL;
    }
    uint64_t sub_type = scorep_system_tree_seq_get_sub_type( node );

    system_tree_node_type_counter* node_counter = get_node_counter( nameData, sub_type );
    return node_counter->class_name;
}

char*
scorep_system_tree_seq_get_name( scorep_system_tree_seq*      node,
                                 uint64_t                     copy,
                                 scorep_system_tree_seq_name* nameData )
{
    const char* class    = NULL;
    uint64_t    sub_type = scorep_system_tree_seq_get_sub_type( node );
    uint64_t    index    = 0;

    if ( node->seq_type == SCOREP_SYSTEM_TREE_SEQ_TYPE_SYSTEM_TREE_NODE )
    {
        system_tree_node_type_counter* node_counter = get_node_counter( nameData, sub_type );
        class = node_counter->class_name;
        index = node_counter->counter;
        node_counter->counter++;
    }
    else if ( node->seq_type == SCOREP_SYSTEM_TREE_SEQ_TYPE_LOCATION_GROUP )
    {
        const uint32_t* rank_mapping = scorep_system_tree_seq_get_rank_order();

        class = scorep_location_group_type_to_string( sub_type );
        index = rank_mapping[ nameData->rank_counter ];
        nameData->rank_counter++;
    }
    else if ( node->seq_type == SCOREP_SYSTEM_TREE_SEQ_TYPE_LOCATION )
    {
        class = scorep_location_type_to_string( sub_type );
        index = copy;
    }
    else
    {
        UTILS_ERROR( SCOREP_ERROR_UNKNOWN_TYPE,
                     "Unknown system tree sequence node type." );
        class = "unknown";
    }

    size_t display_name_length = strlen( class ) + 20;
    char*  display_name        = malloc( display_name_length );
    UTILS_ASSERT( display_name );
    snprintf( display_name,
              display_name_length,
              "%s %" PRIu64,
              class,
              index );

    return display_name;
}
