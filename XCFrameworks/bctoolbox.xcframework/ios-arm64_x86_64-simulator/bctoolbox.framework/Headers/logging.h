/*
 * Copyright (c) 2016-2020 Belledonne Communications SARL.
 *
 * This file is part of bctoolbox.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * \file logging.h
 * \brief Logging API.
 *
 **/

#ifndef BCTBX_LOGGING_H
#define BCTBX_LOGGING_H

#include "bctoolbox/list.h"
#include <bctoolbox/port.h>

/**
 * The BCTBX_LOG_DOMAIN macro should be defined with a preprocessor directive (ex: -DBCTBX_LOG_DOMAIN="my-component") in
 *every software entity (application, library, sub-parts of software etc) using the bctoolbox log facility, so that all
 *invocations of the bctbx_message(), bctbx_warning(), bctbx_error(), bctbx_fatal() outputs their log within the domain
 *title of the software part there are compiled in. It SHALL not be defined in any public header, otherwise it will
 *conflict with upper layer using the logging facility for their own domain. As a special exception, bctoolbox defines
 *the log domain to be "bctbx" if no preprocessor directive defines it. As a result, bctboolbox owns logs will be output
 *into the "bctbx" domain.
 **/
#ifndef BCTBX_LOG_DOMAIN
#define BCTBX_LOG_DOMAIN "bctbx"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	BCTBX_LOG_DEBUG = 1,
	BCTBX_LOG_TRACE = 1 << 1,
	BCTBX_LOG_MESSAGE = 1 << 2,
	BCTBX_LOG_WARNING = 1 << 3,
	BCTBX_LOG_ERROR = 1 << 4,
	BCTBX_LOG_FATAL = 1 << 5,
	BCTBX_LOG_LOGLEV_END = 1 << 6
} BctbxLogLevel;

typedef struct _bctbx_log_handler_t bctbx_log_handler_t;

typedef void (*BctbxLogFunc)(const char *domain, BctbxLogLevel lev, const char *fmt, va_list args);
typedef void (*BctbxLogHandlerFunc)(void *info, const char *domain, BctbxLogLevel lev, const char *fmt, va_list args);
typedef void (*BctbxLogHandlerDestroyFunc)(bctbx_log_handler_t *handler);

/*
 initialise logging functions, add default log handler for stdout output.
 @param[in] bool_t create : No longer used, always created with a default logger to stdout.
 */
BCTBX_PUBLIC void bctbx_init_logger(bool_t create);

/*
 free logging memory
 */
BCTBX_PUBLIC void bctbx_uninit_logger(void);

/*
 Function to create a log handler
 @param[in] BctbxLogHandlerFunc func : the function to call to handle a new line of log
 @param[in] BctbxLogHandlerDestroyFunc destroy : the function to call to free this handler particuler its user_info
 field
 @param[in] void* user_info : complementary information to handle the logs if needed
 @return a new bctbx_log_handler_t
*/
BCTBX_PUBLIC bctbx_log_handler_t *
bctbx_create_log_handler(BctbxLogHandlerFunc func, BctbxLogHandlerDestroyFunc destroy, void *user_data);

/*
 Function to create a file log handler
 @param[in] uint64_t max_size : the maximum size of the log file before rotating to a new one (if 0 then no rotation)
 @param[in] const char* path : the path where to put the log files
 @param[in] const char* name : the name of the log files
 @param[in] FILE* f : the file where to write the logs
 @return a new bctbx_log_handler_t
*/
BCTBX_PUBLIC bctbx_log_handler_t *bctbx_create_file_log_handler(uint64_t max_size, const char *path, const char *name);

/**
 * @brief Request reopening of the log file.
 * @param[in] file_log_handler The log handler whose file will be reopened.
 * @note This function is thread-safe and reopening is done asynchronously.
 */
BCTBX_PUBLIC void bctbx_file_log_handler_reopen(bctbx_log_handler_t *file_log_handler);

