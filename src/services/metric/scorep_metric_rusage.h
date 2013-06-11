/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
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


#ifndef SCOREP_METRIC_RUSAGE_H_
#define SCOREP_METRIC_RUSAGE_H_


/**
 * @file       scorep_metric_rusage.h.h
 * @maintainer Ronny Tschueter <ronny.tschueter@zih.tu-dresden.de>
 *
 * @brief Resource usage metric source definition object.
 */

#include "SCOREP_Metric_Source.h"

/**
   The metric source initialization and finalization function structure for the
   metric adapter.
 */
extern const SCOREP_MetricSource SCOREP_Metric_Rusage;


#endif /* SCOREP_METRIC_RUSAGE_H_ */
