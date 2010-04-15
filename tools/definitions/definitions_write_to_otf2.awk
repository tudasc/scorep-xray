## -*- mode: awk -*-

## 
## This file is part of the SILC project (http://www.silc.de)
##
## Copyright (c) 2009-2011, 
##    RWTH Aachen, Germany
##    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##    Technische Universitaet Dresden, Germany
##    University of Oregon, Eugene USA
##    Forschungszentrum Juelich GmbH, Germany
##    Technische Universitaet Muenchen, Germany
##
## See the COPYING file in the package base directory for details.
##

## file       definitions_write_to_otf2.awk
## maintainer Christian Roessel <c.roessel@fz-juelich.de>

{
    print "static void"
    print "silc_write_" $3 "_definitions_to_otf2( OTF2_DefWriter* definitionWriter )"
    print "{"
    print "    " $2 "_Definition* definition ="
    print "        &( silc_definition_manager." $3 "_definitions_head_dummy );"
    print "    while ( !SILC_MEMORY_MOVABLE_IS_NULL( definition->next ) )"
    print "    {"
    print "        definition = SILC_MEMORY_DEREF_MOVABLE( &( definition->next ),"
    print "                                                " $2 "_Definition* );"
    print "        //OTF2_DefWriter_Def...(definitionWriter, ...);"
    print "        assert( false ); // implement me"
    print "    }"
    print "}\n\n"
}
