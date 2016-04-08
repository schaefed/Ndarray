
#include <vector>
#include <numeric>
#include <iostream>
#include <stdexcept>

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

// void testCounting(){
// 	auto vec = range(0, 100, 2); 
// 	SharedPointer<int64_t> manged (vec.data(), [](int64_t* pointer)->void{});
// 	for (int i; i < 10; i++){
// 		SharedPointer<int64_t>ref(managed);
// 		cout << ref.getCount() << endl;
// 	}
// }

int main(){
	cout << "Testing\n";
	// testCounting();
	// auto vec = range(0, 100, 2); 
	// SharedPointer<int64_t> test (vec.data(), [](int64_t* pointer)->void{});
	return 0;
}
