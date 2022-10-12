#include <iostream>

#include "console_logger.h"

void ConsoleLogger::log(int level, const char *msg) {
	switch (level) {
		case LOG_FATAL: std::cout << "\e[41;1m[FATAL] "; break;
		case LOG_ERROR: std::cout << "\e[31;1m[ERROR] "; break;
		case LOG_WARN: std::cout << "\e[33;1m[WARN] "; break;
		case LOG_NOTICE: std::cout << "\e[1m[NOTICE] "; break;
		case LOG_INFO: std::cout << "[INFO] "; break;
		case LOG_DEBUG: std::cout << "[DEBUG] "; break;
		case LOG_TRACE: std::cout << "[TRACE] "; break;
	}

	std::cout << msg << "\e[0m" << std::endl;
}
