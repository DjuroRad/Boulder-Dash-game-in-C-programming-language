#include "movement.h"

bool moveMiner( Miner* player_user, Tile** tiles, int COLUMNS, int ROWS, SDL_Keycode* keycode, movingObj* rocks, movingObj* diamonds ){
  bool success = false;
  Position pos_to_remove;
  switch ( *keycode ) {
    case SDL_SCANCODE_UP:
      if( allowedMove( player_user->position.col - 1 , player_user->position.row, tiles, COLUMNS, ROWS, keycode, rocks, diamonds ) ){
        if( tiles[ player_user->position.col -1 ][ player_user->position.row ].type == diamond ){
          updateScore( player_user );
          pos_to_remove.col = player_user->position.col - 1;
          pos_to_remove.row = player_user->position.row;
          removeFromList( diamonds, &pos_to_remove );
        }
        tiles[ player_user->position.col -1 ][ player_user->position.row ].type = player;
        tiles[ player_user->position.col ][ player_user->position.row ].type = hole;
        player_user->position.col -= 1;
        success = true;
        }
      break;
    case SDL_SCANCODE_DOWN:
      if( allowedMove(  player_user->position.col + 1 , player_user->position.row, tiles, COLUMNS, ROWS, keycode, rocks, diamonds ) ){
        if( tiles[ player_user->position.col + 1 ][ player_user->position.row ].type == diamond ){
           updateScore( player_user );
           pos_to_remove.col = player_user->position.col + 1;
           pos_to_remove.row = player_user->position.row;
           removeFromList( diamonds, &pos_to_remove );
         }
        tiles[ player_user->position.col + 1 ][ player_user->position.row ].type = player;
        tiles[ player_user->position.col ][ player_user->position.row ].type = hole;
        player_user->position.col += 1;
        success = true;
        }
      break;
    case SDL_SCANCODE_LEFT:
      if( allowedMove( player_user->position.col, player_user->position.row - 1, tiles, COLUMNS, ROWS, keycode, rocks, diamonds ) ){
        if( tiles[ player_user->position.col ][ player_user->position.row - 1 ].type == diamond ){
           updateScore( player_user );
           pos_to_remove.col = player_user->position.col;
           pos_to_remove.row = player_user->position.row - 1;
           removeFromList( diamonds, &pos_to_remove );
         }
        tiles[ player_user->position.col ][ player_user->position.row -1 ].type = player;
        tiles[ player_user->position.col ][ player_user->position.row ].type = hole;
        player_user->position.row -= 1;
        success = true;
        }
      break;
    case SDL_SCANCODE_RIGHT:
      if( allowedMove( player_user->position.col, player_user->position.row + 1, tiles, COLUMNS, ROWS, keycode, rocks, diamonds ) ){
        if( tiles[ player_user->position.col ][ player_user->position.row + 1 ].type == diamond ){
           updateScore( player_user );
           pos_to_remove.col = player_user->position.col;
           pos_to_remove.row = player_user->position.row + 1;
           removeFromList( diamonds, &pos_to_remove );
         }
        tiles[ player_user->position.col ][ player_user->position.row + 1 ].type = player;
        tiles[ player_user->position.col ][ player_user->position.row ].type = hole;
        player_user->position.row += 1;
        success = true;
        }
      break;
  }
  *keycode = SDL_SCANCODE_0;
  return success;
}

void updateScore( Miner* player_user ){
  player_user -> score += DIAMOND_VALUE;
  player_user -> level_score += DIAMOND_VALUE;
  player_user -> points_for_new_life += DIAMOND_VALUE;
  if( player_user -> points_for_new_life >= NEW_LIFE_REQUIREMENT ){
    ( player_user -> lives ) ++;
    if( player_user -> lives > MAX_LIVES ) player_user -> lives = MAX_LIVES;
    player_user -> points_for_new_life = 0;
  }

  return;
}
/* passing keycode if rock is to move left-right */
bool allowedMove( int col, int row, Tile** tiles, int COLUMNS, int ROWS, SDL_Keycode* keycode, movingObj* rocks, movingObj* diamonds ){
  bool success = false;

  if( col < COLUMNS && row < ROWS && col >= 0 && row >= 0 ){

    switch( tiles[ col ][ row ].type ){
      case hole:
      case soil:
      case door:
      case spider:
      case monster:
        success = true;
        break;
      case rock:
        if( playerMoveRock( col, row, tiles, COLUMNS, ROWS, keycode, rocks ) ) success = true;
        break;
      case diamond:
        if( freeFallingDiamond( col, row, diamonds) ) success = false;
        else success = true;
    }
  }

  return success;
}

