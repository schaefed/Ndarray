// #ifndef NDARRAY_H
// #define NDARRAY_H

#pragma once

#include <iostream>
#include <stdexcept>
#include <vector>
#include <assert.h>
#include <memory>

#include "refcounter.hpp"

using namespace std;

template<typename T>
class Ndarray {
 
public:

	std::vector<size_t> shape;
	size_t  ndim;
	SharedPointer<T> data;

	Ndarray(): shape(nullptr), ndim(0), data(nullptr) {}
	

	Ndarray(vector<size_t> shape_){
		ndim = shape_.size();		
		shape = shape_;
		data = SharedPointer<T>(new T[size()]);
	}

	Ndarray(vector<T> &data_){
		ndim = 1;
		shape = {data_.size()};
		data = SharedPointer<T>(&data_[0]);
	}

	Ndarray(vector<T> &data_, vector<size_t> shape_){
		ndim = shape_.size();
		shape = shape_;
		data = SharedPointer<T>(&data_[0]);
	}


	Ndarray(vector<T> &data_, vector<size_t> shape_, size_t refcount_){
		ndim = shape_.size();
		shape = shape_;
		data = SharedPointer<T>(&data_[0], refcount_);
	}

	
	Ndarray(T* data_, vector<size_t> shape_){
		ndim = shape_.size();
		shape = shape_;
		data = SharedPointer<T>(data_);
	}

	Ndarray(T* data_, vector<size_t> shape_, size_t refcount_) {
		ndim = shape_.size();
		shape = shape_;
		data = SharedPointer<T>(data_,refcount_);
	}

	Ndarray(SharedPointer<T> data_, vector<size_t> shape_, size_t ndim_):
		ndim(ndim_), shape(shape_), data(data_) {}


	Ndarray(const Ndarray<T>& other):
		shape(other.shape),ndim(other.ndim),data(other.data){
	}
	
	void checkIndex(int idx){
		if (ndim < 0){
			throw range_error("Too many indices!");
		}
	}

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

	friend void swap(Ndarray<T>& first, Ndarray<T>& second) 
	{
		swap(first.shape, second.shape);
		swap(first.ndim, second.ndim);
		swap(first.data, second.data);		
	}


	operator T(){
		if (ndim > 0){
			throw range_error("Cannot Convert Ndarray to scalar!");
		}
		return data.get()[0];
	}
	
	Ndarray<T> operator[](int idx){
		checkIndex(idx);
		int start = idx;
		if (ndim - 1 > 0){
			start *= shape[1];
		}

		vector<size_t> newshape (&shape[1],&shape[ndim]);
		return Ndarray<T>(data+start,newshape,ndim-1);
		// return SliceProxy(Ndarray<T>(data+start,shape+1,ndim-1));
	}

	const Ndarray<T> operator[](int idx) const{
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


ostream& operator<< (ostream& stream, vector<size_t>shape ) {
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



