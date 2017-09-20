# programming-tricks

## Python like arguments for function call

Simply the code is here

~~~C
 
#include <stdio.h>

double f(int i){return i+1.5;}
#define f(...) ({double ret;int i; __VA_ARGS__; ret=f(i);ret;})

int main (int argc, char **argv)
{
double g=f(i=5);
printf("%lf\n",f(i=4));
  return 1;
}

~~~

you need to add just `#define f(...) (...` line after the code implementation.

This code would work just using new extension of C standard (-std=c99 and after). gcc by default include new features but if you put `-pedantic`  it warn you like following.

~~~bash
gcc -pedantic python-like-argument.c 
python-like-argument.c: In function 'main':
python-like-argument.c:5:16: warning: ISO C forbids braced-groups within expressions [-Wpedantic]
 #define f(...) ({double ret;int i; __VA_ARGS__; ret=f(i);ret;})
                ^
python-like-argument.c:9:10: note: in expansion of macro 'f'
 double g=f(i=5);
          ^
python-like-argument.c:5:16: warning: ISO C forbids braced-groups within expressions [-Wpedantic]
 #define f(...) ({double ret;int i; __VA_ARGS__; ret=f(i);ret;})
                ^
python-like-argument.c:10:16: note: in expansion of macro 'f'
 printf("%lf\n",f(i=4));
                ^

~~~

