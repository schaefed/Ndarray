#ifndef NDARRAY_H
#define NDARRAY_H

#include <iostream>
#include <vector>
#include <assert.h>

#include "refcounter.hpp"
#include "exceptions.hpp"

using namespace std;

class Slice {

private:
	bool stop_given;
public:
	int64_t start;
	int64_t stop;
	int64_t step;

	Slice(int64_t start_, int64_t step_=1):
		stop_given(false),
		start(start_),
		stop(1)
	{}

	Slice(int64_t start_, int64_t stop_, int64_t step_=1 ):
		stop_given(true),
		start(start_),
		stop(stop_),
		step(step_)
	{}

	void update(size_t length){
		if (not stop_given){
			stop = length;
		}
	}
};

template<typename T>
void _deletePointer(T* pointer){
	delete pointer;
}

template<typename T>
void _deleteArray(T* pointer){
	delete[] pointer;
}

template<typename T>
void _deleteNothing(T* pointer){
}


template<typename T, int N=-1>
	class Ndarray {
 
public:

	std::size_t ndim;
	std::vector<size_t> shape;
	std::vector<size_t> stride;
	SharedPointer<T> data;

	Ndarray():
		ndim(0),
		shape(std::vector<size_t>()),
		stride(std::vector<size_t>()),
		data(SharedPointer<T>())
		{}

	Ndarray(vector<size_t> shape_, bool manage=true):
		ndim(shape_.size()),
		shape(shape_),
		data(SharedPointer<T>(new T[size()], manage==true ? _deleteArray<T> : _deleteNothing<T>))
	{
		stride = vector<size_t>(ndim, 1);
		checkDimensionality();
	}

	Ndarray(T* data_, vector<size_t> shape_, bool manage=false):
		ndim(shape_.size()),
		shape(shape_),
		data(SharedPointer<T>(data_, manage==true ? _deletePointer<T> : _deleteNothing<T>))
	{
	 	stride = vector<size_t>(ndim, 1);
		checkDimensionality();
	}

	Ndarray(SharedPointer<T> data_, vector<size_t> shape_):
		ndim(shape_.size()),
		shape(shape_),
		data(data_)
	{
		stride = vector<size_t>(ndim,1);
		checkDimensionality();
	}

	Ndarray(SharedPointer<T> data_, vector<size_t> shape_, vector<size_t> stride_):
		ndim(shape_.size()),
		shape(shape_),
		stride(stride_),
		data(data_)
	{
		checkDimensionality();
	}
	
	Ndarray(const Ndarray<T,N>& other):
		/* copy constructor */
		ndim(other.ndim),
		shape(other.shape),
		stride(other.stride),
		data(other.data)
	{
		checkDimensionality();
	}
		
	
	Ndarray(Ndarray<T,N>&& other):
		/* move constructor */
		Ndarray<T,N>()
	{
		swap(*this, other);
	}
			
	template<typename U, int M=-1>
	operator Ndarray<U,M>(){
		/*
		  Allows to convert in both directions:
		  1. Ndarray<T>   -> Ndarray<T,N>
		  2. Ndarray<T,N> -> Ndarray<T>
		 */
		return Ndarray<U,M>(this->data, this->shape, this->stride);
	}
	
		
	Ndarray<T,N>& operator=(Ndarray<T,N> that){
		swap(*this, that);
		return *this;
	}
		
	void operator=(const T& other){
		if (ndim > 1){
			throw range_error("Assigning to a Ndarray is not supported yet!");
		}
		data.get()[0] = other;
	}

	operator T(){
		if (ndim > 0){
			throw range_error("Cannot Convert Ndarray to scalar!");
		}
		return data.get()[0];
	}
	friend void swap(Ndarray<T,N>& first, Ndarray<T,N>& second){
		swap(first.ndim, second.ndim);
		swap(first.shape, second.shape);
		swap(first.stride, second.stride);
		swap(first.data, second.data);		
	}

	void checkIndex(uint64_t idx){
		if (ndim < 1){
			throw IndexError("Too many indices!");
		}
		if (idx >= shape[0]) {
			throw IndexError("Index out of bounds");
		}
	}

	void checkDimensionality(){
		if (N > 0) {
			if ((int64_t)ndim != N){
				throw DimensionError("Invalid dimensions!");
			}
		}
	}
		
	template<typename U=T, int M=-1>
	Ndarray<T,N> operator[](Slice slc){
		slc.update(shape[0]);
		// TODO: implement some sort of check
		int64_t start = slc.start * stride[0];

		vector<size_t> newshape = shape;//(&shape[1],&shape[ndim]);
		vector<size_t> newstride = stride;
		newshape[0] = (slc.stop - slc.start) / slc.step;
		newstride[0] = newstride[0] * slc.step;
		
		return Ndarray<T>(data+start, newshape, newstride);
	}

	template<typename U=T, int M=-1>
	const Ndarray<T,N> operator[](Slice slc) const{
		return operator[](slc);
	}
	
	template<typename U=T, int M=-1>
	Ndarray<U,M> operator[](int64_t idx){
		checkIndex(idx);
		int64_t start = idx * stride[0];
		// if more than one dimension we are indexing Ndarrays
		if (ndim - 1 > 0){
			start *= shape[1] * stride[1];
		}

		vector<size_t> newshape (&shape[1], &shape[ndim]);
		return Ndarray<U, M>(data+start, newshape);
	}

	template<typename U, int M=-1>
	const Ndarray<U,M> operator[](int64_t idx) const{
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
	for (size_t i = 0; i < shape.size() ; i++){
		stream << shape[i];
		if (i < shape.size() - 1){
			stream << ", ";
		}
	}
	stream << ')';
	return stream;
}

#endif /* NDARRAY_H */



