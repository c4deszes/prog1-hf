#include "stdafx.h"
#include "main.h"

//fullscreen mode
boolean fullscreen = false;
//window resolution
int window_width = 1200;
int window_height = 800;

//GLFW window
GLFWmonitor* primaryMonitor = NULL;
GLFWwindow* main_window = NULL;

//OpenAL audio
ALCdevice *device;
ALCcontext *context;

//mouse coordinates and left click state
Vector2 mouse = { 0, 0 };
boolean mouse_pressed = false;

//active GUI
int gui = SGUI_MENU;

Text* author;
//Menu
Text* title_text;
	//Snake
Button *menu_buttons[3];
	//Play
	//Leaderboard
	//Exit

//Paused menu
boolean paused = false;
Text* paused_text;
	//Paused
Button *esc_buttons[3];
	//Return
	//Quit
	//[Border]

//Select menu
Text* select_text;
	//Press attack to join
Button *selector_buttons[16];
	//P1, P2, ... P16
Button *select_buttons[2];
	//Start
	//Return

Text* scoreboard;			//Scoreboard text
Text *names[16];			//all players textobjects

#define SCOREBOARD_LENGTH 3
Text **topN[SCOREBOARD_LENGTH];
Text *topN_score[SCOREBOARD_LENGTH];

//button click sound object and source
Sound* button_sound;							
Sound_source* button_source;

//menu sound object and source object
Sound* menu_sound;
Sound_source* menu_source;

//time source, used for generating random numbers
time_t t;

//Controls
//Control sensitivity
#define SENS 3

//temporary values for storing joystick axis and button state
int cnt;
float temp_dir = 0;
uint8_t temp_button = 0;

uint16_t joystick_present = 0x0000;				//nth bit is 1, if nth controller is present

//Nth player's color
const int players_color[16] = {RED, PINK, MAGENTA, PURPLE, BLUE, AZURE, CYAN, SPRING_GREEN, GREEN, LIME, YELLOW, ORANGE, CORAL, SKIN, GREY, WHITE};

/* Players */
uint16_t players_present = 0x0000;				//nth bit is 1, if nth player has pressed his/her attack button
uint16_t players_pressed = 0x0000;				//nth bit is 1, if nth player is pressing his/her attack button
int no_players = 0;								//number of players present

char** player_names;							//player name and score table
												//format: ID(1), SCORE(2), NAME(N), where ID is 1-16, and SCORE is 1-65535
												//so: name[0] -> id, name[1] -> score MSB, name[2] -> score LSB
uint8_t names_length = 0;

Snake *snakes[16];								//snake objects
int powerups_size = 0;							//number of powerups
Powerup *powerups[16];							//powerup objects

//Controller format
//0xFFFF 0xFFFF 0x0ZYX
//Z : id of button
//Y : id of axis
//X : id of gamepad
//NOTE: the used axis must have a value between -1 and 1, with a default value of 0
//in order to properly

//Keyboard format
//LEFT, RIGHT, ATTACK
//where each number respresents the keycode according to the GLFW site
//listed here: http://www.glfw.org/docs/latest/group__keys.html

#define BUTTON_MASK 0x0F00
#define AXIS_MASK 0x00F0
#define GAMEPAD_MASK 0x000F
uint8_t temp_buttonid = 0;
uint8_t temp_axisid = 0;
uint8_t temp_gamepadid = 0;

//Default controls in case of config.ini missing
//P1: LEFT, RIGHT, SPACE
//P2: A, S, LEFT_SHIFT
//P3: Gamepad 0, first axis, first button
//P4: Gamepad 1, first axis, first button

uint16_t controls[16][3] = {
	{GLFW_KEY_LEFT, GLFW_KEY_RIGHT, GLFW_KEY_SPACE},
	{GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_LEFT_SHIFT},
	{0xFFFF, 0xFFFF, 0x0000},
	{0xFFFF, 0xFFFF, 0x0001}
};

//Collision handler
int coll = 0;
Segment *seg1;
Segment *seg2;

