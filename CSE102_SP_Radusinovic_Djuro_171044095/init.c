#include "init.h"
/*Functions will return true on failure and false on success*/
int CreateTextureFromSurface( SDL_Texture** texture, SDL_Renderer* renderer, SDL_Surface* surface ){
  *texture = SDL_CreateTextureFromSurface( renderer, surface );
  if( *texture == NULL ) printf("Could not create texture from surface: %s\n", SDL_GetError() );
  return *texture == NULL;
}

bool render ( SDL_Texture** texture, SDL_Renderer* renderer, SDL_Surface* surface, const SDL_Rect* src, const SDL_Rect* dst ){
  bool failed = false;
  if( CreateTextureFromSurface( texture, renderer, surface ) ) failed = true;
  if( !failed && SDL_RenderCopy( renderer, *texture, src, dst ) != 0 ) failed = true;
  SDL_RenderPresent( renderer );
}

int loadImage( SDL_Surface** surface, const char* source ){
    *surface = SDL_LoadBMP( source );
    if( *surface == NULL ) printf("Unable to load image at: %s\n SDL_Error: %s\n", source, SDL_GetError() );
    return *surface == NULL;
}

int loadImageOther( SDL_Surface** surface, const char* source, SDL_Surface* windowSurface ){
    SDL_Surface* optimizedSurface = NULL;
    *surface = IMG_Load( source );
    if( *surface == NULL ) printf("Unable to load image at: %s\n SDL_Error: %s\n", source, SDL_GetError() );
    else{
      optimizedSurface = SDL_ConvertSurface( *surface, windowSurface->format, 0 );
      if( optimizedSurface == NULL ) printf( "Unable to optimize image %s! SDL Error: %s\n", source, SDL_GetError() );
      else *surface = optimizedSurface;
    }
    return *surface == NULL;
}

int getWindow(SDL_Window** window, const char* name, int x, int y, int w, int h, Uint32 flags){
  *window = SDL_CreateWindow( name, x, y, w, h, flags );
  if( *window == NULL ) printf("Unable to create window: %s\n", SDL_GetError() );
  return *window == NULL;
}

int start_SDL(){
    SDL_SetMainReady();
    if( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0 ){
      printf("Unable to initalize SDL: %s\n", SDL_GetError() );
      return true;
    }

    return false;
}

int init_IMG( SDL_Surface** windowSurface, SDL_Window* window ){
  int imgFlags = IMG_INIT_PNG;
  if( !( IMG_Init( imgFlags ) & imgFlags ) ){
    printf("SDL_image could not initialize! SDL_image's error: %s\n", IMG_GetError() );
    return true;
  }
  *windowSurface = SDL_GetWindowSurface( window );
  if( *windowSurface == NULL ) printf("Could not get window surface. SDL Error: %s\n", SDL_GetError() );
  return *windowSurface == NULL;
}

int init_Mixer(){
  int failure;
  if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 ){
    printf(" SDL_mixer could not initialize! SDL_mixer error: %s", Mix_GetError() );
    failure = true;
  }
  else failure = false;

  return failure;
}

int init_TTF(){
  if( TTF_Init() == -1 ){
    printf("SDL_ttf could not initilize: %s\n", TTF_GetError() );
    return true;
  }

  return false;
}
/*
void quit_SDL( SDL_Window** window, SDL_Texture** texture, SDL_Renderer** renderer, bool error ){
  if( texture != NULL ){
    SDL_DestroyTexture(*texture);
    *texture = NULL;
  }
  if( renderer != NULL ){
    SDL_DestroyRenderer(*renderer);
    *renderer = NULL;
  }
  if( window != NULL ){
    SDL_DestroyWindow(*window);
    *window = NULL;
  }
  Mix_Quit();
  TTF_Quit();
  SDL_Quit();
  if( error ) exit(0);
  return;
}
*/
int createRenderer( SDL_Renderer** renderer, SDL_Window* window, int index, Uint32 flags ){
  *renderer = SDL_CreateRenderer( window, index, flags );
  if( *renderer == NULL ) printf("Renderer could not be created: %s\n", SDL_GetError() );
  return *renderer == NULL;
}

int openFont( TTF_Font** font, const char* font_name, int font_size ){
  *font = TTF_OpenFont( font_name, font_size );
  if( *font == NULL ) printf("Failed to open the font: %s. TTF_Error: %s\n", font_name, TTF_GetError() );
  return *font == NULL;
}
int RenderText_Solid( SDL_Surface** textSurface, TTF_Font* font, const char* text, SDL_Color* color ){
  *textSurface = TTF_RenderText_Solid( font, text, *color );
  if( *textSurface == NULL ) printf( "Could not render text surface. Error: %s\n", TTF_GetError() );
  return *textSurface == NULL;
}

bool renderText ( SDL_Texture** texture, SDL_Renderer* renderer, SDL_Surface** textSurface, const SDL_Rect* src, const SDL_Rect* dst, TTF_Font* font, SDL_Color* color, const char* text ){
  bool failed = false;
  if( RenderText_Solid( textSurface, font, text, color) ) failed = true;
  if( ( !failed ) && ( CreateTextureFromSurface( texture, renderer, *textSurface ) ) ) failed = true;
  if( !failed ){
    SDL_RenderCopy( renderer, *texture, src, dst );
    SDL_FreeSurface( *textSurface );
    *textSurface = NULL;
    SDL_DestroyTexture( *texture );
    *texture = NULL;
  }
  return failed;
}

bool loadMusic( Mix_Music** music, const char* file_name ){
  *music = Mix_LoadMUS( file_name );
  if( *music == NULL ) printf(" Failed to load audio file: %s\n", Mix_GetError() );
  return *music == NULL;
}

bool loadChunk( Mix_Chunk** sound_effect, const char* sound_effect_name ){
  *sound_effect = Mix_LoadWAV( sound_effect_name );
  if( *sound_effect == NULL ) printf("Failed to load sound effect: %s. SDL_mixer error:%s \n", Mix_GetError() );
  return *sound_effect == NULL;
}
