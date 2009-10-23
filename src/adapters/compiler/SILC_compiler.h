#ifndef SILC_COMPILER_H_
#define SILC_COMPILER_H_


/* ************************************************************************
 * Compiler interface support
 * ***********************************************************************/


#incluce <stdio.h>


/**
 * Compiler adapter finalizer
 */
extern void
( *silc_comp_finalize )
(
    void
);


#endif /* SILC_COMPILER_H_ */