int main(int argc, char *argv[]) {

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	if (!glfwInit()) {
		print_error("Couldn't load OpenGL Library!");
		return -1;
	}

	//loading config.ini
	load_settings();

	init_audio();

	glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	//glfwWindowHint(GLFW_SAMPLES, 4);

	srand((unsigned)time(&t));

	primaryMonitor = glfwGetPrimaryMonitor();

	//if fullscreen then the resolution equals the monitors resolution
	print_debug("Opening new GLFW Window.");

	if (fullscreen) {
		//getting primary monitor's resolution
		window_width = glfwGetVideoMode(primaryMonitor)->width;
		window_height = glfwGetVideoMode(primaryMonitor)->height;

		main_window = glfwCreateWindow(window_width, window_height, "C->Snake", primaryMonitor, NULL);
	}
	else {
		main_window = glfwCreateWindow(window_width, window_height, "C->Snake", NULL, NULL);
	}
	if (main_window == NULL) {
		print_error("Cannot create GLFW Window!");
		return -1;
	}

	print_debug("Binding callback functions.");

	//Callback functions
	glfwSetKeyCallback(main_window, key_callback);
	glfwSetCursorPosCallback(main_window, pos_callback);
	glfwSetMouseButtonCallback(main_window, mousebutton_callback);
	glfwSetWindowPosCallback(main_window, window_pos_callback);
	glfwSetWindowFocusCallback(main_window, window_focus_callback);
	glfwSetJoystickCallback(joy_callback);

	//Finding gamepad devices
	int no_gamepads = 0;
	print_debug("Searching gamepads...");
	for (int i = GLFW_JOYSTICK_1; i < GLFW_JOYSTICK_16; i++) {
		joystick_present |= (glfwJoystickPresent(i) ? 1 : 0) << i;
		if (joystick_present & (1 << i)) {
			no_gamepads++;
			//const char* joy_name = glfwGetJoystickName(i);
		}
	}
	printf("[DEBUG] - Found gamepads: %d\n", no_gamepads);

	//Create OpenGL Context
	print_debug("Creating OpenGL Context.");
	glfwMakeContextCurrent(main_window);
	glfwSwapInterval(1);

	init_graphics();

	double now, last, delta;
	last = 0;

	//starting menu music as soon as the context is ready
	sound_source_play(menu_source);

	while (glfwWindowShouldClose(main_window) != GLFW_TRUE) {
		now = glfwGetTime();
		delta = now - last;
		last = now;
		update(delta);
		render(delta);
		glfwPollEvents();
		glfwSwapBuffers(main_window);
	}
	dispose();
	glfwDestroyWindow(main_window);
	glfwTerminate();

	return 0;
}

void load_settings() {
	//scanf functions work with only space as a delimiter when working with string:string
	FILE* config;
	config = fopen("config.ini", "r");
	char param[40];
	char value[40];
	if (config != NULL) {
		print_debug("Loading settings: config.ini");
		char c = 0;
		int i = 0;
		char* target = param;
		boolean available = true;
		boolean comment = false;

		//reads character by character, looking out for comment lines, 
		//separator, newline and end of file chars
		//
		while (available) {
			c = fgetc(config);
			if (c == EOF) {
				available = false;
			}
			if (comment || i > 39) {
				i = 0;
			}
			
			if (c == '#') {
				comment = true;
			}
			else if (c == '=') {
				target[i] = '\0';
				target = value;
				i = 0;
				continue;
			}
			else if (c == '\n' || c == EOF) {
				if (!comment && i > 0 && i < 40) {
					target[i] = '\0';
					process(param, value);
				}
				target = param;
				i = 0;
				comment = false;
			}
			else if (!comment && i < 39) {
				target[i] = c;
				i++;
			}
		}
		fclose(config);
	}
	else {
		print_error("Cannot load settings: config.ini");
	}
}

//processes settings based on the parameter name and the value
//also checks if the data is valid
void process(char* param, char* value) {
	int valid = 0;
	//global settings
	if (strcmp(param, "fullscreen") == 0) {
		int va = 0;
		valid = sscanf(value, "%d", &va);
		if (va == 1) {
			fullscreen = true;
		}
		else {
			fullscreen = false;
		}
	}
	else if (strcmp(param, "width") == 0) {
		int width = 0;
		valid = sscanf(value, "%d", &width);
		if (width > 100) {
			window_width = width;
		}
	}
	else if (strcmp(param, "height") == 0) {
		int height = 0;
		valid = sscanf(value, "%d", &height);
		if (height > 100) {
			window_height = height;
		}
	}
	//player settings
	else {
		if (param[0] == 'P') {
			int playerid = 0;
			boolean valid = false;
			int i;
			for (i = 1; param[i] != '.'; i++) {
				playerid *= 10;
				if (isdigit(param[i])) {
					playerid += (param[i] - '0');
					valid = true;
				}
			}
			if (playerid >= 1 && playerid <= 16 && valid) {
				int numeric = 0;
				if (sscanf(value, "%d", &numeric) == 1) {
					if (strcmp(&param[i + 1], "left") == 0) {
						controls[playerid - 1][0] = numeric;
					}
					else if (strcmp(&param[i + 1], "right") == 0) {
						controls[playerid - 1][1] = numeric;
					}
					else if (strcmp(&param[i + 1], "attack") == 0) {
						controls[playerid - 1][2] = numeric;
					}
					else if (strcmp(&param[i + 1], "joy") == 0) {
						controls[playerid - 1][0] = 0xFFFF;
						controls[playerid - 1][1] = 0xFFFF;
						controls[playerid - 1][2] &= ~GAMEPAD_MASK;
						controls[playerid - 1][2] |= numeric & GAMEPAD_MASK;
					}
					else if (strcmp(&param[i + 1], "button") == 0) {
						if (controls[playerid - 1][0] == 0xFFFF && controls[playerid - 1][1] == 0xFFFF) {
							controls[playerid - 1][2] &= ~BUTTON_MASK;
							controls[playerid - 1][2] |= (numeric << 8) & BUTTON_MASK;
						}
					}
					else if (strcmp(&param[i + 1], "axis") == 0) {
						if (controls[playerid - 1][0] == 0xFFFF && controls[playerid - 1][1] == 0xFFFF) {
							controls[playerid - 1][2] &= ~AXIS_MASK;
							controls[playerid - 1][2] |= (numeric << 4) & AXIS_MASK;
						}
					}
				}

				else if (strcmp(&param[i + 1], "name") == 0) {
					boolean found = false;
					for (int q = 0; q < names_length && !found; q++) {
						if (player_names[q][0] == playerid) {
							for (int n = 0; value[n] != '\0'; n++) {
								char* temp = (char*)realloc(player_names[q], 4 + (n + 1) * sizeof(char));
								if (temp == NULL) {
									//shit happens
									exit(0xDEADBEEF);
								}
								else {
									player_names[q] = temp;
								}
								player_names[q][3 + n] = value[n];
								player_names[q][3 + n + 1] = '\0';
							}
							found = true;
						}
					}
					
					if (!found) {
						//allocate new name
						names_length++;
						char** temp = realloc(player_names, sizeof(char*) * names_length);
						if (temp == NULL) {
							//shit happens
							exit(0xDEADBEEF);
						}
						else {
							player_names = temp;
						}
						player_names[names_length - 1] = (char*)malloc(4);			//allocating the first 3 bytes(ID and SCORE)
						if (player_names[names_length - 1] == NULL) {
							//shit happens
							exit(0xDEADBEEF);
						}
						
						player_names[names_length - 1][0] = playerid;
						player_names[names_length - 1][1] = 0;
						player_names[names_length - 1][2] = 0;
						player_names[names_length - 1][3] = '\0';
						for (int n = 0; value[n] != '\0'; n++) {
							char* temp  = (char*)realloc(player_names[names_length - 1], 4 + (n + 1) * sizeof(char));
							if (temp == NULL) {
								//shit happens
								exit(0xDEADBEEF);
							}
							else {
								player_names[names_length - 1] = temp;
							}
							
							player_names[names_length - 1][3 + n] = value[n];
							player_names[names_length - 1][3 + n + 1] = '\0';
						}
					}
					
				}
			}
		}
	}

	
}

