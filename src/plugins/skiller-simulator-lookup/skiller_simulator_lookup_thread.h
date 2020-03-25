/***************************************************************************
 *  skiller_simulator_lookup_thread.h - Get skill exec times from db lookups
 *
 *  Created: Tue 24 Mar 2020 09:40:18 CET 09:40
 *  Copyright  2020  Tarik Viehmann <viehmann@kbsg.rwth-aachen.de>
 ****************************************************************************/

/*  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  Read the full text in the LICENSE.GPL file in the doc directory.
 */

#pragma once

#include <aspect/configurable.h>
#include <aspect/logging.h>
#include <core/threading/thread.h>
#include <plugins/mongodb/aspect/mongodb.h>
#include <plugins/skiller-simulator/execution_time_estimator_aspect/execution_time_estimator_aspect.h>

class SkillerSimulatorLookupEstimatorThread
: public fawkes::Thread,
  public fawkes::LoggingAspect,
  public fawkes::ConfigurableAspect,
  public fawkes::MongoDBAspect,
  public fawkes::skiller_simulator::ExecutionTimeEstimatorsAspect
{
public:
	SkillerSimulatorLookupEstimatorThread();
	void init();
};
