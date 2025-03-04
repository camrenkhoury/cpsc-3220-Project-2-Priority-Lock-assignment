#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>

#define FREE 0 //FREE set to zero for user case handling 
#define BUSY 1 //BUSY set to one for user case handling 

/*Node Structure Definition for node */
typedef struct node_def{ 
  int priority;		  /* priority of the waiting thread          */
  pthread_cond_t waitCV;  /* thread waits on this condition variable */
  struct node_def *next;  /* node is part of a singly-linked list    */
} node_t;

typedef struct{ //plock_t structure
  int value;		  /* FREE or BUSY                            */
  pthread_mutex_t mlock;  /* mutex protecting the plock structure    */
  node_t *head;		  /* sorted linked list of waiting threads   */
} plock_t;

//function prototypes
plock_t *plock_create(); 
void plock_destroy( plock_t *lock );
void plock_acquire( plock_t *lock, int priority );
void plock_release( plock_t *lock );
