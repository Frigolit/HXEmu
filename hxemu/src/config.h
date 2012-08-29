// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// @version $Id: config.h 94 2011-06-27 21:42:57Z frigolit $
// =============================================================================

#ifndef __CONFIG_H__
#define __CONFIG_H__

	#define VIRTUALSERIAL_BUFFERSIZE 256

	// Debugging
	#ifdef DEBUG
		#ifndef DEBUG_LCD
			#define DEBUG_LCD
		#endif
		
		#ifndef DEBUG_CPU
			#define DEBUG_CPU
		#endif
	#endif

#endif

