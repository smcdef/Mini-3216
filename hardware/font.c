#include "font.h"
#include <string.h>

struct ascii_code {
	char index[1];
	char width;
	char encode[16];
};

struct character_code {
	char index[ENCODE_INDEX_SIZE];
	char encode[32];
};

static code struct ascii_code ascii_tables[] = {
	{
		'0',
		ASCII_WIDTH,
		0xFE, 0x7F, 0xFF, 0xFF, 0x03, 0xC0,
		0x03, 0xC0, 0xFF, 0xFF, 0xFE, 0x7F,
	},
	{
		'1',
		ASCII_WIDTH,
		0x00, 0x00, 0x00, 0x00, 0x0C, 0xC0,
		0xFE, 0xFF, 0xFF, 0xFF, 0x00, 0xC0,
	},
	{
		'2',
		ASCII_WIDTH,
		0x0E, 0xF0, 0x0F, 0xFC, 0x03, 0xCF,
		0xC3, 0xC3, 0xFF, 0xC0, 0x3E, 0xC0,
	},
	{
		'3',
		ASCII_WIDTH,
		0x06, 0x60, 0x87, 0xE1, 0x83, 0xC1,
		0x83, 0xC1, 0xFF, 0xFF, 0xFE, 0x7F,
	},
	{
		'4',
		ASCII_WIDTH,
		0xFE, 0x00, 0xFF, 0x01, 0x80, 0x01,
		0x80, 0x01, 0xFF, 0xFF, 0xFE, 0xFF,
	},
	{
		'5',
		ASCII_WIDTH,
		0xFE, 0x60, 0xFF, 0xE1, 0x83, 0xC1,
		0x83, 0xC1, 0x83, 0xFF, 0x02, 0x7F,
	},
	{
		'6',
		ASCII_WIDTH,
		0xFE, 0x7F, 0xFF, 0xFF, 0x83, 0xC1,
		0x83, 0xC1, 0x83, 0xFF, 0x02, 0x7F,
	},
	{
		'7',
		ASCII_WIDTH,
		0x1E, 0x00, 0x1F, 0x00, 0x03, 0x00,
		0x03, 0x00, 0xFF, 0xFF, 0xFE, 0xFF,
	},
	{
		'8',
		ASCII_WIDTH,
		0xFE, 0x7F, 0xFF, 0xFF, 0x83, 0xC1,
		0x83, 0xC1, 0xFF, 0xFF, 0xFE, 0x7F,
	},
	{
		'9',
		ASCII_WIDTH,
		0xFE, 0x60, 0xFF, 0xE1, 0x83, 0xC1,
		0x83, 0xC1, 0xFF, 0xFF, 0xFE, 0x7F,
	},
	{
		' ',
		1,
		0x00,
		0x00,
	},
	{
		'-',
		ASCII_WIDTH,
		0x80, 0x01, 0x80, 0x01, 0x80, 0x01,
		0x80, 0x01, 0x80, 0x01, 0x80, 0x01,
	},
	{
		'.',
		2,
		0x00, 0x60,
		0x00, 0x60,
	},
	{
		'^',
		4,
		0x00, 0x00, 0x78, 0x3C,
		0x78, 0x3C, 0x00, 0x00,
	},
	{
		'c',
		8,
		0x10, 0x00, 0x28, 0x00, 0x10, 0x3F, 0x80, 0x7F,
		0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xE1, 0x80, 0x61,
	},
	{
		'v',
		4,
		0x00, 0x00, 0x3C, 0x78,
		0x3C, 0x78, 0x00, 0x00,
	},
	{
		NONE_ASCII,
		ASCII_WIDTH,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	},
};

