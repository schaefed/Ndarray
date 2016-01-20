
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

template<typename T>
void testEqual(Ndarray<T> &base, std::vector<T> &compare){
	int k;
	for (int i=0 ; i<base.shape[0] ; i++){
		for (int j=0 ; j<base.shape[0] ; j++){
			assert(base[i][j] == compare[k]); // Values not identical
			k++;
		}
	}
}

int main(){

	std::vector<size_t> shape = {16,16};
	std::vector<int> basevec  = range(0,16*16);
	
	Ndarray<int>array(basevec,shape,1);
	testEqual(array,basevec);

	return 0;
}
