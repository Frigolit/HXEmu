// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// @version $Id: hash.h 164 2012-03-09 10:49:03Z frigolit $
// =============================================================================

#include <stdint.h>

class CHash {
	public:
		CHash();
		
		uint32_t crc32(uint8_t *data, uint32_t len);
		
	private:
		uint32_t crc32_tab[256];
};