/* set domain the handler is limited to. NULL for ALL*/
BCTBX_PUBLIC void bctbx_log_handler_set_domain(bctbx_log_handler_t *log_handler, const char *domain);
BCTBX_PUBLIC void bctbx_log_handler_set_user_data(bctbx_log_handler_t *, void *user_data);
BCTBX_PUBLIC void *bctbx_log_handler_get_user_data(const bctbx_log_handler_t *log_handler);

BCTBX_PUBLIC void bctbx_add_log_handler(bctbx_log_handler_t *handler);
BCTBX_PUBLIC void bctbx_remove_log_handler(bctbx_log_handler_t *handler);

/*
 * Set a callback function to render logs for the default handler.
 */
BCTBX_PUBLIC void bctbx_set_log_handler(BctbxLogFunc func);
/*
 * Same as bctbx_set_log_handler but only for a domain. NULL for all.
 * Be careful that if domain is specified, the default log handler will no longer output logs for other domains.
 */
BCTBX_PUBLIC void bctbx_set_log_handler_for_domain(BctbxLogFunc func, const char *domain);
/*Convenient function that creates a static log handler logging into supplied FILE argument.
 Despite it is not recommended to use it in libraries, it can be useful for simple test programs.*/
BCTBX_PUBLIC void bctbx_set_log_file(FILE *f);
BCTBX_PUBLIC bctbx_list_t *bctbx_get_log_handlers(void);

BCTBX_PUBLIC void bctbx_logv_out(const char *domain, BctbxLogLevel level, const char *fmt, va_list args);
BCTBX_PUBLIC void
bctbx_logv_file(void *user_info, const char *domain, BctbxLogLevel level, const char *fmt, va_list args);

/*
 * Returns 1 if the log level 'level' is enabled for the calling thread, otherwise 0.
 * This gives the condition to decide to output a log.
 */
BCTBX_PUBLIC int bctbx_log_level_enabled(const char *domain, BctbxLogLevel level);

BCTBX_PUBLIC void bctbx_logv(const char *domain, BctbxLogLevel level, const char *fmt, va_list args);

/**
 * Flushes the log output queue.
 * WARNING: Must be called from the thread that has been defined with bctbx_set_log_thread_id().
 */
BCTBX_PUBLIC void bctbx_logv_flush(void);

/**
 * Activate all log level greater or equal than specified level argument.
 **/
BCTBX_PUBLIC void bctbx_set_log_level(const char *domain, BctbxLogLevel level);

BCTBX_PUBLIC void bctbx_set_log_level_mask(const char *domain, int levelmask);
BCTBX_PUBLIC unsigned int bctbx_get_log_level_mask(const char *domain);

/**
 * Set a specific log level for the calling thread for domain.
 * When domain is NULL, the log level applies to all domains.
 */
BCTBX_PUBLIC void bctbx_set_thread_log_level(const char *domain, BctbxLogLevel level);

/**
 * Clears the specific log level set for the calling thread by bctbx_set_thread_log_level().
 * After calling this function, the global (not per thread) log level will apply to
 * logs printed by this thread.
 */
BCTBX_PUBLIC void bctbx_clear_thread_log_level(const char *domain);

/**
 * Push a log tag in the current thread.
 * A tag is made of an app-choosen identifier that identifies its type, and a value.
 * The tags are helpful to provide contexts to logs.
 * They persist in a thread-local area, until they are pop.
 * Each push for the same tag_identifier overrides the previous value given for that same
 * tag_identifier. A bctbx_pop_log_tag() with the same tag_identifier restores the previous value.
 * See the LogContext template class for a C++ convenient use of the log tag API.
 */
BCTBX_PUBLIC void bctbx_push_log_tag(const char *tag_identifier, const char *tag_value);

/**
 * Pop a local tag in the current thread.
 */
BCTBX_PUBLIC void bctbx_pop_log_tag(const char *tag_identifier);

/**
 * Retrieve the list of current tags (values only).
 * This helpfull for logger function implementation.
 */