void init_audio() {
	//Audio setup
	print_debug("Creating audio context.");
	device = alcOpenDevice(NULL);
	if (!device) {
		printf("Couldn't find audio device!\n");
		return;
	}

	context = alcCreateContext(device, NULL);
	if (!alcMakeContextCurrent(context)) {
		printf("Couldn't create audio context!\n");
		return;
	}

	//Setting up listener coordinates and orientation
	//Standing still
	ALfloat listenerOrientation[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
	alListener3f(AL_POSITION, 0, 0, 1.0f);
	alListener3f(AL_VELOCITY, 0, 0, 0);
	alListenerfv(AL_ORIENTATION, listenerOrientation);

	print_debug("Loading audio files.");

	//Menu music
	sound_construct(&menu_sound, "menu.wav");
	sound_source_construct(&menu_source, menu_sound);
	sound_source_setloop(menu_source, true);
	sound_source_setvolume(menu_source, 0.2f);

	//Button onClick sound
	sound_construct(&button_sound, "click.wav");
	sound_source_construct(&button_source, button_sound);
	sound_source_setvolume(button_source, 0.4f);

	//Snake audio files
	sound_construct(&snake_sounds[0], "death.wav");
	sound_construct(&snake_sounds[1], "invincible.wav");
}

void init_graphics() {

	//Graphics setup
	glViewport(0, 0, window_width, window_height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glOrtho(0, window_width, 0, window_height, -1, 1);

	//Enabling transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//Thicker lines
	glLineWidth(2);

	int button_width = 200;
	int button_height = 50;

	print_debug("Constructing GUI Elements.");
	text_construct(&author, window_width-340, 20, 7, "created by: eszes balazs");
	/* MAIN MENU */
		//Menu title
		text_construct(&title_text, window_width / 2, 100, 20, "c->snake");
		double text_width, text_height;
		text_getdimensions(title_text, &text_width, &text_height);
		text_setpos(title_text, window_width / 2 - text_width / 2, window_height / 1.6 + text_height + 20);

		//Menu buttons
		button_default(&menu_buttons[0], window_width/2 - button_width/2, window_height / 1.6 - button_height - 10, button_width, button_height, "play");
		button_default(&menu_buttons[1], window_width/2 - button_width/2, window_height / 1.6 - 2*button_height - 2*10, button_width, button_height, "leaderboard");
		button_default(&menu_buttons[2], window_width / 2 - button_width / 2, window_height / 1.6 - 3 * button_height - 3 * 10, button_width, button_height, "exit");


	/* PAUSED MENU */
		//Paused title 
		text_construct(&paused_text, window_width / 2, 100, 10, "paused");
		text_getdimensions(paused_text, &text_width, &text_height);
		text_setpos(paused_text, window_width / 2 - text_width / 2, window_height / 1.6 + text_height + 20);

		//Paused buttons
		button_default(&esc_buttons[0], window_width / 2 - button_width / 2, window_height / 1.6, button_width, button_height, "resume");
		button_default(&esc_buttons[1], window_width / 2 - button_width / 2, window_height / 1.6 - button_height - 10, button_width, button_height, "exit");
		//used as div
		button_default(&esc_buttons[2], window_width / 2 - max(button_width, text_width) / 2 - 10, window_height / 1.6 + text_height + 30, max(button_width, text_width) + 20, 2 * (button_height + 10) + text_height + 30 , "");
	
	/* SELECT MENU */
		text_construct(&select_text, window_width / 2, 100, 10, "press attack to join");
		text_getdimensions(select_text, &text_width, &text_height);
		text_setpos(select_text, window_width / 2 - text_width / 2, window_height / 1.3 + text_height + 20);

		//selector buttons
		button_width = 50;
		button_height = 80;
		char temp_name[4] = "P0";
		for (int i = 0; i < 16; i++) {
			snprintf(temp_name, 4, "p%d", i+1);
			button_default(&selector_buttons[i], window_width / 2 + (i-8) * (button_width + 10), window_height / 1.6 , button_width, button_height, temp_name);
		}

		button_width = 150;
		button_height = 50;

		//Select menu buttons
		button_default(&select_buttons[0], window_width / 2 - button_width - 20, window_height / 2 - 2*button_height - 20, button_width, button_height, "start");
		button_default(&select_buttons[1], window_width / 2 + 10, window_height / 2 - 2*button_height - 20, button_width, button_height, "return");

	/* IN GAME ELEMENTS */
		text_construct(&scoreboard, window_width / 2, 0, 10, "leaderboard");
		text_getdimensions(scoreboard, &text_width, &text_height);
		text_setpos(scoreboard, window_width - text_width - 10, window_height - 10);
		text_setcolor(scoreboard, WHITE & ~0x2F);

		for (int i = 0; i < names_length; i++) {
			uint8_t pid = player_names[i][0];
			text_construct(&names[pid - 1], window_width, 200, 10, &player_names[i][3]);
			text_getdimensions(names[pid - 1], &text_width, &text_height);
			text_setpos(names[pid - 1], window_width - text_width - 80, 200);
			text_setcolor(names[pid - 1], players_color[pid - 1] & ~0x8F);
		}

}

void update(double delta) {
	if (!paused && gui == SGUI_GAME) {
		int alive = 0;

		for (int i = 0; i < 16; i++) {
			temp_button = GLFW_RELEASE;
			temp_dir = 0;

			if (snakes[i] != NULL && players_present & (1<<i)) {
				//if gamepad
				if (controls[i][0] == 0xFFFF && controls[i][1] == 0xFFFF) {
					temp_buttonid = (controls[i][2] & BUTTON_MASK) >> 8;
					temp_axisid = (controls[i][2] & AXIS_MASK) >> 4;
					temp_gamepadid = (controls[i][2] & GAMEPAD_MASK);

					if (joystick_present & (1 << temp_gamepadid)) {
						const float* values = glfwGetJoystickAxes(temp_gamepadid, &cnt);
						if (values != NULL && cnt > temp_axisid) {
							temp_dir = values[temp_axisid];
						}

						const char* buttons = glfwGetJoystickButtons(temp_gamepadid, &cnt);
						if (buttons != NULL && cnt > temp_buttonid) {
							temp_button = buttons[temp_buttonid];
						}
					}
				}
				//if keyboard
				else if (controls[i][0] != 0 && controls[i][1] != 0) {
					if (glfwGetKey(main_window, controls[i][0]) == GLFW_PRESS) {
						temp_dir = -1;
					}
					if (glfwGetKey(main_window, controls[i][1]) == GLFW_PRESS) {
						temp_dir += 1;
					}
					temp_button = glfwGetKey(main_window, controls[i][2]);
				}

				if (!(snakes[i]->dead)) {
					alive++;
					snakes[i]->direction -= SENS * delta * temp_dir * (snakes[i]->confusion > 0 ? -1 : 1);
					snakes[i]->dx = 100 * cos(snakes[i]->direction);
					snakes[i]->dy = 100 * sin(snakes[i]->direction);
				}

				//boosting
				if (temp_button == GLFW_PRESS && snakes[i]->boost > 0) {
					snakes[i]->boost -= delta;
					snakes[i]->speed = SNAKE_DEFAULT_SPEED + BOOST_MODIFIER - (snakes[i]->snail > 0 ? SNAIL_MODIFIER : 0);
				}
				else {
					snakes[i]->speed = SNAKE_DEFAULT_SPEED - (snakes[i]->snail > 0 ? SNAIL_MODIFIER : 0);
				}

				snake_update(snakes[i], delta, &window_width, &window_height);
				
				for (int y = 0; y < powerups_size; y++) {
					if (!(snakes[i]->dead) && powerup_collision(snakes[i], powerups[y]) != COLLISION_NOPE) {

						if (powerups[y]->type == FOOD || powerups[y]->type == BOOST || powerups[y]->type == BLAZE_IT) {
							snake_buff(snakes[i], powerups[y]->type);
						}
						else if (powerups[y]->type == SNAIL || powerups[y]->type == CONFUSION) {
							for (int q = 0; q < 16; q++) {
								if (q != i && snakes[q] != NULL) {
									snake_buff(snakes[q], powerups[y]->type);
								}
							}
						}

						if (powerups[y]->type == FOOD) {
							addscore(i + 1, 2);
						}
						addscore(i + 1, 1);
						scoreboard_recalc();

						powerups[y]->x = 70 + rand_max(window_width - 140);
						powerups[y]->y = 70 + rand_max(window_height - 140);
						powerup_settype(powerups[y], (no_players > 1 ? powerup_random() : FOOD));
					}
				}

				for (int n = i + 1; n < 16; n++) {
					check_collisions(i, n);
				}
			}
		}

		if (alive < 2 && no_players > 1) {
			save_leaderboard();
			system("openleaderboard.bat");
			gui = SGUI_MENU;
			paused = true;
			sound_source_play(menu_source);
		}

	}
	if (gui == SGUI_SELECT) {
		for (int i = 0; i < 16; i++) {
			temp_button = GLFW_RELEASE;
			//if gamepad
			if (controls[i][0] == 0xFFFF && controls[i][1] == 0xFFFF) {
				temp_buttonid = (controls[i][2] & BUTTON_MASK) >> 8;
				temp_gamepadid = (controls[i][2] & GAMEPAD_MASK);
				if (joystick_present & (1 << temp_gamepadid)) {
					//NOTE: buttons = NULL if joystick has been disconnected
					const unsigned char *buttons = glfwGetJoystickButtons(temp_gamepadid, &cnt);
					if (buttons != NULL && cnt > temp_buttonid) {
						temp_button = buttons[temp_buttonid];
					}
				}
			}
			//if keyboard
			else if (controls[i][0] != 0 && controls[i][1] != 0) {
				temp_button = glfwGetKey(main_window, controls[i][2]);
			}

			if (temp_button == GLFW_PRESS && !(players_pressed & (1 << i))) {
				players_present ^= (1 << i);
				players_pressed |= (1 << i);
				if (players_present & (1 << i)) {
					button_setfillcolor(selector_buttons[i], players_color[i]);
				}
				else {
					button_setfillcolor(selector_buttons[i], BLACK);
				}
			}
			else if (temp_button == GLFW_RELEASE) {
				players_pressed &= ~(1 << i);
			}
		}
	}
}

#define BANDS 128.0

uint32_t offset = 0;
uint32_t frame = 0;
double spectrum[(int)BANDS];

double maxof = 0.1;
int lastValue = 0;

#define FRAME_SIZE 1024.0
#define EULER 2.71

void render(double delta) {

	glRenderMode(GL_RENDER);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// Slow shitty, but working Fourier algorithm: https://en.wikipedia.org/wiki/Discrete_Hartley_transform
	if (gui != SGUI_GAME && menu_sound != NULL) {
		alGetSourcei(menu_source->source, AL_BYTE_OFFSET, &offset);

		for (int i = 0; i < BANDS; i++) {
			spectrum[i] = 0;

			double freq =  (2*i + 1) / 3 + 1 ;

			for (int k = offset; k < offset + FRAME_SIZE && k < menu_sound->size; k+=2) {
				//this is magic
				//you don't question magic
				double val = (*((uint16_t*)&menu_sound->data[k])) / 32768.0 * (cos(2 * PI * freq * (k - offset) / FRAME_SIZE) + sin(2 * PI * freq * (k - offset) / FRAME_SIZE)); // 32768.0 * pow(EULER, i * 0.001) * cos((offset - frame) / FRAME_SIZE * 2 * PI / BANDS * i * 1024);
				spectrum[i] += 0.005 * val;
				maxof = max(maxof, spectrum[i]);
			}
		}

		glBegin(GL_QUADS);
		for (int i = 0; i < BANDS - 1 ; i++) {
			select_color((i << 16) | (RED & ~0x80));
			glVertex2d(window_width / BANDS * i, 0);
			glVertex2d(window_width / BANDS * i, window_height * fabs(spectrum[i]) / maxof);
			glVertex2d(window_width / BANDS * (i + 1), window_height * fabs(spectrum[i + 1]) / maxof);
			glVertex2d(window_width / BANDS * (i + 1), 0);
		}
		glEnd();
	}

	if (gui == SGUI_MENU) {
		for (int i = 0; i < 3; i++) {
			if (menu_buttons[i] != NULL) {
				button_draw(menu_buttons[i]);
			}
		}
		text_draw(title_text);
		text_draw(author);
	}

	if (gui == SGUI_GAME) {
		for (int i = 0; i < 16; i++) {
			if (snakes[i] != NULL) {
				snake_draw(snakes[i], &window_width, &window_height);
			}
		}

		for (int i = 0; i < powerups_size; i++) {
			if (powerups != NULL) {
				powerup_draw(powerups[i]);
			}
		}

		text_draw(scoreboard);

		for (int i = 0; i < SCOREBOARD_LENGTH; i++) {
			if (topN[i] != NULL) {
				text_draw(*topN[i]);
				text_draw(topN_score[i]);
			}
		}
	}

	//Paused menu
	if (gui == SGUI_GAME && paused) {
		button_draw(esc_buttons[2]);
		text_draw(paused_text);
		button_draw(esc_buttons[0]);
		button_draw(esc_buttons[1]);		
	}

	//Select menu
	if (gui == SGUI_SELECT) {
		text_draw(select_text);
		for (int i = 0; i < 16; i++) {
			button_draw(selector_buttons[i]);
		}
		button_draw(select_buttons[0]);
		button_draw(select_buttons[1]);
		text_draw(author);
	}
}
//any modern operating system will deallocate the heap after the process has exited
void dispose() {
	for (int i = 0; i < 16; i++) {
		if (snakes[i] != NULL) {
			snake_destruct(snakes[i]);
			snakes[i] = NULL;
		}
		if (powerups[i] != NULL) {
			powerup_destruct(powerups[i]);
			powerups[i] = NULL;
		}
		if (selector_buttons[i] != NULL) {
			button_destruct(selector_buttons[i]);
		}
		if (names[i] != NULL) {
			text_destruct(names[i]);
		}
		if (snake_sounds[i] != NULL) {
			sound_destruct(snake_sounds[i]);
		}
	}

	for (int i = 0; i < SCOREBOARD_LENGTH; i++) {
		if (topN_score[i] != NULL) {
			text_destruct(topN_score[i]);
		}
	}

	for (int i = 0; i < 2; i++) {
		if (select_buttons[i] != NULL) {
			button_destruct(select_buttons[i]);
		}
	}

	for (int i = 0; i < 3; i++) {
		if (menu_buttons[i] != NULL) {
			button_destruct(menu_buttons[i]);
		}
		if (esc_buttons[i] != NULL) {
			button_destruct(esc_buttons[i]);
		}
	}

	text_destruct(title_text);
	text_destruct(author);
	text_destruct(paused_text);
	text_destruct(select_text);
	text_destruct(scoreboard);

	sound_source_destruct(menu_source);
	sound_source_destruct(button_source);

	sound_destruct(menu_sound);
	sound_destruct(button_sound);

	save_leaderboard();

	for (int i = 0; i < names_length; i++) {
		free(player_names[i]);
	}
	free(player_names);

	alcMakeContextCurrent(NULL);
	alcDestroyContext(context);
	alcCloseDevice(device);

	printf("%d", _CrtDumpMemoryLeaks());
	print_debug("Exiting game.");
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	//pausing game when pressing escape button
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS && gui == SGUI_GAME) {
		paused = !paused;
	}
}

void joy_callback(int joy, int event) {
	if (joy >= 0 && joy <= 16) {
		if (event == GLFW_CONNECTED) {
			joystick_present |= (1 << joy);
			printf("[DEBUG] - Connected gamepad(%d)\n", joy);
		}
		if (event == GLFW_DISCONNECTED) {
			joystick_present &= ~(1 << joy);
			printf("[DEBUG] - Disconnected gamepad(%d)\n", joy);
		}
	}
}

void window_pos_callback(GLFWwindow* window, int xpos, int ypos)
{
	//during windows movements delta time values are very high
	//which makes the 
	paused = true;
}

void window_focus_callback(GLFWwindow* window, int focus) {
	//pauses game if window has lost focus
	if(!focus && gui == SGUI_GAME){
		paused = true;
	}
}

void button_handler(Button* button, uint8_t event) {
	if (event == RELEASED_OVER && gui != SGUI_GAME) {
		sound_source_play(button_source);
	}

	if (gui == SGUI_MENU) {
		//Going into select menu
		if (button == menu_buttons[0] && event == RELEASED_OVER) {
			gui = SGUI_SELECT;
		}
		//Exiting game
		if (button == menu_buttons[2] && event == RELEASED_OVER) {
			glfwSetWindowShouldClose(main_window, GLFW_TRUE);
		}
		//open leaderboard txt
		if (button == menu_buttons[1] && event == RELEASED_OVER) {
			//wow, good luck reading it :D
			system("openleaderboard.bat");
		}
	}
	else if (paused && gui == SGUI_GAME) {
		//Resuming game
		if (button == esc_buttons[0] && event == RELEASED_OVER) {
			paused = false;
		}
		//Escaping from game
		if (button == esc_buttons[1] && event == RELEASED_OVER) {
			gui = SGUI_MENU;
			paused = true;
			sound_source_play(menu_source);
		}
	}
	else if (gui == SGUI_SELECT) {
		//Starting game
		if (button == select_buttons[0] && event == RELEASED_OVER) {
			start_game();
		}
		//Returning from select to menu
		if (button == select_buttons[1] && event == RELEASED_OVER) {
			gui = SGUI_MENU;
		}
	}
}

void start_game() {
	//cleaning up previous game
	for (int i = 0; i < 16; i++) {
		if (snakes[i] != NULL) {
			snake_destruct(snakes[i]);
			snakes[i] = NULL;
		}
		if (powerups[i] != NULL) {
			powerup_destruct(powerups[i]);
			powerups[i] = NULL;
		}
	}

	//only starts game if at least one bit is active
	if (players_present != 0) {
		paused = false;
		sound_source_stop(menu_source);

		//getting the number of players joined
		no_players = 0;
		for (int i = 0; i < 16; i++) {
			if (players_present & (1 << i)) {
				no_players++;
			}
		}

		//placing players
		//players are placed in a circle
		Vector2 v = { 100, 100 };
		int number = 0;
		for (int i = 0; i < 16; i++) {
			if (players_present & (1 << i)) {
				v.x = 100 * cos(2 * PI * number / no_players);
				v.y = 100 * sin(2 * PI * number / no_players);
				number++;
				snake_construct(&snakes[i], window_width / 2 + 300 * cos(number * 2 * PI / no_players), window_height / 2 + 300 * sin(number * 2 * PI / no_players), v, players_color[i]);
			}
		}

		//placing powerups
		powerups_size = 0;
		for (int i = 0; i < no_players; i++) {
			powerup_construct(&powerups[i], 70 + rand_max(window_width - 140), 70 + rand_max(window_height - 140), (no_players > 1 ? powerup_random() : FOOD));
			powerups_size++;
		}

		gui = SGUI_GAME;
	}
}

void check_collisions(int i, int n) {
	if (!(players_present & (1 << n))) {
		return;
	}
	if (!(snakes[i]->dead) && !(snakes[n]->dead)) {
		int type = snake_collision(snakes[i], snakes[n], &seg1, &seg2);
		switch (type) {
		case COLLISION_HEAD2SEGM: 
			//if invincible head hit mortal segment then: snake1 gains the segments of snake2
			if (snakes[i]->invincible > 0) {
				int num = snake_split(snakes[n], seg2, 0);
				for (int q = 0; q < num; q++) {
					snake_addsegment(snakes[i]);
				}
				addscore(i + 1, num);
				addscore(n + 1, -num);
				scoreboard_recalc();
			}
			//mortal snake dies
			else {
				snake_kill(snakes[i]);
			}
			break;

		case COLLISION_SEGM2HEAD: 
			//same as the case above
			if (snakes[n]->invincible > 0) {
				int num = snake_split(snakes[i], seg1, 0);
				for (int q = 0; q < num; q++) {
					snake_addsegment(snakes[n]);
				}
				addscore(n + 1, num);
				addscore(i + 1, -num);
				scoreboard_recalc();
			}
			else {
				snake_kill(snakes[n]);
			}
			break;

		case COLLISION_HEAD2HEAD:
			//mortal snakes dies
			if (snakes[i]->invincible == 0 && snakes[n]->invincible == 0) {
				snake_kill(snakes[i]);
				snake_kill(snakes[n]);
			}
			else if (snakes[i]->invincible > 0 && snakes[n]->invincible == 0) {
				snake_kill(snakes[n]);
			}
			else if (snakes[n]->invincible > 0 && snakes[i]->invincible == 0) {
				snake_kill(snakes[i]);
			}
			break;
		}
	}
}

void addscore(int pid, int point) {
	for (int i = 0; i < names_length; i++) {
		//bytehunting, could also use union type
		int16_t* score = &(player_names[i][1]);
		if (player_names[i][0] == pid) {
			*score += point;
			break;
		}
	}
}

void scoreboard_recalc() {
	boolean used[16];
	for (int i = 0; i < 16; i++) {
		used[i] = false;
	}

	for (int n = 0; n < SCOREBOARD_LENGTH; n++) {
		int16_t max = 0;
		int maxindex = 0;

		for (int q = 0; q < names_length; q++) {
			if (!used[player_names[q][0] - 1] && *((int16_t*)&player_names[q][1]) > max) {
				//do stuff
				topN[n] = &names[player_names[q][0] - 1];
				(*topN[n])->y = window_height - 50 - n * 30;
				max = *((int16_t*)&player_names[q][1]);
				maxindex = player_names[q][0] - 1;
			}
		}
		used[maxindex] = true;

		//convert score into string
		char digits[5];
		double w, h;
		snprintf(digits, 5, "%d", max);
		if (topN_score[n] != NULL) {
			text_settext(topN_score[n], digits);
		}
		else {
			text_construct(&topN_score[n], 0, 0, 10, digits);
			text_setcolor(topN_score[n], WHITE & ~0x3F);
		}
		text_getdimensions(topN_score[n], &w, &h);
		text_setpos(topN_score[n], window_width - w - 10, window_height - 50 - n * 30);
	}
}

//file format is the same as the name
void save_leaderboard() {
	FILE* leaderboard;
	leaderboard = fopen("leaderboard.txt", "r");

	int length = 1;
	int index = 1;
	char** content = malloc(sizeof(char*));
	content[0] = malloc(sizeof(char));

	if (leaderboard != NULL) {
		//read file into content table
		char temp = 0;
		boolean available = true;

		while (available) {
			temp = fgetc(leaderboard);
			if (temp == EOF) {
				available = false;
				break;
			}
			else {
				if (temp == '\n') {
					length++;
					index = 1;
					content = realloc(content, sizeof(char*) * length);
					content[length - 1] = malloc(sizeof(char));
				}
				else {
					content[length - 1] = realloc(content[length - 1], (index + 1) * sizeof(char));
					content[length - 1][index - 1] = temp;
					content[length - 1][index] = '\0';
					index++;
				}
			}
		}

		fclose(leaderboard);
	}
	else {
		printf("Cannot open leaderboard.");
	}

	//add points to players and add new players to the table
	for (int q = 0; q < names_length; q++) {
		//for every score
		boolean found = false;
		for (int n = 0; n < length; n++) {
			//if player found
			if (strcmp(&player_names[q][3], &content[n][3]) == 0) {
				*((int16_t*)&content[n][1]) += *((int16_t*)&player_names[q][1]);
				found = true;
			}
		}

		if (!found) {
			length++;
			content = realloc(content, sizeof(char*) * length);
			content[length - 1] = malloc(sizeof(char));
			for (int m = 0; m < 3 || (m >= 3 && player_names[q][m] != '\0'); m++) {
				content[length - 1] = realloc(content[length - 1], (m + 2) * sizeof(char));
				content[length - 1][m] = player_names[q][m];
				content[length - 1][m + 1] = '\0';
			}
		}
	}

	//save file
	leaderboard = fopen("leaderboard.txt", "w");

	for (int i = 0; i < length; i++) {
		fprintf(leaderboard, "%c%c%c", content[i][0], content[i][1], content[i][2]);
		fprintf(leaderboard, "%s", &content[i][3]);
		if (i != length - 1) {
			fprintf(leaderboard, "\n");
		}
		free(content[i]);
	}
	free(content);

	fclose(leaderboard);
}

void pos_callback(GLFWwindow* window, double x, double y) {
	mouse.x = x;
	mouse.y = window_height - y;

	if (gui == SGUI_MENU) {
		for (int i = 0; i < 3; i++) {
			if (button_isover(menu_buttons[i], mouse.x, mouse.y) && menu_buttons[i]->state != PRESS) {
				button_state(menu_buttons[i], HOVER);
				button_handler(menu_buttons[i], ENTERED);
			}
			else if (menu_buttons[i]->state != PRESS) {
				button_state(menu_buttons[i], DEFAULT);
				button_handler(menu_buttons[i], EXITED);
			}
		}
	}
	for (int i = 0; i < 3 - 1; i++) {
		if (button_isover(esc_buttons[i], mouse.x, mouse.y) && esc_buttons[i]->state != PRESS) {
			button_state(esc_buttons[i], HOVER);
			button_handler(esc_buttons[i], ENTERED);
		}
		else if (esc_buttons[i]->state != PRESS) {
			button_state(esc_buttons[i], DEFAULT);
			button_handler(esc_buttons[i], EXITED);
		}
	}

	for (int i = 0; i < 2; i++) {
		if (button_isover(select_buttons[i], mouse.x, mouse.y) && select_buttons[i]->state != PRESS) {
			button_state(select_buttons[i], HOVER);
			button_handler(select_buttons[i], ENTERED);
		}
		else if (select_buttons[i]->state != PRESS) {
			button_state(select_buttons[i], DEFAULT);
			button_handler(select_buttons[i], EXITED);
		}
	}


}

void mousebutton_callback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		mouse_pressed = true;
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT) {
		mouse_pressed = false;
	}

	for (int i = 0; i < 3; i++) {
		if (button_isover(menu_buttons[i], mouse.x, mouse.y) && mouse_pressed && menu_buttons[i]->state != PRESSED) {
			button_state(menu_buttons[i], PRESS);
			button_handler(menu_buttons[i], PRESSED);
		}
		else if (!mouse_pressed) {
			button_state(menu_buttons[i], DEFAULT);
			if (button_isover(menu_buttons[i], mouse.x, mouse.y)) {
				button_handler(menu_buttons[i], RELEASED_OVER);
			}
			else {
				button_handler(menu_buttons[i], RELEASED);
			}
		}
	}

	for (int i = 0; i < 3 - 1; i++) {
		if (button_isover(esc_buttons[i], mouse.x, mouse.y) && mouse_pressed && esc_buttons[i]->state != PRESSED) {
			button_state(esc_buttons[i], PRESS);
			button_handler(esc_buttons[i], PRESSED);
		}
		else if (!mouse_pressed) {
			button_state(esc_buttons[i], DEFAULT);
			if (button_isover(esc_buttons[i], mouse.x, mouse.y)) {
				button_handler(esc_buttons[i], RELEASED_OVER);
			}
			else {
				button_handler(esc_buttons[i], RELEASED);
			}
		}
	}

	for (int i = 0; i < 2; i++) {
		if (button_isover(select_buttons[i], mouse.x, mouse.y) && mouse_pressed && select_buttons[i]->state != PRESSED) {
			button_state(select_buttons[i], PRESS);
			button_handler(select_buttons[i], PRESSED);
		}
		else if (!mouse_pressed) {
			button_state(select_buttons[i], DEFAULT);
			if (button_isover(select_buttons[i], mouse.x, mouse.y)) {
				button_handler(select_buttons[i], RELEASED_OVER);
			}
			else {
				button_handler(select_buttons[i], RELEASED);
			}
		}
	}
}

void print_debug(char* x) {
	printf("[DEBUG] - %s\n", x);
}

void print_error(char* x) {
	printf("[ERROR] - %s\n", x);
}
