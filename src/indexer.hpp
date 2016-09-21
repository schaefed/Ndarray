#ifndef INDEXER_H
#define INDEXER_H

#include <vector>

using namespace std;


class Strider {

private:
	vector<size_t> strides_;

	vector<size_t> calcStrides(vector<size_t> shape){
		auto ndim = shape.size();
		auto out = vector<size_t>(ndim, 1);
		for (int i=ndim-2; i>=0 ; --i){
			out[i] = shape[i+1] * out[i+1]; 
		}
		return out;
	}

public:

	Strider(vector<size_t> shape)
	{
		strides_ = calcStrides(shape);
	}
	Strider(vector<size_t> shape, vector<size_t> strides):
		strides_(strides)
	{
	}
	Strider(){}
		
	vector<size_t> getStrides(){
		return strides_;
	}
};

class Indexer {

private:
	vector<size_t> shape_;
	Strider strides_;
	size_t offset_;
	size_t ndim_;

public:

	size_t index_;

	size_t lastIndex() {
		auto strides = strides_.getStrides();
		return strides[0] * shape_[0];
	}

	size_t nextIndex(){

		index_++;
		auto strides = strides_.getStrides();
		auto idx = index_;
		auto i = ndim_-1;
		auto accum = 0;
		while ((i>0) and (idx%shape_[i] == 0)){
			idx /= shape_[i];
			accum += ((shape_[i]-1) * strides[i]);
			i--;
		}
		return strides[i] - accum;
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
