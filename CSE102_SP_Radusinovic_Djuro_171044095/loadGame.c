#include "loadGame.h"

/*returns NULL on failure*/
Tile** loadLevel( const char* map, int* COLUMNS, int* ROWS, int* level_time, int* level_goal, int* water_timer_N, bool* levelhas4screens, Position* Door, Miner* player_user, SDL_Renderer* renderer,  movingObj* rocks, movingObj* diamonds, movingObj* monsters, movingObj* spiders, movingObj* waterObj ){
  Tile** tiles;
  int x = PADDING_LEFT;
  int y = PADDING_TOP;
  int i, j;
  i = j = 0;
  Element n;
  int typeInt;
  bool error = false;

  FILE* file = fopen( map, "r");
  if( file != NULL ){
    /*Getting the tiles dynamically, sometimes it is for 4 screens and sometimes it is for 2 screens*/
    fscanf( file, "%d", COLUMNS );
    fscanf( file, "%d", ROWS );
    fscanf( file, "%d", level_time );
    fscanf( file, "%d", level_goal );
    fscanf( file, "%d", water_timer_N );
    if( *COLUMNS > 20 ) *levelhas4screens = true;

    tiles = (Tile**)malloc( sizeof( Tile* ) * (*COLUMNS) );
    if( tiles == NULL ) error = true;
    if( !error ){
      for( i = 0; i < *COLUMNS && !error ; i++ ){
        tiles[i] = (Tile*)malloc( sizeof( Tile ) * (*ROWS) );
        if( tiles[i] == NULL ) error = true;
      }
    }
    if( !error ){
      i = 0;
      while( fscanf( file, "%d", &typeInt ) != EOF && j <= *COLUMNS ){
        n = typeInt;
        tiles[ j ][ i ].x = x;
        tiles[ j ][ i ].y = y;
        if( n >= soil && n <= door ){
          tiles[ j ][ i ].type = n;

          switch ( n ) {
            case player:
              player_user -> position.col = j;
              player_user -> position.row = i;
              player_user -> renderingPosition.x = x;
              player_user -> renderingPosition.y = y;
              break;
            case rock:
              addToMovingObj( rocks, j, i );
              break;
            case diamond:
              addToMovingObj( diamonds, j, i );
              break;
            case monster:
              addToMovingObj( monsters, j, i );
              break;
            case spider:
              addToMovingObj( spiders, j, i );
              break;
            case water:
              addToMovingObj( waterObj, j, i );
              break;
            case door:
              tiles[ j ][ i ].type = border;
              Door -> col = j;
              Door -> row = i;
          }

          x += tileWidth;
          i ++;
          if( i == *ROWS ){
            i = 0;
            j ++;
            x = PADDING_LEFT;
            y += tileHeight;
          }
        }
        else{
          printf("Error reading the map at the position %d\n", i + 1 );
          error = true;
          freeTiles( &tiles, *COLUMNS );
        }
      }
    fclose( file );
    }
  }
  else printf( "Could not open map: %s\n", map );

  return tiles;
}

bool loadTileMap( Texture* tileTexture, SDL_Renderer* renderer, const char* file_name){
  bool failed = false;
  SDL_Surface* tileMap = NULL;
  if( loadImage( &tileMap, file_name ) ) failed = true;
  if ( ( !failed ) && ( CreateTextureFromSurface( &( tileTexture->texture ), renderer, tileMap ) ) ) failed = true;
  if( !failed ){
    tileTexture -> width = tileMap -> w;
    tileTexture -> height = tileMap -> h;
    SDL_FreeSurface( tileMap );
  }
  tileMap = NULL;

  return failed;
}

