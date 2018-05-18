
/*
Copyright (c) 2018 Ian Faust (icfaust@gmail.com)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/


/* Register Address Map */

#define MAX_NOOP   0x00
#define MAX_DIG0   0x01
#define MAX_DIG1   0x02
#define MAX_DIG2   0x03
#define MAX_DIG3   0x04
#define MAX_DIG4   0x05
#define MAX_DIG5   0x06
#define MAX_DIG6   0x07
#define MAX_DIG7   0x08
#define MAX_DECO   0x09
#define MAX_INTS   0x0A
#define MAX_SCAN   0x0B
#define MAX_SHUT   0x0C
#define MAX_DISP   0x0F
#define MAX_S      0x5B
#define MAX_H      0x17
#define MAX_O      0x1D
#define MAX_T      0x0F

static unsigned char maxnums[10] = {0x7E,0x30,0x6D,0x79,0x33,0x5B,0x5F,0x70,0x7F,0x7B};

void ICACHE_FLASH_ATTR bit_bang_SPI(int val);

void ICACHE_FLASH_ATTR printf_max(int word1, int word2);

void ICACHE_FLASH_ATTR max_init(void *args);

void ICACHE_FLASH_ATTR clear_max();

//void ICACHE_FLASH_ATTR intensity(int val);

void ICACHE_FLASH_ATTR disp_shot_text();

void ICACHE_FLASH_ATTR printf_max_int(int val);
