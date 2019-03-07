
/***************************************************************************
 *  network_logger.h - Fawkes network logger
 *
 *  Created: Sat Dec 15 00:45:54 2007 (after I5 xmas party)
 *  Copyright  2006-2017  Tim Niemueller [www.niemueller.de]
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

#ifndef _LOGGING_NETWORK_H_
#define _LOGGING_NETWORK_H_

#include <core/utils/lock_list.h>
#include <core/utils/lock_queue.h>
#include <logging/logger.h>
#include <netcomm/fawkes/handler.h>
#include <netcomm/fawkes/message_content.h>

#include <stdint.h>

namespace fawkes {

class Mutex;
class FawkesNetworkHub;

class NetworkLogger : public Logger, public FawkesNetworkHandler
{
public:
	NetworkLogger(FawkesNetworkHub *hub, LogLevel log_level = LL_DEBUG);
	virtual ~NetworkLogger();

	virtual void log_debug(const char *component, const char *format, ...);
	virtual void log_info(const char *component, const char *format, ...);
	virtual void log_warn(const char *component, const char *format, ...);
	virtual void log_error(const char *component, const char *format, ...);

	virtual void log_debug(const char *component, Exception &e);
	virtual void log_info(const char *component, Exception &e);
	virtual void log_warn(const char *component, Exception &e);
	virtual void log_error(const char *component, Exception &e);

	virtual void vlog_debug(const char *component, const char *format, va_list va);
	virtual void vlog_info(const char *component, const char *format, va_list va);
	virtual void vlog_warn(const char *component, const char *format, va_list va);
	virtual void vlog_error(const char *component, const char *format, va_list va);

	virtual void tlog_debug(struct timeval *t, const char *component, const char *format, ...);
	virtual void tlog_info(struct timeval *t, const char *component, const char *format, ...);
	virtual void tlog_warn(struct timeval *t, const char *component, const char *format, ...);
	virtual void tlog_error(struct timeval *t, const char *component, const char *format, ...);

	virtual void tlog_debug(struct timeval *t, const char *component, Exception &e);
	virtual void tlog_info(struct timeval *t, const char *component, Exception &e);
	virtual void tlog_warn(struct timeval *t, const char *component, Exception &e);
	virtual void tlog_error(struct timeval *t, const char *component, Exception &e);

	virtual void
	             vtlog_debug(struct timeval *t, const char *component, const char *format, va_list va);
	virtual void vtlog_info(struct timeval *t, const char *component, const char *format, va_list va);
	virtual void vtlog_warn(struct timeval *t, const char *component, const char *format, va_list va);
	virtual void
	vtlog_error(struct timeval *t, const char *component, const char *format, va_list va);

	virtual void handle_network_message(FawkesNetworkMessage *msg);
	virtual void client_connected(unsigned int clid);
	virtual void client_disconnected(unsigned int clid);

	/** NetworkLogger message types. */
	typedef enum {
		MSGTYPE_SUBSCRIBE   = 1, /**< Subscribe for logging messages. */
		MSGTYPE_UNSUBSCRIBE = 2, /**< Unsubscribe from receiving logging messages. */
		MSGTYPE_LOGMESSAGE  = 3  /**< Log message. */
	} network_logger_msgtype_t;

#pragma pack(push, 4)
	/** Network logging message header. */
	typedef struct
	{
		uint32_t log_level : 4; /**< LogLevel, @see Logger::LogLevel */
		uint32_t exception : 1; /**< 1 if message was generated by exception, 0 otherwise */
		uint32_t reserved : 27; /**< reserved for future use */
		uint64_t time_sec;      /**< time in seconds since the epoch, encoded in network byte order */
		uint32_t time_usec;     /**< addition to time in usec, encoded in network byte order */
	} network_logger_header_t;
#pragma pack(pop)

private:
	void send_message(Logger::LogLevel level,
	                  struct timeval * t,
	                  const char *     component,
	                  bool             is_exception,
	                  const char *     format,
	                  va_list          va);
	void send_message(Logger::LogLevel level,
	                  struct timeval * t,
	                  const char *     component,
	                  bool             is_exception,
	                  const char *     message);

	FawkesNetworkHub *hub;

	LockQueue<FawkesNetworkMessage *> inbound_queue;

	LockList<unsigned int>           subscribers_;
	LockList<unsigned int>::iterator ssit_;
};

class NetworkLoggerMessageContent : public FawkesNetworkMessageContent
{
public:
	NetworkLoggerMessageContent(Logger::LogLevel log_level,
	                            struct timeval * t,
	                            const char *     component,
	                            bool             is_exception,
	                            const char *     message);
	NetworkLoggerMessageContent(Logger::LogLevel log_level,
	                            struct timeval * t,
	                            const char *     component,
	                            bool             is_exception,
	                            const char *     format,
	                            va_list          va);
	NetworkLoggerMessageContent(const NetworkLoggerMessageContent *content);
	NetworkLoggerMessageContent(unsigned int component_id,
	                            unsigned int msg_id,
	                            void *       payload,
	                            size_t       payload_size);
	virtual ~NetworkLoggerMessageContent();

	struct timeval   get_time() const;
	Logger::LogLevel get_loglevel() const;
	const char *     get_component() const;
	const char *     get_message() const;
	bool             is_exception() const;

	virtual void serialize();

private:
	NetworkLogger::network_logger_header_t *header;
	const char *                            component_;
	const char *                            message_;
	bool                                    own_payload_;
};

} // end namespace fawkes

#endif
