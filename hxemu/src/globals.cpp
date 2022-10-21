#include <cstdio>

#include "globals.h"

char datapath[256];

// TODO: Handle bufsize
int get_data_path(char *buf, const char *path, int bufsize) {
	return sprintf(buf, "%s/%s", datapath, path);
}