void loadTileClips( SDL_Rect tileClips[] ){
  /* there are NUM_OF_ELEMENTS tileClips */
  /*each tile is 45x45 */

  tileClips[ soil ].x = 180;
  tileClips[ soil ].y = 180;
  tileClips[ soil ].w = clippedTileWidth;
  tileClips[ soil ].h = clippedTileHeight;

  tileClips[ diamond ].x = 0;
  tileClips[ diamond ].y = 315;
  tileClips[ diamond ].w = clippedTileWidth;
  tileClips[ diamond ].h = clippedTileHeight;

  tileClips[ rock ].x = 180;
  tileClips[ rock ].y = 225;
  tileClips[ rock ].w = clippedTileWidth;
  tileClips[ rock ].h = clippedTileHeight;

  tileClips[ wall ].x = 180;
  tileClips[ wall ].y = 270;
  tileClips[ wall ].w = clippedTileWidth;
  tileClips[ wall ].h = clippedTileHeight;

  tileClips[ border ].x = 225;
  tileClips[ border ].y = 225;
  tileClips[ border ].w = clippedTileWidth;
  tileClips[ border ].h = clippedTileHeight;

  tileClips[ hole ].x = 225;
  tileClips[ hole ].y = 0;
  tileClips[ hole ].w = clippedTileWidth;
  tileClips[ hole ].h = clippedTileHeight;

  tileClips[ player ].x = 0;
  tileClips[ player ].y = 135;
  tileClips[ player ].w = clippedTileWidth;
  tileClips[ player ].h = clippedTileHeight;

  tileClips[ spider ].x = 0;
  tileClips[ spider ].y = 180;
  tileClips[ spider ].w = clippedTileWidth;
  tileClips[ spider ].h = clippedTileHeight;

  tileClips[ monster ].x = 0;
  tileClips[ monster ].y = 225;
  tileClips[ monster ].w = clippedTileWidth;
  tileClips[ monster ].h = clippedTileHeight;

  tileClips[ water ].x = 0;
  tileClips[ water ].y = 270;
  tileClips[ water ].w = clippedTileWidth;
  tileClips[ water ].h = clippedTileHeight;

  tileClips[ door ].x = 225;
  tileClips[ door ].y = 270;
  tileClips[ door ].w = clippedTileWidth;
  tileClips[ door ].h = clippedTileHeight;

  tileClips[ explosion ].x = 225;
  tileClips[ explosion ].y = 180;
  tileClips[ explosion ].w = clippedTileWidth;
  tileClips[ explosion ].h = clippedTileHeight;

  return;
}

void init_texture( Texture* ref_texture ){
  ref_texture -> texture = NULL;
  ref_texture -> width = 0;
  ref_texture -> height = 0;
}

bool renderLevel( Texture* tileTexture, Tile** tiles, int COLUMNS, int ROWS, Miner* player_user, SDL_Rect tileClips[], SDL_Renderer* renderer, bool levelHas4screens){
  bool failed = false;
  int i, j;
  i = j = 0;
  int game_width = tileWidth*ROWS;
  int game_height = tileHeight*COLUMNS;

  if( levelHas4screens ){
    game_width /= 2;
    game_height /= 2;
  }
  SDL_Rect rendRect = { 0, 0, tileWidth, tileHeight };
  int x,y;
  x = PADDING_LEFT ;
  y = PADDING_TOP - tileWidth;
  rendRect.x = tiles[0][0].x;
  rendRect.y = tiles[0][0].y;

  int offsetX = tiles[0][0].x;
  int offsetY = tiles[0][0].y;

  if( levelHas4screens ){
    if(tiles[ player_user -> position.col ][ player_user -> position.row ].x > ( game_width / 2 ) + offsetX ) offsetX = tiles[ player_user -> position.col ][ player_user -> position.row ].x - game_width/2;/*tiles[ player_user -> position.col ][ player_user -> position.row ].x - ( game_width )/ 2*/;
    if(tiles[ player_user -> position.col ][ player_user -> position.row ].y > ( game_height / 2 ) + offsetY ) offsetY = tiles[ player_user -> position.col ][ player_user -> position.row ].y - ( game_height ) / 2;
    if( offsetX > tiles[0][0].x + game_width ) offsetX = tiles[0][0].x + game_width;
    if( offsetY > tiles[0][0].y + game_height ) offsetY = tiles[0][0].y + game_height;
  }

  for( i=0; i < COLUMNS; i++ ){
    for( j=0; j<ROWS; j++){

    if( validTile( tiles[i][j].x, tiles[i][j].y, offsetX, offsetY, game_width, game_height) ){
      if( SDL_RenderCopy( renderer, tileTexture->texture, &tileClips[ tiles[i][j].type ], &rendRect ) != 0 ) failed = true;
      rendRect.x += tileWidth;
        if( rendRect.x >= game_width + tiles[0][0].x ){
          rendRect.y += tileHeight;
          rendRect.x = x;
        }
      }
    }
  }

  return failed;
}

bool validTile( int x, int y, int offsetX, int offsetY, int GWIDTH, int GHEIGHT ) {
  bool success = false;
  if( x >= offsetX && x < offsetX + GWIDTH && y >= offsetY && y < offsetY + GHEIGHT ) success = true;

  return success;
}

void init_Miner( Miner* player_user ){
  player_user -> position.col = player_user->position.row = 0;
  player_user -> lives = LIVES_START;
  player_user -> score = 0;
  player_user -> level_score = 0;
  player_user -> renderingPosition.x = 0;
  player_user -> renderingPosition.y = 0;
  player_user -> renderingPosition.w = tileWidth;
  player_user -> renderingPosition.h = tileHeight;
  player_user -> direction = none;
  player_user -> current_frame = 0;
  player_user -> total_frame = 4;
  player_user -> points_for_new_life = 0;

  return;
}

