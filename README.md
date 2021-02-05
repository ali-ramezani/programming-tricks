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

## Pointer with size information
I answered a question in [Quora](https://qr.ae/pN6yDB). In C when you allocate memory from heap using malloc or other variants you don't have any built-in or standard library function to let users of that memory knows how much is its size. It force you to send size and array as two parameters to whoever needs to use that memory. It is a hassle! 
Here I made a customized malloc and free that will let you know the size of pointer using pointer itself (countof)

The main idea is to store size with the pointer but in -2 index! it means you array start from 0 to n and I embedded n in -2 position.

**Hint**
- You can not free this type of memory using standard free which will cause memory leack or corraption. 
- The countof gives you have given in xmalloc call and it is not the size of the memory in byte. Of course you can change xmalloc code if you like to.
~~~ C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//////////////////////////////////////
//////////////////////////////////////

void* xmalloc(unsigned int n,size_t type_size){
    void* mem=malloc(sizeof(unsigned int)+n*type_size);
    unsigned int* sizePtr=mem;
    sizePtr[0]=n;
    mem+=sizeof(unsigned int);
    return mem;
}
void* xActualPtr(void *mem){
    void* actual_mem=mem;
    actual_mem-=sizeof(unsigned int);
    return actual_mem;
}
void xfree(void* mem){
    free(xActualPtr(mem));
}

unsigned int countof(void* mem){
    void* actual_mem=xActualPtr(mem);
    unsigned int* sizePtr=actual_mem;
    return sizePtr[0];
}
//////////////////////////////////////
//////////////////////////////////////
void FillMyArray(int* myArray){
	unsigned int size=countof(myArray);
	for(int i=0;i<size;i++)
		myArray[i]=i;
}
void PrintMyArray(int* myArray){
	unsigned int size=countof(myArray);
	for(int i=0;i<size;i++)
		printf("\n[%d]=%d",i,myArray[i]);
}
int main()
{
    
    char* s=xmalloc(15,sizeof(char));
    strcpy(s,"Hello World!");
    printf("%s %d",s,countof(s));
    xfree(s);
    
    int* iarr=xmalloc(5,sizeof(int));
    
    FillMyArray(iarr);
    PrintMyArray(iarr);
    
    xfree(iarr);
    
    return 0;
}
~~~
