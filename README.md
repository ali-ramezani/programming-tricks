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

# An efficient combination generator
I know there are lots of combination generator in internet (actually there were about 100 answers to a question about that in StackOverFlow) but mostly had dynamic allocation and consume memory and some of them were slow implementation I thought it would be better instead of spending time to find the right one just develop one that convers my requirements. Here it is

For **m** choices out of **n** items
   - It consume O(n) of memeory.
   - It does 8 operations (1 assignment,1 increment and two swaps) per combination and one call. No dynamic allocation or any other expensive operations.
   - It accepts function as template so it will not be a function pointer if you pass directly your function but in same time it will accept lambda and std::function 

Use it like this to write combinations for 3 out of 5 for example:
~~~ C++
int main(){
    PopulateOnCombination(3,5,
    [](int CombinationCounter,std::vector<unsigned int>& Chosen){
        std::cout<<"\n["<< CombinationCounter +1<<"]:";
        for(auto &v: Chosen) std::cout<<v+1;
    }
    );
}
~~~

The full code is here and also in codes folder

~~~ C++
#include <iostream>
#include <vector>

struct CombinationIterator{
    int SizeOfBasket,SizeOfSet,CombinationCounter=0;
    std::vector<unsigned int> chosen;
    std::vector<bool> inUse;
    std::vector<unsigned int> options;
};
template <typename fun>
void CombinationCall(int m, CombinationIterator& data, fun OnIter) {
    if (m == data.SizeOfBasket) {
        OnIter(data.CombinationCounter, data.chosen);
        data.CombinationCounter++;
        return;
    }
    auto nm = data.SizeOfSet - m - 1;
    for (int i = 0; i <= nm; i++) {
        data.chosen[m] = data.options[i];
        std::swap(data.options[i],data.options[nm]);
        CombinationCall(m + 1, data, OnIter);        
        std::swap(data.options[i], data.options[nm]);
    }
}

template <typename fun>
void PopulateOnCombination(int SizeOfBasket, int SizeOfSet, fun OnComb) {
    if (SizeOfBasket == 0 || SizeOfBasket > SizeOfSet) return;
    if (SizeOfSet == 1) { std::vector<unsigned int> t{ 0 }; OnComb(0, t); return; }
    CombinationIterator ci;
    ci.SizeOfBasket= SizeOfBasket;
    ci.SizeOfSet= SizeOfSet;
    ci.chosen.resize(ci.SizeOfBasket);
    ci.inUse.resize(ci.SizeOfSet,false);
    ci.options.resize(SizeOfSet);
    for (unsigned int i = 0; i < SizeOfSet; i++) ci.options[i] = i;
    CombinationCall(0,ci,OnComb);
}


~~~

## Using nested struct to categorize features
Sometime your object has so many functionality and you want to categorize them nicely to facilate resue when they are used and also implemented. Most of IDEs automatically show the methods of class when you write the code so by this technique you will also have this chance that IDE will guide you to find suitable method.

The idea is to have a nested class with **No member data** so you can cast main class safely to it and then cast back the nested class to main safely as well to have access to data.


~~~C++
#define SUB(cls) friend struct _##cls;_##cls& cls(){return *(_##cls*)this;} struct _##cls
#define TO(cls) (*(cls*)this)

class Image{
    int data;
    public:

//    struct _Read;_Read& Read(){return *(_Read*)this;}
//    struct _Read{
    SUB(Read){
        void FromFile(const std::string& iFileName){ TO(Image).data=iFileName.size();}
        void FromData(const char* ibuf){TO(Image).data=ibuf[0];}
    };

    SUB(Filter){
        SUB(Sobel){
            void X(){TO(Image).data=0;}
            void Y(){TO(Image).data=0;}
        };
    };
};
int main(){
    Image img;
    img.Read().FromFile("a.png");
    img.Filter().Sobel().X();
}
~~~
