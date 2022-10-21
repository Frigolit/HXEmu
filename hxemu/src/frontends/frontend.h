#ifndef HXEMU_FRONTENDS_FRONTEND_H
#define HXEMU_FRONTENDS_FRONTEND_H

#include "../hx20.h"

class Frontend {
	public:
		virtual void start(CHX20 *) {};
		virtual void stop() {};
		virtual void run() {};
};

#endif
