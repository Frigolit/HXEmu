#ifndef __LOGGING_CONSOLE_LOGGER_H__
#define __LOGGING_CONSOLE_LOGGER_H__

#include "logger.h"

class ConsoleLogger : public Logger {
	protected:
		void log(int level, const char *msg);
};

#endif
