#ifndef SLICE_H
#define SLICE_H

using namespace std;

class Slice {

private:
public:
	int64_t start;
	int64_t stop;
	int64_t step;
	
	Slice(){};
	Slice(int64_t start_, int64_t stop_, int64_t step_=1):
		start(start_),
		stop(stop_),
		step(step_)
	{}

	// void update(size_t length){
	// 	stop = length;
	// }
};


#endif /* SLICE_H */
