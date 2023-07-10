#include "stdafx.h"
#include "button.h"

void button_draw(Button *button) {
	if (button == NULL) { return; }

	double x, y, width, height;
	x = button->x;
	y = button->y;
	width = button->width;
	height = button->height;

	select_color(button->fill_color);
	glBegin(GL_QUADS);
	glVertex2d(x, y);
	glVertex2d(x + width, y);
	glVertex2d(x + width, y - height);
	glVertex2d(x, y - height);
	glEnd();

	select_color(button->stroke_color);
	glBegin(GL_LINE_LOOP);
	glVertex2d(x, y);
	glVertex2d(x + width, y);
	glVertex2d(x + width, y - height);
	glVertex2d(x, y - height);
	glEnd();

	text_draw(button->text);
}

void button_default(Button **destination, double x, double y, double width, double height, char* text) {
	button_construct(destination, x, y, width, height, BLACK, WHITE, text);
}

void button_construct(Button **destination, double x, double y, double width, double height, int fill_color, int stroke_color, char* text) {
	if (*destination == NULL) {
		(*destination) = (Button*)malloc(sizeof(Button));
	}

	if (*destination == NULL) {
		//shit happens
		exit(0xDEADBEEF);
	}

	(*destination)->x = x;
	(*destination)->y = y;
	(*destination)->width = width;
	(*destination)->height = height;
	(*destination)->fill_color = fill_color;
	(*destination)->stroke_color = stroke_color;
	(*destination)->text = NULL;

	double w, h;
	text_construct(&((*destination)->text),0, 0, 7, text);
	text_getdimensions((*destination)->text, &w, &h);
	text_setpos((*destination)->text, x + width / 2 - w / 2, y - height / 2 + h / 2);
}

void button_destruct(Button* button) {
	if (button == NULL) { return; }

	text_destruct(button->text);
	free(button);
}

void button_copy(Button *destination, Button *source) {
	if (destination == NULL) { return; }

	destination = source;
}

void button_setpos(Button *button, double x, double y) {
	if (button == NULL) { return; }

	button->x = x;
	button->y = y;
}

void button_setfillcolor(Button *button, int color) {
	if (button == NULL) { return; }

	button->fill_color = color;
}

void button_settextcolor(Button *button, int color) {
	if (button == NULL) { return; }

	button->text->color = color;
}

void button_state(Button *button, uint8_t state) {
	if (button == NULL) { return; }

	switch (state) {
		case DEFAULT: button_setfillcolor(button, BLACK); button_settextcolor(button, WHITE);  break;
		case HOVER:; button_setfillcolor(button, 0x404040FF); break;
		case PRESS:; button_setfillcolor(button, WHITE); button_settextcolor(button, BLACK); break;
	}
	button->state = state;
}

boolean button_isover(Button *button, double x, double y) {
	if (button == NULL) { return false; }

	if (x >= button->x && x <= button->x + button->width) {
		if (y <= button->y && y >= button->y - button->height) {
			return true;
		}
	}
	return false;
}
