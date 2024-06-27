#include <iostream>
#include <random>
#include <string>
#include <thread>
#include <chrono>
#include <vector>
#include <bits/stdc++.h> 

#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_ttf.h>

//GAME SETTINGS
const int SCREEN_WIDTH = 960;
const int SCREEN_HEIGHT = 720;
const int SEGMENT_SIZE = 40;
int playfield_width = 12;
int playfield_height = 18;
const int LINE_ANIMATION_TIME = 40;

//SDL SCREEN 

//The window we'll be rendering to
SDL_Window* window = NULL;
//The surface contained by the window
SDL_Surface* screenSurface = NULL;

SDL_Renderer *renderer = NULL;

TTF_Font* Sans = NULL;


//GAME AREAS
unsigned char *playfield = nullptr;
unsigned char *screen = nullptr;

std::string shapes[7];

//ROTATION FUNCTION
int rotate(int piece_size, int piece_x, int piece_y, int rotation){
	int piece_index = 0;
	if (piece_size == 1) {
		// rotation = 0 degrees
		if (rotation % 4 == 0){
			piece_index = piece_y * 4 + piece_x;
		}
		// rotation = 90 degrees
		else if (rotation % 4 == 1){
			piece_index = 12 + piece_y - (piece_x * 4);
		}
		// rotation = 180 degrees
		else if (rotation % 4 == 2){
			piece_index = 15 - (piece_y * 4) - piece_x;
		}
		// rotation = 270 degrees
		else if (rotation % 4 == 3){
			piece_index = 3 - piece_y + (piece_x * 4);
		}
	}
	else {
		// rotation = 0 degrees
		if (rotation % 4 == 0){
			piece_index = piece_y * 3 + piece_x;
		}
		// rotation = 90 degrees
		else if (rotation % 4 == 1){
			piece_index = 6 + piece_y - (piece_x * 3);
		}
		// rotation = 180 degrees
		else if (rotation % 4 == 2){
			piece_index = 8 - (piece_y * 3) - piece_x;
		}
		// rotation = 270 degrees
		else if (rotation % 4 == 3){
			piece_index = 2 - piece_y + (piece_x * 3);
		}
	}
	return piece_index;
}
//COLLISION CHECK FUNCTION
bool check_for_no_collision(int piece_size, int piece, int rotation_a, int piece_a, int piece_b)
{
	if (piece_size == 1) {
		for (int piece_x = 0; piece_x < 4; piece_x++){
			for (int piece_y = 0; piece_y < 4; piece_y++){
				int piece_index = rotate(piece_size, piece_x, piece_y, rotation_a);
				int field_index = (piece_b + piece_y) * playfield_width + (piece_a + piece_x);
				if (piece_a + piece_x >= 0 && piece_a + piece_x < playfield_width)
				{
					if (piece_b + piece_y >= 0 && piece_b + piece_y < playfield_height)
					{
						if (shapes[piece][piece_index] != '0' && screen[field_index] != 0){
							return false;
		                }
					}
				}
			}
		}
	}
	else {
		for (int piece_x = 0; piece_x < 3; piece_x++){
			for (int piece_y = 0; piece_y < 3; piece_y++){
				int piece_index = rotate(piece_size, piece_x, piece_y, rotation_a);
				int field_index = (piece_b + piece_y) * playfield_width + (piece_a + piece_x);
				if (piece_a + piece_x >= 0 && piece_a + piece_x < playfield_width)
				{
					if (piece_b + piece_y >= 0 && piece_b + piece_y < playfield_height)
					{
						if (shapes[piece][piece_index] != '0' && screen[field_index] != 0){
							return false;
		                }
					}
				}
			}
		}
	}
	return true;
}
int current_piece;
int current_piece_size;
int next_piece;
int next_piece_size;
int current_rotation;
int current_x;
int current_y;
int pity;

//DISPLAY
auto print_screen() {
//APPLY PIECE
	if (current_piece_size == 1) {
		for (int piece_x = 0; piece_x < 4; piece_x++){
		    for (int piece_y = 0; piece_y < 4; piece_y++){
				if (shapes[current_piece][rotate(current_piece_size, piece_x, piece_y, current_rotation)] != '0'){
				    playfield[(current_y + piece_y)*playfield_width + (current_x + piece_x)] = current_piece + 1;
		        }
		    }
		}
	}
	else {
		for (int piece_x = 0; piece_x < 3; piece_x++){
		    for (int piece_y = 0; piece_y < 3; piece_y++){
				if (shapes[current_piece][rotate(current_piece_size, piece_x, piece_y, current_rotation)] != '0'){
				    playfield[(current_y + piece_y)*playfield_width + (current_x + piece_x)] = current_piece + 1;
		        }
		    }
		}
	}
//BACKGROUND
	SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));
	
