#pragma once
#include "library.h"
//the Text structure
/*
	Text is stored in a two dimensional array
	where each pixel is based on the value of
	element
	an element is 8bit, and represents a single
	pixel, this would allow storing basic images
	up to 256 colors
*/

//the Character
/*
	character is stored in an array
	where each pixel is based on the value of
	element
	an a element is 8bit, and each bit represents a single
	pixel, hence the maximum width of a character is 8pixel

	each characters first byte defines how wide and how
	long the character will be
	0x57 would specify a character with the width of 5pixels
	and the height of 7pixels
	and therefore the format must be as follows:
	0bXXXXX, 0bXXXXX, ...
*/

extern const uint8_t CHARDEF_QSTN[];

typedef struct Text {
	double x, y;			//position of text
	double pixel_size;		//size of a single pixel
	uint8_t *pixels;		//pointer to array of pixels
	int width;				//amount of horizontal pixels
	int height;				//amount of vertical pixels
	int color;				//color of text
} Text;

//returns the pointer of the character array representing the input character
const uint8_t* char_get(char c);
//returns the width of the given character in pixels
uint8_t char_getwidth(const uint8_t *c);
//returns the height of the given character in pixels
uint8_t char_getheight(const uint8_t *c);

uint8_t text_getdimensions(Text *text, double *w, double *h);

void text_draw(Text *text);

//allocates a new text object and puts it's pointer  into the target pointer
void text_construct(Text **destination, double x, double y, double size, char* text);

void text_destruct(Text* text);
//allocates a 2D array for the text object
void text_settext(Text *destination, char* text);
//sets the color of the text object
void text_setcolor(Text *destination, int color);
//sets the position of the target text
void text_setpos(Text *destination, double x, double y);
//sets the text size
void text_setpixelsize(Text *destination, double a);