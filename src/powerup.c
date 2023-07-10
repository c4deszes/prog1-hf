#include "stdafx.h"
#include "powerup.h"

const double powerup_sizes[POWERUPS_LAST] = { POWERUP_SIZE, POWERUP_SIZE, POWERUP_SIZE, POWERUP_SIZE * 2, POWERUP_SIZE * 2 };

void powerup_construct(Powerup **p, double x, double y, int type) {
	*p = (Powerup*) malloc(sizeof(Powerup));
	if (*p == NULL) {
		//shit happens
		exit(0xDEADBEEF);
	}
	(*p)->x = x;
	(*p)->y = y;
	(*p)->type = type;
	(*p)->size = powerup_sizes[type];
}

void powerup_settype(Powerup *p, int type) {
	if (p == NULL) { return; }
	p->type = type;
	p->size = powerup_sizes[type];
}

void powerup_destruct(Powerup *p) {
	if (p == NULL) { return; }
	free(p);
}

void powerup_draw(Powerup *powerup) {
	if (powerup == NULL) { return; }
	
	switch (powerup->type) {
		case FOOD: select_color(GREEN);  break;
		case BOOST: select_color(BLUE);  break;
		case BLAZE_IT: select_color(color_rand());  break;
		case SNAIL: select_color(PURPLE); break;
		case CONFUSION: select_color(PURPLE); break;
		default: select_color(RED);
	}
	glBegin(GL_TRIANGLE_FAN);
	glVertex2d(powerup->x, powerup->y);
	for (double rad = 0; rad < 2 * PI; rad += 0.1) {
		glVertex2d(powerup->x + powerup->size * cos(rad), powerup->y + powerup->size * sin(rad));
	}
	glVertex2d(powerup->x + powerup->size, powerup->y);
	glEnd();

	select_color(WHITE);

	glBegin(GL_LINE_STRIP);
	for (double rad = 0; rad < 2 * PI; rad += 0.1) {
		glVertex2d(powerup->x + powerup->size * cos(rad), powerup->y + powerup->size * sin(rad));
	}
	glVertex2d(powerup->x + powerup->size, powerup->y);
	glEnd();

	if (powerup->type == BOOST || powerup->type == SNAIL) {
		select_color(YELLOW);
		glBegin(GL_TRIANGLE_STRIP);
		glVertex2d(powerup->x, powerup->y + powerup->size);
		glVertex2d(powerup->x - powerup->size / 3, powerup->y - powerup->size / 10);
		glVertex2d(powerup->x, powerup->y + powerup->size / 10);

		glVertex2d(powerup->x, powerup->y - powerup->size / 10);
		glVertex2d(powerup->x + powerup->size / 3, powerup->y + powerup->size / 10);
		glVertex2d(powerup->x, powerup->y - powerup->size);
		glEnd();
	}

	if (powerup->type == CONFUSION) {
		select_color(WHITE);
		glBegin(GL_QUADS);

		for (int y = 0; y < char_getheight(CHARDEF_QSTN); y++) {
			for (int x = 0; x < char_getwidth(CHARDEF_QSTN); x++) {
				if (*(CHARDEF_QSTN + 1 + y) & (1 << x)) {
					glVertex2d(powerup->x + powerup->size / 2 - (x + 1.1) * 2, powerup->y - powerup->size / 2 - (y - 7) * 2);
					glVertex2d(powerup->x + powerup->size / 2 - (x + 2.5) * 2, powerup->y - powerup->size / 2 - (y - 7) * 2);
					glVertex2d(powerup->x + powerup->size / 2 - (x + 2.5) * 2, powerup->y - powerup->size / 2 - (y - 6) * 2);
					glVertex2d(powerup->x + powerup->size / 2 - (x + 1.5) * 2, powerup->y - powerup->size / 2 - (y - 6) * 2);
				}
			}
		}
		glEnd();
	}

	if (powerup->type == SNAIL) {
		select_color(WHITE);
		glBegin(GL_LINES);
		glVertex2d(powerup->x - powerup->size * cos(PI / 4), powerup->y + powerup->size * sin(PI / 4));
		glVertex2d(powerup->x + powerup->size * cos(PI / 4), powerup->y - powerup->size * sin(PI / 4));
		glEnd();
	}
}

int powerup_collision(Snake *snake, Powerup *powerup) {
	if (powerup == NULL || snake == NULL) { return COLLISION_NOPE; }
	double dist = sqrt(pow(snake->first.x - powerup->x, 2) + pow(snake->first.y - powerup->y, 2));
	if (dist < SNAKE_SIZE + powerup->size) {
		return COLLISION_POWERUP;
	}
	return COLLISION_NOPE;
}

//probabilities
// FOOD:		0.70
// BOOST:		0.10
// BLAZE_IT:	0.05
// SNAIL:		0.05
// CONFUSION:	0.10
int powerup_random() {
	int rand = randint_max(1000);
	if (rand > 300) {
		return FOOD;
	}
	else if (rand > 200) {
		return BOOST;
	}
	else if (rand > 150) {
		return SNAIL;
	}
	else if (rand > 50) {
		return CONFUSION;
	}
	else {
		return BLAZE_IT;
	}
}
