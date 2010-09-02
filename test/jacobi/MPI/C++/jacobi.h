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

#ifndef _JACOBI_H
#define _JACOBI_H

struct JacobiData
{
    // input data
    int    iRows;
    int    iCols;
    int    iRowFirst;
    int    iRowLast;
    int    iIterMax;
    double fAlpha;
    double fRelax;
    double fTolerance;

    // calculated dx & dy
    double fDx;
    double fDy;

    // pointers to the allocated memory
    double* afU;
    double* afF;

    // start and end timestamps
    double fTimeStart;
    double fTimeStop;

    // calculated residual (output jacobi)
    double fResidual;
    // effective interation count (output jacobi)
    int    iIterCount;

    // calculated error (output error_check)
    double fError;

    // MPI-Variables
    int iMyRank;       // current process rank (number)
    int iNumProcs;     // how many processes
};

// jacobi calculation routine
void
Jacobi( JacobiData &data );

// final cleanup routines
void
Finish( JacobiData &data );

#endif
