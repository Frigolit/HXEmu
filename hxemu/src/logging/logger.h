#ifndef HXEMU_LOGGING_LOGGER_H
#define HXEMU_LOGGING_LOGGER_H

#define LOG_FATAL 0
#define LOG_ERROR 1
#define LOG_WARN 2
#define LOG_NOTICE 3
#define LOG_INFO 4
#define LOG_DEBUG 5
#define LOG_TRACE 6

class Logger {
	public:
		virtual void fatal(const char *msg) { log(LOG_FATAL, msg); }
		virtual void error(const char *msg) { log(LOG_ERROR, msg); }
		virtual void warn(const char *msg) { log(LOG_WARN, msg); }
		virtual void notice(const char *msg) { log(LOG_NOTICE, msg); }
		virtual void info(const char *msg) { log(LOG_INFO, msg); }
		virtual void debug(const char *msg) { log(LOG_DEBUG, msg); }
		virtual void trace(const char *msg) { log(LOG_TRACE, msg); }

	protected:
		virtual void log(int level, const char *msg) { }
};

extern Logger *logger;

#endif
