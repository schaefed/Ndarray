
#include <vector>
#include <numeric>
#include <iostream>
#include "ndarray.hpp"

template<typename T>
void printVector(std::vector<T> v){
	for (auto value : v){
		std::cout << value << " ";
	}
	std::cout << std::endl;
}

std::vector<int> range(int start, int stop){
	if (stop <= start){
		// should be something nicer...
		throw 10;
	}
	std::vector<int> out(stop-start);
	std::iota(std::begin(out), std::end(out), start);
	return out;
}

int main(){

	std::vector<size_t> shape = {16,16};
	std::vector<int> base = range(0,16*16);
	
	Ndarray<int>array(base,shape,1);

	return 0;
}
