#include "init.h"
#define MAXLEVELS 10
typedef struct Position{
  int col;
  int row;
}Position;

typedef enum{ right, left, top, bottom, none }Direction;

typedef struct PositionDyn{
  Position position;
  bool freeFall;
  Direction direction;
  struct PositionDyn* next;
}PositionDyn;

typedef struct movingObj{
  PositionDyn* positionDyn;
  Element type;
  freeFallAbility freeFall;
  int n;
}movingObj;

typedef struct Texture{
  SDL_Texture* texture;
  int width;
  int height;
}Texture;

typedef struct Tile{
  int x;
  int y;
  Element type;
}Tile;

typedef struct Miner{
  Position position;
  int lives;
  int score;
  int level_score;
  const char* name;
  SDL_Rect renderingPosition;
  Direction direction;
  int current_frame;
  int total_frame;
  int points_for_new_life;
}Miner;

void init_texture( Texture* ref_texture );
void init_Miner( Miner* player_user );
Tile** loadLevel( const char* map, int* COLUMNS, int* ROWS, int* level_time, int* level_goal, int* water_timer_N, bool* levelhas4screens, Position* door, Miner* player_user, SDL_Renderer* renderer, movingObj* rocks, movingObj* diamonds, movingObj* monsters, movingObj* spiders, movingObj* waterObj );
bool renderLevel( Texture* tileTexture, Tile** tiles, int COLUMNS, int ROWS, Miner* player_user, SDL_Rect tileClips[], SDL_Renderer* renderer, bool levelHas4screens );
void freeTiles( Tile*** tiles, int COLUMNS );
bool loadTileMap( Texture* tileTexture, SDL_Renderer* renderer, const char* file_name );
void setInfo( char info_to_player[], Miner* player_user, int lives_index, int time_left, int level_goal, int time_index[], int score_index[], int to_goal[], int goal[], int level_index_text[], int current_level );
bool validTile( int x, int y, int offsetX, int offsetY, int GWIDTH, int GHEIGHT );
void reset( movingObj* monsters, movingObj* spiders, movingObj* rocks, movingObj* diamonds, movingObj* waterObj, Uint32* start_time, int* prev_time, int* water_prev_time, int* water_new_time, int* new_time, int* time_left, Uint32* paused_time, Mix_Music** current_music );
void loadTileClips( SDL_Rect tileClips[] );
void update_frame( SDL_Rect frames[], SDL_Rect tileClips[], int frames_total, int* current_frame, Element type );
void setFrames( SDL_Rect water_frames[], SDL_Rect diamond_frames[], SDL_Rect spider_frames[], SDL_Rect monster_frames[], SDL_Rect miner_frames[][4] );
void updateFrameMiner( SDL_Rect miner_frames[][4], Miner* player_user, SDL_Rect tileClips[] );
bool updateMusic( Mix_Music** current_music, Mix_Music* music, Mix_Music* music_10perc_faster, Mix_Music* music_30perc_faster, Mix_Music* music_60perc_faster, int level_time, int time_left );
void setFinalScore( char info_to_player[], int n1, int score_index[], int n2, char final_score_to_render[], int n3);
