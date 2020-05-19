# programming-tricks

## Named parameters for function call

Simply the code is here

~~~C
#include <stdio.h>

double f(int i,int j){printf("\ni=%d j=%d => i+j/2=%lg",i,j,i+j/2.0); return i+j/2.0;}
#define f(...) ({double ret;int i,j=5; __VA_ARGS__; ret=f(i,j);ret;})

int main (int argc, char **argv)
{
double g=f(i=6);
printf("\n\t%lg\n\t%lg\n",f(j=3,i=4),g);
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
## Using sscanf like fscanf
To improve performance it would be better to read whole file once and then read items from string but the problem is there is no file pointer to get moved forward when you use sscanf several times like fscanf.
Here is the simple solution, just 

* add the following macro in your code 
* define two variables `ps` and `lps` and set ps to zero in the begining
 
~~~c
#define sScanf(fp,fmt,...)  sscanf(&fp[ps], fmt"%n",__VA_ARGS__,&lps);ps+=lps;
~~~
A sample usage of this would be:
~~~c
#define sScanf(fp,fmt,...)  sscanf(&fp[ps], fmt"%n",__VA_ARGS__,&lps);ps+=lps;

char* ReadFile(const char* fn) {

	FILE *f = fopen(fn, "rb");
	if (f == 0) return 0;

	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);  /* same as rewind(f); */

	char *string = (char*)malloc(fsize + 1);
	fread(string, 1, fsize, f);
	fclose(f);

	string[fsize] = 0;
	return string;
}

...
	char* fp = ReadFile(filename);//fopen(filename,"r");
	int lps,ps=0;
  
  	int n;
	sScanf(fp, "%d", &n);//fscanf(fp, "%d", &n);
	
	float f;
	sScanf(fp,"%f",&f);//fscanf(fp,"%f",&f);
	
	free(fp);//fclose(fp);
  ...
~~~
