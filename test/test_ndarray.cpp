
#include <vector>
#include <numeric>
#include <iostream>
#include <stdexcept>
#include <assert.h>

#include "ndarray.hpp"

// template<typename T>
// void testIterator(Ndarray<T> &base){
// }

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
	return out;
}

template<typename T>
void testGetItem(Ndarray<T> &base, vector<T> &compare){

	assert(base.ndim >= 2); // test array needs to have 2 or more dimensions
	int k=0;
	for (uint64_t i=0 ; i<base.shape[0] ; i++){
		for (uint64_t j=0 ; j<base.shape[0] ; j++){
			assert(base[i][j] == compare[k]); // Values not identical
			k++;
		}
	}
}
	
template<typename T>
void testGetSlice(Ndarray<T> &base){

	auto checker = [](Ndarray<int64_t> &sliced, Ndarray<int64_t> &base){
		int64_t start = 0;
		int64_t stop = base.shape[0];
		int64_t step = sliced.stride[0];
		int64_t k = 0;
		for (auto i : range(start, stop, step)){
			for (auto j : range(0, base.shape[1], base.stride[1])){
				assert(sliced[k][j] == base[i][j]); // slicing incorrect!
			}
			k++;
		}
	};
	
	auto sliced1 = base[Slice(0, base.shape[0], 2)];
	auto sliced2 = sliced1[Slice(0, base.shape[0], 3)];
	
	checker(sliced1, base);
	checker(sliced2, base);
	checker(sliced2, sliced1);
}

template<typename T>
void testAssignment(Ndarray<T> array){
	assert(array.ndim >= 2); // test array needs to have 2 or more dimensions
	for (uint64_t i=0 ; i<array.shape[0] ; i++){
		for (uint64_t j=0 ; j< array.shape[0] ; j++){
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
	} catch(IndexError){}

	try{
		array[array.shape[0]];
		assert(false); // exception not thrown!
	} catch(IndexError){}
}

template<typename T>
void testStaticDims(Ndarray<T> &array){
	vector<size_t> shape = {16, 16};
	vector<int64_t> basevec  = range(0, 16*16);
	Ndarray<int64_t> free(basevec.data(), shape);
	Ndarray<int64_t, 2> fixed(basevec.data(), shape);

	try{
		Ndarray<int64_t, 3> t1(basevec.data(), shape);
		assert(false); // DimensionError not thrown
	}catch(DimensionError){}

	try{
		Ndarray<int64_t, 2> t2(basevec.data(), vector<size_t>{2,4,4});
		assert(false); // DimensionError not thrown
	}catch(DimensionError){}

	
	// copy assign Ndarray<T, N> to Ndarray<T, N>
	Ndarray<int64_t, 2> t3 = fixed;
	try{
		Ndarray<int64_t, 3> t4 = fixed;
		assert(false); // DimensionError not thrown
	}catch(DimensionError){}

	// auto convert from Ndarray<T> to Ndarray<T,N>
	Ndarray<int64_t, 2> t5 = free; //.freeze();
	
	// auto convert from Ndarray<T,N> to Ndarray<T>
	Ndarray<int64_t> t6 = fixed;

	// slicing integer
	Ndarray<int64_t, 1> t7 = fixed[1];
	try{
		Ndarray<int64_t, 3> t8 = fixed[1];
		assert(false); // DimensionError not thrown
	}catch(DimensionError){}

	// slicing Slice
	Ndarray<int64_t, 2> test5 = fixed[Slice(0,5,1)];
	try{
		Ndarray<int64_t, 1> t8 = fixed[Slice(0,5,1)];
		assert(false); // DimensionError not thrown
	}catch(DimensionError){}
} 

int main(){


	vector<size_t> shape = {16, 16};
	vector<int64_t> basevec  = range(0, 16*16);
	Ndarray<int64_t> array(basevec.data(), shape);

	testGetSlice(array);
	testAssignment(array);
	testOutOfBounds(array);
	testStaticDims(array);
	// // testIterator(array);
	// Ndarray<int64_t,2> dummy(array);
	return 0;
}