static code struct character_code character_tables[] = {
	{
		{ "设" },
		0x00, 0x00, 0x20, 0x00, 0x23, 0x20, 0xEE, 0x7F,
		0xF4, 0x3F, 0xA0, 0x90, 0x40, 0x88, 0xE0, 0x40,
		0xBF, 0x61, 0x8E, 0x36, 0x82, 0x1C, 0x9E, 0x1C,
		0xBF, 0x37, 0xF2, 0x63, 0xB0, 0xC0, 0x10, 0x40,
	},
	{
		{ "置" },
		0x00, 0x00, 0x20, 0x80, 0x2F, 0x80, 0x2F, 0x80,
		0xE9, 0xFF, 0xA9, 0xFF, 0xAF, 0xAA, 0xFF, 0xAA,
		0xF9, 0xAA, 0xAF, 0xAA, 0xAF, 0xAA, 0xA9, 0xFF,
		0xE9, 0xFF, 0xAF, 0x80, 0x2F, 0xC0, 0x20, 0x80,
	},
	{
		{ "时" },
		0x00, 0x00, 0xFE, 0x3F, 0xFC, 0x1F, 0x44, 0x08,
		0x44, 0x08, 0xFC, 0x1F, 0xFE, 0x0F, 0x04, 0x00,
		0xD0, 0x00, 0x90, 0x07, 0x10, 0x43, 0x10, 0x40,
		0xFF, 0xFF, 0xFE, 0x7F, 0x10, 0x00, 0x10, 0x00,
	},
	{
		{ "间" },
		0x00, 0x00, 0xFC, 0xFF, 0xF9, 0x7F, 0x03, 0x00,
		0x06, 0x00, 0xF2, 0x1F, 0xE0, 0x0F, 0x22, 0x09,
		0x22, 0x09, 0xE2, 0x1F, 0xF2, 0x0F, 0x22, 0x40,
		0x02, 0x40, 0xFE, 0xFF, 0xFF, 0x7F, 0x02, 0x00,
	},
	{
		{ "年" },
		0x00, 0x00, 0x20, 0x04, 0x10, 0x04, 0x18, 0x04,
		0xEC, 0x07, 0xCF, 0x07, 0x4A, 0x04, 0x48, 0x04,
		0xF8, 0xFF, 0xF8, 0xFF, 0x48, 0x04, 0x48, 0x04,
		0x68, 0x04, 0x4C, 0x04, 0x08, 0x06, 0x00, 0x04,
	},
	{
		{ "月" },
		0x00, 0x00, 0x00, 0x80, 0x00, 0x40, 0x00, 0x70,
		0xFF, 0x3F, 0xFE, 0x0F, 0x22, 0x01, 0x22, 0x01,
		0x22, 0x01, 0x22, 0x01, 0x22, 0x41, 0x22, 0x41,
		0xFE, 0xFF, 0xFF, 0x7F, 0x02, 0x00, 0x00, 0x00,
	},
	{
		{ "日" },
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFF,
		0xFC, 0x7F, 0x84, 0x20, 0x84, 0x20, 0x84, 0x20,
		0x84, 0x20, 0x84, 0x20, 0x84, 0x20, 0x84, 0x20,
		0xFC, 0xFF, 0xFE, 0x7F, 0x04, 0x00, 0x00, 0x00,
	},
	{
		{ "分" },
		0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0xC0, 0x80,
		0xF0, 0x40, 0xBE, 0x70, 0x8F, 0x3F, 0x82, 0x0F,
		0x80, 0x20, 0x80, 0x40, 0x8F, 0xFF, 0xBE, 0x7F,
		0x70, 0x00, 0xE0, 0x00, 0xC0, 0x01, 0x80, 0x00,
	},
	{
		{ "完" },
		0x00, 0x00, 0x08, 0x80, 0x1C, 0x81, 0x0E, 0x41,
		0x24, 0x71, 0x24, 0x3F, 0x24, 0x0F, 0x25, 0x01,
		0x27, 0x01, 0x26, 0x7F, 0x24, 0xFF, 0x34, 0xC1,
		0x24, 0xC1, 0x9C, 0xC1, 0x0E, 0xF9, 0x04, 0x60,
	},
	{
		{ "成" },
		0x00, 0x00, 0x00, 0x80, 0x00, 0x70, 0xFC, 0x3F,
		0xF8, 0x0F, 0x88, 0x10, 0x88, 0x30, 0x88, 0x1F,
		0xC8, 0x8F, 0xBF, 0x40, 0xFE, 0x67, 0xC9, 0x3F,
		0x0A, 0x3C, 0x8E, 0x77, 0xEA, 0xC1, 0x48, 0xF0,
	},
	{
		{ "模" },
		0x00, 0x00, 0x10, 0x06, 0xD0, 0x01, 0xFF, 0xFF,
		0xFE, 0x7F, 0xD0, 0x80, 0x94, 0x89, 0xF4, 0x4B,
		0xEF, 0x6B, 0xAF, 0x3A, 0xA4, 0x1E, 0xA4, 0x0E,
		0xAF, 0x3A, 0xEF, 0x6B, 0xF4, 0xCB, 0x24, 0x48,
	},
	{
		{ "式" },
		0x00, 0x00, 0x00, 0x20, 0x90, 0x60, 0x90, 0x20,
		0x90, 0x3F, 0x90, 0x3F, 0x90, 0x10, 0xD0, 0x10,
		0x90, 0x08, 0xFF, 0x01, 0xFE, 0x07, 0x11, 0x1E,
		0x1E, 0x38, 0x14, 0x60, 0x10, 0xC0, 0x00, 0xF0,
	},
	{
		{ "与" },
		0x00, 0x00, 0x00, 0x08, 0x00, 0x08, 0x80, 0x08,
		0xFF, 0x09, 0xFE, 0x08, 0x88, 0x08, 0x88, 0x08,
		0x88, 0x08, 0x88, 0x08, 0x88, 0x0C, 0x88, 0x48,
		0x88, 0xC0, 0x8C, 0x7F, 0xC8, 0x3F, 0x80, 0x00,
	},
	{
		{ "温" },
		0x00, 0x00, 0x10, 0x04, 0xE1, 0x78, 0x4E, 0xFE,
		0x84, 0x63, 0x60, 0x40, 0x00, 0x7F, 0xFF, 0x7E,
		0x7E, 0x42, 0x4A, 0x7E, 0x4A, 0x42, 0x4A, 0x7E,
		0xFE, 0x42, 0x7F, 0x7E, 0x02, 0x7F, 0x00, 0x42,
	},
	{
		{ "期" },
		0x00, 0x00, 0x00, 0x88, 0x04, 0x68, 0xFF, 0x3F,
		0xFE, 0x1F, 0x24, 0x09, 0x24, 0x19, 0xFF, 0x7F,
		0xFE, 0xAF, 0x04, 0x68, 0xFE, 0x3F, 0xFC, 0x1F,
		0x44, 0x42, 0x44, 0x42, 0xFC, 0xFF, 0xFE, 0x7F,
	},
	{
		{ "退" },
		0x00, 0x00, 0x40, 0x40, 0x46, 0xE0, 0xDC, 0x3F,
		0xE8, 0x1F, 0x40, 0x30, 0x00, 0x68, 0xFF, 0xDF,
		0xFE, 0xCF, 0xAA, 0xC8, 0x2A, 0xC5, 0x2A, 0xC3,
		0x7E, 0xC7, 0xBF, 0xDD, 0x02, 0xC9, 0x00, 0x40,
	},
	{
		{ "出" },
		0x00, 0x00, 0x00, 0xFE, 0xFE, 0x7C, 0x7C, 0x40,
		0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0xFF, 0x7F,
		0xFE, 0x7F, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
		0xFE, 0x40, 0x7C, 0xFE, 0x00, 0x7C, 0x00, 0x00,
	},
	{
		{ "报" },
		0x00, 0x00, 0x10, 0x0C, 0x10, 0x46, 0xFF, 0xFF,
		0xFE, 0x7F, 0x10, 0x01, 0x90, 0x00, 0xFF, 0xFF,
		0xFE, 0x7F, 0x82, 0x83, 0x82, 0x6C, 0x92, 0x18,
		0xBE, 0x37, 0xDF, 0x63, 0x82, 0xC0, 0x00, 0x40,
	},
	{
		{ "始" },
		0x00, 0x00, 0x20, 0x80, 0x20, 0x63, 0xFF, 0x37,
		0x2E, 0x0E, 0xE0, 0x7B, 0xF0, 0x30, 0x20, 0x00,
		0x70, 0xFF, 0x38, 0x7E, 0x2E, 0x42, 0x27, 0x42,
		0x22, 0x42, 0x38, 0xFE, 0x70, 0x7F, 0x20, 0x02,
	},
	{
		{ "结" },
		0x00, 0x00, 0x60, 0x22, 0x78, 0x67, 0xDE, 0x23,
		0xE7, 0x32, 0x72, 0x12, 0x28, 0x08, 0x48, 0xFF,
		0x48, 0x7E, 0x48, 0x42, 0x7F, 0x42, 0x7E, 0x42,
		0x48, 0x42, 0x68, 0xFE, 0x4C, 0x7F, 0x08, 0x02,
	},
	{
		{ "束" },
		0x00, 0x00, 0x04, 0x80, 0x04, 0x40, 0xF4, 0x63,
		0xE4, 0x31, 0x24, 0x1D, 0x24, 0x07, 0xFF, 0xFF,
		0xFF, 0xFF, 0x24, 0x05, 0x24, 0x19, 0xE4, 0x33,
		0xF4, 0x61, 0x26, 0x60, 0x04, 0xC0, 0x00, 0x40,
	},
	{
		{ "打" },
		0x00, 0x00, 0x10, 0x0C, 0x10, 0x46, 0xFF, 0xFF,
		0xFE, 0x7F, 0x10, 0x01, 0x90, 0x00, 0x08, 0x00,
		0x08, 0x00, 0x08, 0x20, 0x08, 0x40, 0xF8, 0xFF,
		0xF8, 0x7F, 0x08, 0x00, 0x0C, 0x00, 0x08, 0x00,
	},
	{
		{ "开" },
		0x00, 0x00, 0x40, 0x00, 0x40, 0x80, 0x42, 0x40,
		0x42, 0x70, 0xFE, 0x3F, 0xFE, 0x0F, 0x42, 0x00,
		0x42, 0x00, 0x42, 0x00, 0xFE, 0xFF, 0xFE, 0x7F,
		0x43, 0x00, 0x42, 0x00, 0x60, 0x00, 0x40, 0x00,
	},
	{
		{ "关" },
		0x00, 0x00, 0x00, 0x80, 0x00, 0x41, 0x10, 0x61,
		0x11, 0x31, 0x16, 0x19, 0x1E, 0x0F, 0xF4, 0x07,
		0xF0, 0x03, 0x18, 0x07, 0x1E, 0x1D, 0x17, 0x39,
		0x1A, 0x71, 0x90, 0xE1, 0x00, 0xC1, 0x00, 0x40,
	},
	{
		{ "闭" },
		0x00, 0x00, 0xFC, 0xFF, 0xFD, 0xFF, 0x03, 0x10,
		0x27, 0x08, 0x22, 0x06, 0xA0, 0x13, 0xE4, 0x20,
		0xF4, 0x7F, 0xF4, 0x3F, 0x24, 0x00, 0x24, 0x20,
		0x04, 0x40, 0xFC, 0xFF, 0xFE, 0x7F, 0x04, 0x00,
	},
};

