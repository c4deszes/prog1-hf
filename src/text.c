#include "stdafx.h"
#include "text.h"

//Character definitions
const uint8_t CHARDEF_NULL[] = { 0x57, 0b11111, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b11111 };
const uint8_t CHARDEF_SPACE[] = { 0x57, 0, 0, 0, 0, 0, 0, 0 };

//Numbers
const uint8_t CHARDEF_0[] = { 0x57, 0b01110, 0b10001, 0b10011, 0b10101, 0b11001, 0b10001, 0b01110 };
const uint8_t CHARDEF_1[] = { 0x57, 0b00100, 0b01100, 0b00100, 0b00100, 0b00100, 0b00100, 0b11111 };
const uint8_t CHARDEF_2[] = { 0x57, 0b01110, 0b10001, 0b00001, 0b00110, 0b01000, 0b10000, 0b11111 };
const uint8_t CHARDEF_3[] = { 0x57, 0b01110, 0b10001, 0b00001, 0b00110, 0b00001, 0b10001, 0b01110 };
const uint8_t CHARDEF_4[] = { 0x57, 0b00011, 0b00101, 0b01001, 0b10001, 0b11111, 0b00001, 0b00001 };
const uint8_t CHARDEF_5[] = { 0x57, 0b11111, 0b10000, 0b11110, 0b00001, 0b00001, 0b10001, 0b01110 };
const uint8_t CHARDEF_6[] = { 0x57, 0b00110, 0b01000, 0b10000, 0b11110, 0b10001, 0b10001, 0b01110 };
const uint8_t CHARDEF_7[] = { 0x57, 0b11111, 0b10001, 0b00001, 0b00010, 0b00100, 0b00100, 0b00100 };
const uint8_t CHARDEF_8[] = { 0x57, 0b01110, 0b10001, 0b10001, 0b01110, 0b10001, 0b10001, 0b01110 };
const uint8_t CHARDEF_9[] = { 0x57, 0b01110, 0b10001, 0b10001, 0b01111, 0b00001, 0b00010, 0b01100 };

//Letters
const uint8_t CHARDEF_A[] = { 0x57, 
							  0b01110, 
							  0b10001,
							  0b10001, 
							  0b11111, 
							  0b10001, 
							  0b10001, 
							  0b10001 };
const uint8_t CHARDEF_B[] = { 0x57, 0b11110, 0b10001, 0b10001, 0b11110, 0b10001, 0b10001, 0b11110 };
const uint8_t CHARDEF_C[] = { 0x57, 0b01110, 0b10001, 0b10000, 0b10000, 0b10000, 0b10001, 0b01110 };
const uint8_t CHARDEF_D[] = { 0x57, 0b11110, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b11110 };
const uint8_t CHARDEF_E[] = { 0x57, 0b11111, 0b10000, 0b10000, 0b11110, 0b10000, 0b10000, 0b11111 };
const uint8_t CHARDEF_F[] = { 0x57, 0b11111, 0b10000, 0b10000, 0b11110, 0b10000, 0b10000, 0b10000 };
const uint8_t CHARDEF_G[] = { 0x57, 0b01110, 0b10001, 0b10000, 0b10011, 0b10001, 0b10001, 0b01110 };
const uint8_t CHARDEF_H[] = { 0x57, 0b10001, 0b10001, 0b10001, 0b11111, 0b10001, 0b10001, 0b10001 };
const uint8_t CHARDEF_I[] = { 0x17, 1, 1, 1, 1, 1, 1, 1 };
const uint8_t CHARDEF_J[] = { 0x57, 0b00001, 0b00001, 0b00001, 0b00001, 0b00001, 0b10001, 0b01110 };
const uint8_t CHARDEF_K[] = { 0x57, 0b10001, 0b10010, 0b10100, 0b11000, 0b10100, 0b10010, 0b10001 };
const uint8_t CHARDEF_L[] = { 0x57, 0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b11111 };
const uint8_t CHARDEF_M[] = { 0x57, 0b10001, 0b11011, 0b10101, 0b10001, 0b10001, 0b10001, 0b10001 };
const uint8_t CHARDEF_N[] = { 0x57, 0b10001, 0b11001, 0b10101, 0b10011, 0b10001, 0b10001, 0b10001 };
const uint8_t CHARDEF_O[] = { 0x57, 0b01110, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01110 };
const uint8_t CHARDEF_P[] = { 0x57, 0b11110, 0b10001, 0b10001, 0b11110, 0b10000, 0b10000, 0b10000 };
const uint8_t CHARDEF_Q[] = { 0x57, 0b01110, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01110 };
const uint8_t CHARDEF_R[] = { 0x57, 0b11110, 0b10001, 0b10001, 0b11110, 0b10001, 0b10001, 0b10001 };
const uint8_t CHARDEF_S[] = { 0x57, 0b01111, 0b10000, 0b10000, 0b01110, 0b00001, 0b00001, 0b11110 };
const uint8_t CHARDEF_T[] = { 0x57, 0b11111, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100 };
const uint8_t CHARDEF_U[] = { 0x57, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01110 };
const uint8_t CHARDEF_V[] = { 0x57, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01010, 0b00100 };
const uint8_t CHARDEF_W[] = { 0x57, 0b11110, 0b10001, 0b10001, 0b11110, 0b10001, 0b10001, 0b10001 };
const uint8_t CHARDEF_X[] = { 0x57, 0b10001, 0b10001, 0b01010, 0b00100, 0b01010, 0b10001, 0b10001 };
const uint8_t CHARDEF_Y[] = { 0x57, 0b10001, 0b10001, 0b01010, 0b00100, 0b00100, 0b00100, 0b00100 };
const uint8_t CHARDEF_Z[] = { 0x57, 0b11111, 0b00001, 0b00010, 0b00100, 0b01000, 0b10000, 0b11111 };

