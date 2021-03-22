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
void PopulateOnCombination(int m,int n,fun OnComb){
    CombinationIterator ci;
    ci.SizeOfBasket=m;
    ci.SizeOfSet=n;
    ci.chosen.resize(ci.SizeOfBasket);
    ci.inUse.resize(ci.SizeOfSet,false);
    for (unsigned int i = 0; i < n; i++) ci.options.push_back(i);
    CombinationCall(0,ci,OnComb);
}

int main(){
    PopulateOnCombination(3,5,[](int CombinationCounter,std::vector<unsigned int>& Chosen){
        std::cout<<"\n["<< CombinationCounter +1<<"]:";
        for(auto &v: Chosen) std::cout<<v+1;
    });
}