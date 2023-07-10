#pragma once
//Game
#include "text.h"
#include "sound.h"
#include "button.h"
#include "snake.h"
#include "powerup.h"
#include <string.h>

#define SGUI_MENU 1
#define SGUI_GAME 2
#define SGUI_SELECT 3

//window and graphics handling
void init_audio();
void init_graphics();
void update(double delta);
void render(double delta);
void dispose();

//debug functions
void print_debug(char* x);
void print_error(char* x);

//game functions
void load_settings();
void process(char* param, char* value);
void start_game();
void check_collisions(int i, int n);
void addscore(int pid, int score);
void scoreboard_recalc();
void save_leaderboard();

//callback functions
void key_callback(GLFWwindow* window, int key, int scan, int action, int mods);
void pos_callback(GLFWwindow* window, double x, double y);
void mousebutton_callback(GLFWwindow* window, int button, int action, int mods);
void window_pos_callback(GLFWwindow* window, int xpos, int ypos);
void window_focus_callback(GLFWwindow* window, int focus);
void joy_callback(int joy, int event);

//gui functions
void button_handler(Button* button, uint8_t event);