//Special characters
const uint8_t CHARDEF_EXCL[] = { 0x17, 1, 1, 1, 1, 1, 0, 1 };
const uint8_t CHARDEF_QSTN[] = { 0x57, 0b01110, 0b10001, 0b00001, 0b00010, 0b00100, 0b00000, 0b00100 };
const uint8_t CHARDEF_PERD[] = { 0x17, 0, 0, 0, 0, 0, 0, 1 };
const uint8_t CHARDEF_COLN[] = { 0x17, 0, 0, 1, 0, 0, 1, 0 };
const uint8_t CHARDEF_LINE[] = { 0x37, 0b000, 0b000, 0b000, 0b111, 0b000, 0b000, 0b000 };
const uint8_t CHARDEF_HIGH[] = { 0x37, 0b000, 0b100, 0b010, 0b001, 0b010, 0b100, 0b000 };
const uint8_t CHARDEF_LOWE[] = { 0x37, 0b000, 0b001, 0b010, 0b100, 0b010, 0b001, 0b000 };
const uint8_t CHARDEF_UNSC[] = { 0x37, 0b000, 0b000, 0b000, 0b000, 0b000, 0b000, 0b111 };

const uint8_t* char_get(char c) {
	switch (tolower(c)) {
		case '0': return CHARDEF_0;
		case '1': return CHARDEF_1;
		case '2': return CHARDEF_2;
		case '3': return CHARDEF_3;
		case '4': return CHARDEF_4;
		case '5': return CHARDEF_5;
		case '6': return CHARDEF_6;
		case '7': return CHARDEF_7;
		case '8': return CHARDEF_8;
		case '9': return CHARDEF_9;

		case ' ': return CHARDEF_SPACE;
		case '!': return CHARDEF_EXCL;
		case '?': return CHARDEF_QSTN;
		case '.': return CHARDEF_PERD;
		case ':': return CHARDEF_COLN;
		case '-': return CHARDEF_LINE;
		case '>': return CHARDEF_HIGH;
		case '<': return CHARDEF_LOWE;
		case '_': return CHARDEF_UNSC;
		

		case 'a': return CHARDEF_A;
		case 'b': return CHARDEF_B;
		case 'c': return CHARDEF_C;
		case 'd': return CHARDEF_D;
		case 'e': return CHARDEF_E;
		case 'f': return CHARDEF_F;
		case 'g': return CHARDEF_G;
		case 'h': return CHARDEF_H;
		case 'i': return CHARDEF_I;
		case 'j': return CHARDEF_J;
		case 'k': return CHARDEF_K;
		case 'l': return CHARDEF_L;
		case 'm': return CHARDEF_M;
		case 'n': return CHARDEF_N;
		case 'o': return CHARDEF_O;
		case 'p': return CHARDEF_P;
		case 'q': return CHARDEF_Q;
		case 'r': return CHARDEF_R;
		case 's': return CHARDEF_S;
		case 't': return CHARDEF_T;
		case 'u': return CHARDEF_U;
		case 'v': return CHARDEF_V;
		case 'w': return CHARDEF_W;
		case 'x': return CHARDEF_X;
		case 'y': return CHARDEF_Y;
		case 'z': return CHARDEF_Z;
	}
	return CHARDEF_NULL;
}

