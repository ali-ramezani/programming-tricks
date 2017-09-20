 
#include <stdio.h>

double f(int i){return i+1.5;}
#define f(...) ({double ret;int i; __VA_ARGS__; ret=f(i);ret;})

int main (int argc, char **argv)
{
double g=f(i=5);
printf("%lf\n",f(i=4));
  return 1;
}