//TMP RECT
	SDL_Rect segment;
	segment.x = 0;
	segment.y = 0;
	segment.w = SEGMENT_SIZE;
	segment.h = SEGMENT_SIZE;

//RENDER  SCREEN
    for(int i = 0; i <= playfield_width*playfield_height; i++){
        if(i % playfield_width == playfield_width - 1){
			segment.x = (playfield_width - 1)*SEGMENT_SIZE;
			segment.y = ((i - playfield_width - 1) / playfield_width)*SEGMENT_SIZE;
        	SDL_FillRect(screenSurface, &segment, SDL_MapRGB(screenSurface->format, 0x00, 0x00, 0x00));
        }
		else if(i == playfield_width*playfield_height){
			segment.x = (playfield_width - 1)*SEGMENT_SIZE;
			segment.y = (playfield_height - 1)*SEGMENT_SIZE;
        	SDL_FillRect(screenSurface, &segment, SDL_MapRGB(screenSurface->format, 0x00, 0x00, 0x00));
        }
        else{
            if(screen[i] == 9){
                segment.x = (i % playfield_width)*SEGMENT_SIZE;
				segment.y = ((i - (i % playfield_width)) / playfield_width)*SEGMENT_SIZE;
	        	SDL_FillRect(screenSurface, &segment, SDL_MapRGB(screenSurface->format, 0x00, 0x00, 0x00));
            }
            else if(playfield[i] == 1){
                segment.x = (i % playfield_width)*SEGMENT_SIZE;
				segment.y = ((i - (i % playfield_width)) / playfield_width)*SEGMENT_SIZE;
	        	SDL_FillRect(screenSurface, &segment, SDL_MapRGB(screenSurface->format, 0xFF, 0x00, 0x00));
            }
            else if(playfield[i] == 2){
                segment.x = (i % playfield_width)*SEGMENT_SIZE;
				segment.y = ((i - (i % playfield_width)) / playfield_width)*SEGMENT_SIZE;
	        	SDL_FillRect(screenSurface, &segment, SDL_MapRGB(screenSurface->format, 0x00, 0xFF, 0x00));
            }
            else if(playfield[i] == 3){
                segment.x = (i % playfield_width)*SEGMENT_SIZE;
				segment.y = ((i - (i % playfield_width)) / playfield_width)*SEGMENT_SIZE;
	        	SDL_FillRect(screenSurface, &segment, SDL_MapRGB(screenSurface->format, 0x00, 0x00, 0xFF));
            }
            else if(playfield[i] == 4){
                segment.x = (i % playfield_width)*SEGMENT_SIZE;
				segment.y = ((i - (i % playfield_width)) / playfield_width)*SEGMENT_SIZE;
	        	SDL_FillRect(screenSurface, &segment, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0x00));
            }
            else if(playfield[i] == 5){
                segment.x = (i % playfield_width)*SEGMENT_SIZE;
				segment.y = ((i - (i % playfield_width)) / playfield_width)*SEGMENT_SIZE;
	        	SDL_FillRect(screenSurface, &segment, SDL_MapRGB(screenSurface->format, 0xFF, 0x00, 0xFF));
            }
            else if(playfield[i] == 6){
                segment.x = (i % playfield_width)*SEGMENT_SIZE;
				segment.y = ((i - (i % playfield_width)) / playfield_width)*SEGMENT_SIZE;
	        	SDL_FillRect(screenSurface, &segment, SDL_MapRGB(screenSurface->format, 0x00, 0xFF, 0xFF));
            }
            else if(playfield[i] == 7){
                segment.x = (i % playfield_width)*SEGMENT_SIZE;
				segment.y = ((i - (i % playfield_width)) / playfield_width)*SEGMENT_SIZE;
	        	SDL_FillRect(screenSurface, &segment, SDL_MapRGB(screenSurface->format, 0xFF, 0xC0, 0x00));
            }
            else if(playfield[i] >= 10){
                segment.x = (i % playfield_width)*SEGMENT_SIZE;
				segment.y = ((i - (i % playfield_width)) / playfield_width)*SEGMENT_SIZE;
	        	SDL_FillRect(screenSurface, &segment, SDL_MapRGB(screenSurface->format, playfield[i], playfield[i], playfield[i]));
            }
            else{
            	segment.x = (i % playfield_width)*SEGMENT_SIZE;
				segment.y = ((i - (i % playfield_width)) / playfield_width)*SEGMENT_SIZE;
	        	SDL_FillRect(screenSurface, &segment, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));
            }
        }
    }
	if (next_piece_size == 1) {
		for (int piece_x = 0; piece_x < 4; piece_x++){
		    for (int piece_y = 0; piece_y < 4; piece_y++){
				if (shapes[next_piece][rotate(next_piece_size, piece_x, piece_y, 0)] != '0'){
					segment.x = (playfield_width+2+piece_x)*SEGMENT_SIZE;
					segment.y = (2+piece_y)*SEGMENT_SIZE;
					SDL_FillRect(screenSurface, &segment, SDL_MapRGB(screenSurface->format, 0x00, 0x00, 0x00));
				}
		    }
		}
	}
	else {
		for (int piece_x = 0; piece_x < 3; piece_x++){
		    for (int piece_y = 0; piece_y < 3; piece_y++){
				if (shapes[next_piece][rotate(next_piece_size, piece_x, piece_y, 0)] != '0'){
					segment.x = (playfield_width+2+piece_x)*SEGMENT_SIZE;
					segment.y = (2+piece_y)*SEGMENT_SIZE;
					SDL_FillRect(screenSurface, &segment, SDL_MapRGB(screenSurface->format, 0x00, 0x00, 0x00));
				}
		    }
		}
	}  

	SDL_Texture* screen = SDL_CreateTextureFromSurface(renderer, screenSurface);
	segment.x = 0;
	segment.y = 0;
	segment.w = SCREEN_WIDTH;
	segment.h = SCREEN_HEIGHT;
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, screen, NULL, &segment);

	SDL_Color black = {0, 0, 0, 0};
