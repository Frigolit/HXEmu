#ifndef HXEMU_LOGGING_CONSOLE_LOGGER_H
#define HXEMU_LOGGING_CONSOLE_LOGGER_H

#include "logger.h"

class ConsoleLogger : public Logger {
	protected:
		void log(int level, const char *msg);
};

#endif
