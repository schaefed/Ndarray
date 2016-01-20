
#include <vector>
#include <numeric>
#include <iostream>
#include <stdexcept>

#include "ndarray.hpp"

template<typename T>
void printVector(vector<T> v){
	for (auto value : v){
		cout << value << " ";
	}
	cout << endl;
}

vector<int64_t> range(int64_t start, int64_t stop, int64_t step=1){
	if (stop <= start){
		// should be something nicer...
		throw range_error("stop >= step");
	}
	
	int64_t i = 0;
	vector<int64_t> out((stop-start)/step);
	for (int64_t x=start ; x < stop ; x = x + step){
		out[i++] = x;
	}
	// iota(begin(out), end(out), start);
	return out;
}

template<typename T>
void testGetItem(Ndarray<T> &base, vector<T> &compare){

	assert(base.ndim >= 2); // test array needs to have 2 or more dimensions
	int k;
	for (int i=0 ; i<base.shape[0] ; i++){
		for (int j=0 ; j<base.shape[0] ; j++){
			assert(base[i][j] == compare[k]); // Values not identical
			k++;
		}
	}
}

template<typename T>
void testGetSlice(Ndarray<T> &base){

	assert(base.ndim >= 2); // test array needs to have 2 or more dimensions
	int64_t start = 0;
	int64_t stop = base.shape[0];
	int64_t step = 2;
	auto sliced = base[Slice(start,stop,step)];

	int64_t k = 0;
	for (auto i : range(start, stop, step)){
		for (auto j : range(0, base.shape[1])){
			assert(sliced[k][j] == base[i][j]); // slicing incorrect!
		}
		k++;
	}
}



template<typename T>
void testAssignment(Ndarray<T> array){
	assert(array.ndim >= 2); // test array needs to have 2 or more dimensions
	for (int i=0 ; i< array.shape[0] ; i++){
		for (int j=0 ; j< array.shape[0] ; j++){
			array[i][j] = 42;
			assert(array[i][0] == 42); // Assignment failed!
		}
	}
}

template<typename T>
void testOutOfBounds(Ndarray<T> &array){
	assert(array.ndim >= 1); // test array needs to have 2 or more dimensions
	try{
		array[-1];
		assert(false); // exception not thrown!
	} catch(range_error){}

	try{
		array[array.shape[0]];
		assert(false); // exception not thrown!
	} catch(range_error){}
}

int main(){

	vector<size_t> shape = {16,16};
	vector<int64_t> basevec  = range(0,16*16);
	Ndarray<int64_t>array(basevec,shape,1);

	testGetItem(array,basevec);
	testGetSlice(array);
	testAssignment(array);
	testOutOfBounds(array);

	return 0;
}