// as TTF_RenderText_Solid could only be used on
// SDL_Surface then you have to create the surface first
	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans, "Your score ", black);
	if (surfaceMessage == NULL) {
//broken
        //printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
    }

// now you can convert it into a texture
	SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
	segment.x = (playfield_width+2)*SEGMENT_SIZE;
	segment.y = SEGMENT_SIZE;
	segment.w = 100;
	segment.h = 30;
	SDL_RenderCopy(renderer, Message, NULL, &segment);

	SDL_RenderPresent(renderer);

// Don't forget to free your surface and texture
	SDL_DestroyTexture(screen);
	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(Message);

}
int main(int argc, char* args[]) {

//INITIALIZE SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    }
 	else{
//CREATE WINDOW
        window = SDL_CreateWindow("TETRIS", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if(window == NULL){
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        }
 		else{
//GET WINDOW SURFACE
			screenSurface = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0, 0, 0, 0);

			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
			if (renderer == NULL) {
				printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
				return false;
			}
//broken
			//TTF_Init();

//this opens a font style and sets a size
			TTF_Font* Sans = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeSans.ttf", 24);
			if (Sans == NULL) {
//broken
				//printf("Error loading font: %s", TTF_GetError());
			}
			
//EVENT HANDLER
            SDL_Event e;

//SHAPES
//shape I
			shapes[0].append("0100");
			shapes[0].append("0100");
			shapes[0].append("0100");
			shapes[0].append("0100");
//shape L
			shapes[1].append("010");
			shapes[1].append("010");
			shapes[1].append("011");
//shape J
			shapes[2].append("010");
			shapes[2].append("010");
			shapes[2].append("110");
//shape T
			shapes[3].append("010");
			shapes[3].append("110");
			shapes[3].append("010");
//shape S
			shapes[4].append("100");
			shapes[4].append("110");
			shapes[4].append("010");
//shape Z
			shapes[5].append("010");
			shapes[5].append("110");
			shapes[5].append("100");
//shape O
			shapes[6].append("0000");
			shapes[6].append("0110");
			shapes[6].append("0110");
			shapes[6].append("0000");

//PLAYFIELD & SCREEN
			playfield = new unsigned char[playfield_width*playfield_height];
			for(int i = 0; i < playfield_height; i++){
				for(int j = 0; j < playfield_width; j++){
				    if(j == 0){
				        playfield[i*playfield_width + j] = 9;
				    }
				    else if(i == playfield_height - 1){
				        playfield[i*playfield_width + j] = 9;
				    }
				    else if(j == playfield_width -1){
				        playfield[i*playfield_width + j] = 9;
				    }
				    else{
				        playfield[i*playfield_width + j] = 0;
				    }
				}
			}
			screen = new unsigned char[playfield_width*playfield_height];
			for(int i = 0; i < playfield_height; i++){
				for(int j = 0; j < playfield_width; j++){
				    if(j == 0){
				        screen[i*playfield_width + j] = 9;
				    }
				    else if(i == playfield_height - 1){
				        screen[i*playfield_width + j] = 9;
				    }
				    else if(j == playfield_width -1){
				        screen[i*playfield_width + j] = 9;
				    }
				    else{
				        screen[i*playfield_width + j] = 0;
				    }
				}
			}

//INITIAL VALUES
			std::random_device rd;
	        std::uniform_int_distribution<int> d7 (0, 6);
	        current_piece = d7(rd);
	        next_piece = d7(rd);
			pity = 0;
			if (current_piece == 0) {
				current_piece_size = 1;
			}
			else if (current_piece == 6) {
				current_piece_size = 1;
			}
			else {
				current_piece_size = 0;
			}  
			if (next_piece == 0) {
				next_piece_size = 1;
			}
			else if (next_piece == 6) {
				next_piece_size = 1;
			}
			else {
				next_piece_size = 0;
			}  
			current_rotation = 0;
			current_x = playfield_width / 2;
			current_y = 0;

//INITIAL TIMING VALUES
			int game_difficulty = 20;
			int game_speed = 0; 
			bool gravity = false;
			int piece_count = 0;
			int score = 0;
			std::vector<int> current_lines;
			int current_lines_lifetime = 0;

//FIRST SCREEN
			print_screen();
//GAME LOOP
			bool game_over = false;
			while (game_over == false) {
//TIMING
				if (game_speed == game_difficulty) {
				    gravity = true;
				    game_speed = 0; 
				}
//CONTROLS
				while(SDL_PollEvent(&e) != 0){
                    //User requests quit
                    if(e.type == SDL_QUIT){
                        game_over = true;
                    }
                    //User presses a key
                    else if(e.type == SDL_KEYDOWN){
                        //Select surfaces based on key press
                        switch(e.key.keysym.sym){
                            case SDLK_UP:
		                        if (check_for_no_collision(current_piece_size, current_piece, current_rotation + 1, current_x, current_y) == true) {
									current_rotation = current_rotation + 1;
									for(int i = 0; i < playfield_height; i++){
										for(int j = 0; j < playfield_width; j++){
											playfield[i*playfield_width + j] = screen[i*playfield_width + j];
										}
									}
									print_screen();
								}
								else {
						        	print_screen();
								}
		                        break;

                            case SDLK_DOWN:
								if (check_for_no_collision(current_piece_size, current_piece, current_rotation, current_x, current_y + 1) == true) {
					    			current_y = current_y + 1;
				        			for(int i = 0; i < playfield_height; i++){
										for(int j = 0; j < playfield_width; j++){
											playfield[i*playfield_width + j] = screen[i*playfield_width + j];
										}
									}
									print_screen();
								}
								else {
						        	print_screen();
								}
                            	break;

                            case SDLK_LEFT:
		                        if (check_for_no_collision(current_piece_size, current_piece, current_rotation, current_x - 1, current_y) == true) {
									current_x = current_x - 1;
									for(int i = 0; i < playfield_height; i++){
										for(int j = 0; j < playfield_width; j++){
											playfield[i*playfield_width + j] = screen[i*playfield_width + j];
										}
									}
									print_screen();
								}
								else {
						        	print_screen();
								}
		                        break;

                            case SDLK_RIGHT:
								if (check_for_no_collision(current_piece_size, current_piece, current_rotation, current_x + 1, current_y) == true) {
							    	current_x = current_x + 1;
								    for(int i = 0; i < playfield_height; i++){
										for(int j = 0; j < playfield_width; j++){
											playfield[i*playfield_width + j] = screen[i*playfield_width + j];
										}
									}
									print_screen();
							    }
								else {
						        	print_screen();
								}
    	                        break;

                            default:
						        print_screen();
                            	break;
                        }
                    }
                }
				
				std::this_thread::sleep_for(std::chrono::milliseconds(50));

//PIECE DROP
				if (gravity == true) {
					if (check_for_no_collision(current_piece_size, current_piece, current_rotation, current_x, current_y + 1) == true) {
				    	current_y = current_y + 1;
						for(int i = 0; i < playfield_height; i++){
							for(int j = 0; j < playfield_width; j++){
								playfield[i*playfield_width + j] = screen[i*playfield_width + j];
							}
						}
				        print_screen();
				    }
				    else {
//SETTLE PIECE - UPDATE SCREEN
						if (current_piece_size == 1) {
						    for (int piece_x = 0; piece_x < 4; piece_x++){
						        for (int piece_y = 0; piece_y < 4; piece_y++){
									if (shapes[current_piece][rotate(current_piece_size, piece_x, piece_y, current_rotation)] != '0'){
									    screen[(current_y + piece_y)*playfield_width + (current_x + piece_x)] = current_piece + 1;
						            }
						        }
						    }
						}
						else {
							for (int piece_x = 0; piece_x < 3; piece_x++){
						        for (int piece_y = 0; piece_y < 3; piece_y++){
									if (shapes[current_piece][rotate(current_piece_size, piece_x, piece_y, current_rotation)] != '0'){
									    screen[(current_y + piece_y)*playfield_width + (current_x + piece_x)] = current_piece + 1;
						            }
						        }
						    }
						}
//FULL LINES
				        for (int piece_y = 0; piece_y < 4; piece_y++) {
				            bool full_line = true;
				            if (current_y + piece_y < playfield_height - 1) {
				                for (int piece_x = 1; piece_x < playfield_width - 1; piece_x++) {
				                    if (screen[(current_y + piece_y) * playfield_width + piece_x] == 0) {
				                        full_line = false;
				                    }

				                }
								for (int piece_x = 1; piece_x < playfield_width - 1; piece_x++) {
				                    if (screen[(current_y + piece_y) * playfield_width + piece_x] >= 10) {
				                        full_line = false;
				                    }

				                }
				                if (full_line == true) {
				                    for (int piece_x = 1; piece_x < playfield_width - 1; piece_x++) {
				                        screen[(current_y + piece_y) * playfield_width + piece_x] = 10;
				                    }
				                    current_lines.push_back(current_y + piece_y);
				                }
				            }
				        }
//SCORE
				        score = score + 25;
//choose new piece - check
				        current_piece = next_piece;
						current_piece_size = next_piece_size;
				        next_piece = d7(rd);
						if (pity > (playfield_width/2)*3) {
							next_piece = 0;
						}
						if (next_piece == 0) {
							next_piece_size = 1;
							pity = 0;
						}
						else if (next_piece == 6) {
							next_piece_size = 1;
							pity++;
						}
						else {
							next_piece_size = 0;
							pity++;
						}  
				        current_rotation = 0;
				        current_x = playfield_width / 2;
				        current_y = 0;
				        if (check_for_no_collision(current_piece_size, current_piece, current_rotation, current_x, current_y) == false) {
				            game_over = true;
				        }
						print_screen();
//CLEAR PLAYFIELD
				        for(int i = 0; i < playfield_height; i++){
				            for(int j = 0; j < playfield_width; j++){
				                playfield[i*playfield_width + j] = screen[i*playfield_width + j];
								if (screen[i*playfield_width + j] >= 10) {
									screen[i*playfield_width + j] = screen[i*playfield_width + j] + 6;
								}
				            }
				        }
				        
				        game_speed = 0;
				        piece_count++;
				        if (piece_count % 15 == 0) {
				            if (game_difficulty > 5) {
				                game_difficulty--;
				            }
				        }
				    }
				    gravity = false;
				}
//CLEAR PLAYFIELD
				for(int i = 0; i < playfield_height; i++){
				    for(int j = 0; j < playfield_width; j++){
				        playfield[i*playfield_width + j] = screen[i*playfield_width + j];
						if (screen[i*playfield_width + j] >= 10) {
							screen[i*playfield_width + j] = screen[i*playfield_width + j] + 6;
						}
				    }
				}

//REMOVE FULL LINES
		        if (current_lines.empty() == false) {
					current_lines_lifetime++;
					if (current_lines_lifetime == LINE_ANIMATION_TIME) {
						sort(current_lines.begin(), current_lines.end());
				        for (auto current_line : current_lines) {
				            for (int piece_x = 1; piece_x < playfield_width - 1; piece_x++) {
				                for (int piece_y = current_line; piece_y > 0; piece_y--) {
				                    screen[piece_y * playfield_width + piece_x] = screen[(piece_y - 1) * playfield_width + piece_x];
				                }
				                screen[piece_x] = 0;
				            }
							printf("Line %d removed, lines above lowered\n", current_line);
				        }
						current_lines_lifetime = 0;
//CLEAR STORED ROWS
				        score = score + (current_lines.size() * current_lines.size() * 25);
						current_lines.clear();
						print_screen();
					}
		        }
				print_screen();
				game_speed++;
			}
			std::cout << "Game over! Your score was: " << score << "\n";
        }
    }
//Destroy window
    SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);

//Quit SDL subsystems
	TTF_CloseFont(Sans);
	TTF_Quit();
    SDL_Quit();

    return 0;
}
