/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen University, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */


#ifndef SCOREP_INTERNAL_METRICS_H
#define SCOREP_INTERNAL_METRICS_H

/**
 * @file       scorep_metrics.h
 * @maintainer Ronny Tschueter <ronny.tschueter@zih.tu-dresden.de>
 *
 * @status alpha
 *
 *
 */


typedef struct SCOREP_Metric_LocationData SCOREP_Metric_LocationData;


struct SCOREP_Metric_LocationData
{
    uint64_t* values;
};

#endif /* SCOREP_INTERNAL_METRICS_H */