uint8_t char_getwidth(const uint8_t *c) {
	return ((*c)>>4) & 0x0F;
}

uint8_t char_getheight(const uint8_t *c) {
	return (*c) & 0x0F;
}

uint8_t text_getdimensions(Text *text, double *w, double *h) {
	if (text == NULL) {
		*w = 0;
		*h = 0;
		return 0;
	}
	*w = text->width * text->pixel_size;
	*h = text->height * text->pixel_size;
	return 1;
}

void text_setpos(Text *destination, double x, double y) {
	if (destination == NULL) { return; }

	destination->x = x;
	destination->y = y;
}

void text_setpixelsize(Text *destination, double a) {
	if (!destination) { return; }

	destination->pixel_size = a;
}

void text_setcolor(Text *destination, int color) {
	if (destination == NULL) { return; }

	destination->color = color;
}

void text_settext(Text* destination, char* text) {
	if (destination == NULL) { return; }
	
	if (destination->pixels != NULL) {
		free(destination->pixels);
	}

	destination->width = 0;
	destination->height = 0;

	for (int i = 0; text[i] != '\0'; i++) {
		if (char_getheight(char_get(text[i])) > destination->height) {
			destination->height = char_getheight(char_get(text[i]));
		}
		destination->width += char_getwidth(char_get(text[i])) + 1;
	}
	destination->width--;

	int w = destination->width;
	int h = destination->height;

	if (w != 0 && h != 0) {
		destination->pixels = (uint8_t*)malloc(w * h * sizeof(uint8_t));
	}
	else {
		destination->pixels = (uint8_t*)malloc(sizeof(uint8_t));
	}

	uint8_t *pt = destination->pixels;
	for (int i = 0; i < w*h; i++) {
		*pt = 0;
		pt++;
	}

	int y1 = 0;
	int x1 = 0;

	for (int i = 0; text[i] != '\0'; i++) {
		int width = char_getwidth(char_get(text[i]));
		int height = char_getheight(char_get(text[i]));

		for (int n = 0; n < height; n++) {
			uint8_t line = *(char_get(text[i]) + 1 + n);
			for (int m = width - 1; m >= 0; m--) {
				if (line & (1 << m)) {
					*(destination->pixels + (y1 + n) * w + (x1 + width - 1 - m)) = 1;
				}
				else {
					*(destination->pixels + (y1 + n) * w + (x1 + width - 1 - m)) = 0;
				}
			}
		}
		x1 += width + 1;
	}
}

void text_construct(Text **destination, double x, double y, double size, char* text) {
	if (destination == NULL) { return; }

	*destination = (Text*)malloc(sizeof(Text));

	if (*destination == NULL) {
		//shit happens
		exit(0xDEADBEEF);
	}

	(*destination)->x = x;
	(*destination)->y = y;
	(*destination)->pixel_size = size / 3;
	(*destination)->color = WHITE;
	(*destination)->pixels = NULL;

	text_settext(*destination, text);
}

void text_destruct(Text* text) {
	if (text == NULL) { return; }
	if (text->pixels != NULL) {
		free(text->pixels);
	}
	free(text);
}

void text_draw(Text *text) {
	if (text == NULL) { return; }

	glBegin(GL_QUADS);
	select_color(text->color);

	uint8_t *element = text->pixels;

	for (int y = 0; y < text->height; y++) {
		for (int x = 0; x < text->width; x++) {
			if (*element) {
				glVertex2d(text->x + x * text->pixel_size, text->y - y * text->pixel_size);
				glVertex2d(text->x + (x + 1) * text->pixel_size, text->y - y * text->pixel_size);
				glVertex2d(text->x + (x + 1) * text->pixel_size, text->y - (y + 1) * text->pixel_size);
				glVertex2d(text->x + x * text->pixel_size, text->y - (y + 1) * text->pixel_size);
			}
			element++;
		}	
	}
	glEnd();
}
