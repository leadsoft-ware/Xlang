#pragma once
union Content{
    void* ptrc;
    long int intc;
    double dblc;
    char chc[8];
};
template<typename ta,typename tb>
long pointerSubtract(ta a,tb b){
	Content x,y;
	x.ptrc = (void*) a;
	y.ptrc = (void*) b;
	return x.intc - y.intc;
}