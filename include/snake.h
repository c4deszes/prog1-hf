#pragma once
#include "library.h"
#include "sound.h"
#include "text.h"

//Magic numbers
//don't even bother testing other values
//these values must be calibrated for a specific snake size
#define SNAKE_OFFSET 4
#define SNAKE_WARP_MULTIPLIER 40										
#define SNAKE_SIZE 10													//width of a segment

#define QST_SIZE 4														//size of confusion question icon

#define SNAKE_DEFAULT_SPEED 1.6											//default speed
#define SNAKE_DEFAULT_LENGTH 10											//default number of segments
#define SNAKE_LENGTH_SPEED_COEFFICIENT log2(SNAKE_DEFAULT_LENGTH)		//this means for snakes with the default length the speed will be default_speed 	

#define SNAKE_DEATH_TIMER 0.5											//time it takes to fade out a snake

//Collision types
#define COLLISION_NOPE 0
#define COLLISION_HEAD2HEAD 1
#define COLLISION_HEAD2SEGM 2
#define COLLISION_SEGM2HEAD 3
//The game's mechanic allows two segment to hit each other
//but this event should be discarded
#define COLLISION_SEGM2SEGM 4

extern Sound *snake_sounds[16];

// linked list structure
typedef struct Segment {
	double x, y;			//position of segment
	struct Segment *next;	//pointer to next segment
} Segment;

//
typedef struct Snake {
	Segment first;			//head segment
	double direction;		//used for the angle of direction vector
	double dx, dy;			//direction vector
	int color;				//color
	int length;				//amount of segments excluding the head
	
	boolean dead;			
	double dead_timer;		//used for fading snake out
	double speed;			//used for storing the current speed of the snake
	double invincible;		//the amount of invincibility in seconds, always active
	double boost;			//the amount of boost in seconds, active when attacking
	double snail;			//the amount of snail effect in seconds, always active
	double confusion;		//the amount of confusion debuff in seconds, always active
	
	Sound_source* sound;
} Snake;

//allocates a new snake object and puts it's pointer into the target pointer
Snake snake_construct(Snake **snake, double x, double y, Vector2 dir, int color);
//deallocates the target snake object
void snake_destruct(Snake *snake);
//initiates the death of a snake
void snake_kill(Snake *snake);
//adds a new segment to target snake object
void snake_addsegment(Snake *snake);
//removes the last segment from the target snake object
void snake_removesegment(Snake *snake);
//removes all the segments of the snake starting from the target segment
//also returns the number of segments removed
int snake_split(Snake *snake, Segment *segment, int i);
//sets the color of the snake
void snake_setcolor(Snake *snake, int color);
//buffs or debuffs snake
void snake_buff(Snake *snake, int type);

void snake_draw(Snake *snake, int *map_width, int *map_height);
void snake_update(Snake *snake, double delta, int *w, int *h);

//returns the type of collision between the snakes
//also sets the target pointers to the collided segment's pointers if
//a collision has happened
int snake_collision(Snake *snake1, Snake *snake2, Segment **seg1, Segment **seg2);