// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// @version $Id: romfixer.c 158 2012-03-02 09:39:54Z frigolit $
// =============================================================================

#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
	if (argc < 3) {
		printf("Usage: %s <input file> <output file>\n", argv[0]);
		return 1;
	}
	
	// Open ROM
	FILE *fin = fopen(argv[1], "rb");
	if (fin == NULL) {
		printf("ERROR: Cannot open input file %s\n", argv[1]);
		return 1;
	}
	
	// Read ROM
	char buf[8193];
	memset(buf, 0, 8193);
	
	if (fread(buf, 1, 8193, fin) == 8193) {
		printf("ERROR: ROM is larger than 8kB\n");
		fclose(fin);
		return 1;
	}
	fclose(fin);
	
	// Write output file
	FILE *fout = fopen(argv[2], "wb");
	if (fout == NULL) {
		printf("ERROR: Cannot create output file %s\n", argv[2]);
		return 1;
	}
	
	if (fwrite(buf, 1, 8192, fout) < 8192) {
		fclose(fout);
		printf("ERROR: Failed to write entire ROM\n");
		return 1;
	}
	fclose(fout);
	
	return 0;
}

