#include "LList.h"

void initMovingObj( movingObj* myMovingObj, Element obj_type ){
  myMovingObj->positionDyn = NULL;
  myMovingObj->type = obj_type;
  myMovingObj->n = 0;
}

bool addToList( PositionDyn** positionD, int col_to_add, int row_to_add ){
  bool success = true;
  if( *positionD == NULL ){
    *positionD = ( PositionDyn* )malloc( sizeof( PositionDyn ) );
    if( *positionD != NULL ){
    ( *positionD ) -> position . col = col_to_add;
    ( *positionD ) -> position . row = row_to_add;
    ( *positionD ) -> freeFall = false;
    ( *positionD ) -> direction = none;
    ( *positionD ) -> next = NULL;
    success = true;
    }
    else success = false;
  }
  else success = addToList ( &( *positionD ) -> next, col_to_add, row_to_add );

  return success;
}

bool addToMovingObj( movingObj* myMovingObj, int col_to_add, int row_to_add ){
  bool success = false;
  success = addToList( &( myMovingObj -> positionDyn ), col_to_add, row_to_add );
  if( success ) ( myMovingObj -> n ) ++;
  return success;
}
/*Returns false if the list is empty or if the position to be removed does not exist*/
bool removeFromList( movingObj* my_moving_obj, Position* pos_to_remove ){
    bool success = false;
    PositionDyn* temp = my_moving_obj -> positionDyn;
    PositionDyn* prev = NULL;
    /* If the list is not empty */
    if( temp != NULL ){
      if( ComparePosition( &( temp -> position ) , pos_to_remove ) ){
      my_moving_obj -> positionDyn = temp -> next;
      ( my_moving_obj -> n ) --;
      free( temp );
      success = true;
      }
      else if( temp -> next != NULL ){
          prev = temp;
          temp = temp -> next;
          while( !ComparePosition( &( temp->position ), pos_to_remove ) ) {
            prev = temp;
            temp = temp -> next;
            if( temp == NULL ) break;
          }
          if( temp != NULL ){
          success = true;
          ( my_moving_obj -> n ) --;
          prev -> next = temp -> next;
          free( temp );
        }
      }
    }

    return success;
}

bool ComparePosition( Position* position1, Position* position2 ){
  return ( ( position1 -> col == position2 -> col ) &&
           ( position1 -> row == position2 -> row ) );
}

bool updateMovingObj( movingObj* my_moving_obj, Position* pos_to_update, Position* new_position ){
  PositionDyn* temp = my_moving_obj -> positionDyn;
  bool success = true;
  if( temp != NULL )
  while( !ComparePosition( &( temp->position), pos_to_update ) ){
    temp = temp -> next;
    if( temp == NULL ){
      success = false;
      break;
    }
  }
  if( success ){
    temp -> position = *new_position;
  }

  return success;
}

void init_queuePath( queuePath* queue ){
    queue -> size = 0;
    queue -> head = queue -> tail = NULL;
}

void pushToQueue( queuePath* queue, Position* pos_to_add, Direction direction ){
    if( queue -> size == 0 ){
      queue -> head = queue -> tail = ( qp_element* )malloc( sizeof( qp_element ) );
    }
    else{
      queue -> tail -> next = ( qp_element* )malloc( sizeof( qp_element ) );
      queue -> tail = queue -> tail -> next ;
    }
    queue -> tail -> position.col = pos_to_add -> col;
    queue -> tail -> position.row = pos_to_add -> row;
    queue -> tail -> direction = direction;
    queue -> tail -> next = NULL;
    ( queue -> size ) ++;
}
/*returns false if the queue is empty*/
bool popFromQueue( queuePath* queue ){
  bool success = true;
  if( queue -> size != 0 ){
    qp_element* to_free = queue -> head;
    queue -> head = queue -> head -> next;
    free( to_free );
    ( queue -> size ) -- ;
  }
  else success = false;

  return success;
}

void emptyOutQueue( queuePath* queue ){
  qp_element* to_free;
  while( queue -> size != 0 ){
    to_free = queue -> head;
    queue -> head = queue -> head -> next;
    free( to_free );
    ( queue -> size ) --;
  }
}

void emptyOutMovingObj( movingObj* my_obj ){
  PositionDyn* temp = my_obj -> positionDyn;
  PositionDyn* to_free = NULL;
  while( temp != NULL ){
    to_free = temp;
    temp = temp -> next;
    free( to_free );
  }

  my_obj -> positionDyn = NULL;
  my_obj -> n = 0;
  return;
}
