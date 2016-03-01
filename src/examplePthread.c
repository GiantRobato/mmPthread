#include <stdio.h>
#include <pthread.h>

//pthread function
void *inc_x(void *x_ptr){
  int *xptr = (int*)x_ptr;
  while(++(*xptr) < 100);
  printf("x inc fin\n");
  return NULL;
}

int main(void) {

  //instantiate vars
  int x = 0, y = 0;
  printf("x: %d, y: %d\n",x,y);

  pthread_t pt;

  if(pthread_create(&pt,NULL, inc_x, &x)){
    printf("Error creating thread");
    return 1;
  }

  while(++y < 100);

  printf("y inc fin\n");

  if(pthread_join(pt,NULL)){
    printf("Error joining threads");
  }

  printf("x: %d, y: %d\n",x,y);

  return 0;
}


