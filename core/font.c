/**
 * Internal font API.
 *
 * Copyright (c) 2019-2020  smcdef.
 *
 * Author: songmuchun <smcdef@163.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "font.h"
#include <string.h>

struct ascii_code {
	char index[1];
	char width;
	unsigned char encode[16];
};

struct character_code {
	char index[ENCODE_INDEX_SIZE + 1];
	unsigned char encode[32];
};

static code struct ascii_code ascii_tables[] = {
	{
		{ '0' },
		ASCII_WIDTH,
		{
			0xFE, 0x7F, 0xFF, 0xFF, 0x03, 0xC0,
			0x03, 0xC0, 0xFF, 0xFF, 0xFE, 0x7F,
		},
	},
	{
		{ '1' },
		ASCII_WIDTH,
		{
			0x00, 0x00, 0x00, 0x00, 0x0C, 0xC0,
			0xFE, 0xFF, 0xFF, 0xFF, 0x00, 0xC0,
		},
	},
	{
		{ '2' },
		ASCII_WIDTH,
		{
			0x0E, 0xF0, 0x0F, 0xFC, 0x03, 0xCF,
			0xC3, 0xC3, 0xFF, 0xC0, 0x3E, 0xC0,
		},
	},
	{
		{ '3' },
		ASCII_WIDTH,
		{
			0x06, 0x60, 0x87, 0xE1, 0x83, 0xC1,
			0x83, 0xC1, 0xFF, 0xFF, 0xFE, 0x7F,
		},
	},
	{
		{ '4' },
		ASCII_WIDTH,
		{
			0xFE, 0x00, 0xFF, 0x01, 0x80, 0x01,
			0x80, 0x01, 0xFF, 0xFF, 0xFE, 0xFF,
		},
	},
	{
		{ '5' },
		ASCII_WIDTH,
		{
			0xFE, 0x60, 0xFF, 0xE1, 0x83, 0xC1,
			0x83, 0xC1, 0x83, 0xFF, 0x02, 0x7F,
		},
	},
	{
		{ '6' },
		ASCII_WIDTH,
		{
			0xFE, 0x7F, 0xFF, 0xFF, 0x83, 0xC1,
			0x83, 0xC1, 0x83, 0xFF, 0x02, 0x7F,
		},
	},
	{
		{ '7' },
		ASCII_WIDTH,
		{
			0x1E, 0x00, 0x1F, 0x00, 0x03, 0x00,
			0x03, 0x00, 0xFF, 0xFF, 0xFE, 0xFF,
		},
	},
	{
		{ '8' },
		ASCII_WIDTH,
		{
			0xFE, 0x7F, 0xFF, 0xFF, 0x83, 0xC1,
			0x83, 0xC1, 0xFF, 0xFF, 0xFE, 0x7F,
		},
	},
	{
		{ '9' },
		ASCII_WIDTH,
		{
			0xFE, 0x60, 0xFF, 0xE1, 0x83, 0xC1,
			0x83, 0xC1, 0xFF, 0xFF, 0xFE, 0x7F,
		},
	},
	{
		{ ' ' },
		1,
		{
			0x00,
			0x00,
		},
	},
	{
		{ '-' },
		ASCII_WIDTH,
		{
			0x80, 0x01, 0x80, 0x01, 0x80, 0x01,
			0x80, 0x01, 0x80, 0x01, 0x80, 0x01,
		},
	},
	{
		{ '.' },
		2,
		{
			0x00, 0x60,
			0x00, 0x60,
		}
	},
	{
		{ '^' },
		4,
		{
			0x00, 0x00, 0x78, 0x3C,
			0x78, 0x3C, 0x00, 0x00,
		}
	},
	{
		{ 'c' },
		8,
		{
			0x10, 0x00, 0x28, 0x00, 0x10, 0x3F, 0x80, 0x7F,
			0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xE1, 0x80, 0x61,
		}
	},
	{
		{ 'v' },
		4,
		{
			0x00, 0x00, 0x3C, 0x78,
			0x3C, 0x78, 0x00, 0x00,
		}
	},
	{
		{ NONE_ASCII },
		ASCII_WIDTH,
		{
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		},
	},
};

static code struct character_code character_tables[] = {
	{
		{ "设" },
		{
			0x00, 0x00, 0x20, 0x00, 0x23, 0x20, 0xEE, 0x7F,
			0xF4, 0x3F, 0xA0, 0x90, 0x40, 0x88, 0xE0, 0x40,
			0xBF, 0x61, 0x8E, 0x36, 0x82, 0x1C, 0x9E, 0x1C,
			0xBF, 0x37, 0xF2, 0x63, 0xB0, 0xC0, 0x10, 0x40,
		},
	},
	{
		{ "置" },
		{
			0x00, 0x00, 0x20, 0x80, 0x2F, 0x80, 0x2F, 0x80,
			0xE9, 0xFF, 0xA9, 0xFF, 0xAF, 0xAA, 0xFF, 0xAA,
			0xF9, 0xAA, 0xAF, 0xAA, 0xAF, 0xAA, 0xA9, 0xFF,
			0xE9, 0xFF, 0xAF, 0x80, 0x2F, 0xC0, 0x20, 0x80,
		},
	},
	{
		{ "时" },
		{
			0x00, 0x00, 0xFE, 0x3F, 0xFC, 0x1F, 0x44, 0x08,
			0x44, 0x08, 0xFC, 0x1F, 0xFE, 0x0F, 0x04, 0x00,
			0xD0, 0x00, 0x90, 0x07, 0x10, 0x43, 0x10, 0x40,
			0xFF, 0xFF, 0xFE, 0x7F, 0x10, 0x00, 0x10, 0x00,
		},
	},
	{
		{ "间" },
		{
			0x00, 0x00, 0xFC, 0xFF, 0xF9, 0x7F, 0x03, 0x00,
			0x06, 0x00, 0xF2, 0x1F, 0xE0, 0x0F, 0x22, 0x09,
			0x22, 0x09, 0xE2, 0x1F, 0xF2, 0x0F, 0x22, 0x40,
			0x02, 0x40, 0xFE, 0xFF, 0xFF, 0x7F, 0x02, 0x00,
		},
	},
	{
		{ "分" },
		{
			0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0xC0, 0x80,
			0xF0, 0x40, 0xBE, 0x70, 0x8F, 0x3F, 0x82, 0x0F,
			0x80, 0x20, 0x80, 0x40, 0x8F, 0xFF, 0xBE, 0x7F,
			0x70, 0x00, 0xE0, 0x00, 0xC0, 0x01, 0x80, 0x00,
		},
	},
};

static struct character_code code *xdata
	character_sort_entry[ARRAY_SIZE(character_tables)];

static signed char search_ascii_encode(const char *index, char code **out)
{
	char low = 10, high = ARRAY_SIZE(ascii_tables) - 1;

	if (index[0] <= '9' && index[0] >= '0') {
		*out = ascii_tables[index[0] - '0'].encode;
		return ascii_tables[index[0] - '0'].width;
	}

	/* binary search */
	while (high >= low) {
		char mid = low + ((high - low) >> 1);

		if (ascii_tables[mid].index[0] > *index) {
			high = mid - 1;
		} else if (ascii_tables[mid].index[0] < *index) {
			low = mid + 1;
		} else {
			*out = ascii_tables[mid].encode;
			return ascii_tables[mid].width;
		}
	}
	*out = (void code *)sizeof(ascii_tables[0].index);

	return -1;
}

