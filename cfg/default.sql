BEGIN TRANSACTION;
CREATE TABLE config (
  path      TEXT NOT NULL,
  type      TEXT NOT NULL,
  value     NOT NULL,
  comment   TEXT,
  PRIMARY KEY (path)
);
INSERT INTO "config" VALUES('/fawkes/mainapp/min_loop_time','unsigned int',30000,'Minimum loop time of main thread; microseconds; 0 to disable');
INSERT INTO "config" VALUES('/fawkes/mainapp/blackboard_size','unsigned int','2097152','Size of BlackBoard memory segment; bytes');
INSERT INTO "config" VALUES('/fawkes/mainapp/blackboard_magic_token','string','FawkesBlackBoard','Magic token for the BlackBoard shared memory segment');
INSERT INTO "config" VALUES('/firevision/fountain/tcp_port','unsigned int',2208,'Fountain TCP Port');
INSERT INTO "config" VALUES('/worldinfo/multicast_addr','string','224.16.0.1','Multicast address to send world info messages to.');
INSERT INTO "config" VALUES('/worldinfo/udp_port','unsigned int','2806','UDP port to listen for and send world info messages to.');
INSERT INTO "config" VALUES('/worldinfo/encryption_key','string','AllemaniACsX0rz','Default encryption key for world info.');
INSERT INTO "config" VALUES('/worldinfo/encryption_iv','string','DoesAnyOneCare','Default encryption initialization vector for world info.');
INSERT INTO "config" VALUES('/worldinfo/enable_fatmsg','bool','0','Send legacy fat message?');
INSERT INTO "config" VALUES('/ballposlog/log_level','unsigned int','0','Log level for ballposlog example plugin; sum of any of debug=0, info=1, warn=2, error=4, none=8');
INSERT INTO "config" VALUES('/skiller/skillspace','string','test','Skill space');
INSERT INTO "config" VALUES('/skiller/watch_files','bool',1,'Watch lua files for modification and automatically reload Lua if files have been changed; true to enable');
INSERT INTO "config" VALUES('/skiller/interfaces/test/reading/navigator','string','NavigatorInterface::Navigator',NULL);
INSERT INTO "config" VALUES('/skiller/interfaces/test/reading/wm_pose','string','ObjectPositionInterface::WM Pose',NULL);
INSERT INTO "config" VALUES('/skiller/interfaces/test/reading/speechsynth','string','SpeechSynthInterface::Flite',NULL);
COMMIT;