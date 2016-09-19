
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

void testGetSlice(){

	vector<size_t> shape = {16, 16};
	vector<int64_t> basevec  = range(0, 16*16);
	Ndarray<int64_t> array(basevec.data(), shape);
	Ndarray<int64_t> sliced;

	sliced = array[Slice(0,3)];
	assert (sliced[0][0] == 0); // Slicing test failed
	assert (sliced[0][7] == 7); // Slicing test failed
	assert (sliced[0][15] == 15); // Slicing test failed

	assert (sliced[1][0] == 16); // Slicing test failed
	assert (sliced[1][7] == 23); // Slicing test failed
	assert (sliced[1][15] == 31); // Slicing test failed
	assert (sliced[2][0] == 32); // Slicing test failed
	assert (sliced[2][7] == 39); // Slicing test failed
	assert (sliced[2][15] == 47); // Slicing test failed

	sliced = array[0][Slice(1,8)];
	assert (sliced[0] == 1); // Slicing test failed
	assert (sliced[1] == 2); // Slicing test failed
	assert (sliced[4] == 5); // Slicing test failed
	assert (sliced[6] == 7); // Slicing test failed

	sliced = array[0][Slice(1,11,2)];
	assert (sliced[0] == 1); // Slicing test failed
	assert (sliced[1] == 3); // Slicing test failed
	assert (sliced[4] == 9); // Slicing test failed

	sliced = array[2][Slice(1,11,2)];
	assert (sliced[0] == 32 + 1); // Slicing test failed
	assert (sliced[1] == 32 + 3); // Slicing test failed
	assert (sliced[4] == 32 + 9); // Slicing test failed

}

void testIterator(){
	vector<size_t> shape = {16, 16};
	vector<int64_t> basevec  = range(0, 16*16);
	Ndarray<int64_t> array(basevec.data(), shape);
	
	cout << array[Slice(3,8,2)].shape << endl;
// 	for (auto e : array[Slice(3,8,2)]){
// 		cout << e << endl;
// 	}
}

int main(){

	vector<size_t> shape = {16, 16};
	vector<int64_t> basevec  = range(0, 16*16);
	Ndarray<int64_t> array(basevec.data(), shape);

	testGetSlice();
	testAssignment(array);
	testOutOfBounds(array);
	testStaticDims(array);
	testIterator();

	return 0;
}
