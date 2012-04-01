
/***************************************************************************
 *  tf.cpp - Transform aspect for Fawkes
 *
 *  Created: Tue Oct 25 21:35:14 2011
 *  Copyright  2006-2011  Tim Niemueller [www.niemueller.de]
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

#include <aspect/tf.h>

#include <cstring>
#include <cstdlib>
#include <core/threading/thread_initializer.h>

namespace fawkes {
#if 0 /* just to make Emacs auto-indent happy */
}
#endif

/** @class TransformAspect <aspect/tf.h>
 * Thread aspect to access the transform system.

 * Give this aspect to your thread to gain access to the transform
 * library.  Depending on the parameters to the ctor only the listener
 * or additionaly the publisher is created.
 * It is guaranteed that if used properly from within plugins that the
 * blackboard member has been initialized properly.
 * @ingroup Aspects
 * @author Tim Niemueller
 */


/** @var tf::TransformListener *  TransformAspect::tf_listener
 * This is the transform listener which saves transforms published by
 * other threads in the system.
 */

/** @var tf::TransformPublisher *  TransformAspect::tf_publisher
 * This is the transform publisher which can be used to publish
 * transforms via the blackboard. It is only created if the constructor
 * taking the blackboard interface ID parameter is used!
 */

/** Constructor.
 * @param mode mode of operation
 * @param tf_bb_iface_id interface ID to be used for the transform
 * publisher. Note that this will be prefixed with "TF ".
 */
TransformAspect::TransformAspect(Mode mode, const char *tf_bb_iface_id)
  : __tf_aspect_mode(mode)
{
  add_aspect("TransformAspect");
  if (((mode == ONLY_PUBLISHER) || (mode == BOTH)) && tf_bb_iface_id) {
    __tf_aspect_bb_iface_id = strdup(tf_bb_iface_id);
  } else {
    __tf_aspect_bb_iface_id = 0;
  }
}


/** Virtual empty destructor. */
TransformAspect::~TransformAspect()
{
  if (__tf_aspect_bb_iface_id)  free(__tf_aspect_bb_iface_id);
}


/** Init transform aspect.
 * This creates the listener and potentially publisher.
 * @param blackboard blackboard used to create listener and/or publisher.
 */
void
TransformAspect::init_TransformAspect(BlackBoard *blackboard)
{
  if (((__tf_aspect_mode == ONLY_PUBLISHER) || (__tf_aspect_mode == BOTH)) &&
      (__tf_aspect_bb_iface_id == NULL))
  {
    throw CannotInitializeThreadException("TransformAspect was initialized "
                                          "in mode %s but BB interface ID"
                                          "is not set",
                                          (__tf_aspect_mode == BOTH) ? "BOTH"
                                          : "ONLY_PUBLISHER");
  }

  if ((__tf_aspect_mode == ONLY_LISTENER) || (__tf_aspect_mode == BOTH)) {
    tf_listener = new tf::TransformListener(blackboard);
  } else {
    tf_listener = new tf::TransformListener(NULL);
  }

  if ((__tf_aspect_mode == ONLY_PUBLISHER) || (__tf_aspect_mode == BOTH)) {
    tf_publisher =
      new tf::TransformPublisher(blackboard, __tf_aspect_bb_iface_id);
  } else {
    tf_publisher = new tf::TransformPublisher(NULL, NULL);
  }
}

/** Finalize transform aspect.
 * This deletes the transform listener and publisher.
 */
void
TransformAspect::finalize_TransformAspect()
{
  delete tf_listener;
  delete tf_publisher;
  tf_listener = 0;
  tf_publisher = 0;
}

} // end namespace fawkes