void freeTiles( Tile*** tiles, int COLUMNS ){
  int i;
  for( i = 0; i<COLUMNS; i++ ) free( (*tiles)[i] );
  free(*tiles);
  *tiles = NULL;

  return;
}
void setFinalScore(char info_to_player[], int n1, int score_index[], int n2, char final_score_to_render[], int n3){
  int i;
  if( n3 <= n1 ){
    for( i = 0; i < n2; i++ ){
       final_score_to_render[ i ] = info_to_player[ score_index[i] ];
    }
  }
}

void setInfo( char info_to_player[], Miner* player_user, int lives_index, int time_left, int level_goal, int time_index[], int score_index[], int to_goal[], int goal[], int level_index_text[], int current_level ){
      info_to_player[ lives_index ] = player_user -> lives + '0';

      info_to_player[ time_index[2] ] = time_left % 10 + '0';
      info_to_player[ time_index[1] ] = ( time_left / 10 ) % 10 + '0';
      info_to_player[ time_index[0] ] = time_left / 100 + '0';

      info_to_player[ score_index[9] ] = ( player_user -> score ) % 10 + '0';
      info_to_player[ score_index[8] ] = ( player_user -> score / 10 % 10 ) + '0';
      info_to_player[ score_index[7] ] = ( player_user -> score / 100 % 10 ) + '0';
      info_to_player[ score_index[6] ] = ( player_user -> score / 1000 ) % 10 + '0';
      info_to_player[ score_index[5] ] = ( player_user -> score / 10000 ) % 10 + '0';
      info_to_player[ score_index[4] ] = ( player_user -> score / 100000 ) % 10 + '0';
      info_to_player[ score_index[3] ] = ( player_user -> score / 1000000 ) % 10 + '0';
      info_to_player[ score_index[2] ] = ( player_user -> score / 10000000 ) % 10 + '0';
      info_to_player[ score_index[1] ] = ( player_user -> score / 100000000 ) % 10 + '0';
      info_to_player[ score_index[0] ] = player_user -> score / 1000000000 + '0';

      info_to_player[ to_goal[3] ] = player_user -> level_score % 10 + '0';
      info_to_player[ to_goal[2] ] = ( player_user -> level_score / 10 ) % 10 + '0';
      info_to_player[ to_goal[1] ] = ( player_user -> level_score / 100 ) % 10 + '0';
      info_to_player[ to_goal[0] ] = player_user -> level_score / 1000 + '0';

      info_to_player[ goal[3] ] = level_goal % 10 + '0';
      info_to_player[ goal[2] ] = ( level_goal / 10 )% 10 + '0';
      info_to_player[ goal[1] ] = ( level_goal / 100 ) % 10 + '0';
      info_to_player[ goal[0] ] = level_goal / 1000 + '0';

      info_to_player[ level_index_text[0] ] = ( current_level + 1 )/10 + '0';
      info_to_player[ level_index_text[1] ] = ( current_level + 1 )%10 + '0';
}

void reset( movingObj* monsters, movingObj* spiders, movingObj* rocks, movingObj* diamonds, movingObj* waterObj, Uint32* start_time, int* prev_time, int* water_prev_time, int* water_new_time, int* new_time, int* time_left, Uint32* paused_time, Mix_Music** current_music ){

  *current_music = NULL;
  emptyOutMovingObj( monsters );
  emptyOutMovingObj( spiders );
  emptyOutMovingObj( rocks );
  emptyOutMovingObj( diamonds );
  emptyOutMovingObj( waterObj );
  *start_time = SDL_GetTicks();
  *prev_time = *new_time = 0;
  *water_prev_time = *water_new_time = 0;
  *paused_time = 0;
}

/* very carefull when using this function, every parameter should be passed correctly */
void update_frame( SDL_Rect frames[], SDL_Rect tileClips[], int frames_total, int* current_frame, Element type ){
  (*current_frame) ++;
  if( *current_frame == frames_total ) *current_frame = 0;
  tileClips[ type ] = frames[ *current_frame ];
}

void updateFrameMiner( SDL_Rect miner_frames[][4], Miner* player_user, SDL_Rect tileClips[] ){
  if( player_user -> direction != none ){
    ( player_user -> current_frame ) ++;
    if( player_user -> current_frame == player_user -> total_frame ) player_user -> current_frame = 0;
    tileClips[ player ] = miner_frames[ player_user -> direction ][ player_user -> current_frame ];
  }
  else{
    player_user -> current_frame = 0;
    tileClips[ player ] = miner_frames[ bottom ][ player_user -> current_frame ];
  }
}

