#pragma once
#include "library.h"
#include "snake.h"

#define POWERUP_SIZE 8			//size of the normal powerup

#define COLLISION_POWERUP 6		

//Powerup types
#define FOOD 0
#define BOOST 1
#define BLAZE_IT 2
#define SNAIL 3
#define CONFUSION 4

#define POWERUPS_LAST 5

//Powerup settings
//Boost: speeds up snake
#define BOOST_MAX 10.0
#define BOOST_AMOUNT 4.0
#define BOOST_MODIFIER 1

//Snail: slows down snake
#define SNAIL_MAX 10.0
#define SNAIL_AMOUNT 4.0
#define SNAIL_MODIFIER 0.5

//Invincibility: can cut through or kill other players
#define INVINCIBILITY_AMOUNT 10.0

//Confusion: inverts controls
#define CONFUSION_AMOUNT 5.0

typedef struct Powerup {
	double x, y;			//position of the powerup
	int type;				//type of powerup
	double size;			//the radius of the powerup
} Powerup;

//allocates a new powerup object and puts it's pointer into the target pointer
void powerup_construct(Powerup **p, double x, double y, int type);
void powerup_draw(Powerup *powerup);
//sets the type of the powerup
void powerup_settype(Powerup *p, int type);
//deallocates the powerup object
void powerup_destruct(Powerup *p);
//checks whether the target snake and target powerup itersects
int powerup_collision(Snake *snake, Powerup *powerup);
//returns random powerup id
int powerup_random();