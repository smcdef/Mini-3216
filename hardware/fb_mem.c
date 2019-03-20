#include "fb_core.h"
#include "font.h"
#include "fb_mem.h"

unsigned int fb_scan_string(struct fb_info *fb_info, unsigned char speed,
			    const char *s)
{
	char code *encode;
	bool first = true;
	unsigned int start_column = 0, new_columns = 0, offset = 0;

	while (*s) {
		char width = search_encode(s, &encode);

		/* skip invalid input */
		if (width < 0) {
			s += *encode;
			continue;
		}
		s += width == CHARACTER_WIDTH ? ENCODE_INDEX_SIZE : 1;
		offset += fb_copy(offset, encode, width);
		new_columns += width;
		if (offset > FB_COLUMNS - MATRIXS_COLUMNS - CHARACTER_WIDTH &&
		    !first) {
			fb_info->offset = start_column;
			fb_scan(fb_info, new_columns + MATRIXS_COLUMNS,
				speed);
			start_column += new_columns;
			new_columns = 0;
		} else if (first &&
			   new_columns > FB_COLUMNS - CHARACTER_WIDTH) {
			fb_info->offset = start_column;
			fb_scan(fb_info, new_columns, speed);
			start_column += new_columns - MATRIXS_COLUMNS;
			new_columns = 0;
			first = false;
		}
	}

	fb_info->offset = start_column;
	if (first)
		new_columns -= MATRIXS_COLUMNS;
	fb_scan(fb_info, new_columns + MATRIXS_COLUMNS, speed);

	return start_column + new_columns;
}

unsigned int fb_copy_string(unsigned int offset, const char *s)
{
	char code *encode;
	unsigned int n = 0;

	while (*s) {
		char width = search_encode(s, &encode);

		/* skip invalid input */
		if (width < 0) {
			s += *encode;
			continue;
		}
		s += width == CHARACTER_WIDTH ? ENCODE_INDEX_SIZE : 1;
		offset += fb_copy(offset, encode, width);
		n += width;
	}

	return n;
}