bool freeFallingDiamond( int col, int row, movingObj* diamonds ){
  bool success = false;
  Position to_compare = { col, row };

  PositionDyn* temp = diamonds -> positionDyn;
  if( diamonds -> type == diamond ){
    while( temp != NULL ){
      if( ComparePosition( &to_compare, &( temp -> position ) ) ){
        if( temp -> freeFall ) success = true;
        break;
      }
      temp = temp -> next;
    }
  }

  return success;
}
/* caller needs to check if there is a rock in the cell in the direction player is moving */
bool playerMoveRock( int col, int row, Tile** tiles, int COLUMNS, int ROWS, SDL_Keycode* keycode, movingObj* rocks ){
  bool success = false;
  Position pos_to_update = { col, row };
  Position new_position;

  switch( *keycode ){
    case SDL_SCANCODE_LEFT:
      if( row >= 1 ){
        if( tiles[ col ][ row - 1].type == hole ){
          tiles[ col ][ row - 1 ].type = rock;
          new_position.col = col;
          new_position.row = row - 1;
          updateMovingObj( rocks, &pos_to_update, &new_position );
          success = true;
        }
      }
      break;
    case SDL_SCANCODE_RIGHT:
      if( row <= ROWS - 2 ){
        if( tiles[ col ][ row + 1].type == hole ){
          tiles[ col ][ row + 1 ].type = rock;
          new_position.col = col;
          new_position.row = row + 1;
          updateMovingObj( rocks, &pos_to_update, &new_position );
          success = true;
        }
      }
      break;
  }

  return success;
}

/* Returns false if type of the movingObj passed does not possess capability of free falling */
bool freeFall( Tile** tiles, int COLUMNS, int ROWS, movingObj* freeFallingObj, movingObj* spiders, movingObj* monsters, movingObj* diamonds, movingObj* rocks, bool* gameOver, bool* fell_to_ground ){
  bool success = true;
  PositionDyn* temp = freeFallingObj -> positionDyn;
  bool prev_freeFall_state = false;
  int total = freeFallingObj -> n;
  /*printf("N AT THE BEGGINING IS %d", total);*/
  int i = 0;
  if( isFreeFallingObj( freeFallingObj -> type ) ){
    while( i < total ){
      prev_freeFall_state = temp -> freeFall;
      if( freeFall_helper( temp, tiles, COLUMNS, ROWS, freeFallingObj -> type, spiders, monsters, diamonds, rocks, gameOver ) ) temp -> freeFall = true;
      else{
        if( prev_freeFall_state ) *fell_to_ground = true;
        temp -> freeFall = false;
      }
      i++;
      temp = temp -> next;
    }
  }
  else success = false;
  return success;
}

