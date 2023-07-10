#include "stdafx.h"
#include "library.h"

int color_rgba(Color *c) {
	return ((c->r) << 24) | ((c->g) << 16) | ((c->b) << 8) | (c->a);
}

int color_inv(int color) {
	return ~color;
}

int color_rand() {
	return (rand() % 256) << 24 | (rand() % 256) << 16 | (rand() % 256) << 8 | 0xFF;
}

void select_color(int color) {
	glColor4ub((color >> 24) & 0xFF, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
}

double rand_max(double max) {
	return (rand() / ((double) RAND_MAX) * max);
}

int randint_max(int max) {
	return rand() % max;
}