static struct character_code code *xdata
	character_sort_entry[ARRAY_SIZE(character_tables)];

static char search_ascii_encode(const char *index, char code **out)
{
	char low = 10, high = ARRAY_SIZE(ascii_tables) - 1;

	if (index[0] <= '9' && index[0] >= '0') {
		*out = ascii_tables[index[0] - '0'].encode;
		return ascii_tables[index[0] - '0'].width;
	}

	/* binary search */
	while (high >= low) {
		char mid = low + ((high - low) >> 1);

		if (ascii_tables[mid].index[0] > *index)
			high = mid - 1;
		else if (ascii_tables[mid].index[0] < *index)
			low = mid + 1;
		else {
			*out = ascii_tables[mid].encode;
			return ascii_tables[mid].width;
		}
	}
	*out = (void *)sizeof(ascii_tables[0].index);

	return -1;
}

static char search_character_encode(const char *index, char code **out)
{
	char low = 0, high = ARRAY_SIZE(character_sort_entry) - 1;
	while(high >= low)
	{
		char mid = low + ((high -low) >> 1);
		int result = memcmp(index, character_sort_entry[mid]->index,
			sizeof(character_sort_entry[mid]->index));
		if (result > 0)
		{
			low = mid + 1;
		} else if(result < 0)
		{
			high = mid - 1;
		} else
		{
			*out = character_sort_entry[mid]->encode;
			return CHARACTER_WIDTH;
		}
	}
	*out = (void *)sizeof(character_tables[0].index);
	return -1;
}

void font_sort(void)
{
	struct character_code code *xdata encode = character_tables;
	struct character_code code *xdata *xdata entry = character_sort_entry;

	while (encode != character_tables + ARRAY_SIZE(character_tables))
		*entry++ = encode++;
	int i,preIndex;
	for(i = 1; i < ARRAY_SIZE(character_tables); i++)
	{
		preIndex = i -1;
		struct character_code code *xdata current
						= character_sort_entry[i];
		while(preIndex >= 0 &&
		memcmp(character_sort_entry[prIndex]->index, current->index,
			sizeof(character_sort_entry[0]->index)) > 0)
		{
			character_sort_entry[preIndex + 1] =
				character_sort_entry[preIndex];
			--preIndex;
		}
		character_sort_entry[preIndex + 1] = current;
	}

}

char search_encode(const char *index, char code **out)
{
	if (index[0] & 0x80)
		return search_character_encode(index, out);

	return search_ascii_encode(index, out);
}