/* Should not be used outside of freeFall function! */
bool freeFall_helper( PositionDyn* position_to_check, Tile** tiles, int COLUMNS, int ROWS, Element type_falling, movingObj* spiders, movingObj* monsters, movingObj* diamonds, movingObj* rocks, bool* gameOver ){
  bool success = false;
  Position pos_to_remove;
  if( ( position_to_check -> position  .col ) + 1 < COLUMNS - 1){/*MINUS 1 BECAUSE THERE IS A BORDER ON THE LAST ONE SO THERE IS NO NEED TO CHECK THAT*/
    if( tiles[ ( position_to_check -> position  .col ) + 1 ][ position_to_check -> position.row ].type == hole ){
      tiles[ position_to_check -> position  .col ][ position_to_check -> position.row ].type = hole;
      tiles[ ( position_to_check -> position  .col ) + 1 ][ position_to_check -> position.row ].type = type_falling;
      ( position_to_check -> position.col ) ++;
      success = true;
    }
    else if( tiles[ ( position_to_check -> position  .col ) + 1 ][ position_to_check -> position.row ].type == player && position_to_check -> freeFall ) *gameOver = true;
    else if( ( tiles[ ( position_to_check -> position  .col ) + 1 ][ position_to_check -> position.row ].type == spider || tiles[ ( position_to_check -> position  .col ) + 1 ][ position_to_check -> position.row ].type == monster ) && position_to_check -> freeFall ){
      if( tiles[ ( position_to_check -> position  .col ) + 1 ][ position_to_check -> position.row ].type == spider ){
          pos_to_remove.col = ( position_to_check -> position  .col ) + 1;
          pos_to_remove.row = position_to_check -> position  .row;
          removeFromList( spiders, &pos_to_remove );
          TurnToDiamonds( spider, tiles, COLUMNS, ROWS, &pos_to_remove, diamonds, rocks );
         }
      else if ( tiles[ ( position_to_check -> position  .col ) + 1 ][ position_to_check -> position.row ].type == monster ){
          pos_to_remove.col = ( position_to_check -> position  .col ) + 1;
          pos_to_remove.row = position_to_check -> position  .row;
          removeFromList( monsters, &pos_to_remove );
          TurnToDiamonds( monster, tiles, COLUMNS, ROWS, &pos_to_remove, diamonds, rocks );
         }
    }
    else if( ( position_to_check -> position.col ) - 1 >= 0 ){
      if( isFreeFallingObj ( tiles[ ( position_to_check -> position  .col ) + 1 ][ position_to_check -> position.row ].type ) ){
        if( position_to_check -> position.row - 1 >= 0 ){
        if( tiles[( position_to_check -> position.col ) + 1 ][ position_to_check -> position.row - 1].type == hole && tiles[ position_to_check -> position.col ][ position_to_check -> position.row - 1].type == hole && !isFreeFallingObj( tiles[( position_to_check -> position.col ) - 1 ][ position_to_check -> position.row - 1].type ) ){
              tiles[ position_to_check -> position  .col ][ position_to_check -> position  .row ].type = hole;
              tiles[ position_to_check -> position  .col ][ position_to_check -> position  .row - 1 ].type = type_falling;
              ( position_to_check -> position.row ) --;
              success = true;
            }
        }
        if( position_to_check -> position  .row + 1 < ROWS ){
          if( tiles[( position_to_check -> position.col ) + 1 ][ position_to_check -> position.row + 1].type == hole && tiles[ position_to_check -> position.col ][ position_to_check -> position.row + 1].type == hole && !isFreeFallingObj( tiles[( position_to_check -> position.col ) - 1 ][ position_to_check -> position.row + 1].type ) ){
              tiles[ position_to_check -> position  .col ][ position_to_check -> position  .row ].type = hole;
              tiles[ position_to_check -> position  .col ][ position_to_check -> position  .row + 1 ].type = type_falling;
              ( position_to_check -> position.row ) ++;
              success = true;
          }
        }
      }
    }
  }
  return success;
}

/*Objects free falling defined in here*/
bool isFreeFallingObj( Element type ){
  bool success = true;
  if( type != diamond && type != rock ) success = false;

  return success;
}

bool gameOver( Tile** tiles, int COLUMNS, int ROWS, Miner* player_user ){
  bool success = false;
  if( tiles[ player_user -> position.col ][ player_user -> position.row ].type != player ) success = true;/* no need to check the position since player's position is already always checked */
  else{
    if( isValidPosition( player_user -> position.col - 1, player_user -> position.row, COLUMNS, ROWS ) && ( tiles[ player_user -> position.col - 1 ][ player_user -> position.row ].type == monster || tiles[ player_user -> position.col - 1 ][ player_user -> position.row ].type == spider ) ) success = true;
    else if( isValidPosition( player_user -> position.col + 1, player_user -> position.row, COLUMNS, ROWS ) && ( tiles[ player_user -> position.col + 1 ][ player_user -> position.row ].type == monster || tiles[ player_user -> position.col + 1 ][ player_user -> position.row ].type == spider ) ) success = true;
    else if( isValidPosition( player_user -> position.col, player_user -> position.row - 1, COLUMNS, ROWS ) && ( tiles[ player_user -> position.col ][ player_user -> position.row - 1 ].type == monster || tiles[ player_user -> position.col ][ player_user -> position.row - 1 ].type == spider ) ) success = true;
    else if( isValidPosition( player_user -> position.col, player_user -> position.row + 1, COLUMNS, ROWS ) && ( tiles[ player_user -> position.col ][ player_user -> position.row + 1 ].type == monster || tiles[ player_user -> position.col ][ player_user -> position.row + 1 ].type == spider ) ) success = true;
  }
  return success;
}

