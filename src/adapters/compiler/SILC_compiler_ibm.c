/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */


/**
 * @ file      SILC_compiler_ibm.c
 * @maintainer Rene Jaekel <rene.jaekel@tu-dresden.de>
 *
 * @brief Support for XL IBM-Compiler
 * Will be triggered by the frunctrion trace option by the xl
 * compiler.
 */

#include <stdio.h>


#include <SILC_Utils.h>
#include <SILC_Events.h>
#include <SILC_Definitions.h>
#include <SILC_RuntimeManagement.h>

#include <SILC_Compiler_Init.h>
#include <SILC_Compiler_Data.h>



static int xl_init = 1;


/**
 * data structure to map function name and region identifier
 */
/*
   typedef struct HashNode {
   long id;
   const char* name;
   const char* fname;
   int lnobegin;
   int lnoend;
   SILC_LineNo       lnobegin;
   SILC_LineNo       lnoend;
   int reghandle;
   SILC_RegionHandle reghandle;
   struct HashNode* next;
   } HashNode;
 */

/*
 #define HASH_MAX 1021
 */

/*
   static HashNode* htab[HASH_MAX];
 */

/*
 * Stores region identifier `e' under hash code `h'
 */

/*
   void hash_put(
   long        h,
   const char* n,
   const char* fn,
    int         lno
   )
   {
   long      id  = h % HASH_MAX;
   HashNode* add = ( HashNode* )malloc( sizeof( HashNode ) );
   add->id        = h;
   add->name      = n;
   add->fname     = fn ? ( const char* )strdup( fn ) : fn;
   add->lnobegin  = lno;

   add->lnoend    = lno;

   add->reghandle = 1;
   add->next      = htab[ id ];
   htab[ id ]     = add;
   }
 */

/*
 * Lookup hash code `h'
 * Returns pointer to function data if already stored, otherwise 0
 */

/*
   static HashNode *hash_get(long h) {
   long id = h % HASH_MAX;
   HashNode *curr = htab[id];
   while ( curr ) {
   if ( curr->id == h ) {
     return curr;
   }
   curr = curr->next;
   }
   return 0;
   }
 */


/*
 * register new region
 */
/*
   HashNode*
   silc_xl_register_region
   (
   HashNode* hn
   )
   {
   printf( " register a region: %i \n", hn->reghandle );


    hn->reghandle =  SILC_DefineRegion( hn->name,
                                        SILC_INVALID_SOURCE_FILE,
                                        SILC_INVALID_LINE_NO,
                                        SILC_INVALID_LINE_NO,
                                        SILC_ADAPTER_COMPILER,
                                       SILC_REGION_FUNCTION
                                        );


   }
 */

void
xl_finalize( void );
void
__func_trace_enter( char* name,
                    char* fname,
                    int   lno );
void
__func_trace_exit( char* name,
                   char* fname,
                   int   lno );

/*
 * finalize instrumentation interface
 */
void
xl_finalize()
{
}

/*
 * This function is called at the entry of each function
 * The call is generated by the IBM xl compilers
 */
void
__func_trace_enter( char* name,
                    char* fname,
                    int   lno )
{
    HashNode* hn;

    printf( " function name: %s %s \n", name, fname );

    if ( xl_init )
    {
        /*      SILC_InitMeasurement();   */
        xl_finalize();
    }

    /* put function to list */
    if ( ( hn = hash_get( ( long )name ) ) == 0 )
    {
        hash_put( ( long )name, name, fname, lno );
        printf( " number %ld and put name -- %s -- to list \n", ( long )name, name );
    }
    else
    {
        /*      if ( (hn->reghandle == SILC_INVALID_REGION ) ) */
        if ( ( hn->reghandle == 1 ) )
        {
            silc_compiler_register_region( hn );
        }
    }
}

/*
 * This function is called at the exit of each function
 * The call is generated by the IBM xl compilers
 */
void
__func_trace_exit( char* name,
                   char* fname,
                   int   lno )
{
    printf( "call function -- %s -- at exit!!!\n", name );
    HashNode* hn;
    /*    SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, "call function exit!!!" ); */
    if ( hn = hash_get( ( long )name ) )
    {
        /*    SILC_ExitRegion( hn->reghandle ); */
    }
}
