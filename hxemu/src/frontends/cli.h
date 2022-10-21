#ifdef FRONTEND_CLI
#ifndef HXEMU_FRONTENDS_CLI
#define HXEMU_FRONTENDS_CLI

#include "frontend.h"

#include "cli/lcd_interface.h"

class FrontendCli : public Frontend {
	public:
		virtual void start(CHX20 *);
		virtual void stop();
		virtual void run();

	protected:
		CliLcdInterface *lcd;
};

#endif
#endif
