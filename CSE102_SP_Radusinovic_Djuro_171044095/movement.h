#include "LList.h"
#define NEW_LIFE_REQUIREMENT 150
#define MAX_LIVES 3
#define DIAMOND_VALUE 20

bool moveMiner( Miner* player_user, Tile** tiles, int COLUMNS, int ROWS, SDL_Keycode* keycode, movingObj* rocks, movingObj* diamonds );
bool allowedMove( int col, int row, Tile** tiles, int COLUMNS, int ROWS, SDL_Keycode* keycode, movingObj* rocks, movingObj* diaomonds );
bool playerMoveRock( int col, int row, Tile** tiles, int COLUMNS, int ROWS, SDL_Keycode* keycode, movingObj* rocks );
bool freeFall( Tile** tiles, int COLUMNS, int ROWS, movingObj* freeFallingObj, movingObj* spiders, movingObj* monsters, movingObj* diamonds, movingObj* rocks, bool* gameOver, bool* fell_to_ground );
bool isFreeFallingObj( Element type );
bool freeFall_helper( PositionDyn* position_to_check, Tile** tiles, int COLUMNS, int ROWS, Element type, movingObj* spiders, movingObj* monsters, movingObj* diamonds, movingObj* rocks, bool* gameOver );
bool gameOver( Tile** tiles, int COLUMNS, int ROWS, Miner* player_user );
bool moveSpiders( Tile** tiles, int COLUMNS, int ROWS, movingObj* spiders );
bool setDirectionSpider( Tile** tiles, int COLUMNS, int ROWS, PositionDyn* position_spider, int i );
void moveSpiders_helper( PositionDyn* position_spider, Tile** tiles, int COLUMNS, int ROWS );
void moveMonsters( movingObj* monsters, Tile** tiles, int COLUMNS, int ROWS, Miner* player_user );
Direction moveMonsters_helper( PositionDyn* monster, Tile** tiles, int COLUMNS, int ROWS, Miner* player_user );
void moveMonster( PositionDyn* monster_temp, Direction direction, Tile** tiles, int COLUMNS, int ROWS );
bool isValidPosition( int col, int row, int COLUMNS, int ROWS );
bool CanTurnToDiamond( Tile** tiles, int COLUMNS, int ROWS, int col, int row, movingObj* diamonds, movingObj* rocks );
void TurnToDiamonds( Element type, Tile** tiles, int COLUMNS, int ROWS, Position* center, movingObj* diamonds, movingObj* rocks );
void moveWater( movingObj* waterObj, Tile** tiles, int COLUMNS, int ROWS );
void moveWater_helper( PositionDyn* water_pos, Tile** tiles, int COLUMNS, int ROWS, movingObj* waterObj );
bool freeFallingDiamond( int col, int row, movingObj* diamonds );
void updateScore( Miner* player_user );
