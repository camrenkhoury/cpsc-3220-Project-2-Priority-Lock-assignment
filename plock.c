#include "plock.h"

// private helper functions

node_t *create_node( int priority ){
  int rc; //create int 'rc' 
  node_t *node = malloc( sizeof(node_t) ); //malloc size of a node

  if( node == NULL ){ 
    printf( "** create node failed\n" ); //fail safe message
    exit( -1 ); 
  }

  rc = pthread_cond_init( &(node->waitCV), NULL ); //attempt to lock failed

  if( rc ) { 
    printf( "** init node->waitCV failed\n" ); //condition failure
    exit( -1 ); 
  }

  node->priority = priority; //set structure element priority to the priority that was passed.
  node->next = NULL; //initalize pointer to next to NULL
  return node; //returns pointer to node after creating it.
}

void destroy_node( node_t *node ){
  pthread_cond_destroy( &(node->waitCV) );
  free( node );
}


// public interface functions

plock_t *plock_create(){
  int rc; //create int 'rc' 
  plock_t *lock = malloc( sizeof(plock_t) ); //malloc a block size of plock_t 

  if( lock == NULL ) { 
    printf( "** create lock failed\n" ); //fail safe message
    exit( -1 );
    }

  rc = pthread_mutex_init( &(lock->mlock), NULL );

  if( rc ){ 
    printf( "** init lock->mlock failed\n" );  //attempt to lock failed
    exit( -1 ); 
    }

  lock->value = FREE; //initalize value to FREE
  lock->head = NULL; //intalize head pointer to NULL (remove risk of dangling pointer, but also standard protocol for intitalizing)
  return lock; //return pointer/address to node to function that called it
}


void plock_destroy( plock_t *lock ){
  node_t *ptr, *next;
  pthread_mutex_destroy( &(lock->mlock) );
  if( lock->head != NULL ){
    ptr = lock->head; //create ptr as rover to move down list
    while( ptr != NULL ){ //while not at end of list
      next = ptr->next; //update next pointer for traversing through linked list
      destroy_node( ptr ); //destroy node indiviudally
      ptr = next; //traverse through linked list
    }
  }
  free( lock ); //free the plock_t structure orignially passed to function that encompasses node
}


void plock_acquire( plock_t *lock, int priority ){
  node_t *node; //creates node pointer for node_t structure
  node_t *ptr; //address holder for native/local function operations
  node_t *prev; //previous pointer to use in function


  pthread_mutex_lock( &(lock->mlock) ); //lock mutex

  if(lock->value == FREE) {
    lock->value = BUSY; //intalize to BUSY first
    pthread_mutex_unlock(&(lock->mlock));
    return;
  }

  node = create_node(priority); //set node pointer equal to address returned by create_node function passed with the priority by the function caller


  if(lock->head == NULL || lock->head->priority > priority) { 
    node->next = lock->head;
    lock->head = node;
  } else {
    prev = lock->head;
    ptr = lock->head->next;
    while (ptr != NULL && ptr->priority <= priority) {
      prev = ptr;
      ptr = ptr->next;

    }
    //linked structure traversal updates
    prev->next = node; 
    node->next = ptr;
  }

  pthread_cond_wait(&(node->waitCV), &(lock->mlock)); //wait for signal 

  pthread_mutex_unlock( &(lock->mlock)); //unlock the mutex 

  //dangling pointer management
  node = NULL;
  ptr = NULL;
  prev = NULL;
}


void plock_release( plock_t *lock ){
  node_t *node; 

  pthread_mutex_lock( &(lock->mlock)); //Lock the mutex

  if(lock->head == NULL) {
    lock->value = FREE;
  }else {
    node = lock->head;
    lock->head = node->next; //traverse list to next

    pthread_cond_signal(&(node->waitCV));
    destroy_node(node);
  
  }



  pthread_mutex_unlock( &(lock->mlock) ); //unlock the mutex
}
