#ifndef NDARRAY_H
#define NDARRAY_H

#include <iostream>
#include <vector>
#include <functional>
#include <memory>
#include <math.h>

#include "utils.hpp"
#include "slice.hpp"
#include "iterator.hpp"
#include "exceptions.hpp"

using namespace std;

template<typename T, size_t N> class Ndarray {
		
private:

	vector<size_t> calcStrides(){
		auto out = vector<size_t>(ndim, 1);
		for (int i=ndim-2; i>=0 ; --i){
			out[i] = shape[i+1] * out[i+1]; 
		}
		return out;
	}

public:
	
	typedef DataIterator<T>	iterator;
	typedef DataIterator<const T> const_iterator;
	
	size_t ndim;
	size_t size;
	size_t offset;
	shared_ptr<T> data;
	vector<size_t> shape;
	vector<size_t> strides;

	Ndarray():
		ndim(0),
		size(0),
		offset(0),
		data(shared_ptr<T>(nullptr)),
		shape(vector<size_t>()),
		strides(vector<size_t>())
		{}

	Ndarray(vector<size_t> shape_, function<void(T*)> destructor=_deleteArray<T>):
		ndim(shape_.size()),
		size(product(shape_)),
		offset(0),
		data(shared_ptr<T>(new T[size()], destructor)),
		shape(shape_)
	{
		strides = calcStrides();
		checkDimensionality();
	}

	Ndarray(T* data_, vector<size_t> shape_, function<void(T*)> destructor=_deleteNothing<T>):
		ndim(shape_.size()),
		size(product(shape_)),
		offset(0),
		data(shared_ptr<T>(data_, destructor)),
		shape(shape_)
	{
		strides = calcStrides();
		checkDimensionality();
	}

	Ndarray(shared_ptr<T> data_, vector<size_t> shape_, size_t offset=0):
		ndim(shape_.size()),
		size(product(shape_)),
		offset(offset),
		data(data_),
		shape(shape_)
	{
		strides = calcStrides();
		checkDimensionality();
	}


	Ndarray(shared_ptr<T> data_, vector<size_t> shape_, vector<size_t> strides_, size_t offset=0):
		ndim(shape_.size()),
		size(product(shape_)),
		offset(offset),
		data(data_),
		shape(shape_),
		strides(strides_)
	{
		checkDimensionality();
	}

	Ndarray(const Ndarray<T,N>& other):
		/* copy constructor */
		ndim(other.ndim),
		size(other.size),
		offset(other.offset),
		data(other.data),
		shape(other.shape),
		strides(other.strides)
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
		return iterator(&data.get()[0], shape, strides);
	}
	iterator end(){
		return iterator(&data.get()[shape[0] * strides[0]], shape, strides);
	}

	const_iterator cbegin(){
		return iterator(&data.get()[0], shape, strides);
	}
	const_iterator cend(){
		return iterator(&data.get()[shape[0] * strides[0]], shape, strides);
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
		swap(first.size, second.size);
		swap(first.offset, second.offset);
		swap(first.shape, second.shape);
		swap(first.data, second.data);		
		swap(first.strides, second.strides);
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
		if (N != ndim){
			throw DimensionError("Invalid shape for array of given dimensions!");
		}
	}
		
	template<size_t dim=0>
	Ndarray<T, N> operator[](Slice<dim> slc){
		if (dim > ndim){
			throw DimensionError("Invalid slicing dimension");
		}
		slc.update(shape[dim]);
		auto newshape = shape;
		auto newstrides = strides;
		int64_t start = slc.start * newstrides[dim]; 
		newshape[dim] = ceil((slc.stop - slc.start) / static_cast<double>(slc.step));
		newstrides[dim] = newstrides[dim] * slc.step;
		return Ndarray<T, N>(shared_ptr<T>(data, data.get()+start),
							 newshape, newstrides, start + offset // is start+data accesibly from the smart pointer?
							 );
	}

	template<size_t dim>
	const Ndarray<T, N> operator[](Slice<dim> slc) const{
		return operator[](slc);
	}
	
	Ndarray<T,N-1> operator[](int64_t idx){
		if (idx < 0){
			idx += shape[0];
		}
		checkIndex(idx);
		vector<size_t> newshape (&shape[1], &shape[ndim]);
		vector<size_t> newstrides (&strides[1], &strides[ndim]);
		int64_t start = idx * strides[0];
		return Ndarray<T, N-1>(shared_ptr<T>(data, data.get()+start),
							   newshape, newstrides, start + offset
							   );
	}

	const Ndarray<T, N-1> operator[](int64_t idx) const{
		return operator[](idx);

	}

};

#endif /* NDARRAY_H */