BCTBX_PUBLIC const bctbx_list_t *bctbx_get_log_tags(void);

/*
 * An opaque type to represent a copy of current tags.
 */
typedef struct _bctbx_log_tags bctbx_log_tags_t;

/*
 * Create a copy of the current tags for the calling thread.
 * This copy can then be paste to a newly created thread, so that its log messages
 * can bear the same tags that as the thread that created it at the moment it was created.
 */
BCTBX_PUBLIC bctbx_log_tags_t *bctbx_create_log_tags_copy(void);

/**
 * Paste the logs to the current thread.
 * This is supposed to be done a on newly thread for which no tags were pushed yet.
 * The tags will remain until the thread exits.
 */
BCTBX_PUBLIC void bctbx_paste_log_tags(const bctbx_log_tags_t *log_tags);

/**
 * Destroy the opaque representation of the tag set state.
 */
BCTBX_PUBLIC void bctbx_log_tags_destroy(bctbx_log_tags_t *log_tags);

/**
 * Tell oRTP the id of the thread used to output the logs.
 * This is meant to output all the logs from the same thread to prevent deadlock problems at the application level.
 * @param[in] thread_id The id of the thread that will output the logs (can be obtained using bctbx_thread_self()).
 */
BCTBX_PUBLIC void bctbx_set_log_thread_id(unsigned long thread_id);

#ifdef __GNUC__
#define CHECK_FORMAT_ARGS(m, n) __attribute__((format(printf, m, n)))
#else
#define CHECK_FORMAT_ARGS(m, n)
#endif
#ifdef __clang__
/*in case of compile with -g static inline can produce this type of warning*/
#pragma GCC diagnostic ignored "-Wunused-function"
#endif
#ifdef BCTBX_DEBUG_MODE
static BCTBX_INLINE void CHECK_FORMAT_ARGS(1, 2) bctbx_debug(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	bctbx_logv(BCTBX_LOG_DOMAIN, BCTBX_LOG_DEBUG, fmt, args);
	va_end(args);
}
#else

#define bctbx_debug(...)

#endif

#ifdef BCTBX_NOMESSAGE_MODE

#define bctbx_log(...)
#define bctbx_message(...)
#define bctbx_warning(...)

#else

static BCTBX_INLINE void bctbx_log(const char *domain, BctbxLogLevel lev, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	bctbx_logv(domain, lev, fmt, args);
	va_end(args);
}

static BCTBX_INLINE void CHECK_FORMAT_ARGS(1, 2) bctbx_message(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	bctbx_logv(BCTBX_LOG_DOMAIN, BCTBX_LOG_MESSAGE, fmt, args);
	va_end(args);
}

static BCTBX_INLINE void CHECK_FORMAT_ARGS(1, 2) bctbx_warning(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	bctbx_logv(BCTBX_LOG_DOMAIN, BCTBX_LOG_WARNING, fmt, args);
	va_end(args);
}

#endif

static BCTBX_INLINE void CHECK_FORMAT_ARGS(1, 2) bctbx_error(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	bctbx_logv(BCTBX_LOG_DOMAIN, BCTBX_LOG_ERROR, fmt, args);
	va_end(args);
}

static BCTBX_INLINE void CHECK_FORMAT_ARGS(1, 2) bctbx_fatal(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	bctbx_logv(BCTBX_LOG_DOMAIN, BCTBX_LOG_FATAL, fmt, args);
	va_end(args);
}

#ifdef __QNX__
void bctbx_qnx_log_handler(const char *domain, BctbxLogLevel lev, const char *fmt, va_list args);
#endif

#ifdef __cplusplus
}

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#if !defined(_WIN32) && !defined(__QNX__)
#include <syslog.h>
#endif

