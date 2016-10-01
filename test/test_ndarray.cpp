
#include <vector>
#include <numeric>
#include <iostream>
#include <stdexcept>
#include <assert.h>

#include "ndarray.hpp"


void testAssignment(){
	array<size_t, 2> shape = {16, 16};
	vector<int64_t> basevec  = range(product<size_t>(shape));
	auto array = ndarray<int64_t, 2>(basevec.data(), shape);

	array = 21;
	for (auto e: array){
		assert(e == 21); // array assignment failed!
	}
	
	array[Slice<1>(0,-1,2)] = 42;
	bool tmp = true;
	for (auto e: array){
		assert ( e = (tmp) ? 42 : 21); // array assignment failed
		tmp = not tmp;
	}
	
}

void testOutOfBounds(){
	array<size_t, 2> shape = {16, 16};
	vector<int64_t> basevec  = range(product<size_t>(shape));
	auto array = ndarray<int64_t, 2>(basevec.data(), shape);
	
	try{
		array[array.shape[0]];
		assert(false); // exception not thrown!
	} catch(IndexError){}
}

void testSlicing(){

	array<size_t, 2> shape = {16, 16};
	vector<int64_t> basevec  = range(0, 16*16);
	auto array = ndarray<int64_t, 2>(basevec.data(), shape);
	Ndarray<int64_t, 1> sliced1;
	Ndarray<int64_t, 2> sliced2;

	sliced2 = array[Slice<>(0,3)];
	assert (sliced2.shape[0] == 3); // Slicing return wrong shape
	assert (sliced2.shape[1] == 16); // Slicing return wrong shape
	// assert (sliced2[0][0] == 0); // Slicing test failed
	cout << sliced2[0][1] << endl;
	assert (sliced2[0][7] == 7); // Slicing test failed
	assert (sliced2[0][15] == 15); // Slicing test failed
	assert (sliced2[1][0] == 16); // Slicing test failed
	assert (sliced2[1][7] == 23); // Slicing test failed
	assert (sliced2[1][15] == 31); // Slicing test failed
	assert (sliced2[2][0] == 32); // Slicing test failed
	assert (sliced2[2][7] == 39); // Slicing test failed
	assert (sliced2[2][15] == 47); // Slicing test failed

	sliced1 = array[0][Slice<>(1,8)];
	assert (sliced1.shape[0] == 7); // Slicing return wrong shape
	assert (sliced1[0] == 1); // Slicing test failed
	assert (sliced1[1] == 2); // Slicing test failed
	assert (sliced1[4] == 5); // Slicing test failed
	assert (sliced1[6] == 7); // Slicing test failed

	sliced1 = array[0][Slice<>(1,11,2)];
	assert (sliced1.shape[0] == 5); // Slicing return wrong shape
	assert (sliced1[0] == 1); // Slicing test failed
	assert (sliced1[1] == 3); // Slicing test failed
	assert (sliced1[4] == 9); // Slicing test failed

	sliced1 = array[2][Slice<>(1,11,2)];
	assert (sliced1.shape[0] == 5); // Slicing return wrong shape
	assert (sliced1[0] == 32 + 1); // Slicing test failed
	assert (sliced1[1] == 32 + 3); // Slicing test failed
	assert (sliced1[4] == 32 + 9); // Slicing test failed

	sliced1 = array[-1];
	assert (sliced1[0] == 240); // negative integer slicing failed
	assert (sliced1[6] == 246); // negative integer slicing failed
	assert (sliced1[-10] == 246); // negative integer slicing failed
	assert (sliced1[-1] == 255); // negative integer slicing failed

	sliced1 = array[0][Slice<>(0,-3)];
	assert (sliced1[0] == 0); // negative integer slicing failed
	assert (sliced1[6] == 6); // negative integer slicing failed
	assert (sliced1[12] == 12); // negative integer slicing failed
	assert (sliced1[-1] == 12); // negative integer slicing failed
		
	sliced1 = array[-5][Slice<>(-12,-3,2)];
	assert (sliced1[0] == 180); // negative integer slicing failed
	assert (sliced1[2] == 184); // negative integer slicing failed
	assert (sliced1[-3] == 184); // negative integer slicing failed

	sliced2 = array[Slice<0>(0,-1,2)][Slice<1>(0,-1,2)];
	assert (sliced2[0][0] == 0);
	assert (sliced2[0][3] == 6);
	assert (sliced2[0][-1] == 14);

	assert (sliced2[-2][0] == 192);
	assert (sliced2[-2][3] == 198);
	assert (sliced2[-2][-1] == 206);
}

void testIterator(){
	array<size_t, 2> shape = {4, 4};
	vector<int64_t> basevec  = range(product<size_t>(shape));
	auto array = ndarray<int64_t, 2>(basevec.data(), shape);
	Ndarray<int64_t,2> test;

	auto tester = [](auto array, vector<int64_t> check){
		int64_t i = 0;
		// cout << "shape: " << array.shape << endl;
		// cout << "strides: " << array.strides << endl;
		for (auto e: array){
			// cout << e  << " vs. " << check[i] << endl;
			assert (e == check[i++]); // Wrong iteration element
		}
	};

	tester(array, range(16));
	tester(array[3], range(12, 16));
	tester(array[Slice<>(0, 2)], range(8));
	tester(array[Slice<>(0, 4, 2)],
		   concat<int64_t>({range(4), range(8, 12)})
		   );
}

void testBroadcasting(){
	array<size_t, 2> shape = {8, 8};
	array<size_t, 2> strides = {0, 1};
	vector<int64_t> basevec  = range(shape[0]);
	auto array = ndarray<int64_t, 2>(basevec.data(), shape, strides);

	for (size_t i=0; i<shape[0]; i++){
		for (size_t j=0; j<shape[1]; j++){
			assert (array[i].data.get()[j] == basevec[j]); // broadcasting incorrect
			assert (array[i][j] == basevec[j]); // broadcasting incorrect
		}
	}

	// should be moved to testIteration at one point in time...
	size_t idx = 0;
	for (auto e: array){
		idx %= array.shape[0];
		assert(e == basevec[idx]);
		idx++;
	}
	
}

int main(){

	// testAssignment();
	// testOutOfBounds();
	testSlicing();
	testIterator();
	testBroadcasting();

	return 0;
}
