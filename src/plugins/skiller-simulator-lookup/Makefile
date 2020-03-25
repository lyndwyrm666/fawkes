#*****************************************************************************
#                      Makefile Build System for Fawkes
#                            -------------------
#   Created on Tue 24 Mar 2020 09:04:34 CET
#   Copyright (C) 2020 by Tarik Viehmann <viehmann@kbsg.rwth-aachen.de>
#
#*****************************************************************************
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#*****************************************************************************

BASEDIR = ../../..

include $(BASEDIR)/etc/buildsys/config.mk
include $(BASEDIR)/src/plugins/mongodb/mongodb.mk

LIBS_skiller_simulator_lookup = fawkescore fawkesutils fawkesaspects \
                                  fawkesmongodbaspect \
                                  fawkeslogging fawkes_skiller_time_estimator
OBJS_skiller_simulator_lookup = skiller_simulator_lookup_plugin.o \
                                  skiller_simulator_lookup_thread.o \
                                  lookup_estimator.o

CFLAGS_skiller_simulator_lookup = $(CFLAGS_MONGODB)
LDFLAGS_skiller_simulator_lookup = $(LDFLAGS_MONGODB)
CFLAGS += $(CFLAGS_skiller_simulator_lookup)
LDFLAGS += $(LDFLAGS_skiller_simulator_lookup)

OBJS_all      = $(OBJS_skiller_simulator_lookup)
PLUGINS_all   = $(PLUGINDIR)/skiller-simulator-lookup.so

ifeq ($(HAVE_CPP17)$(HAVE_MONGODB),11)
  CFLAGS       += $(CFLAGS_CPP17)
  PLUGINS_build = $(PLUGINS_all)
else
  ifneq ($(HAVE_MONGODB),1)
    WARN_TARGETS += warning_mongodb
  endif
  ifneq ($(HAVE_CPP17),1)
    WARN_TARGETS += warning_cpp17
  endif
endif

ifeq ($(OBJSSUBMAKE),1)
all: $(WARN_TARGETS)
.PHONY: $(WARN_TARGETS)

warning_cpp17:
	$(SILENT)echo -e "$(INDENT_PRINT)--> $(TRED)Omitting skiller simulator lookup Plugin$(TNORMAL) (C++17 not supported)"
warning_mongodb:
	$(SILENT)echo -e "$(INDENT_PRINT)--> $(TRED)Omitting skiller simulator lookup Plugin$(TNORMAL) (mongodb[-devel] not installed)"
endif

include $(BUILDSYSDIR)/base.mk
