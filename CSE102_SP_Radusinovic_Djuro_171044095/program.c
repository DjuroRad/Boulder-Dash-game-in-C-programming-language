#include "movement.h"

int main(){
  /*Declarations*/

  /*General*/
  SDL_Window* window = NULL;
  SDL_Renderer* renderer = NULL;
  SDL_Texture* texture = NULL;
  SDL_Texture* textTexture = NULL;
  SDL_Surface* windowSurface = NULL;
  /*Text*/
  TTF_Font* font = NULL;
  SDL_Surface* textSurface = NULL;
  /*Audio*/
  Mix_Music* current_music = NULL;
  Mix_Music* music = NULL;
  Mix_Music* music_10perc_faster = NULL;
  Mix_Music* music_30perc_faster = NULL;
  Mix_Music* music_60perc_faster = NULL;
  Mix_Chunk* playerMove = NULL;
  Mix_Chunk* diamondEffect = NULL;
  Mix_Chunk* fallEffect = NULL;
  bool fell_to_ground = false;
  Mix_Chunk* lifeLost = NULL;
  Mix_Chunk* levelPassed = NULL;
  /*Timers*/
  Uint32 start_time = 0;
  Uint32 end_time = 0;
  Uint32 pause_time = 0;
  int screen_ticks_per_frame = 1000/60;
  Uint32 capped_frame_start = 0;
  Uint32 capped_frame_end = 0;
  int counted_frames = 0;
  float fps = 0;
  bool levelhas4screens = false;
  Position Door = { 0, 0 };

  bool pause = false;
  char info_to_player[] = "Time: 000 | Goal: 0000/0000 | Lives: 0 | Score: 0000000000 | Current level: 00";/*Score 10 digits, hope its enough*/
  char final_score_to_render[] = "0000000000";
  int lives_index = 37;
  int time_index[] = { 6 , 7 , 8 };
  int score_index[] = { 48, 49, 50, 51, 52, 53, 54, 55, 56, 57 };

  int to_goal[] = { 18, 19, 20, 21 };
  int goal[] = { 23, 24, 25, 26 };
  int level_index_text[] = { 76, 77 };

  int total_time = 0;
  int level_time = 0;
  int time_left = 0;

  int level_score = 0;
  int level_goal = 0;

  movingObj rocks;
  movingObj diamonds;
  movingObj monsters;
  movingObj spiders;
  movingObj waterObj;
  int water_timer_N = 5;
  bool water_moved = false;
  initMovingObj( &rocks, rock );
  initMovingObj( &diamonds, diamond );
  initMovingObj( &monsters, monster );
  initMovingObj( &spiders, spider );
  initMovingObj( &waterObj, water );

  Miner player_user;
  bool lost_life = false;
  bool next_level = false;
  init_Miner( &player_user );
  Texture tileTexture;
  init_texture( &tileTexture );
  Tile** tiles = NULL;
  int COLUMNS;
  int ROWS;
  SDL_Rect tileClips[ NUM_OF_ELEMENTS ];
  int vel_mili_rate = 120;
  int player_frame_update_rate = vel_mili_rate / player_user.total_frame;
  int prev_score = 0;

  const char levels[][19] = {
    "levels/level.map",
    "levels/level2.map",
    "levels/level3.map",
    "levels/level4.map",
    "levels/level5.map",
    "levels/level6.map",
    "levels/level7.map",
    "levels/level8.map",
    "levels/level9.map",
    "levels/level10.map",
  };
  int current_level = 0;/*current level will be 9 when the last level is reached. When current_level reaches 10 it means that the player finished the game*/

  /*Variables for animation*/

  /*Diamonds*/
  int diamond_current_frame = 0;
  int diamond_frames_total = 4;
  SDL_Rect diamond_frames[ diamond_frames_total ];
  /*Spider*/
  int spider_frames_total = 3;
  int spider_current_frame = 0;
  SDL_Rect spider_frames[ spider_frames_total ];
  /*Monster*/
  int monster_current_frame = 0;
  int monster_frames_total = 2;
  SDL_Rect monster_frames[ monster_frames_total ];
  /*Water*/
  int water_current_frame = 0;
  int water_frames_total = 4;
  SDL_Rect water_frames[ water_frames_total ];
  /*Player*/
  int player_directions = 4;
  int player_frames_total = 4;
  SDL_Rect miner_frames[ player_directions ][ player_frames_total ];
  setFrames( water_frames, diamond_frames, spider_frames, monster_frames, miner_frames );


  /*PROGRAM*/

  if ( start_SDL() ) goto quit_SDL;
  if( init_TTF() ) goto quit_SDL;
  if( init_Mixer() ) goto quit_SDL;

  SDL_DisplayMode DM;
  SDL_GetDesktopDisplayMode( 0, &DM );
  int ScreenWidth = DM.w;
  int ScreenHeight = DM.h;

  if( getWindow( &window, "Boulder Dash Remake by Djuro", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, ScreenWidth, ScreenHeight, SDL_WINDOW_RESIZABLE ) ) goto quit_SDL;
  if( createRenderer( &renderer, window, -1, SDL_RENDERER_ACCELERATED ) ) goto quit_SDL;


  if( loadMusic( &music, "audio/POL-star-way-short.wav" ) )  goto quit_SDL;
  if( loadMusic( &music_10perc_faster, "audio/POL-star-way-short_10_posto.wav" ) )  goto quit_SDL;
  if( loadMusic( &music_30perc_faster, "audio/POL-star-way-short_30_posto.wav" ) )  goto quit_SDL;
  if( loadMusic( &music_60perc_faster, "audio/POL-star-way-short_60_posto.wav" ) )  goto quit_SDL;
  if( loadChunk( &playerMove, "audio/moveMiner.wav" ) ) goto quit_SDL;
  if( loadChunk( &diamondEffect, "audio/diamond.wav" ) ) goto quit_SDL;
  if( loadChunk( &fallEffect, "audio/fall.wav" ) ) goto quit_SDL;
  if( loadChunk( &lifeLost, "audio/lifeLost.wav" ) ) goto quit_SDL;
  if( loadChunk( &levelPassed, "audio/levelPassed.wav" ) ) goto quit_SDL;

  SDL_Color color = { 120, 120, 120, 125 };/*grey/off white, for button effect*/
  if( openFont( &font, "fonts/256-bytes/256BYTES.TTF", 40) ) goto quit_SDL;
  SDL_Rect buttonPlay = { ScreenWidth/2 - ScreenWidth/8 , ScreenHeight/2 - ScreenWidth/8, ScreenWidth/4, ScreenHeight/4 };
  SDL_Rect place_for_text = { 60, 10, 980, 70 };
  SDL_Rect gameOverSpace = { ScreenWidth/4, ScreenHeight/8 , ScreenWidth/2 , ScreenHeight/10 };

  bool quit = false;
  int state = false;
  SDL_Event e;
  int x_mouse = buttonPlay.w + 1;
  int y_mouse = buttonPlay.h + 1;

  while( !quit ){
    while( SDL_PollEvent( &e ) != 0 ){
      SDL_GetMouseState( &x_mouse, &y_mouse );
      if( renderText( &textTexture, renderer, &textSurface, NULL, &buttonPlay, font, &color, "Play BDash") ) goto quit_SDL;
      else SDL_RenderPresent( renderer );
      if( e.type == SDL_QUIT ) goto quit_SDL;
      else if( e.type == SDL_MOUSEMOTION || e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP ){
        if( x_mouse >= buttonPlay.x && x_mouse <= ( buttonPlay.x + buttonPlay.w ) && y_mouse >= buttonPlay.y && y_mouse <= ( buttonPlay.y + buttonPlay.h ) ){
          color.r = color.g = color.b = 255;
          color.a = 125;
          if( e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP ){
          switch( e.type ){
            case SDL_MOUSEBUTTONDOWN:
              state = SDL_MOUSEBUTTONDOWN;
              break;
            case SDL_MOUSEBUTTONUP:
              if( state == SDL_MOUSEBUTTONDOWN ) quit = true;
              break;
            default:
              state = false;
            }
          }
        }
        else{
          color.r = color.g = color.b = 120;/*getting button effect by updating the color when mouse not over the button*/
          color.a = 125;
        }
      }
    }
  }

  /*Loading media if user continues to play*/
  if( loadTileMap( &tileTexture, renderer, "images/tileMap.bmp" ) ) goto quit_SDL;
  loadTileClips( tileClips );
  tiles = loadLevel( *(levels + current_level) , &COLUMNS, &ROWS, &level_time, &level_goal, &water_timer_N, &levelhas4screens, &Door, &player_user, renderer, &rocks, &diamonds, &monsters, &spiders, &waterObj );
  if( tiles == NULL ) goto quit_SDL;

  const Uint8 *states = SDL_GetKeyboardState( NULL );
  quit = false;
  start_time = SDL_GetTicks();
  SDL_Keycode keycode;
  int prev_time = 0;
  int new_time = 0;
  int water_prev_time = 0;
  int water_new_time = 0;

  while( !quit ){
    capped_frame_start = SDL_GetTicks();
    if( next_level ){
      Mix_HaltMusic();
      Mix_PlayChannel( -1, levelPassed, 0 );
      SDL_Delay( 1000 );
      current_level ++;
      reset( &monsters, &spiders, &rocks, &diamonds, &waterObj, &start_time, &prev_time, &water_prev_time, &water_new_time, &new_time, &time_left, &pause_time, &current_music );
      freeTiles( &tiles, COLUMNS );
      if( current_level != MAXLEVELS ){
        tiles = loadLevel( *( levels + current_level ) , &COLUMNS, &ROWS, &level_time, &level_goal, &water_timer_N, &levelhas4screens, &Door, &player_user, renderer, &rocks, &diamonds, &monsters, &spiders, &waterObj );
        if( tiles == NULL ) goto quit_SDL;
      }
      else break;
      next_level = false;
      player_user.level_score = 0;
    }
    if( lost_life ){
      Mix_HaltMusic();
      Mix_PlayChannel( -1, lifeLost, 0 );
      SDL_Delay( 500 );
      player_user.score -= player_user.level_score;
      player_user.level_score = 0;
      player_user.points_for_new_life -= player_user.level_score;
      if( player_user.points_for_new_life < 0 ) player_user.points_for_new_life = 0;
      reset( &monsters, &spiders, &rocks, &diamonds, &waterObj, &start_time, &prev_time, &water_prev_time, &water_new_time, &new_time, &time_left, &pause_time, &current_music );
      freeTiles( &tiles, COLUMNS );
      tiles = loadLevel( *( levels + current_level ) , &COLUMNS, &ROWS, &level_time, &level_goal, &water_timer_N, &levelhas4screens, &Door, &player_user, renderer, &rocks, &diamonds, &monsters, &spiders, &waterObj );
      if( tiles == NULL ) goto quit_SDL;
      lost_life = false;
    }

    while( SDL_PollEvent( &e ) != 0 ){
      if( e.type == SDL_QUIT ) quit = true;
      else if( e.type == SDL_KEYDOWN ){
        if( e.key.keysym.sym == SDLK_p && !pause ){
          pause = true;
          pause_time += ( end_time - start_time );

          SDL_SetRenderDrawColor( renderer, 0x00, 0x00, 0x00, 0x00 );
          SDL_RenderClear( renderer );
          if( renderLevel( &tileTexture, tiles, COLUMNS, ROWS, &player_user, tileClips, renderer, levelhas4screens ) ) goto quit_SDL;
          if( renderText( &textTexture, renderer, &textSurface, NULL, &place_for_text, font, &color, "Game Paused") ) goto quit_SDL;
          SDL_RenderPresent ( renderer );
        }
        else if( e.key.keysym.sym == SDLK_p && pause ) pause = false;
      }
    }
    if( !pause && player_user.lives != 0 ){

      end_time = SDL_GetTicks();
      if( new_time > prev_time ){
        prev_score = player_user.level_score;
        if( updateMusic( &current_music, music, music_10perc_faster, music_30perc_faster, music_60perc_faster, level_time, time_left ) ) goto quit_SDL;
        if( states[ SDL_SCANCODE_UP ] ){
          keycode = SDL_SCANCODE_UP;
          if( moveMiner( &player_user, tiles, COLUMNS, ROWS, &keycode, &rocks, &diamonds ) ) player_user.direction = top;
          else player_user.direction = none;
        }
        else if( states[ SDL_SCANCODE_DOWN ] ){
          keycode = SDL_SCANCODE_DOWN;
          if( moveMiner( &player_user, tiles, COLUMNS, ROWS, &keycode, &rocks, &diamonds ) ) player_user.direction = bottom;
          else player_user.direction = none;
        }
        else if( states[ SDL_SCANCODE_LEFT ] ){
          keycode = SDL_SCANCODE_LEFT;
          if( moveMiner( &player_user, tiles, COLUMNS, ROWS, &keycode, &rocks, &diamonds ) ) player_user.direction = left;
          else player_user.direction = none;
        }
        else if( states[ SDL_SCANCODE_RIGHT ] ){
          keycode = SDL_SCANCODE_RIGHT;
          if( moveMiner( &player_user, tiles, COLUMNS, ROWS, &keycode, &rocks, &diamonds ) ) player_user.direction = right;
          else player_user.direction = none;
        }
        else player_user.direction = none;
        updateFrameMiner( miner_frames, &player_user, tileClips );
        if( player_user.direction != none ) Mix_PlayChannelTimed( -1, playerMove, 0, vel_mili_rate );
        if( player_user.level_score > prev_score ) Mix_PlayChannel( -1, diamondEffect, 0 );

        freeFall( tiles, COLUMNS, ROWS, &rocks, &spiders, &monsters, &diamonds, &rocks, &lost_life, &fell_to_ground );
        freeFall( tiles, COLUMNS, ROWS, &diamonds, &spiders, &monsters, &diamonds, &rocks, &lost_life, &fell_to_ground );
        update_frame( diamond_frames, tileClips, diamond_frames_total, &diamond_current_frame, diamond );
        if( fell_to_ground ){
          Mix_PlayChannelTimed( -1, fallEffect, 0, vel_mili_rate );
          fell_to_ground = false;
        }

        moveSpiders( tiles, COLUMNS, ROWS, &spiders );
        update_frame( spider_frames, tileClips, spider_frames_total, &spider_current_frame, spider );

        moveMonsters( &monsters, tiles, COLUMNS, ROWS, &player_user);
        update_frame( monster_frames, tileClips, monster_frames_total, &monster_current_frame, monster );

        update_frame( water_frames, tileClips, water_frames_total, &water_current_frame, water );
        if( water_new_time > water_prev_time ){
          moveWater( &waterObj, tiles, COLUMNS, ROWS );
          water_prev_time = water_new_time;
        }

        if( lost_life ){ /* life can be lost from freeFalling objects like diamonds and rocks */
          player_user . lives --;
        }
        if( !lost_life && gameOver( tiles, COLUMNS, ROWS, &player_user ) ){
          player_user . lives --;
          lost_life = true;
        }
        if( !lost_life && time_left <= 0 ){
          player_user . lives --;
          lost_life = true;
        }
        if( lost_life ) tiles[ player_user.position.col ][ player_user.position.row ].type = explosion;

        if( player_user.level_score >= level_goal && !lost_life && !next_level ){
          tiles[ Door . col ][ Door . row ].type = door;
          if( Door.col == player_user.position.col && Door.row == player_user.position.row ) next_level = true;
        }
        if( player_user.lives == 0 ){ setFinalScore( info_to_player, sizeof( info_to_player ), score_index, sizeof( score_index )/sizeof( int ), final_score_to_render, sizeof( final_score_to_render ) );
          }

        prev_time = new_time;
      }

      SDL_SetRenderDrawColor( renderer, 0x00, 0x00, 0x00, 0x00 );
      SDL_RenderClear( renderer );
      if( renderLevel( &tileTexture, tiles, COLUMNS, ROWS, &player_user, tileClips, renderer, levelhas4screens) ) goto quit_SDL;

      time_left = level_time - total_time;

      setInfo( info_to_player, &player_user, lives_index, time_left, level_goal, time_index, score_index, to_goal, goal, level_index_text, current_level );
      if( renderText( &textTexture, renderer, &textSurface, NULL, &place_for_text, font, &color, info_to_player) ) goto quit_SDL;

      SDL_RenderPresent( renderer );
      new_time = (end_time - start_time + pause_time) / vel_mili_rate;
      water_new_time = (end_time - start_time + pause_time) / ( water_timer_N * 1000 );/*water timer N inputted should be an integer in seconds*/
      total_time = (end_time - start_time + pause_time)/1000;

    }
    else if( pause ) start_time = SDL_GetTicks();
    else{
      SDL_SetRenderDrawColor( renderer, 0x00, 0x00, 0x00, 0x00 );
      SDL_RenderClear( renderer );
      if( renderText( &textTexture, renderer, &textSurface, NULL, &gameOverSpace, font, &color, "Your score:") ) goto quit_SDL;
      gameOverSpace.y += gameOverSpace.h;
      if( renderText( &textTexture, renderer, &textSurface, NULL, &gameOverSpace, font, &color, final_score_to_render) ) goto quit_SDL;

      gameOverSpace.y += gameOverSpace.h;
      if( renderText( &textTexture, renderer, &textSurface, NULL, &gameOverSpace, font, &color, "Credits: Djuro Radusinovic") ) goto quit_SDL;
      gameOverSpace.y += ScreenHeight/100;
      SDL_Delay( 700 );
      if( gameOverSpace.y - gameOverSpace.h >= ScreenHeight ) gameOverSpace.y = 0;
      SDL_RenderPresent( renderer );
    }
    /*capping the framerate*/
    counted_frames ++;
    capped_frame_end = SDL_GetTicks();
    if( (capped_frame_end - capped_frame_start) < screen_ticks_per_frame ) SDL_Delay( screen_ticks_per_frame - ( capped_frame_end - capped_frame_start ) );
    fps = (float) counted_frames / ((end_time - start_time + pause_time) / 1000.f );
    if(fps >= 20000) fps = 0;
  }

  fps = (float) counted_frames / ((end_time - start_time + pause_time) / 1000.f );

  quit_SDL:
  TTF_CloseFont ( font );
  SDL_FreeSurface( textSurface );
  end_time = SDL_GetTicks();
  Mix_HaltMusic();
  Mix_FreeMusic( music );
  Mix_FreeChunk( playerMove );
  Mix_FreeChunk( diamondEffect );
  Mix_FreeChunk( fallEffect );
  Mix_FreeChunk( lifeLost );
  music = playerMove = diamondEffect = fallEffect = lifeLost = NULL;

  SDL_DestroyTexture( texture );
  SDL_DestroyRenderer( renderer );
  SDL_DestroyWindow( window );

  Mix_Quit();
  TTF_Quit();
  SDL_Quit();
  return 0;
}