bool moveSpiders( Tile** tiles, int COLUMNS, int ROWS, movingObj* spiders ){
  bool success = true;
  PositionDyn* temp = spiders -> positionDyn;
  if( spiders -> type == spider ){
    while( temp != NULL ){
     if( setDirectionSpider( tiles, COLUMNS, ROWS, temp, 0 ) ) moveSpiders_helper( temp, tiles, COLUMNS, ROWS );
     else temp -> direction = right;
      temp = temp -> next;
    }
  }else success = false;

  return success;
}

bool setDirectionSpider( Tile** tiles, int COLUMNS, int ROWS, PositionDyn* position_spider, int i ){


  bool set = false;
  if( i <= 4){
  switch( position_spider -> direction ){
  case right:
  if( position_spider -> position.row < ROWS - 1 ){
    if( tiles[ position_spider -> position.col ][ position_spider -> position.row + 1 ].type != hole && tiles[ position_spider -> position.col ][ position_spider -> position.row + 1 ].type != player ){
      position_spider -> direction = bottom;
      i++;
      set = setDirectionSpider( tiles, COLUMNS, ROWS, position_spider, i );
    }
    else set = true;
  }
  break;

  case bottom:
  if( position_spider -> position.col < COLUMNS - 1 ){
    if( tiles[ position_spider -> position.col + 1 ][ position_spider -> position.row ].type != hole && tiles[ position_spider -> position.col + 1 ][ position_spider -> position.row ].type != player ){
      position_spider -> direction = left;
      i++;
      set = setDirectionSpider( tiles, COLUMNS, ROWS, position_spider, i );
    }
    else set = true;
  }
  break;

  case left:
  if( position_spider -> position.row > 0 ){
    if( tiles[ position_spider -> position.col ][ position_spider -> position.row - 1 ].type != hole && tiles[ position_spider -> position.col ][ position_spider -> position.row - 1 ].type != player ){
      position_spider -> direction = top;
      i++;
      set = setDirectionSpider( tiles, COLUMNS, ROWS, position_spider, i );
    }
    else set = true;
  }
  break;

  case top:
  if( ( position_spider -> position.col > 0 ) ){
    if( tiles[ position_spider -> position.col - 1 ][ position_spider -> position.row ].type != hole && tiles[ position_spider -> position.col - 1 ][ position_spider -> position.row ].type != player ){
      position_spider -> direction = right;
      i++;
      set = setDirectionSpider( tiles, COLUMNS, ROWS, position_spider, i );
     }
     else set = true;
  }
  break;

  case none:
    position_spider -> direction = right;
    set = setDirectionSpider( tiles, COLUMNS, ROWS, position_spider, i );
    break;
  }
}

  return set;
}

void moveSpiders_helper( PositionDyn* position_spider, Tile** tiles, int COLUMNS, int ROWS ){

  switch( position_spider -> direction ){
    case right:
      tiles[ position_spider -> position.col ][ position_spider -> position.row ].type = hole;
      position_spider -> position.row ++;
      tiles[ position_spider -> position.col ][ position_spider -> position.row ].type = spider;
      break;

    case bottom:
      tiles[ position_spider -> position.col ][ position_spider -> position.row ].type = hole;
      position_spider -> position.col ++;
      tiles[ position_spider -> position.col ][ position_spider -> position.row ].type = spider;
      break;

    case left:
      tiles[ position_spider -> position.col ][ position_spider -> position.row ].type = hole;
      position_spider -> position.row --;
      tiles[ position_spider -> position.col ][ position_spider -> position.row ].type = spider;
      break;

    case top:
      tiles[ position_spider -> position.col ][ position_spider -> position.row ].type = hole;
      position_spider -> position.col --;
      tiles[ position_spider -> position.col ][ position_spider -> position.row ].type = spider;
      break;
  }

}

