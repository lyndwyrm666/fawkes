
/***************************************************************************
 *  pcl_db_merge_thread.cpp - Restore and merge point clouds from MongoDB
 *
 *  Created: Wed Nov 28 10:56:10 2012 (Freiburg)
 *  Copyright  2012  Tim Niemueller [www.niemueller.de]
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

#include "pcl_db_merge_thread.h"
#include <interfaces/PclDatabaseMergeInterface.h>

#include <blackboard/utils/on_message_waker.h>

using namespace fawkes;

#define CFG_PREFIX "/perception/pcl-db-merge/"

/** @class PointCloudDBMergeThread "pcl_db_merge_thread.h"
 * Thread to merge point clouds from database on request.
 * @author Tim Niemueller
 */


/** Constructor. */
PointCloudDBMergeThread::PointCloudDBMergeThread()
  : Thread("PointCloudDBMergeThread", Thread::OPMODE_WAITFORWAKEUP)
{
}


/** Destructor. */
PointCloudDBMergeThread::~PointCloudDBMergeThread()
{
}


void
PointCloudDBMergeThread::init()
{
  pl_xyz_ = NULL;
  pl_xyzrgb_ = NULL;
  merge_if_ = NULL;
  msg_waker_ = NULL;

  cfg_database_name_ = config->get_string(CFG_PREFIX"database-name");
  cfg_global_frame_  = config->get_string(CFG_PREFIX"global-frame");
  cfg_output_id_     = config->get_string(CFG_PREFIX"output-pcl-id");

  foutput_ = new pcl::PointCloud<pcl::PointXYZRGB>();
  //foutput_->header.frame_id = finput_->header.frame_id;
  foutput_->is_dense = false;
  pcl_manager->add_pointcloud<pcl::PointXYZRGB>(cfg_output_id_.c_str(), foutput_);
  output_ = pcl_utils::cloudptr_from_refptr(foutput_);

  
  pl_xyz_ =
    new PointCloudDBMergePipeline<pcl::PointXYZ>(mongodb_client,
						 cfg_database_name_,
						 cfg_global_frame_,
						 output_, logger);

  pl_xyzrgb_ =
    new PointCloudDBMergePipeline<pcl::PointXYZRGB>(mongodb_client,
						    cfg_database_name_,
						    cfg_global_frame_,
						    output_, logger);

  try {
    merge_if_ =
      blackboard->open_for_writing<PclDatabaseMergeInterface>("PCL Database Merge");

    msg_waker_ = new BlackBoardOnMessageWaker(blackboard, merge_if_, this);
  } catch (Exception &e) {
    finalize();
    throw;
  }
}

void
PointCloudDBMergeThread::finalize()
{
  delete msg_waker_;
  blackboard->close(merge_if_);

  delete pl_xyz_;
  delete pl_xyzrgb_;

  output_.reset();
  pcl_manager->remove_pointcloud(cfg_output_id_.c_str());
  foutput_.reset();
}


void
PointCloudDBMergeThread::loop()
{
  std::vector<long long> times;
  std::string collection;
  //= "PointClouds.openni_pointcloud_xyz";

  if (merge_if_->msgq_empty()) return;

  if (PclDatabaseMergeInterface::MergeMessage *msg =
        merge_if_->msgq_first_safe(msg))
  {
    merge_if_->set_final(false);
    merge_if_->set_msgid(msg->id());
    merge_if_->set_error("");
    merge_if_->write();

    int64_t *timestamps = msg->timestamps();
    for (size_t i = 0; i < msg->maxlenof_timestamps(); ++i) {
      if (timestamps[i] > 0) {
	times.push_back(timestamps[i]);
      }
    }
    collection = msg->collection();
  }

  merge_if_->msgq_pop();

  /* For testing
  collection = "PointClouds.openni_pointcloud_xyz";
  times.clear();
  times.push_back(1354200347715);
  times.push_back(1354200406578);
  times.push_back(1354200473345);
  */

  logger->log_info(name(), "Restoring from '%s' for the following times",
		   collection.c_str());
  for (size_t i = 0; i < times.size(); ++i) {
    logger->log_info(name(), "  %lli", times[i]);
  }

  if (pl_xyz_->applicable(times, collection)) {
    pl_xyz_->merge(times, collection);
  } else if (pl_xyzrgb_->applicable(times, collection)) {
    pl_xyzrgb_->merge(times, collection);
  } else {
    logger->log_warn(name(), "No applicable merging pipeline known");
    merge_if_->set_error("Invalid input data");
  }

  foutput_->header.frame_id = cfg_global_frame_;
  Time now(clock);
  pcl_utils::set_time(foutput_, now);

  merge_if_->set_final(true);
  merge_if_->write();
}

