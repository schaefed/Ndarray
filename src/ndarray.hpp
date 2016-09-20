#ifndef NDARRAY_H
#define NDARRAY_H

#include <iostream>
#include <vector>
#include <functional>
#include <memory>
#include <math.h>

#include "utils.hpp"
#include "slice.hpp"
#include "indexer.hpp"
#include "iterator.hpp"
#include "exceptions.hpp"

using namespace std;

template<typename T, int N=-1>
	class Ndarray {

private:

	vector<size_t> initStrides(){
		auto strides = vector<size_t>(ndim, 1);
		for (int i=ndim-2; i>=0 ; --i){
			strides[i] = shape[i+1] * strides[i+1]; 
		}
		return strides;
	}

	size_t lastIndex() {
		// That's probably not the most efficient way...
		size_t out = stride[0] * shape[0];
		uint64_t i;
		for (i=0; i<ndim; i++){
			out += (stride[i] * shape[i]);
		}
		return out + offset - stride[i];
	}
		
public:

	typedef DataIterator<T>	iterator;
	typedef DataIterator<const T> const_iterator;

	size_t ndim;
	size_t offset;
	vector<size_t> shape;
	vector<size_t> stride;
	shared_ptr<T> data;

	Ndarray():
		ndim(0),
		offset(0),
		shape(vector<size_t>()),
		stride(vector<size_t>()),
		data(shared_ptr<T>(nullptr))
		{}

	Ndarray(vector<size_t> shape_, function<void(T*)> destructor=_deleteArray<T>):
		ndim(shape_.size()),
		offset(0),
		shape(shape_),
		data(shared_ptr<T>(new T[size()], destructor))
	{
		// stride = vector<size_t>(ndim, 1);
		stride = initStrides();
		checkDimensionality();
	}

	Ndarray(T* data_, vector<size_t> shape_, function<void(T*)> destructor=_deleteNothing<T>):
		ndim(shape_.size()),
		offset(0),
		shape(shape_),
		data(shared_ptr<T>(data_, destructor))
	{
	 	// stride = vector<size_t>(ndim, 1);
		stride = initStrides();
		checkDimensionality();
	}

	Ndarray(shared_ptr<T> data_, vector<size_t> shape_, size_t offset_=0):
		ndim(shape_.size()),
		offset(offset_),
		shape(shape_),
		data(data_)
	{
		// stride = vector<size_t>(ndim,1);
		stride = initStrides();
		checkDimensionality();
	}

	Ndarray(shared_ptr<T> data_, vector<size_t> shape_, vector<size_t> stride_, size_t offset_=0):
		ndim(shape_.size()),
		offset(offset_),
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

	iterator begin(){
		return iterator(&data.get()[0], shape, stride);
	}
	iterator end(){
		return iterator(&data.get()[lastIndex()], shape, stride);
	}

	const_iterator cbegin(){
		return iterator(&data.get()[0], shape, stride);
	}
	const_iterator cend(){
		return iterator(&data.get()[lastIndex()], shape, stride);
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
		/* copy asignment operator*/
		swap(*this, that);
		return *this;
	}
		
	void operator=(const T& other){
		/*
		  Asignment is handled here and not in operator[]. This seems to
		  be an easy way to allow asignments of higher complexity
		  (i.e. broadcasting)
		 */
		if (ndim > 1){
			throw range_error("Assigning to a Ndarray is not supported yet!");
		}
		data.get()[0] = other;
	}

	operator T(){
		if (ndim > 0){
			throw range_error("Only length-1 arrays can be converted to scalars!");
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
			if (static_cast<int64_t>(ndim) != N){
				throw DimensionError("Invalid dimensions!");
			}
		}
	}
		
	template<typename U=T, int M=-1>
	Ndarray<U,M> operator[](Slice slc){
		int64_t start = slc.start * stride[0]; 
		vector<size_t> newshape = shape;
		vector<size_t> newstride = stride;
		newshape[0] = ceil((slc.stop - slc.start) / static_cast<double>(slc.step));
		newstride[0] = newstride[0] * slc.step;
	
		return Ndarray<T>(shared_ptr<T>(data, data.get()+start),
						  newshape, newstride, start + offset // is start+data accesibly from the smart pointer?
						  );
	}

	template<typename U=T, int M=-1>
	const Ndarray<U,M> operator[](Slice slc) const{
		return operator[](slc);
	}
	
	template<typename U=T, int M=-1>
	Ndarray<U,M> operator[](int64_t idx){
		checkIndex(idx);
		int64_t start = idx * stride[0];
		vector<size_t> newshape (&shape[1], &shape[ndim]);
		return Ndarray<U, M>(shared_ptr<T>(data, data.get()+start),
							 newshape, start + offset
							 );
	}

	template<typename U, int M=-1>
	const Ndarray<U,M> operator[](int64_t idx) const{
		return operator[](idx);
	}

	size_t size(){
		if (ndim > 0){
			size_t out = 1;
			for (uint64_t i=0; i<ndim; i++){
				out *= shape[i];
			}
			return out;
		}
		return 0;
	}

};

#endif /* NDARRAY_H */
