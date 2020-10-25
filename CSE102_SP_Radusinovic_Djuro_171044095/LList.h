#include "loadGame.h"

typedef struct qp_element{
  Position position;
  struct qp_element* next;
  Direction direction;
}qp_element;

typedef struct queuePath{
  qp_element* head,
            * tail;
  int size;
}queuePath;

void initMovingObj( movingObj* myMovingObj, Element obj_type );
bool addToMovingObj( movingObj* myMovingObj, int col_to_add, int row_to_add );
bool addToList( PositionDyn** positionD, int col_to_add, int row_to_add );
bool ComparePosition( Position* position1, Position* position2 );
bool removeFromList( movingObj* my_moving_obj, Position* pos_to_remove );
bool updateMovingObj( movingObj* my_moving_obj, Position* pos_to_update, Position* new_position );
bool popFromQueue( queuePath* queue );
void init_queuePath( queuePath* queue );
void pushToQueue( queuePath* queue, Position* pos_to_add, Direction direction );
void emptyOutQueue( queuePath* queue );
void emptyOutMovingObj( movingObj* my_obj );
