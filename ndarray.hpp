#ifndef NDARRAY_H
#define NDARRAY_H

// #pragma once

#include <iostream>
#include <stdexcept>
#include <vector>
#include <assert.h>
#include <memory>

#include "refcounter.hpp"

using namespace std;

class Slice {

private:
	bool stop_given;
public:
	int64_t start;
	int64_t stop;
	int64_t step;

	Slice(int64_t start_, int64_t step_=1):
		start(start_), stop(1), step(step_), stop_given(false) {}

	Slice(int64_t start_, int64_t stop_, int64_t step_=1 ):
		start(start_), stop(stop_), step(step_), stop_given(true) {}

	void update(size_t length){
		if (not stop_given){
			stop = length;
		}
	}
};
	
template<typename T>
class Ndarray {
 
public:

	std::vector<size_t> shape;
	std::vector<size_t> stride;
	size_t  ndim;
	SharedPointer<T> data;

	Ndarray(): shape(nullptr), ndim(0), data(nullptr) {}

	Ndarray(vector<size_t> shape_, size_t refcount=0):
		ndim(shape_.size()),
		shape(shape_),
		data(SharedPointer<T>(new T[size()], refcount)){
		stride = vector<size_t>(ndim,1);
	}

	Ndarray(vector<T> &data_, size_t refcount=1):
		ndim(1),
		shape(data_.size()),
		stride(vector<size_t>(1, 1)),
		data(SharedPointer<T>(&data_[0], refcount)){}

	Ndarray(vector<T> &data_, vector<size_t> shape_, size_t refcount=1):
		ndim(shape_.size()),
		shape(shape_),
	 	data(SharedPointer<T>(&data_[0], refcount)) {
	 	stride = vector<size_t>(ndim, 1);
	}
	
	Ndarray(T* data_, vector<size_t> shape_, size_t refcount=1):
		ndim(shape_.size()), shape(shape_),
		data(SharedPointer<T>(data_, refcount)) {
	 	stride = vector<size_t>(ndim, 1);
		}

	Ndarray(SharedPointer<T> data_, vector<size_t> shape_):
		ndim(shape_.size()), shape(shape_), data(data_) {
		stride = vector<size_t>(ndim,1);
	}

	Ndarray(SharedPointer<T> data_, vector<size_t> shape_, vector<size_t> stride_):
		ndim(shape_.size()), shape(shape_), stride(stride_), data(data_) {}
	

	Ndarray(const Ndarray<T>& other):
		ndim(other.ndim), shape(other.shape), stride(other.stride), data(other.data){}


	Ndarray operator=(Ndarray<T> that){
		swap(*this, that);
		return *this;
	}

	void operator=(const T& other){
		if (ndim > 1){
			throw range_error("Assigning to a Ndarray is not supported yet!");
		}
		data.get()[0] = other;
	}

	friend void swap(Ndarray<T>& first, Ndarray<T>& second){
		swap(first.shape, second.shape);
		swap(first.ndim, second.ndim);
		swap(first.data, second.data);		
	}

	
	// operator Ndarray<N, T>(){
		
	// }
	
	operator T(){
		if (ndim > 0){
			throw range_error("Cannot Convert Ndarray to scalar!");
		}
		return data.get()[0];
	}
	
	void checkIndex(int64_t idx){
		if (ndim < 0){
			throw range_error("Too many indices!");
		}
		if (idx < 0) {
			throw range_error("Negative indices not supported!");
		}
		if (idx >= shape[0]) {
			throw range_error("Index out of bounds");
		}
	}

	Ndarray<T> operator[](Slice slc){
		slc.update(shape[0]);
		// TODO: implement some sort of check
		int64_t start = slc.start * stride[0];

		vector<size_t> newshape = shape;//(&shape[1],&shape[ndim]);
		vector<size_t> newstride = stride;
		newshape[0] = (slc.stop - slc.start) / slc.step;
		newstride[0] = newstride[0] * slc.step;
		
		return Ndarray<T>(data+start, newshape, newstride);
	}

	const Ndarray<T> operator[](Slice slc) const{
		return operator[](slc);
	}

	Ndarray<T> operator[](int64_t idx){
		checkIndex(idx);
		int64_t start = idx * stride[0];
		// if more than one dimension we are indexing Ndarrays
		if (ndim - 1 > 0){
			start *= shape[1] * stride[1];
		}

		vector<size_t> newshape (&shape[1],&shape[ndim]);
		return Ndarray<T>(data+start,newshape);
	}

	const Ndarray<T> operator[](int64_t idx) const{
		return operator[](idx);
	}

	size_t size(){
		if (ndim > 0){
			size_t out = 1;
			for (int i=0; i<ndim; i++){
				out *= shape[i];
			}
			return out;
		}
		return 0;
	}
};


template<typename T>
ostream& operator<< (ostream& stream, vector<T>shape ) {
	stream << '(';
	for (int i = 0; i < shape.size() ; i++){
		stream << shape[i];
		if (i < shape.size() - 1){
			stream << ", ";
		}
	}
	stream << ')';
	return stream;
}

#endif /* NDARRAY_H */



