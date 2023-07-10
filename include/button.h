#pragma once
#include "library.h"
#include "text.h"

//STATES
#define DEFAULT 0
#define HOVER 1
#define PRESS 2

//EVENTS
#define ENTERED 5
#define EXITED 6
#define RELEASED_OVER 7
#define RELEASED 8
#define PRESSED 9

typedef struct Button {
	//position and dimensions in screen coordinates
	double x, y;
	double width, height;
	//color format: RGBA where each channel is 8bit
	int fill_color;
	int stroke_color;

	uint8_t state;
	Text *text;
} Button;

//allocates a new button object and returns the pointer into the target pointer
//uses default values: fill = black, stroke = white
void button_default(Button **destination, double x, double y, double width, double height, char* text);

//allocates a new button object and returns the pointer into the target pointer
void button_construct(Button **destination, double x, double y, double width, double height, int fill_color, int stroke_color, char* text);

void button_destruct(Button* button);

//@SuppressWarnings("Deprecated")
void button_copy(Button *destination, Button *source);

void button_draw(Button *button);

//sets the position of the target button
void button_setpos(Button *button, double x, double y);
//sets the fill color of the target button
void button_setfillcolor(Button *button, int color);
//sets the text color of the target button
void button_settextcolor(Button *button, int color);

//sets the button state
//state	DEFAULT -> fill = black, stroke = white, text = white
//state	HOVER -> fill = gray, stroke = white, text = white
//state	PRESS -> fill = white, stroke = white, text = black
void button_state(Button *button, uint8_t state);

//returns true if the given position is within the rectangle of the button
boolean button_isover(Button *button, double x, double y);

