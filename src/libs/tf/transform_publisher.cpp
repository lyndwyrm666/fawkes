/***************************************************************************
 *  transform_publisher.cpp - Fawkes transform publisher (based on ROS tf)
 *
 *  Created: Mon Oct 24 17:13:20 2011
 *  Copyright  2011  Tim Niemueller [www.niemueller.de]
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

/* This code is based on ROS tf with the following copyright and license:
 *
 * Copyright (c) 2008, Willow Garage, Inc.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Willow Garage, Inc. nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <tf/transform_publisher.h>

#include <blackboard/blackboard.h>
#include <interfaces/TransformInterface.h>

#include <core/threading/mutex.h>
#include <core/threading/mutex_locker.h>

namespace fawkes {
  namespace tf {
#if 0 /* just to make Emacs auto-indent happy */
  }
}
#endif

/** @class TransformPublisher <tf/transform_publisher.h>
 * Utility class to send transforms.
 * The transform publisher opens an instance of TransformInterface on
 * the blackboard for writing and publishes every transform through
 * that interface. Assuming that the event-based listener is used
 * it will catch all updates even though we might send them in quick
 * succession.
 * @author Tim Niemueller
 *
 * @fn   void TransformPublisher::send_transform(const Transform &transform, const fawkes::Time &time, const std::string frame, const std::string child_frame)
 * Convenience wrapper to send a transform.
 * This simply calls send_transform() with a StampedTransform created
 * from the data pased into this method.
 * @param transform transform to publish
 * @param time time of the transform to publish
 * @param frame reference frame ID
 * @param child_frame child frame ID
 */

/** Constructor.
 * @param bb blackboard to open transform interface on, if 0 the
 * publisher will be disabled. Trying to send a transform will
 * result in a DisabledException being thrown.
 * @param bb_iface_id the blackboard interface ID to be used for the
 * opened TransformInterface. Note that the name is prefixed with "TF ".
 */
TransformPublisher::TransformPublisher(BlackBoard *bb,
                                       const char *bb_iface_id)
  : __bb(bb), __mutex(new Mutex())
{
  if (__bb) {
    std::string bbid = std::string("TF ") + bb_iface_id;
    std::string owner = std::string("TF/P:") + bb_iface_id;
    __tfif = __bb->open_for_writing<TransformInterface>(bbid.c_str(), owner.c_str());
    __tfif->set_auto_timestamping(false);
  }
}


/** Destructor.
 * Closes TransformInterface, hence BlackBoard must still be alive and
 * valid.
 */
TransformPublisher::~TransformPublisher()
{
  if (__bb) __bb->close(__tfif);
  delete __mutex;
}


/** Publish transform.
 * @param transform transform to publish
 */
void
TransformPublisher::send_transform(const StampedTransform &transform)
{
  if (! __bb) {
    throw DisabledException("TransformPublisher is disabled");
  }

  MutexLocker lock(__mutex);

  __tfif->set_timestamp(&transform.stamp);
  __tfif->set_frame(transform.frame_id.c_str());
  __tfif->set_child_frame(transform.child_frame_id.c_str());
  double translation[3], rotation[4];
  const Vector3 &t = transform.getOrigin();
  translation[0] = t.x(); translation[1] = t.y(); translation[2] = t.z();
  Quaternion r = transform.getRotation();
  assert_quaternion_valid(r);
  rotation[0] = r.x(); rotation[1] = r.y();
  rotation[2] = r.z(); rotation[3] = r.w();
  __tfif->set_translation(translation);
  __tfif->set_rotation(rotation);
  __tfif->write();
}


} // end namespace tf
} // end namespace fawkes
