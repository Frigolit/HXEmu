#ifndef __LOGGING_NULL_LOGGER_H__
#define __LOGGING_NULL_LOGGER_H__

#include "logger.h"

class NullLogger : public Logger {
	protected:
		void log(int level, const char *msg) { /* do nothing */ }
};

#endif
