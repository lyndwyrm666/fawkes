
/***************************************************************************
 *  pddl_robot_memory_thread.h - pddl_robot_memory
 *
 *  Plugin created: Thu Oct 13 13:34:05 2016

 *  Copyright  2016  Frederik Zwilling
 *
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

#ifndef __PLUGINS_PDDL_ROBOT_MEMORYTHREAD_H_
#define __PLUGINS_PDDL_ROBOT_MEMORYTHREAD_H_

#include <core/threading/thread.h>
#include <aspect/blocked_timing.h>
#include <aspect/logging.h>
#include <aspect/blackboard.h>
#include <aspect/configurable.h>

#include <string>

namespace fawkes {
}

class PddlRobotMemoryThread 
: public fawkes::Thread,
  public fawkes::BlockedTimingAspect,
  public fawkes::LoggingAspect,
  public fawkes::ConfigurableAspect,
  public fawkes::BlackBoardAspect
{

 public:
  PddlRobotMemoryThread();

  virtual void init();
  virtual void finalize();
  virtual void loop();

  /** Stub to see name in backtrace for easier debugging. @see Thread::run() */
  protected: virtual void run() { Thread::run(); }

 private:
  //Define class member variables here

};


#endif