/*very dumb function just to make main more readable*/
void setFrames( SDL_Rect water_frames[], SDL_Rect diamond_frames[], SDL_Rect spider_frames[], SDL_Rect monster_frames[], SDL_Rect miner_frames[][4] ){
  int i,j;

  diamond_frames[0].x = 0;
  diamond_frames[0].y = 315;
  diamond_frames[1].x = 45;
  diamond_frames[1].y = 315;
  diamond_frames[2].x = 90;
  diamond_frames[2].y = 315;
  diamond_frames[3].x = 135;
  diamond_frames[3].y = 315;

  spider_frames[0].x = 0;
  spider_frames[0].y = 180;
  spider_frames[1].x = 45;
  spider_frames[1].y = 180;
  spider_frames[2].x = 90;
  spider_frames[2].y = 180;

  monster_frames[0].x = 0;
  monster_frames[0].y = 225;
  monster_frames[1].x = 45;
  monster_frames[1].y = 225;

  water_frames[0].x = 0;
  water_frames[0].y = 270;
  water_frames[1].x = 45;
  water_frames[1].y = 270;
  water_frames[2].x = 90;
  water_frames[2].y = 270;
  water_frames[3].x = 135;
  water_frames[3].y = 270;

  for( i = 0; i < 2; i++ ){
    water_frames[i].w = spider_frames[i].w = monster_frames[i].w = diamond_frames[i].w = clippedTileWidth;
    water_frames[i].h = spider_frames[i].h = monster_frames[i].h = diamond_frames[i].h = clippedTileHeight;
  }
  for(; i < 3 ; i++){
    water_frames[i].w = spider_frames[i].w = diamond_frames[i].w = clippedTileWidth;
    water_frames[i].h = spider_frames[i].h = diamond_frames[i].h = clippedTileHeight;
  }
  water_frames[i].w = diamond_frames[i].w = clippedTileWidth;
  water_frames[i].h = diamond_frames[i].h = clippedTileHeight;

  /*Player*/
  miner_frames[0][0].x = 0;
  miner_frames[0][0].y = 0;
  miner_frames[0][1].x = 45;
  miner_frames[0][1].y = 0;
  miner_frames[0][2].x = 90;
  miner_frames[0][2].y = 0;
  miner_frames[0][3].x = 135;
  miner_frames[0][3].y = 0;

  miner_frames[1][0].x = 0;
  miner_frames[1][0].y = 45;
  miner_frames[1][1].x = 45;
  miner_frames[1][1].y = 45;
  miner_frames[1][2].x = 90;
  miner_frames[1][2].y = 45;
  miner_frames[1][3].x = 135;
  miner_frames[1][3].y = 45;

  miner_frames[2][0].x = 0;
  miner_frames[2][0].y = 90;
  miner_frames[2][1].x = 45;
  miner_frames[2][1].y = 90;
  miner_frames[2][2].x = 90;
  miner_frames[2][2].y = 90;
  miner_frames[2][3].x = 135;
  miner_frames[2][3].y = 90;

  miner_frames[3][0].x = 0;
  miner_frames[3][0].y = 135;
  miner_frames[3][1].x = 45;
  miner_frames[3][1].y = 135;
  miner_frames[3][2].x = 90;
  miner_frames[3][2].y = 135;
  miner_frames[3][3].x = 135;
  miner_frames[3][3].y = 135;

  for( i = 0 ; i < 4 ; i++ ){
    for( j = 0 ; j < 4 ; j++ ){
      miner_frames[i][j].w = clippedTileWidth;
      miner_frames[i][j].h = clippedTileWidth;
    }
  }
}


bool updateMusic( Mix_Music** current_music, Mix_Music* music, Mix_Music* music_10perc_faster, Mix_Music* music_30perc_faster, Mix_Music* music_60perc_faster, int level_time, int time_left ){

  bool not_initialized = false;
  float perc_left = ( (float)time_left / (float)level_time ) * 100.0;
  Mix_Music* prev_music = *current_music;

  if( music != NULL && music_10perc_faster != NULL && music_30perc_faster != NULL && music_60perc_faster != NULL ){
    if( perc_left <= 10 ) *current_music = music_60perc_faster;
    else if( perc_left <= 20 ) *current_music = music_30perc_faster;
    else if( perc_left <= 30 ) *current_music = music_10perc_faster;
    else *current_music = music;
    if( *current_music != prev_music ) Mix_PlayMusic( *current_music, -1 );
  }
  else{
    printf(" Error. Files not loaded\n ");
    not_initialized = true;
  }

  return not_initialized;
}
