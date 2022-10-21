// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// =============================================================================

#ifndef HXEMU_CONFIG_H
#define HXEMU_CONFIG_H

	#define VIRTUALSERIAL_BUFFERSIZE 256

	// Debugging
	#ifdef DEBUG
		#ifdef NOPRINTF
			#error Cannot combine DEBUG and NOPRINTF
		#endif

		#ifndef DEBUG_LCD
			#define DEBUG_LCD
		#endif

		#ifndef DEBUG_CPU
			#define DEBUG_CPU
		#endif

		#ifndef DEBUG_IOCTL
			#define DEBUG_IOCTL
		#endif

		#ifndef DEBUG_RTC
			#define DEBUG_RTC
		#endif
	#endif

#endif