void moveMonsters( movingObj* monsters, Tile** tiles, int COLUMNS, int ROWS, Miner* player_user ){
  PositionDyn* temp_monster = monsters -> positionDyn;
  Direction direction;
  while( temp_monster != NULL ){
    direction = moveMonsters_helper( temp_monster, tiles, COLUMNS, ROWS, player_user );
    if( direction != none ) moveMonster( temp_monster, direction, tiles, COLUMNS, ROWS );
    temp_monster  = temp_monster -> next;
  }
}

/*returns none when there is no move ot be executed, otherwise direction in which the monster should move*/
Direction moveMonsters_helper( PositionDyn* monster, Tile** tiles, int COLUMNS, int ROWS, Miner* player_user ){
  Direction direction = none;
  bool visited[ COLUMNS ][ ROWS ];
  int i,j;
  for( i = 0; i<COLUMNS; i++ ){
    for( j = 0; j<ROWS; j++){
      if( tiles[ i ][ j ].type == hole || tiles[ i ][ j ].type == player ) visited[ i ][ j ] = false;
      else visited[ i ][ j ] = true;
    }
  }

  queuePath moves;
  init_queuePath( &moves );
  Position pos_to_add = { monster -> position.col, monster -> position.row };
  Position temp = pos_to_add;

  /*setting directions*/
  /*up move*/
  if( temp.col - 1 >= 0 && !visited[ temp.col - 1 ][ temp.row ] ){
    pos_to_add.row = temp.row;
    pos_to_add.col = temp.col - 1;
    pushToQueue( &moves, &pos_to_add, top );
    visited[ pos_to_add.col ][ pos_to_add.row ] = true;
  }
  /*down move*/
  if( temp.col + 1 < COLUMNS && !visited[ temp.col + 1 ][ temp.row ] ){
    pos_to_add.row = temp.row;
    pos_to_add.col = temp.col + 1;
    pushToQueue( &moves, &pos_to_add, bottom );
    visited[ pos_to_add.col ][ pos_to_add.row ] = true;
  }
  /*right move*/
  if( temp.row + 1 < ROWS && !visited[ temp.col ][ temp.row + 1 ] ){
    pos_to_add.col = temp.col;
    pos_to_add.row = temp.row + 1;
    pushToQueue( &moves, &pos_to_add, right );
    visited[ pos_to_add.col ][ pos_to_add.row ] = true;
  }
  /*left move*/
  if( temp.row - 1 < ROWS && !visited[ temp.col ][ temp.row - 1 ] ){
    pos_to_add.col = temp.col;
    pos_to_add.row = temp.row - 1;
    pushToQueue( &moves, &pos_to_add, left );
    visited[ pos_to_add.col ][ pos_to_add.row ] = true;
  }

  qp_element* temp_el = moves.head;
  while( moves.size != 0 ){
    if( tiles [ moves.head -> position.col ][ moves.head -> position.row ].type == player ){
      direction = moves.head -> direction ;
      break;
    }
    pos_to_add.col = moves.head -> position.col;
    pos_to_add.row = moves.head -> position.row;
    temp = pos_to_add;
    /*if it still couldn't reach the player*/
    if( temp.col - 1 >= 0 && !visited[ temp.col - 1 ][ temp.row ] ){
      pos_to_add.col --;
      pushToQueue( &moves, &pos_to_add, moves.head -> direction );
      visited[ pos_to_add.col ][ pos_to_add.row ] = true;
    }
    /*down move*/
    if( temp.col + 1 < COLUMNS && !visited[ temp.col + 1 ][ temp.row ] ){
      pos_to_add.col = temp.col + 1;
      pushToQueue( &moves, &pos_to_add, moves.head -> direction );
      visited[ pos_to_add.col ][ pos_to_add.row ] = true;
    }
    /*right move*/
    if( temp.row + 1 < ROWS && !visited[ temp.col ][ temp.row + 1 ] ){
      pos_to_add.col = temp.col;
      pos_to_add.row = temp.row + 1;
      pushToQueue( &moves, &pos_to_add, moves.head -> direction );
      visited[ pos_to_add.col ][ pos_to_add.row ] = true;
    }
    /*left move*/
    if( temp.row - 1 < ROWS && !visited[ temp.col ][ temp.row - 1 ] ){
      pos_to_add.row = temp.row - 1;
      pushToQueue( &moves, &pos_to_add, moves.head -> direction );
      visited[ pos_to_add.col ][ pos_to_add.row ] = true;
    }

    popFromQueue( &moves );
  }
  /*empty out the queue*/
  emptyOutQueue( &moves );
  return direction;
}

