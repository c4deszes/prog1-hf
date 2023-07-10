#include "stdafx.h"
#include "snake.h"
#include "powerup.h"

Sound *snake_sounds[16];

void snake_addsegment(Snake *snake) {
	if (snake == NULL) {
		return;
	}

	Segment *seg = &(snake->first);
	while (seg->next != NULL) {
		seg = seg->next;		
	}
	seg->next = (Segment*)malloc(sizeof(Segment));

	if (seg->next == NULL) {
		//shit happens
		exit(0xDEADBEEF);
	}

	seg->next->x = seg->x;
	seg->next->y = seg->y;
	seg->next->next = NULL;

	snake->length++;
}
void snake_removesegment(Snake *snake) {
	if (snake == NULL) { return; }

	Segment *seg = &(snake->first);
	Segment *prev = seg;
	while (seg->next != NULL) {
		prev = seg;
		seg = seg->next;
	}
	free(seg);
	snake->length--;

	prev->next = NULL;
}

int snake_split(Snake *snake, Segment *segment, int i) {
	if (snake == NULL || segment == NULL) { return 0; }

	if (segment->next != NULL) {
		i = snake_split(snake, segment->next, i + 1);
		segment->next = NULL;
	}
	free(segment->next);
	snake->length--;
	return i;
}

int snake_collision(Snake *snake1, Snake *snake2, Segment **seg1, Segment **seg2) {
	if (snake1 == NULL || snake2 == NULL) {
		return COLLISION_NOPE;
	}

	double dist = 0;
	Segment *head1 = &(snake1->first);
	while (head1 != NULL) {
		Segment *head2 = &(snake2->first);
		while (head2 != NULL) {
			dist = sqrt(pow(head2->x - head1->x, 2) + pow(head2->y - head1->y, 2));

			if (dist < SNAKE_SIZE * 2) {
				*seg1 = head1;
				*seg2 = head2;
				if (head1 == &(snake1->first)) {
					if (head2 == &(snake2->first)) {
						return COLLISION_HEAD2HEAD;
					}
					else {
						return COLLISION_HEAD2SEGM;
					}
				}
				else {
					if (head2 == &(snake2->first)) {
						return COLLISION_SEGM2HEAD;
					}
					else {
						return COLLISION_SEGM2SEGM;
					}
				}
			}
			head2 = head2->next;
		}
		head1 = head1->next;
	}
	return COLLISION_NOPE;
}

Snake snake_construct(Snake **snake, double x, double y, Vector2 dir, int color) {
	(*snake) = (Snake*) malloc(sizeof(Snake));
	if ((*snake) == NULL) {
		//This code shouldn't be reached normally, but sometimes shit happens.
		//in this case the code below will make the program crash
		exit(0xDEADBEEF);
	}

	(*snake)->first.x = x;
	(*snake)->first.y = y;
	(*snake)->first.next = NULL;
	(*snake)->length = 0;

	(*snake)->direction = atan2(dir.y, dir.x);
	(*snake)->dx = dir.x;
	(*snake)->dy = dir.y;
	(*snake)->color = color;

	(*snake)->speed = SNAKE_DEFAULT_SPEED;
	(*snake)->dead = false;
	(*snake)->dead_timer = 0;
	(*snake)->invincible = 0;
	(*snake)->boost = 0;
	(*snake)->confusion = 0;

	sound_source_construct(&((*snake)->sound), NULL);
	sound_source_setvolume((*snake)->sound, 0.5f);
	
	for (int i = 0; i < SNAKE_DEFAULT_LENGTH; i++) {
		snake_addsegment(*snake);
	}

	return **snake;
}

void snake_destruct(Snake *snake) {
	if (snake == NULL) { return; }
	sound_source_destruct(snake->sound);
	while (snake->length != 0) {
		snake_removesegment(snake);
	}
	free(snake);
}

void snake_kill(Snake *snake) {
	if (snake == NULL) { return; }
	snake->dead_timer = SNAKE_DEATH_TIMER;
	snake->dead = true;
	sound_source_setsound(snake->sound, snake_sounds[0]);
	sound_source_play(snake->sound);
}

