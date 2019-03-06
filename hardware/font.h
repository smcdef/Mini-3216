#ifndef __FONT_H__
#define __FONT_H__

#include "core.h"

#define NONE_ASCII              '\x7f'

#define CHARACTER_WIDTH		16
#define ASCII_WIDTH		6

#ifdef GB2312_ENCODE
#define ENCODE_INDEX_SIZE       2
#else
#define ENCODE_INDEX_SIZE       3
#endif

char search_encode(const char *index, char code **out);
void font_sort(void);

#endif