void moveMonster( PositionDyn* monster_temp, Direction direction, Tile** tiles, int COLUMNS, int ROWS ){
  tiles[ monster_temp -> position.col ][ monster_temp -> position.row ].type = hole;
  switch ( direction ) {
    case top:
      monster_temp -> position.col --;
      break;
    case bottom:
      monster_temp -> position.col ++;
      break;
    case right:
      monster_temp -> position.row ++;
      break;
    case left:
      monster_temp -> position.row --;
      break;
  }
  tiles[ monster_temp -> position.col ][ monster_temp -> position.row ].type = monster;
}

void TurnToDiamonds( Element type, Tile** tiles, int COLUMNS, int ROWS, Position* center, movingObj* diamonds, movingObj* rocks ){

  if( type == monster || type == spider ){
    /*for both diamond and monster*/
    tiles[ center -> col ][ center -> row ].type = diamond;
    addToMovingObj( diamonds, center -> col, center -> row );/*this is in place of the spider || monster*/

    if( isValidPosition( center -> col, center -> row - 1, COLUMNS, ROWS ) && CanTurnToDiamond( tiles, COLUMNS, ROWS, center -> col, center -> row - 1, diamonds, rocks ) ) tiles[ center -> col ][ center -> row - 1 ].type = diamond;
    if( isValidPosition( center -> col, center -> row + 1, COLUMNS, ROWS ) && CanTurnToDiamond( tiles, COLUMNS, ROWS, center -> col, center -> row  + 1, diamonds, rocks ) ) tiles[ center -> col ][ center -> row + 1 ].type = diamond;
    if( isValidPosition( center -> col - 1, center -> row, COLUMNS, ROWS ) && CanTurnToDiamond( tiles, COLUMNS, ROWS, center -> col - 1, center -> row, diamonds, rocks ) ) tiles[ center -> col - 1 ][ center -> row ].type = diamond;
    if( isValidPosition( center -> col - 1, center -> row - 1, COLUMNS, ROWS ) && CanTurnToDiamond( tiles, COLUMNS, ROWS, center -> col - 1, center -> row - 1, diamonds, rocks ) ) tiles[ center -> col - 1 ][ center -> row - 1 ].type = diamond;
    if( isValidPosition( center -> col - 1, center -> row + 1, COLUMNS, ROWS ) && CanTurnToDiamond( tiles, COLUMNS, ROWS, center -> col - 1, center -> row + 1, diamonds, rocks ) ) tiles[ center -> col - 1 ][ center -> row + 1 ].type = diamond;
    if( isValidPosition( center -> col + 1, center -> row, COLUMNS, ROWS ) && CanTurnToDiamond( tiles, COLUMNS, ROWS, center -> col + 1, center -> row, diamonds, rocks ) ) tiles[ center -> col + 1 ][ center -> row ].type = diamond;
    if( isValidPosition( center -> col + 1, center -> row - 1, COLUMNS, ROWS ) && CanTurnToDiamond( tiles, COLUMNS, ROWS, center -> col + 1, center -> row - 1, diamonds, rocks ) ) tiles[ center -> col + 1 ][ center -> row - 1 ].type = diamond;
    if( isValidPosition( center -> col + 1, center -> row + 1, COLUMNS, ROWS ) && CanTurnToDiamond( tiles, COLUMNS, ROWS, center -> col + 1, center -> row + 1, diamonds, rocks ) ) tiles[ center -> col + 1 ][ center -> row + 1 ].type = diamond;
    /*for monsetr additional 4 cells will be rock*/
    if( type == monster ){
      if( isValidPosition( center -> col + 2, center -> row, COLUMNS, ROWS ) && CanTurnToDiamond( tiles, COLUMNS, ROWS, center -> col + 2, center -> row, diamonds, rocks ) ) tiles[ center -> col + 2 ][ center -> row ].type = diamond;
      if( isValidPosition( center -> col - 2, center -> row, COLUMNS, ROWS ) && CanTurnToDiamond( tiles, COLUMNS, ROWS, center -> col - 2, center -> row, diamonds, rocks ) ) tiles[ center -> col - 2 ][ center -> row ].type = diamond;
      if( isValidPosition( center -> col , center -> row + 2, COLUMNS, ROWS ) && CanTurnToDiamond( tiles, COLUMNS, ROWS, center -> col, center -> row + 2, diamonds, rocks ) ) tiles[ center -> col ][ center -> row + 2 ].type = diamond;
      if( isValidPosition( center -> col , center -> row - 2, COLUMNS, ROWS ) && CanTurnToDiamond( tiles, COLUMNS, ROWS, center -> col, center -> row - 2, diamonds, rocks ) ) tiles[ center -> col ][ center -> row - 2 ].type = diamond;
    }
  }
}
bool CanTurnToDiamond( Tile** tiles, int COLUMNS, int ROWS, int col, int row, movingObj* diamonds, movingObj* rocks ){
  bool success = false;
  Position pos_to_remove;
  switch( tiles[ col ][ row ].type ){
    case hole:
    case soil:
      success = true;
      break;
  }
  if( success ) addToMovingObj( diamonds, col, row );

  return success;
}
/*should also pass column and row*/
bool isValidPosition( int col, int row, int COLUMNS, int ROWS ){
  bool success = true;
  if( col < 0 || col >= COLUMNS || row < 0 || row >= ROWS ) success = false;
  return success;
}

