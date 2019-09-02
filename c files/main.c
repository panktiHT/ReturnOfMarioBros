
#include "allegro.h"

//create some colors
#define WHITE makecol(255,255,255)
#define BLACK makecol(0,0,0)
#define RED makecol(255,0,0)
#define GREEN makecol(0,255,0)
#define BLUE makecol(0,0,255)

//variables
int mousex, mousey, leftClick;
int leftClickBeingHeld;
int currentKey, scancode, ascii;
int startScreenMode = 1;
int mainMenuMode = 0;
int gameMode = 0;
int endScreenMode = 0;
int inputIndex;
int win = 0; 

char input[10];
PACKFILE *currentFile;
MIDI *music;
SAMPLE *SoundEffects[3];
int musicMode = 1; 
int gamePlayMode = 0; 
int points = 0;

//bitmaps
BITMAP *buffer;
BITMAP* buffers;


//functions
void clearScreen()
{
	rectfill(buffer, 0, 0, 640, 480, BLACK);
}




void updateGameInfo() 
{
	textout_ex(buffer, font, "DodgeIT (ESC to quit)", 0, 1, WHITE, 0);	
}

void updateGameInfos()
{
	textout_ex(buffers, font, "DodgeIT (ESC to quit)", 0, 1, WHITE, 0);
}

void clearScreens() 
{
	rectfill(buffers, 0, 0, 640, 480, BLACK);
}



void getInputFromKeyboard() 
{
	currentKey = readkey();
	scancode = (currentKey >> 8);
	ascii = scancode_to_ascii(scancode);

	//number
	if (ascii > 47 && ascii < 58 && inputIndex < 10)
	{
		input[inputIndex] = (char)ascii;
		inputIndex++;
	}

	//backspace
	else if (scancode == 63 || scancode == 77)
	{
		if (inputIndex > 0) 
		{
			inputIndex--;
			input[inputIndex] = 0;
		}
	}
}