namespace bctoolbox {
namespace log {

// Here we define our application severity levels.
enum level { normal, trace, debug, info, warning, error, fatal };

// The formatting logic for the severity level
template <typename CharT, typename TraitsT>
inline std::basic_ostream<CharT, TraitsT> &operator<<(std::basic_ostream<CharT, TraitsT> &strm,
                                                      const bctoolbox::log::level lvl) {
	static const char *const str[] = {"normal", "trace", "debug", "info", "warning", "error", "fatal"};
	if (static_cast<std::size_t>(lvl) < (sizeof(str) / sizeof(*str))) strm << str[lvl];
	else strm << static_cast<int>(lvl);
	return strm;
}

template <typename CharT, typename TraitsT>
inline std::basic_istream<CharT, TraitsT> &operator>>(std::basic_istream<CharT, TraitsT> &strm,
                                                      bctoolbox::log::level &lvl) {
	static const char *const str[] = {"normal", "trace", "debug", "info", "warning", "error", "fatal"};

	std::string s;
	strm >> s;
	for (unsigned int n = 0; n < (sizeof(str) / sizeof(*str)); ++n) {
		if (s == str[n]) {
			lvl = static_cast<bctoolbox::log::level>(n);
			return strm;
		}
	}
	// Parse error
	strm.setstate(std::ios_base::failbit);
	return strm;
}
} // namespace log
} // namespace bctoolbox

#include <ostream>

class pumpstream {
public:
	/* This constructor assumes that "domain" remains valid, which is a reasonable assumption because the pumpstream
	 * is used through macros (below) where the usage is within single line of code. */
	pumpstream(const char *domain, BctbxLogLevel level) : mDomain(domain), mLevel(level) {
#ifndef BCTBX_DEBUG_MODE
		/* If debug mode is not enabled, the pumpstream shall do nothing if level requested is BCTBX_LOG_DEBUG.
		 * bctbx_log_level_enabled() does not even need to be called. */
		if (level == BCTBX_LOG_DEBUG) {
			mIslogLevelEnabled = false;
			return;
		}
#endif
		mIslogLevelEnabled = bctbx_log_level_enabled(domain, mLevel);
	}

	~pumpstream() {
		if (mIslogLevelEnabled) bctbx_log(mDomain, mLevel, "%s", mOstringstream.str().c_str());
	}

	template <typename _Tp>
	friend pumpstream &operator<<(pumpstream &__os, _Tp &&__x);
	template <typename _Tp>
	friend pumpstream &operator<<(pumpstream &&__os, _Tp &&__x);
	friend pumpstream &operator<<(pumpstream &__os, std::ostream &(*pf)(std::ostream &));

private:
	std::ostringstream mOstringstream{};
	bool mIslogLevelEnabled = false;
	const char *mDomain;
	const BctbxLogLevel mLevel;
};

inline pumpstream &operator<<(pumpstream &pumpStream, std::ostream &(*pf)(std::ostream &)) {
	if (pumpStream.mIslogLevelEnabled) {
		pumpStream.mOstringstream << pf;
	}
	return pumpStream;
}

template <typename T>
inline pumpstream &operator<<(pumpstream &pumpStream, T &&x) {
	if (pumpStream.mIslogLevelEnabled) {
		pumpStream.mOstringstream << std::forward<T>(x);
	}
	return pumpStream;
}

template <typename T>
inline pumpstream &operator<<(pumpstream &&pumpStream, T &&x) {
	if (pumpStream.mIslogLevelEnabled) {
		pumpStream.mOstringstream << std::forward<T>(x);
	}
	return pumpStream;
}

#define BCTBX_SLOG(domain, thelevel) pumpstream(domain, thelevel)

#define BCTBX_SLOGD BCTBX_SLOG(BCTBX_LOG_DOMAIN, BCTBX_LOG_DEBUG)
#define BCTBX_SLOGI BCTBX_SLOG(BCTBX_LOG_DOMAIN, BCTBX_LOG_MESSAGE)
#define BCTBX_SLOGW BCTBX_SLOG(BCTBX_LOG_DOMAIN, BCTBX_LOG_WARNING)
#define BCTBX_SLOGE BCTBX_SLOG(BCTBX_LOG_DOMAIN, BCTBX_LOG_ERROR)

#endif
#endif
