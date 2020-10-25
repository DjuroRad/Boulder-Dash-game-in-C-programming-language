
#define LIVES_START 3
#define PADDING_TOP 80
#define PADDING_LEFT 60
#define tileWidth 45
#define tileHeight 45
#define clippedTileWidth 45
#define clippedTileHeight 45
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#define NUM_OF_ELEMENTS 12
typedef enum{ soil, player, diamond, wall, rock, hole, monster, spider, water, border, door, explosion }Element;
typedef enum{ unable, able, falling }freeFallAbility;
typedef enum{false, true} bool;

int quit();
int start_SDL();
int init_TTF();
int init_IMG( SDL_Surface** windowSurface, SDL_Window* window );
int init_Mixer();
int getWindow(SDL_Window** window, const char* name, int x, int y, int w, int h, Uint32 flags);
/*void quit_SDL( SDL_Window** window, SDL_Texture** texture, SDL_Renderer** renderer, bool error );*/
int createRenderer( SDL_Renderer** renderer, SDL_Window* window, int index, Uint32 flags );
int loadImage( SDL_Surface** surface, const char* source );
int CreateTextureFromSurface( SDL_Texture** texture, SDL_Renderer* renderer, SDL_Surface* surface );
int openFont( TTF_Font** font, const char* font_name, int font_size );
int RenderText_Solid( SDL_Surface** textSurface, TTF_Font* font, const char* text, SDL_Color* color );
bool render ( SDL_Texture** texture, SDL_Renderer* renderer, SDL_Surface* surface, const SDL_Rect* src, const SDL_Rect* dst );
bool renderText ( SDL_Texture** texture, SDL_Renderer* renderer, SDL_Surface** textSurface, const SDL_Rect* src, const SDL_Rect* dst, TTF_Font* font, SDL_Color* color, const char* text );
int loadImageOther( SDL_Surface** surface, const char* source , SDL_Surface* windowSurface);
bool loadChunk( Mix_Chunk** sound_effect, const char* sound_effect_name );
