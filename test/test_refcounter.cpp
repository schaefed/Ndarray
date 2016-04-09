
#include <vector>
#include <numeric>
#include <iostream>
#include <stdexcept>
#include <assert.h>

#include "refcounter.hpp"


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

void testCounting(){

	typedef SharedPointer<int64_t> sptr;

	auto vec = range(0, 100, 2); 
	auto references = std::vector<sptr>();
	int niter = 10;
	
	sptr managed (vec.data(), [](int64_t* pointer)->void{});
	assert(managed.getCount() == 1); // reference counter incorrect!

	for (int i=0; i<niter; i++){
		assert (managed.getCount() == (size_t)i+1); // Refernce counting incorrect
		references.push_back(sptr(managed));
		auto tmp1 = sptr(managed);
		auto tmp2 = sptr(managed);
		assert(tmp1.getCount() == managed.getCount()); // Reference counter not identical
		assert(tmp2.getCount() == managed.getCount()); // Reference counter not identical
	}

	for (int i=0; i<niter; i++){
		references.pop_back();
		assert(managed.getCount() == (size_t)niter-i); // reference counter incorrect!
	}
	assert(managed.getCount() == 1); // reference counter incorrect!
}

void testSlicing(){

	typedef SharedPointer<int64_t> sptr;

	auto vec = range(0, 100, 2); 
	sptr managed (vec.data(), [](int64_t* pointer) -> void{});

	for (uint i=0 ; i<vec.size() ; i++){
		assert(vec[i] == managed[i]) ; // Element access incorrect
	}
}

int main(){
	testCounting();
	testSlicing();
	return 0;
}
