#ifndef HXEMU_LOGGING_NULL_LOGGER_H
#define HXEMU_LOGGING_NULL_LOGGER_H

#include "logger.h"

class NullLogger : public Logger {
	protected:
		void log(int level, const char *msg) { /* do nothing */ }
};

#endif
