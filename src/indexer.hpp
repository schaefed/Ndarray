#ifndef INDEXER_H
#define INDEXER_H

#include <vector>

using namespace std;


class Strider {

private:
	vector<size_t> strides_;
	vector<size_t> offsets_;

	vector<size_t> calcStrides(vector<size_t> shape){
		auto ndim = shape.size();
		auto out = vector<size_t>(ndim, 1);
		for (int i=ndim-2; i>=0 ; --i){
			out[i] = shape[i+1] * out[i+1]; 
		}
		return out;
	}

	vector<size_t> calcOffsets(vector<size_t> shape){

		// most likely not the most efficient way to handle this.
		// The loop should be replaced by Ndarray calculations one day...
		// cout << "entered strideOffset()\n";
		auto ndim = shape.size();
		auto tmp = vector<size_t>(ndim, 0);
		for (int64_t i=ndim-1; i>=0; --i){
			tmp[i] = strides_[i] * (shape[i]-1);
			if (i < ndim-1){
				tmp[i] += tmp[i+1];
			}
		}

		auto out = strides_;
		for (int64_t i=ndim-1; i>0; --i){
			out[i-1] = strides_[i-1] - tmp[i];
		}

		return out;
	}

public:

	Strider(vector<size_t> shape)
	{
		strides_ = calcStrides(shape);
		offsets_ = calcOffsets(shape);
	}
	Strider(vector<size_t> shape, vector<size_t> strides):
		strides_(strides)
	{
		offsets_ = calcOffsets(shape);
	}
	Strider(){}
		

	vector<size_t> getOffsets(){
		return offsets_;
	}

	vector<size_t> getStrides(){
		return strides_;
	}
};

class Indexer {

private:
	vector<size_t> shape_;
	Strider strides_;
	size_t offset_;
	size_t index_;
	size_t ndim_;

public:

	size_t lastIndex() {
		// That's probably not the most efficient way...
		auto strides = strides_.getStrides();
		auto offsets = strides_.getOffsets();
		auto out = strides[0] * shape_[0];
		uint64_t i;
		for (i=0; i<ndim_; i++){
			out += (strides[i] * shape_[i]);
		}
		return out + offset_ - strides[i];
	}

	size_t nextIndex(){

		index_++;
		auto strides = strides_.getStrides();
		auto offsets = strides_.getOffsets();
		auto index = index_;
		auto i = ndim_-1;
		while ((i>0) and (index%shape_[i] == 0)){
			index /= shape_[i];
			i--;
		}
		return offsets[i];
	}
	


	Indexer(vector<size_t> shape, Strider strides, size_t offset=0, size_t index=0):
		shape_(shape), strides_(strides), offset_(offset), index_(index){
		ndim_ = shape.size();
	}
	Indexer(){}

	size_t getOffset(){
		return offset_;
	}
};

#endif /* INDEXER_H */
