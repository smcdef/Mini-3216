#include "fb_core.h"
#include "font.h"
#include "fb_mem.h"

unsigned int fb_scan_string(struct fb_info *fb_info, unsigned char speed,
			    const char *s)
{
	char code *encode;
	unsigned int new_columns = 0, offset = fb_info->offset;

	while (*s) {
		char width = search_encode(s, &encode);

		/* skip invalid input */
		if (width < 0) {
			s += (char)encode;
			continue;
		}
		s += width == CHARACTER_WIDTH ? ENCODE_INDEX_SIZE : 1;
		offset += fb_copy(offset, encode, width);
		new_columns += width;
		if (new_columns > FB_COLUMNS - CHARACTER_WIDTH) {
			fb_info->offset = fb_scan(fb_info, new_columns, speed);
			new_columns = MATRIXS_COLUMNS;
		}
	}

	return fb_scan(fb_info, new_columns, speed);
}

unsigned int fb_copy_string(unsigned int offset, const char *s)
{
	char code *encode;
	unsigned int offset_old = offset;

	while (*s) {
		char width = search_encode(s, &encode);

		/* skip invalid input */
		if (width < 0) {
			s += (char)encode;
			continue;
		}
		s += width == CHARACTER_WIDTH ? ENCODE_INDEX_SIZE : 1;
		offset += fb_copy(offset, encode, width);
	}

	return offset - offset_old;
}
