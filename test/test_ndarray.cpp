
#include <vector>
#include <numeric>
#include <iostream>
#include <stdexcept>
#include <assert.h>

#include "ndarray.hpp"


void testAssignment(){
	array<size_t, 2> shape = {16, 16};
	vector<int64_t> basevec  = range(product<size_t>(shape));
	auto array2d = ndarray<int64_t, 2>(basevec.data(), shape);
	auto array1d = array2d[0];

	array1d[0] = 3;
	assert(array1d[0] == 3); // single value assignment failed!
	
	array2d[eee] = 21;
	for (auto e: array2d){
		assert(e == 21); // broadcast assignment failed!
	}
	
	array2d[Slice<1>(0,-1,2)][eee] = 42;
	bool tmp = true;
	for (auto e: array1d){
		assert ( e = (tmp) ? 42 : 21); // broadcast assignment failed
		tmp = not tmp;
	}
	
	array2d[__] = array1d;
	for (size_t i=0; i < array2d.shape[0] ; i++){
		assert (array2d[i][0] == array1d[0]); // broadcast assignment failed
		assert (array2d[i][8] == array1d[8]); // broadcast assignment failed
		assert (array2d[i][15] == array1d[15]); // broadcast assignment failed
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
	assert (sliced2[0][0] == 0); // Slicing test failed
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
		for (auto e: array){
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
	array<size_t, 1> shape = {8};
	vector<int64_t> basevec  = range(shape[0]);
	auto array1 = ndarray<int64_t, 1>(basevec.data(), shape);
	auto array2 = array1.broadcastTo<2>({8,8});

	for (size_t i=0; i<array2.shape[0]; i++){
		for (size_t j=0; j<array2.shape[1]; j++){
			assert (array2[i].data.get()[j] == basevec[j]); // broadcasting incorrect
			assert (array2[i][j] == basevec[j]); // broadcasting incorrect
		}
	}

	// should be moved to testIteration at one point in time...
	size_t idx = 0;
	for (auto e: array2){
		idx %= array2.shape[0];
		assert(e == basevec[idx]);
		idx++;
	}
}

int main(){

	testAssignment();
	testOutOfBounds();
	testSlicing();
	testIterator();
	testBroadcasting();

	return 0;
}
