
#include <vector>
#include <numeric>
#include <iostream>
#include <stdexcept>

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

template<typename T>
void testAssignment(Ndarray<T> array){
	for (int i=0 ; i< array.shape[0] ; i++){
		for (int j=0 ; j< array.shape[0] ; j++){
			array[i][j] = 42;
			assert(array[i][0] == 42); // Assignment failed
		}
	}
}

template<typename T>
void testOutOfBounds(Ndarray<T> &array){
	try{
		array[-1];
		assert(false); // exception not thrown
	} catch(range_error){}

	try{
		array[array.shape[0]];
		assert(false); // exception not thrown
	} catch(range_error){}
}

int main(){

	std::vector<size_t> shape = {16,16};
	std::vector<int> basevec  = range(0,16*16);
	
	Ndarray<int>array(basevec,shape,1);
	testEqual(array,basevec);
	testAssignment(array);
	testOutOfBounds(array);
	return 0;
}
