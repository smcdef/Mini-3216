#include "fb_core.h"
#include "font.h"
#include "fb_mem.h"

unsigned char fb_scan_string(struct fb_info *fb_info, char speed, const char *s)
{
	char code *encode;
	char width;
	bool first = true;
	unsigned char start_column = 0, new_columns = 0, offset = 0;

	while (*s) {
		width = search_encode(s, &encode);
		/* skip invalid input */
		if (width < 0) {
			s += *encode;
			continue;
		}
		s += width == CHARACTER_WIDTH ? ENCODE_INDEX_SIZE : 1;
		offset += fb_set(offset, encode, width);
		new_columns += width;
		if (offset > FB_HALF_COLUMNS - CHARACTER_WIDTH && !first) {
			fb_info->offset = start_column;
			fb_scan(fb_info, new_columns + FB_HALF_COLUMNS,
				speed);
			start_column += new_columns;
			new_columns = 0;
		} else if (first &&
			   new_columns > FB_COLUMNS - CHARACTER_WIDTH) {
			fb_info->offset = start_column;
			fb_scan(fb_info, new_columns, speed);
			start_column += new_columns - FB_HALF_COLUMNS;
			new_columns = 0;
			first = false;
		}
	}

	fb_info->offset = start_column;
	if (first)
		new_columns -= FB_HALF_COLUMNS;
	fb_scan(fb_info, new_columns + FB_HALF_COLUMNS, speed);

	return start_column + new_columns;
}

unsigned char fb_set_string(unsigned char offset, const char *s)
{
	char code *encode;
	unsigned char n = 0;

	while (*s) {
		char width = search_encode(s, &encode);

		/* skip invalid input */
		if (width < 0) {
			s += *encode;
			continue;
		}
		s += width == CHARACTER_WIDTH ? ENCODE_INDEX_SIZE : 1;
		offset += fb_set(offset, encode, width);
		n += width;
	}

	return n;
}