void moveWater( movingObj* waterObj, Tile** tiles, int COLUMNS, int ROWS ){
    PositionDyn* temp = waterObj -> positionDyn;
    int i = 0;
    int ni = waterObj -> n;
    while( i < ni ){
      moveWater_helper( temp, tiles, COLUMNS, ROWS, waterObj );
      temp = temp -> next;
      i++;/*since new water cells are being added to */
    }

    return;
}
void moveWater_helper( PositionDyn* water_pos, Tile** tiles, int COLUMNS, int ROWS, movingObj* waterObj ){
  if( isValidPosition( water_pos -> position.col + 1, water_pos -> position.row, COLUMNS, ROWS ) && tiles[ water_pos -> position.col + 1 ][ water_pos -> position.row ].type == soil ){
    tiles[ water_pos -> position.col + 1 ][ water_pos -> position.row ].type = water;
    addToMovingObj( waterObj, water_pos -> position.col + 1, water_pos -> position.row );
  }
  if( isValidPosition( water_pos -> position.col - 1, water_pos -> position.row, COLUMNS, ROWS ) && tiles[ water_pos -> position.col - 1 ][ water_pos -> position.row ].type == soil ){
     tiles[ water_pos -> position.col - 1 ][ water_pos -> position.row ].type = water;
     addToMovingObj( waterObj, water_pos -> position.col - 1, water_pos -> position.row );
   }
  if( isValidPosition( water_pos -> position.col, water_pos -> position.row + 1, COLUMNS, ROWS ) && tiles[ water_pos -> position.col ][ water_pos -> position.row + 1 ].type == soil ){
    tiles[ water_pos -> position.col ][ water_pos -> position.row + 1 ].type = water;
    addToMovingObj( waterObj, water_pos -> position.col, water_pos -> position.row + 1 );
  }
  if( isValidPosition( water_pos -> position.col, water_pos -> position.row - 1, COLUMNS, ROWS ) && tiles[ water_pos -> position.col ][ water_pos -> position.row - 1 ].type == soil ){
    tiles[ water_pos -> position.col ][ water_pos -> position.row - 1 ].type = water;
    addToMovingObj( waterObj, water_pos -> position.col, water_pos -> position.row - 1 );
  }

  return;
}
