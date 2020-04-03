#! /usr/bin/env python3
# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#
# Copyright © 2020 Tarik Viehmann <viehmann@kbsg.rwth-aachen.de>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Library General Public License for more details.
#
# Read the full text in the LICENSE.GPL file in the doc directory.
#

"""
Extract skill durations from the skiller messages obtained via mongodb blackboard logger.
"""

import argparse
from datetime import datetime
import pymongo
import re

STATUS_SUCCESS = 1
STATUS_RUNNING = 2

def time_diff_in_sec(end, start):
    try:
        return int(max((end - start)/1000, 0))
    except:
        return int(max((end - start).total_seconds(), 0))

def split_skill_string(skill):
    skill = skill.strip().replace(" ","").replace("\n","")
    skill_regex = "([\w_-]+)\{(.*)\}"
    try_match = re.match(skill_regex, skill)
    if try_match:
        return try_match.group(1,2)
    else:
        raise Exception("invalid skill format", skill)


class MongoTransformer:

  def __init__(
          self,
          dst_mongodb_uri,
          dst_database,
          dst_collection):
    self.client = pymongo.MongoClient(dst_mongodb_uri)
    self.lookup_col = self.client[dst_database][dst_collection]

  def transform(self, src_mongodb_uri, src_database, src_collection):
    self.clone_collection(src_mongodb_uri, src_database, src_collection)
    col = self.client[src_database][src_collection]
    for skill_start in col.find({"status": STATUS_RUNNING}).sort("timestamp", 1):
        for skill_end in col.find({"skill_string": skill_start["skill_string"],
                               "thread": skill_start["thread"],
                               "timestamp": {"$gt": skill_start["timestamp"]}
                              }).sort("timestamp", 1).limit(1):
          if skill_end["status"] == STATUS_SUCCESS:
            name, args = split_skill_string(skill_start["skill_string"])
            lookup_entry = {"_id": {"thread": skill_start["thread"],
                                    "start_time": skill_start["timestamp"],
                                    "end_time": skill_end["timestamp"]},
                            "name": name,
                            "args": args,
                            "duration": time_diff_in_sec(skill_end["timestamp"],skill_start["timestamp"])}
            if not self.lookup_col.find_one(lookup_entry):
              self.lookup_col.insert_one(lookup_entry)
    self.client.drop_database(src_database)

  def clone_collection(self, src_mongodb_uri, src_database,src_collection):
    # drop "mongodb://" suffix from uri
    src_conn = src_mongodb_uri[10:]
    if src_conn[-1] == "/":
        src_conn = src_conn[:-1]
    self.client.admin.command({'cloneCollection': src_database+"."+src_collection, 'from': src_conn})

  def drop_collection(self, mongodb_uri, database, collection):
    curr_client = pymongo.MongoClient(mongodb_uri)
    curr_client[database][collection].drop()


def main():
  parser = argparse.ArgumentParser(description='Blackboard log to skill time lookup transformer', formatter_class=argparse.ArgumentDefaultsHelpFormatter)
  parser.add_argument(
      '--src-uri',
      type=str,
      help='The MongoDB URI of the blackboard log connection',
      default='mongodb://localhost:27017/')
  parser.add_argument(
      '--dst-uri',
      type=str,
      help='The MongoDB URI of the skiller simulator lookup connection',
      default='mongodb://localhost:27019/')
  parser.add_argument(
      '--src-db',
      type=str,
      help='The name of the blackboard log database',
      default='fflog')
  parser.add_argument(
      '--dst-db',
      type=str,
      help='The name of the skiller simulator lookup database',
      default='skills')
  parser.add_argument(
      '--src-col',
      type=str,
      help='The name of the blackboard log collection',
      default='SkillerInterface.Skiller')
  parser.add_argument(
      '--dst-col',
      type=str,
      help='The name of the skiller simulator lookup collection',
      default='exec_times')
  parser.add_argument(
      '--drop-src-col',
      type=bool,
      help='Delete the skiller blackboard log collection afterwards',
      default=False)
  args = parser.parse_args()
  transformer = MongoTransformer(args.dst_uri,args.dst_db,args.dst_col)
  transformer.transform(args.src_uri,args.src_db,args.src_col)
  if args.drop_src_col:
      transformer.drop_collection(args.src_mongodb_uri, args.src_db, args.src_col)


if __name__ == '__main__':
  main()