void snake_buff(Snake *snake, int type) {
	switch (type) {
	case FOOD: 
		snake_addsegment(snake);
		break;

	case BOOST: 
		snake->boost = snake->boost + BOOST_AMOUNT; 
		if (snake->boost > BOOST_MAX) { snake->boost = BOOST_MAX; } 
		break;

	case BLAZE_IT: 
		snake->invincible = INVINCIBILITY_AMOUNT; 
		sound_source_setsound(snake->sound, snake_sounds[1]);
		//sound_source_setloop(snake->sound, true);
		sound_source_play(snake->sound);
		break;

	case SNAIL: 
		snake->snail = snake->snail + SNAIL_AMOUNT; 
		if (snake->snail > SNAIL_MAX) { snake->snail = SNAIL_MAX; } 
		break;

	case CONFUSION: 
		snake->confusion = CONFUSION_AMOUNT; 
		break;
	}
}

void snake_update(Snake *snake, double delta, int *map_width, int *map_height) {
	if (snake == NULL) { return; }

	if (snake->invincible > 0) {
		snake->invincible -= delta;
	}
	else {
		snake->invincible = 0;
	}

	if (snake->confusion > 0) {
		snake->confusion -= delta;
	}
	else {
		snake->confusion = 0;
	}

	if (snake->snail > 0) {
		snake->snail -= delta;
	}
	else {
		snake->snail = 0;
	}

	if (snake->dead_timer > 0 && snake->dead) {
		snake->dead_timer -= delta;
		snake_setcolor(snake, (snake->color & ~0x000000FF) | ((uint8_t) (snake->dead_timer / SNAKE_DEATH_TIMER * 255) ));
	}
	if(snake->dead_timer <= 0 && snake->dead){
		snake_setcolor(snake, 0);
	}
	if (snake->dead) {
		return;
	}

	double nx = snake->dx;
	double ny = snake->dy;
	Segment *seg = &(snake->first);
	Segment *prev = seg;

	double dx = 0;
	double dy = 0;

	seg->x += nx * delta * snake->speed * (SNAKE_LENGTH_SPEED_COEFFICIENT > 0 ? SNAKE_LENGTH_SPEED_COEFFICIENT / log2(snake->length) : 1);
	seg->y += ny * delta * snake->speed * (SNAKE_LENGTH_SPEED_COEFFICIENT > 0 ? SNAKE_LENGTH_SPEED_COEFFICIENT / log2(snake->length) : 1);

	if (seg->x > *map_width) {
		seg->x = 0;
	}
	if (seg->x < 0) {
		seg->x = *map_width;
	}

	if (seg->y > *map_height) {
		seg->y = 0;
	}
	if (seg->y < 0) {
		seg->y = *map_height;
	}

	seg = seg->next;

	while (seg != NULL) {
		dx = (prev->x - seg->x);
		dy = (prev->y - seg->y);

		if (dx > SNAKE_WARP_MULTIPLIER) {
			dx -= *map_width;
		}
		if (dx < -SNAKE_WARP_MULTIPLIER) {
			dx += *map_width;
		}
		if (dy > SNAKE_WARP_MULTIPLIER) {
			dy -= *map_height;
		}
		if (dy < -SNAKE_WARP_MULTIPLIER) {
			dy += *map_height;
		}

		seg->x += delta * dx * snake->speed * (SNAKE_LENGTH_SPEED_COEFFICIENT > 0 ? SNAKE_LENGTH_SPEED_COEFFICIENT / log2(snake->length) : 1) * SNAKE_OFFSET;
		seg->y += delta * dy * snake->speed * (SNAKE_LENGTH_SPEED_COEFFICIENT > 0 ? SNAKE_LENGTH_SPEED_COEFFICIENT / log2(snake->length) : 1) * SNAKE_OFFSET;

		if (seg->x > *map_width) {
			seg->x = 0;
		}
		if (seg->x < 0) {
			seg->x = *map_width;
		}

		if (seg->y > *map_height) {
			seg->y = 0;
		}
		if (seg->y < 0) {
			seg->y = *map_height;
		}

		prev = seg;
		seg = seg->next;
	}
}

void snake_setcolor(Snake *snake, int color) {
	if (snake == NULL) {
		return;
	}
	snake->color = color;
}