void loadBackgroundImage(char *filename) 
{
	BITMAP *background;
	background = load_bitmap(filename, NULL);
	if (!background) 
	{
		allegro_message("couldnt load bg");
		return;
	}
	blit(background, buffer, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
	destroy_bitmap(background);
}

void loadBackgroundImages(char* filename) 
{
	BITMAP* background;
	background = load_bitmap(filename, NULL);
	if (!background) 
	{
		allegro_message("couldnt load bg");
		return;
	}
	blit(background, buffers, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
	destroy_bitmap(background);
}

int maxi(int a, int b)
{
	if (a >= b)
		return a;

	else
		return b;
}

int mini(int a, int b)
{
	if (a <= b)
		return a;
	else
		return b;
}



//pixel perfect method for collision detection 
int pixel_perfect_colision(int x1, int y1, BITMAP * obj1, int x2, int y2, BITMAP * obj2)
{
	int i, j;
	int collision = FALSE;

	//if rectangles intersect 
	if (!((x1 > x2 + obj2->w) || (y1 > y2 + obj2->h) || (x2 > x1 + obj1->w) || (y2 > y1 + obj1->h)))
	{

		int top = maxi(y1, y2);
		int low = mini(y1 + obj1->h, y2 + obj2->h);
		int left = maxi(x1, x2);
		int right = mini(x1 + obj1->w, x2 + obj2->w);

		for (i = top; i < low && !collision; i++)
		{
			for (j = left; j < right && !collision; j++)
			{
				if (getpixel(obj1, j - x1, i - y1) != makecol(255, 0, 255) && getpixel(obj2, j - x2, i - y2) != makecol(255, 0, 255)) {
					collision = TRUE;
				}
				
			}
		}
	}

	return collision;
}



//game reset
void resetGame()
{
	startScreenMode = 0;
	mainMenuMode = 0;
	gamePlayMode = 1; 
}

//local variable    
float teta = 0.0;
float beta = 0.0;


int main()
{
	allegro_init();
	set_window_title("Return of the Mario Bros");
	set_color_depth(32);
	set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0);
	install_keyboard();
	install_mouse();
	install_timer();
	install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL);
	buffer = create_bitmap(640, 480);
	buffers = create_bitmap(640, 480);

	set_mouse_sprite_focus(15, 15);
	show_mouse(screen);

	//reference: https://bitmidi.com/mario-bros-super-mario-bros-theme-mid
	music = load_midi("sounds/mainTheme.mid");

	// getting a coin
	SoundEffects[0] = load_sample("sounds/coin.wav"); 

	// hitting enemies 
	SoundEffects[1] = load_sample("sounds/death.wav"); 

	// winning 
	SoundEffects[2] = load_sample("sounds/princessmusic.wav");

	
	play_midi(music, 0);



	//Uisng MayppyWin32 saved fmp files as text files and converted them into bmp file using the following online convert
	//https://image.online-convert.com/convert-to-bmp

	//create bitmaps
	BITMAP* sky = load_bitmap("images/122.bmp", NULL);
	BITMAP* mario = load_bitmap("images/mario/mario.bmp", NULL);

	//settings for intro page
	BITMAP* day = load_bitmap("images/11.bmp", NULL);
	BITMAP* night = load_bitmap("images/12.bmp", NULL);

	//background sprite
	//enemies
	BITMAP* enemy1 = load_bitmap("images/ico2.bmp", NULL);
	BITMAP* enemy2 = load_bitmap("images/beetle_1.bmp", NULL);
	BITMAP* fireball1 = load_bitmap("images/firework1.bmp", NULL);
	BITMAP* fireball2 = load_bitmap("images/firework2.bmp", NULL);

	//coins 
	BITMAP* coin1 = load_bitmap("images/coin_0.bmp", NULL);
	BITMAP* bonus = load_bitmap("images/bonus.bmp", NULL);
	//BITMAP* mushroom = load_bitmap("images/mushroom.bmp", NULL);

	//flag 
	BITMAP* crown = load_bitmap("images/crown.bmp", NULL);
	

	//enemy for night mode 
	BITMAP* bridge = load_bitmap("images/bridge_1.bmp", NULL);
	

	//point to collect for night mode is stars
	BITMAP* star = load_bitmap("images/star_0.bmp", NULL);
	

	//Uisng MayppyWin32 saved fmp files as text files and converted them into bmp file using the following online convert
	//https://image.online-convert.com/convert-to-bmp

	/*
	//night
	if (MapLoad("nightmode.fmp") != 0)
	{
		allegro_message("Can't find nightmode.fmp");
		return 1;
	}

	//day
	if (MapLoad("daymode.fmp") != 0)
	{
		allegro_message("Can't find map1.fmp");
		return 1;
	}


	*/
	while (!key[KEY_ESC])
	{
		clearScreen();
		//updateGameInfo();
		mousex = mouse_x;
		mousey = mouse_y;
		leftClick = (mouse_b & 1);

		if (startScreenMode == 1)
		{

			loadBackgroundImage("images/super_mario_bros.bmp");

			textout_centre_ex(buffer, font, "To turn off/on the music PRESS CTRL+M", 320, 400, WHITE, BLACK);
			textout_centre_ex(buffer, font, "Dodge enemies, collect coins and capture the flag to win.", 320, 420, WHITE, BLACK);
			textout_centre_ex(buffer, font, "Press ENTER to begin", 320, 440, WHITE, BLACK);
			textout_centre_ex(buffer, font, "Press ESC to terminate", 320, 460, WHITE, BLACK);

			blit(buffer, screen, 0, 0, 0, 0, 640, 480);


			//by default musicMode = 0 which means music 
			if (key[KEY_LCONTROL] && key[KEY_M] || key[KEY_RCONTROL] && key[KEY_M])
			{
				//if music is currently on then turn it off
				if (musicMode == 1)
				{
					musicMode = 0;
					stop_midi();
				}
				

				//if music is currently off then turn it on 
				else if (musicMode == 0 )
				{
					musicMode = 1;
					music = load_midi("sounds/mainTheme.mid");
					play_midi(music, 0);
				}

			}

			if (startScreenMode == 1)
			{
				if (key[KEY_ENTER])
				{
					clearScreen();
					startScreenMode = 0;
					mainMenuMode = 1;

				}
			}
		}

		int lifeline = 5;

		if (mainMenuMode == 1)
		{
			textout_ex(sky, font, "MarioBros (ESC to quit)", 0, 1, BLACK, WHITE);
			textout_ex(sky, font, "Press Ctrl+H for Help", 460, 1, BLACK, WHITE);

			//help instruction s
			if (key[KEY_LCONTROL] && key[KEY_H] || key[KEY_RCONTROL] && key[KEY_H])
			{
				textout_ex(sky, font, "Moving Your Sprites: ", 180, 35, BLACK, WHITE);
				textout_ex(sky, font, " > to move right ", 180, 45, BLACK, WHITE);
				textout_ex(sky, font, " < to move left ", 180, 55, BLACK, WHITE);
				textout_ex(sky, font, " ^ to jump up", 180, 65, BLACK, WHITE);

				textout_ex(sky, font, " Lifelines will reduce upon colliding with enemies.", 130, 85, BLACK, WHITE);
			}

			
			//draw
			//Reference: https://rpg.hamsterrepublic.com/ohrrpgce/Free_Sprites
			set_trans_blender(225, 225, 255, 128);


			draw_sprite(buffers, sky, 0, 0);
			draw_sprite_ex(sky, mario, 385, 227, DRAW_SPRITE_NORMAL, DRAW_SPRITE_NO_FLIP);

			textout_ex(sky, font, "Game Mode", 485, 340, WHITE, BLACK);
			
					   			 
			//selection
			textout_centre_ex(sky, font, "Press F1: Day Mode", 540, 360, WHITE, BLACK);
			textout_centre_ex(sky, font, "Press F2: Night Mode", 540, 380, WHITE, BLACK);

			draw_sprite(screen, buffers, 0, 0);
			clear(buffers);

			//if selected F1: Day Mode
			if (key[KEY_F1])
			{
				mainMenuMode = 0;
				gamePlayMode = 1;
				clearScreens();
			}


			//if selected F2: Night Mode
			else if (key[KEY_F2])
			{

				mainMenuMode = 0;
				gamePlayMode = 2;
				clearScreens();

			}
			
		}



	
		//int lifeline = 5;

		if (gamePlayMode == 1)
		{
			//set buffer
			draw_sprite(buffers, day, 0, 0);


			//give animation to person ability to jump and move left/right
			int x = 10, y = 10;
			int velx, vely;
			velx = vely = 0;
			const int gravity = 1;
			int groundheight = 350;
			int screenScrollXPosition = 0;
			int width = SCREEN_W;
			int lifeline = 5;

			while (gamePlayMode == 1)
			{

				//need to clear 
				clearScreens();


				//aliens move around the planet (2-3) and player has to catch coins without crashing 
				draw_sprite(buffers, day, screenScrollXPosition, 0);

				//randomly put coins everywhere
				teta += 2.5;

				//coins worth 1 point 
				rotate_sprite(day, coin1, 50, 350, ftofix(teta));
				rotate_sprite(day, coin1, 100, 300, ftofix(teta));
				rotate_sprite(day, coin1, 150, 350, ftofix(teta));
				rotate_sprite(day, coin1, 200, 300, ftofix(teta));
				rotate_sprite(day, coin1, 250, 350, ftofix(teta));
				rotate_sprite(day, coin1, 300, 300, ftofix(teta));
				rotate_sprite(day, coin1, 350, 350, ftofix(teta));
				rotate_sprite(day, coin1, 400, 300, ftofix(teta));
				rotate_sprite(day, coin1, 450, 350, ftofix(teta));
				rotate_sprite(day, coin1, 500, 300, ftofix(teta));
				rotate_sprite(day, coin1, 550, 350, ftofix(teta));
				rotate_sprite(day, coin1, 600, 300, ftofix(teta));
				rotate_sprite(day, coin1, 650, 350, ftofix(teta));
				rotate_sprite(day, coin1, 700, 300, ftofix(teta)); 
				rotate_sprite(day, coin1, 750, 350, ftofix(teta));
				rotate_sprite(day, coin1, 800, 300, ftofix(teta));
				rotate_sprite(day, coin1, 850, 350, ftofix(teta));
				rotate_sprite(day, coin1, 900, 300, ftofix(teta));
				rotate_sprite(day, coin1, 950, 350, ftofix(teta));
				rotate_sprite(day, coin1, 1000, 300, ftofix(teta));

				rotate_sprite(day, coin1, 2000, 350, ftofix(teta));
				rotate_sprite(day, coin1, 2100, 300, ftofix(teta));
				rotate_sprite(day, coin1, 2200, 350, ftofix(teta));
				rotate_sprite(day, coin1, 2300, 300, ftofix(teta));
				rotate_sprite(day, coin1, 2400, 350, ftofix(teta));
				rotate_sprite(day, coin1, 2500, 300, ftofix(teta));
				rotate_sprite(day, coin1, 2600, 350, ftofix(teta));
				rotate_sprite(day, coin1, 2700, 300, ftofix(teta));
				rotate_sprite(day, coin1, 2800, 350, ftofix(teta));
				rotate_sprite(day, coin1, 2900, 300, ftofix(teta));
				rotate_sprite(day, coin1, 3000, 350, ftofix(teta));
				rotate_sprite(day, coin1, 4000, 300, ftofix(teta));
				rotate_sprite(day, coin1, 4100, 350, ftofix(teta));
				rotate_sprite(day, coin1, 4100, 300, ftofix(teta));
				rotate_sprite(day, coin1, 4200, 350, ftofix(teta));
				rotate_sprite(day, coin1, 4300, 300, ftofix(teta));
				rotate_sprite(day, coin1, 4400, 350, ftofix(teta));
				rotate_sprite(day, coin1, 4500, 300, ftofix(teta));
				rotate_sprite(day, coin1, 4800, 350, ftofix(teta));
				rotate_sprite(day, coin1, 4900, 300, ftofix(teta));


				//worth double the money
				/*
				rotate_sprite(day, mushroom, 75, 390, ftofix(0));
				rotate_sprite(day, mushroom, 745, 390, ftofix(0));
				rotate_sprite(day, mushroom, 956, 390, ftofix(0));
				rotate_sprite(day, mushroom, 1175, 390, ftofix(0));
				rotate_sprite(day, mushroom, 1375, 390, ftofix(0));
				rotate_sprite(day, mushroom, 1575, 390, ftofix(0));
				rotate_sprite(day, mushroom, 1775, 390, ftofix(0));
				rotate_sprite(day, mushroom, 1975, 390, ftofix(0));
				rotate_sprite(day, mushroom, 2175, 390, ftofix(0));
				rotate_sprite(day, mushroom, 2375, 390, ftofix(0));
				rotate_sprite(day, mushroom, 2575, 390, ftofix(0));
				rotate_sprite(day, mushroom, 2775, 390, ftofix(0));
				rotate_sprite(day, mushroom, 3075, 390, ftofix(0));
				rotate_sprite(day, mushroom, 3575, 390, ftofix(0));
				rotate_sprite(day, mushroom, 4075, 390, ftofix(0));
				rotate_sprite(day, mushroom, 5055, 390, ftofix(0));
				rotate_sprite(day, mushroom, 2475, 390, ftofix(0));
				rotate_sprite(day, mushroom, 3175, 390, ftofix(0));
				rotate_sprite(day, mushroom, 3875, 390, ftofix(0));
				*/


				// worth 15 points
				rotate_sprite(day, bonus, 400, 250, ftofix(0));
				rotate_sprite(day, bonus, 800, 250, ftofix(0));
				rotate_sprite(day, bonus, 1200, 250, ftofix(0));
				rotate_sprite(day, bonus, 1800, 250, ftofix(0));
				rotate_sprite(day, bonus, 2300, 250, ftofix(0));
				rotate_sprite(day, bonus, 2900, 250, ftofix(0));
				rotate_sprite(day, bonus, 3400, 250, ftofix(0));
				rotate_sprite(day, bonus, 4100, 250, ftofix(0));
				rotate_sprite(day, bonus, 4700, 250, ftofix(0));
				rotate_sprite(day, bonus, 5100, 250, ftofix(0));
				rotate_sprite(day, bonus, 5600, 250, ftofix(0));



				//randomly put enemies everywhere 
				//enemy type 1
				draw_sprite(day, enemy1, 123, 390);
				draw_sprite(day, enemy1, 234, 390);
				draw_sprite(day, enemy1, 445, 390);
				draw_sprite(day, enemy1, 676, 390);
				draw_sprite(day, enemy1, 885, 390);
				draw_sprite(day, enemy1, 1034, 390);
				draw_sprite(day, enemy1, 1645, 390);
				draw_sprite(day, enemy1, 2676, 390);
				draw_sprite(day, enemy1, 2999, 390);
				draw_sprite(day, enemy1, 3385, 390);
				draw_sprite(day, enemy1, 4145, 390);
				draw_sprite(day, enemy1, 4676, 390);



				//enemy type 2 
				draw_sprite(day, enemy2, 300, 385);
				draw_sprite(day, enemy2, 4000, 385);
				draw_sprite(day, enemy2, 3000, 385);




				//fireball1
				draw_sprite(day, fireball1, 181, 385);
				draw_sprite(day, fireball1, 345, 385);
				draw_sprite(day, fireball1, 2020, 385);
				draw_sprite(day, fireball1, 3030, 385);
				draw_sprite(day, fireball1, 4040, 385);
				draw_sprite(day, fireball1, 845, 385);
				draw_sprite(day, fireball1, 1485, 385);



				//fireball2
				draw_sprite(day, fireball2, 500, 385);
				draw_sprite(day, fireball2, 505, 385);
				draw_sprite(day, fireball2, 508, 385);
				draw_sprite(day, fireball2, 5000, 385);
				draw_sprite(day, fireball2, 5003, 385);
				draw_sprite(day, fireball2, 5005, 385);


		

				//deal with winning flag capture
				//collision with the flag and you win sign appears and goes to the next level....
				draw_sprite(day, crown, 6250, 350);


				
				

				if (key[KEY_ESC])
				{
					break;
				}

				

				//enemies collision
				//deal with collisions for coins
				// dealing with collision with coins 
				//add points and destroy the sprite 
				if (pixel_perfect_colision(x, y, mario, 50, 350, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 100, 300, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 150, 350, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 200, 300, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 250, 350, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 300, 300, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 350, 350, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 400, 300, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 450, 350, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 500, 300, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 550, 350, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 600, 300, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 650, 350, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 700, 300, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 750, 350, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 800, 300, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 850, 350, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 900, 300, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 950, 350, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 1000, 300, coin1) == TRUE)
				{
					points = points + 1;
					play_sample(SoundEffects[0], 225, 128, 1000, FALSE);

				}


			


				if (pixel_perfect_colision(x, y, mario, 2000, 350, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 2100, 300, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 2200, 350, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 2300, 300, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 2400, 350, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 2500, 300, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 2600, 350, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 2700, 300, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 2800, 350, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 2900, 300, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 3000, 350, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 4000, 300, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 4100, 350, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 4100, 300, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 4200, 350, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 4300, 300, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 4400, 350, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 4500, 300, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 4800, 350, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 4900, 300, coin1) == TRUE)
				{
					points = points + 1;
					play_sample(SoundEffects[0], 225, 128, 1000, FALSE);

				}


				/*
				if (pixel_perfect_colision(x, y, mario, 75, 390, mushroom) == TRUE ||
					pixel_perfect_colision(x, y, mario, 745, 390, mushroom) == TRUE ||
					pixel_perfect_colision(x, y, mario, 956, 390, mushroom) == TRUE ||
					pixel_perfect_colision(x, y, mario, 1175, 390, mushroom) == TRUE ||
					pixel_perfect_colision(x, y, mario, 1375, 390, mushroom) == TRUE ||
					pixel_perfect_colision(x, y, mario, 1575, 390, mushroom) == TRUE ||
					pixel_perfect_colision(x, y, mario, 1775, 390, mushroom) == TRUE ||
					pixel_perfect_colision(x, y, mario, 1975, 390, mushroom) == TRUE ||
					pixel_perfect_colision(x, y, mario, 2175, 390, mushroom) == TRUE ||
					pixel_perfect_colision(x, y, mario, 2375, 390, mushroom) == TRUE ||
					pixel_perfect_colision(x, y, mario, 2575, 390, mushroom) == TRUE ||
					pixel_perfect_colision(x, y, mario, 2775, 390, mushroom) == TRUE ||
					pixel_perfect_colision(x, y, mario, 3075, 390, mushroom) == TRUE ||
					pixel_perfect_colision(x, y, mario, 3575, 390, mushroom) == TRUE ||
					pixel_perfect_colision(x, y, mario, 4075, 390, mushroom) == TRUE ||
					pixel_perfect_colision(x, y, mario, 5055, 390, mushroom) == TRUE ||
					pixel_perfect_colision(x, y, mario, 2475, 390, mushroom) == TRUE ||
					pixel_perfect_colision(x, y, mario, 3175, 390, mushroom) == TRUE ||
					pixel_perfect_colision(x, y, mario, 3875, 390, mushroom) == TRUE )
				{
					points = points + 3; 
					play_sample(SoundEffects[0], 225, 128, 1000, FALSE);

				}
				*/
							   				 
				

			
				if (pixel_perfect_colision(x, y, mario, 400, 250, bonus) == TRUE ||
					pixel_perfect_colision(x, y, mario, 800, 250, bonus) == TRUE ||
					pixel_perfect_colision(x, y, mario, 1200, 250, bonus) == TRUE ||
					pixel_perfect_colision(x, y, mario, 1800, 250, bonus) == TRUE ||
					pixel_perfect_colision(x, y, mario, 2300, 250, bonus) == TRUE ||
					pixel_perfect_colision(x, y, mario, 2900, 250, bonus) == TRUE ||
					pixel_perfect_colision(x, y, mario, 3400, 250, bonus) == TRUE ||
					pixel_perfect_colision(x, y, mario, 3100, 250, bonus) == TRUE ||
					pixel_perfect_colision(x, y, mario, 4700, 250, bonus) == TRUE ||
					pixel_perfect_colision(x, y, mario, 5100, 250, bonus) == TRUE ||
					pixel_perfect_colision(x, y, mario, 5600, 250, bonus) == TRUE )
				{
					points = points + 15;
					play_sample(SoundEffects[0], 225, 128, 1000, FALSE);

				}




				//they as in who whom is he who 
				if (pixel_perfect_colision(x, y, mario, 123, 390, enemy1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 234, 390, enemy1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 445, 390, enemy1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 676, 390, enemy1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 885, 390, enemy1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 1034, 390, enemy1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 1645, 390, enemy1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 2676, 390, enemy1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 2999, 390, enemy1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 3385, 390, enemy1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 4145, 390, enemy1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 4676, 390, enemy1) == TRUE )
				{
					lifeline = lifeline - 1; 
					play_sample(SoundEffects[1], 225, 128, 1000, FALSE);
				}
				



				if (pixel_perfect_colision(x, y, mario, 300, 385, enemy2) == TRUE ||
					pixel_perfect_colision(x, y, mario, 4000, 385, enemy2) == TRUE ||
					pixel_perfect_colision(x, y, mario, 3000, 385, enemy2) == TRUE )
				{
					lifeline = lifeline - 1;
					play_sample(SoundEffects[1], 225, 128, 1000, FALSE);

				}




				if (pixel_perfect_colision(x, y, mario, 181, 385, fireball1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 345, 385, fireball1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 2020, 385, fireball1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 3030, 385, fireball1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 4040, 385, fireball1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 845, 385, fireball1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 1485, 385, fireball1) == TRUE)
				{
					lifeline = lifeline - 1;
					play_sample(SoundEffects[1], 225, 128, 1000, FALSE);

				}




				if (pixel_perfect_colision(x, y, mario, 500, 385, fireball2) == TRUE ||
					pixel_perfect_colision(x, y, mario, 505, 385, fireball2) == TRUE ||
					pixel_perfect_colision(x, y, mario, 508, 385, fireball2) == TRUE ||
					pixel_perfect_colision(x, y, mario, 5000, 385, fireball2) == TRUE ||
					pixel_perfect_colision(x, y, mario, 5003, 385, fireball2) == TRUE || 
					pixel_perfect_colision(x, y, mario, 5005, 385, fireball2) == TRUE )
				{
					lifeline = lifeline - 1;
					play_sample(SoundEffects[1], 225, 128, 1000, FALSE);

				}




				if (pixel_perfect_colision(x, y, mario, 6250, 350, crown) == TRUE)
				{
					win = 1; 

				}
			

				if (key[KEY_LEFT]) 
				{
					velx = -1;
					screenScrollXPosition = screenScrollXPosition + 5; 
					width = width - 5; 
				}


				else if (key[KEY_RIGHT]) 
				{
					velx = 1;
					screenScrollXPosition = screenScrollXPosition - 10;
					width = width + 10;
				}


				else
				{
					velx = 0;
				}
				
				if (key[KEY_UP] && y >= groundheight) 
				{
					vely = -15;	
					width = width + 30;
				}
				
				vely += gravity;
				x += velx;
				y += vely;
				

				if (y >= groundheight)
				{
					y = groundheight;
				}
				
				draw_sprite(buffers, mario, x, y); 
				rest(30);

		
				draw_sprite(screen, buffers, 0, 0);

			



				//ESCAPE to quit
				textout_ex(day, font, "MarioBros (ESC to quit)", 10, 1, BLACK, WHITE);
				textprintf_ex(day, font, 10, 15, BLACK, WHITE, " Health: %i", lifeline);
				textprintf_ex(day, font, 10, 30, BLACK, WHITE, " Points: %i", points);


				if (lifeline == 0)
				{
					textout_ex(day, font, "Game Over. Press ESC and Try Again.", SCREEN_W/2, 1, BLACK, WHITE);
					textout_ex(day, font, "MarioBros (ESC to quit)", SCREEN_W / 2, 1, BLACK, WHITE);
					textprintf_ex(day, font, x, y-15, BLACK, WHITE, " Health: %i", lifeline);
					textprintf_ex(day, font, x, y-30, BLACK, WHITE, " Points: %i", points);

					gamePlayMode = 0;
				}

							   				 
				if (win == 1)
				{
					textout_ex(day, font, "You won. Press ESC and Play Again.", SCREEN_W / 2, 1, BLACK, WHITE);
					textout_ex(day, font, "MarioBros (ESC to quit)", SCREEN_W / 2, 1, BLACK, WHITE);
					textprintf_ex(day, font, SCREEN_W / 2, 15, BLACK, WHITE, " Health: %i", lifeline);
					textprintf_ex(day, font, SCREEN_W / 2, 30, BLACK, WHITE, " Points: %i", points);

					//play winning music 
					play_sample(SoundEffects[2], 225, 128, 1000, FALSE);

					gamePlayMode = 0;
				}

			
			}



		}

		if (gamePlayMode == 0)
		{
			//clearScreens();
			//mainMenuMode == 1;
			gamePlayMode == 2;

		}
		

		if (gamePlayMode == 2)
		{
			clearScreens();

			//set buffer
			draw_sprite(buffers, night, 0, 0);


			//give animation to person ability to jump and move left/right
			int x = 10, y = 10;
			int velx, vely;
			velx = vely = 0;
			const int gravity = 1;
			int groundheight = 350;
			int screenScrollXPosition = 0;
			int x1 = 310;
			int x2 = 400;
			int width = SCREEN_W;
			int lifeline = 5;


			while (gamePlayMode == 2)
			{

				//need to clear 
				clearScreens();


				//aliens move around the planet (2-3) and player has to catch coins without crashing 
				draw_sprite(buffers, night, screenScrollXPosition, 0);

				//randomly put coins everywhere
				teta += 0.5;

				//coins worth 1 point 
				rotate_sprite(night, star, 50, 350, ftofix(teta));
				rotate_sprite(night, star, 100, 300, ftofix(teta));
				rotate_sprite(night, star, 150, 350, ftofix(teta));
				rotate_sprite(night, star, 200, 300, ftofix(teta));
				rotate_sprite(night, star, 250, 350, ftofix(teta));
				rotate_sprite(night, star, 300, 300, ftofix(teta));
				rotate_sprite(night, star, 350, 350, ftofix(teta));
				rotate_sprite(night, star, 400, 300, ftofix(teta));
				rotate_sprite(night, star, 450, 350, ftofix(teta));
				rotate_sprite(night, star, 500, 300, ftofix(teta));
				rotate_sprite(night, star, 550, 350, ftofix(teta));
				rotate_sprite(night, star, 600, 300, ftofix(teta));
				rotate_sprite(night, star, 650, 350, ftofix(teta));
				rotate_sprite(night, star, 700, 300, ftofix(teta));
				rotate_sprite(night, star, 750, 350, ftofix(teta));
				rotate_sprite(night, star, 800, 300, ftofix(teta));
				rotate_sprite(night, star, 850, 350, ftofix(teta));
				rotate_sprite(night, star, 900, 300, ftofix(teta));
				rotate_sprite(night, star, 950, 350, ftofix(teta));
				rotate_sprite(night, star, 1000, 300, ftofix(teta));

				rotate_sprite(night, star, 2000, 350, ftofix(teta));
				rotate_sprite(night, star, 2100, 300, ftofix(teta));
				rotate_sprite(night, star, 2200, 350, ftofix(teta));
				rotate_sprite(night, star, 2300, 300, ftofix(teta));
				rotate_sprite(night, star, 2400, 350, ftofix(teta));
				rotate_sprite(night, star, 2500, 300, ftofix(teta));
				rotate_sprite(night, star, 2600, 350, ftofix(teta));
				rotate_sprite(night, star, 2700, 300, ftofix(teta));
				rotate_sprite(night, star, 2800, 350, ftofix(teta));
				rotate_sprite(night, star, 2900, 300, ftofix(teta));
				rotate_sprite(night, star, 3000, 350, ftofix(teta));
				rotate_sprite(night, star, 4000, 300, ftofix(teta));
				rotate_sprite(night, star, 4100, 350, ftofix(teta));
				rotate_sprite(night, star, 4100, 300, ftofix(teta));
				rotate_sprite(night, star, 4200, 350, ftofix(teta));
				rotate_sprite(night, star, 4300, 300, ftofix(teta));
				rotate_sprite(night, star, 4400, 350, ftofix(teta));
				rotate_sprite(night, star, 4500, 300, ftofix(teta));
				rotate_sprite(night, star, 4800, 350, ftofix(teta));
				rotate_sprite(night, star, 4900, 300, ftofix(teta));


			
				//rotators enemy
				beta += 1.5;

				rotate_sprite(night, bridge, 400, 150, ftofix(beta));
				rotate_sprite(night, bridge, 800, 150, ftofix(beta));
				rotate_sprite(night, bridge, 1200, 150, ftofix(beta));
				rotate_sprite(night, bridge, 1800, 150, ftofix(beta));
				rotate_sprite(night, bridge, 2300, 150, ftofix(beta));
				rotate_sprite(night, bridge, 2900, 150, ftofix(beta));
				rotate_sprite(night, bridge, 3400, 150, ftofix(beta));
				rotate_sprite(night, bridge, 4100, 150, ftofix(beta));
				rotate_sprite(night, bridge, 4700, 100, ftofix(beta));
			

				
		
				//deal with winning flag capture
				//collision with the flag and you win sign appears and goes to the next level....
				draw_sprite(night, crown, 4800, 350);





				if (key[KEY_ESC])
				{
					break;
				}



				//enemies collision
				//deal with collisions for coins
				// dealing with collision with coins 
				//add points and destroy the sprite 
				if (pixel_perfect_colision(x, y, mario, 50, 350, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 100, 300, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 150, 350, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 200, 300, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 250, 350, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 300, 300, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 350, 350, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 400, 300, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 450, 350, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 500, 300, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 550, 350, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 600, 300, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 650, 350, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 700, 300, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 750, 350, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 800, 300, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 850, 350, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 900, 300, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 950, 350, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 1000, 300, coin1) == TRUE)
				{

					points = points + 1;
					play_sample(SoundEffects[0], 225, 128, 1000, FALSE);

				}





				if (pixel_perfect_colision(x, y, mario, 2000, 350, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 2100, 300, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 2200, 350, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 2300, 300, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 2400, 350, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 2500, 300, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 2600, 350, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 2700, 300, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 2800, 350, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 2900, 300, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 3000, 350, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 4000, 300, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 4100, 350, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 4100, 300, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 4200, 350, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 4300, 300, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 4400, 350, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 4500, 300, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 4800, 350, coin1) == TRUE ||
					pixel_perfect_colision(x, y, mario, 4900, 300, coin1) == TRUE)
				{

					points = points + 1;
					play_sample(SoundEffects[0], 225, 128, 1000, FALSE);
				}


											   
				
				//getting the crown 
				if (pixel_perfect_colision(x, y, mario, 400, 150, bridge) == TRUE ||
					pixel_perfect_colision(x, y, mario, 800, 150, bridge) == TRUE ||
					pixel_perfect_colision(x, y, mario, 1200, 150, bridge) == TRUE ||
					pixel_perfect_colision(x, y, mario, 1800, 150, bridge) == TRUE ||
					pixel_perfect_colision(x, y, mario, 2300, 150, bridge) == TRUE ||
					pixel_perfect_colision(x, y, mario, 2900, 150, bridge) == TRUE ||
					pixel_perfect_colision(x, y, mario, 3400, 150, bridge) == TRUE ||
					pixel_perfect_colision(x, y, mario, 4100, 150, bridge) == TRUE || 
					pixel_perfect_colision(x, y, mario, 4700, 100, bridge) == TRUE )
				{
					
					lifeline = lifeline - 1;
					play_sample(SoundEffects[1], 225, 128, 1000, FALSE);
				}


			

				if (pixel_perfect_colision(x, y, mario, 4800, 350, crown) == TRUE)
				{
					win = 1;
				}


				if (key[KEY_LEFT])
				{
					velx = -1;
					screenScrollXPosition = screenScrollXPosition + 5;
					width = width - 5;
				}


				else if (key[KEY_RIGHT])
				{
					velx = 1;
					screenScrollXPosition = screenScrollXPosition - 10;
					width = width + 10;
				}


				else
				{
					velx = 0;
				}

				if (key[KEY_UP] && y >= groundheight)
				{
					vely = -15;
					width = width + 30;
				}

				vely += gravity;
				x += velx;
				y += vely;


				if (y >= groundheight)
				{
					y = groundheight;
				}

				draw_sprite(buffers, mario, x, y);
				rest(60);


				draw_sprite(screen, buffers, 0, 0);





				//ESCAPE to quit
				textout_ex(night, font, "MarioBros (ESC to quit)", 10, 1, BLACK, WHITE);
				textprintf_ex(night, font, 10, 15, BLACK, WHITE, " Health: %i", lifeline);
				textprintf_ex(night, font, 10, 30, BLACK, WHITE, " Points: %i", points);		


				if (lifeline == 0)
				{
					textout_ex(day, font, "Game Over. Press ESC and Try Again.", SCREEN_W / 2, 1, BLACK, WHITE);
					textout_ex(day, font, "MarioBros (ESC to quit)", SCREEN_W / 2, 1, BLACK, WHITE);
					textprintf_ex(day, font, SCREEN_W / 2, 15, BLACK, WHITE, " Health: %i", lifeline);
					textprintf_ex(day, font, SCREEN_W / 2, 30, BLACK, WHITE, " Points: %i", points);

					// play losing music
					//play_sample(SoundEffects[3], 225, 128, 1000, FALSE);
					break;
				}


				if (win == 1)
				{
					textout_ex(day, font, "You won. Press ESC and Play Again.", SCREEN_W / 2, 1, BLACK, WHITE);
					textout_ex(day, font, "MarioBros (ESC to quit)", SCREEN_W / 2, 1, BLACK, WHITE);
					textprintf_ex(day, font, SCREEN_W / 2, 15, BLACK, WHITE, " Health: %i", lifeline);
					textprintf_ex(day, font, SCREEN_W / 2, 30, BLACK, WHITE, " Points: %i", points);

					//play winning music 	
					play_sample(SoundEffects[2], 225, 128, 1000, FALSE);
					break;
				}


				//draw map with single layer
				//MapDrawBG(buffers, 0, 0, 0, 0, SCREEN_W - 1, SCREEN_H - 1);

				blit(buffers, screen, 0, 0, 0, 0, SCREEN_W - 1, SCREEN_H - 1);

				//fix collisions 
				//fix endings and thats it 
			}


		}
	}

	
	stop_midi();
	destroy_midi(music);
	remove_sound();
	set_mouse_sprite(NULL);
	//MapFreeMem();



	//destroy
	destroy_bitmap(buffer);
	destroy_bitmap(mario);
	destroy_bitmap(sky);
	destroy_bitmap(enemy1);
	destroy_bitmap(enemy2);
	destroy_bitmap(buffers);
	destroy_bitmap(coin1);

	

	allegro_exit();
}
END_OF_MAIN()


