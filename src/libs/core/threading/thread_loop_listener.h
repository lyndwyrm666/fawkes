
/***************************************************************************
 *  thread_loop_listener.h - thread loop listener interface
 *
 *  Created: Thu Feb 19 13:50:42 2015
 *  Copyright  2015 Till Hofmann
 *
 ****************************************************************************/

/*  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version. A runtime exception applies to
 *  this software (see LICENSE.GPL_WRE file mentioned below for details).
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  Read the full text in the LICENSE.GPL_WRE file in the doc directory.
 */

#ifndef _CORE_THREADING_THREAD_LOOP_LISTENER_H_
#define _CORE_THREADING_THREAD_LOOP_LISTENER_H_

namespace fawkes {


class Thread;

class ThreadLoopListener
{
 public:
  virtual ~ThreadLoopListener();

  virtual void pre_loop(Thread *thread);
  virtual void post_loop(Thread *thread);
};


} // end namespace fawkes

#endif
