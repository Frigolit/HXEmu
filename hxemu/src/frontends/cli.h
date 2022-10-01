#ifdef FRONTEND_CLI
#ifndef __FRONTENDS_CLI__
#define __FRONTENDS_CLI__

#include "frontend.h"

class FrontendCli : public Frontend {
	public:
		virtual void start(CHX20 *);
		virtual void stop();
		virtual void run();
};

#endif
#endif
