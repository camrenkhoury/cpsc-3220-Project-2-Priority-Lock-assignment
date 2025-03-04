/* priority lock test driver
 *
 * compile with:             gcc -Wall plock.c main.c -pthread
 * run helgrind tests with:  valgrind --tool=helgrind ./a.out
 *
 * compile verbose with:     gcc -Wall -DVERBOSE plock.c main.c -pthread
 *
 * priorities are descending
 */

#include "plock.h"

plock_t *priority_lock; /* global lock shared among worker threads */
struct shared_struct{
  int x;
  int y;
} a = { 0, 100 };

#ifdef VERBOSE
void print_waiting_list( plock_t *lock ){
  node_t *ptr = lock->head;
  printf( "                         [waiting]" );
  while( ptr != NULL ){
    printf( "  %d", ptr->priority );
    ptr = ptr->next;
  }
  printf( "  [end]\n" );
}
#endif


void *worker( void *args ){
  int thread_id = *( (int *) args );
  int priority  = *( ( (int *) args ) + 1 );
  int local_a;
  plock_acquire( priority_lock, priority );

#ifdef VERBOSE
  printf( "thread %d with priority %d enters the critical section\n",
    thread_id, priority );
#endif

  local_a = a.x + thread_id;
  a.y -= thread_id;

  usleep( 100 ); /* extra dwell time within critical section */

  a.x = local_a;

#ifdef VERBOSE
  printf( "thread %d with priority %d leaves the critical section\n",
    thread_id, priority );
  print_waiting_list( priority_lock );
#endif

  plock_release( priority_lock );
  pthread_exit( NULL );
}


int main( int argc, char **argv ){
  pthread_t threads[10];
  int i;
  int rc;
  int args[10][2] =            /* pairs of thread id and priority */
    { {  0,  6 }, {  1,  5 }, {  2,  3 }, {  3,  4 }, {  4,  1 },
      {  5, 10 }, {  6, 15 }, {  7, 13 }, {  8, 14 }, {  9, 11 } };

  priority_lock = plock_create();

  for( i = 0; i < 10 ; i++ ){
    rc = pthread_create( &threads[i], NULL, &worker, (void *)( args[i] ) );
    if( rc ){ printf( "** could not create thread %d\n", i ); exit( -1 ); }
  }

  for( i = 0; i < 10; i++ ){
    rc = pthread_join(threads[i], NULL);
    if( rc ){ printf("** could not join thread %d\n", i); exit( -1 ); }
  }

  plock_destroy( priority_lock );

  printf( "final state of a: x = %d, y = %d, x+y = %d\n", a.x, a.y, a.x+a.y );

  return 0;
}