static signed char search_character_encode(const char *index, char code **out)
{
	unsigned char low = 0, high = ARRAY_SIZE(character_sort_entry) - 1;

	while (high >= low) {
		unsigned char mid = low + ((high - low) >> 1);
		signed char result = memcmp(index,
					    character_sort_entry[mid]->index,
					    ENCODE_INDEX_SIZE);

		if (result > 0) {
			low = mid + 1;
		} else if (result < 0) {
			high = mid - 1;
		} else {
			*out = character_sort_entry[mid]->encode;
			return CHARACTER_WIDTH;
		}
	}
	*out = (void code *)ENCODE_INDEX_SIZE;

	return -1;
}

void font_sort(void)
{
	int i, j;
	struct character_code code *encode = character_tables;
	struct character_code code *xdata *entry = character_sort_entry;

	while (encode != character_tables + ARRAY_SIZE(character_tables))
		*entry++ = encode++;

	for (i = 1; i < ARRAY_SIZE(character_tables); i++) {
		struct character_code code *current = character_sort_entry[i];

		j = i - 1;
		while (j >= 0 &&
		       memcmp(character_sort_entry[j]->index, current->index,
			      ENCODE_INDEX_SIZE) > 0) {
			character_sort_entry[j + 1] = character_sort_entry[j];
			--j;
		}
		character_sort_entry[j + 1] = current;
	}
}

signed char search_encode(const char *index, char code **out)
{
	if (index[0] & 0x80)
		return search_character_encode(index, out);

	return search_ascii_encode(index, out);
}
