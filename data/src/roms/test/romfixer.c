// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// @version $Id: romfixer.c 158 2012-03-02 09:39:54Z frigolit $
// =============================================================================

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv) {
	if (argc < 4) {
		fprintf(stderr, "Usage: %s <input file> <output file> <offset>\n", argv[0]);
		return 1;
	}
	
	long offset = atol(argv[3]);
	
	// Open ROM
	FILE *fin = fopen(argv[1], "rb");
	if (fin == NULL) {
		fprintf(stderr, "ERROR: Cannot open input file %s\n", argv[1]);
		return 1;
	}
	
	// Read ROM
	char buf[32769];
	memset(buf, 0, 32769);
	int r = fread(buf, 1, 32768, fin);
	if (r < 32768) {
		fprintf(stderr, "ERROR: ROM is smaller than 32kB\n");
		return 1;
	}
	fclose(fin);
	
	// Write output file
	FILE *fout = fopen(argv[2], "wb");
	if (fout == NULL) {
		fprintf(stderr, "ERROR: Cannot create output file %s\n", argv[2]);
		return 1;
	}
	
	if (fwrite(buf + offset, 1, 8192, fout) < 8192) {
		fclose(fout);
		fprintf(stderr, "ERROR: Failed to write the entire 8kB ROM\n");
		return 1;
	}
	fclose(fout);
	
	return 0;
}