void snake_draw(Snake *snake, int *map_width, int *map_height) {
	if (snake == NULL) { return; }

	Segment prev = { snake->first.x + snake->dx , snake->first.y + snake->dy , NULL };
	Segment *seg = &(snake->first);
	double nx = prev.x - seg->x;
	double ny = prev.y - seg->y;
	double dist = sqrt(nx*nx + ny*ny);

	if (snake->invincible > 0 && !(snake->dead)) {
		select_color(color_rand());
	}
	else {
		select_color(snake->color);
	}

	glBegin(GL_TRIANGLE_FAN);
	glVertex2d(seg->x, seg->y);
	for (double rad = snake->direction - PI/2; rad < snake->direction + PI/2; rad += 0.2) {
		glVertex2d(seg->x + SNAKE_SIZE * cos(rad), seg->y + SNAKE_SIZE * sin(rad));
	}
	glVertex2d(seg->x + SNAKE_SIZE * cos(snake->direction + PI / 2), seg->y + SNAKE_SIZE * sin(snake->direction + PI / 2));
	glEnd();
	
	glBegin(GL_QUAD_STRIP);
	while(seg != NULL) {
		nx = prev.x - seg->x;
		ny = prev.y - seg->y;

		if (dist > 0) {
			boolean warp = false;
			if (seg != &(snake->first) && (nx > 40 || nx < -40 || ny > 40 || ny < -40)) {
				warp = true;
				if (nx > SNAKE_WARP_MULTIPLIER) {
					nx -= *map_width;
				}
				if (nx < -SNAKE_WARP_MULTIPLIER) {
					nx += *map_width;
				}
				if (ny > SNAKE_WARP_MULTIPLIER) {
					ny -= *map_height;
				}
				if (ny < -SNAKE_WARP_MULTIPLIER) {
					ny += *map_height;
				}
			}

			dist = sqrt(nx*nx + ny*ny);
			nx /= dist;
			ny /= dist;
			if (warp) {
				glVertex2d(prev.x - ny * SNAKE_SIZE - nx * SNAKE_WARP_MULTIPLIER, prev.y + nx * SNAKE_SIZE - ny * SNAKE_WARP_MULTIPLIER);
				glVertex2d(prev.x + ny * SNAKE_SIZE - nx * SNAKE_WARP_MULTIPLIER, prev.y - nx * SNAKE_SIZE - ny * SNAKE_WARP_MULTIPLIER);
				glEnd();
				glBegin(GL_QUAD_STRIP);
				glVertex2d(seg->x - ny * SNAKE_SIZE + nx * SNAKE_WARP_MULTIPLIER, seg->y + nx * SNAKE_SIZE + ny * SNAKE_WARP_MULTIPLIER);
				glVertex2d(seg->x + ny * SNAKE_SIZE + nx * SNAKE_WARP_MULTIPLIER, seg->y - nx * SNAKE_SIZE + ny * SNAKE_WARP_MULTIPLIER);
			}
		}

		glVertex2d(seg->x - ny * SNAKE_SIZE, seg->y + nx * SNAKE_SIZE);
		glVertex2d(seg->x + ny * SNAKE_SIZE, seg->y - nx * SNAKE_SIZE);

		prev.x = seg->x;
		prev.y = seg->y;
		seg = seg->next;
	}
	glEnd();
	
	glBegin(GL_TRIANGLE_FAN);
	glVertex2d(prev.x, prev.y);
	for (double rad = atan2(ny,nx) - PI / 2; rad < atan2(ny, nx) + PI / 2; rad += 0.2) {
		glVertex2d(prev.x - SNAKE_SIZE * cos(rad), prev.y - SNAKE_SIZE * sin(rad));
	}
	glVertex2d(prev.x - SNAKE_SIZE * cos(atan2(ny, nx) + PI / 2), prev.y - SNAKE_SIZE * sin(atan2(ny, nx) + PI / 2));
	glEnd();


	if (!(snake->dead) && snake->confusion > 0) {
		select_color(WHITE);
		glBegin(GL_QUADS);

		for (int y = 0; y < char_getheight(CHARDEF_QSTN); y++) {
			for (int x = 0; x < char_getwidth(CHARDEF_QSTN); x++) {
				if (*(CHARDEF_QSTN + 1 + y) & (1 << x)) {
					glVertex2d(snake->first.x - (x - 3) * QST_SIZE, snake->first.y - (y - 13) * QST_SIZE);
					glVertex2d(snake->first.x - (x - 2) * QST_SIZE, snake->first.y - (y - 13) * QST_SIZE);
					glVertex2d(snake->first.x - (x - 2) * QST_SIZE, snake->first.y - (y - 12) * QST_SIZE);
					glVertex2d(snake->first.x - (x - 3) * QST_SIZE, snake->first.y - (y - 12) * QST_SIZE);
				}
			}
		}
		glEnd();
	}

}
