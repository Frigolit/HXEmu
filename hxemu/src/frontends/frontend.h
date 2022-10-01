#ifndef __FRONTEND_H__
#define __FRONTEND_H__

#include "../hx20.h"

class Frontend {
	public:
		virtual void start(CHX20 *) {};
		virtual void stop() {};
		virtual void run() {};
};

#endif
