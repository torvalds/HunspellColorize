#include "xdg_dirs.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define PATH_BUF_SIZE 128

static int is_regular_file(const char *path) {
	struct stat st;
	return (!stat(path, &st) && S_ISREG(st.st_mode));
}

/// if dst is null or too small, returns the size of buffer needed
/// returns 0 when fails
static int find_file(char *dst, size_t dst_len, const char *filename) {
	char path_buf[PATH_BUF_SIZE];

	const char *dirs = getenv("XDG_DATA_DIRS");
	if (!dirs) return 0;

	// copy dirs because per getenv manpage you
	// shouldn't modify the returned string
	char *dir, *dir_original;
	dir = dir_original = malloc(strlen(dirs) + 1);
	strcpy(dir, dirs);

	dir = strtok(dir, ":");
	while (dir != NULL) {
		size_t len = snprintf(path_buf, PATH_BUF_SIZE, "%s/hunspell/%s", dir, filename) + 1;
		if (is_regular_file(path_buf)) {
			if (dst != NULL && len <= dst_len) strcpy(dst, path_buf);
			free(dir_original);
			return len;
		}

		dir = strtok(NULL, ":");
	}

	free(dir_original);
	return 0;
}

int find_affix_file(char *dst, size_t dst_len) {
	return find_file(dst, dst_len, "en_US.aff");
}

int find_dic_file(char *dst, size_t dst_len) {
	return find_file(dst, dst_len, "en_US.dic");
}

