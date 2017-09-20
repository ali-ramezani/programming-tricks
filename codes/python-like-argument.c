 
#include <stdio.h>

double f(int i,int j){printf("\ni=%d j=%d => i+j/2=%lg",i,j,i+j/2.0); return i+j/2.0;}
#define f(...) ({double ret;int i,j=5; __VA_ARGS__; ret=f(i,j);ret;})

int main (int argc, char **argv)
{
double g=f(i=6);
printf("\n\t%lg\n\t%lg\n",f(j=3,i=4),g);
  return 1;
}
