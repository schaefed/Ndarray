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

template<typename T, int N=-1> class Ndarray {
		
private:

public:
	
	typedef DataIterator<T>	iterator;
	typedef DataIterator<const T> const_iterator;
	
	size_t ndim;
	vector<size_t> shape;
	shared_ptr<T> data;
	Strider strider;
	Indexer indexer;
	// vector<size_t> stride;

	Ndarray():
		ndim(0),
		shape(vector<size_t>()),
		data(shared_ptr<T>(nullptr))
		{}

	Ndarray(vector<size_t> shape_, function<void(T*)> destructor=_deleteArray<T>):
		ndim(shape_.size()),
		shape(shape_),
		data(shared_ptr<T>(new T[size()], destructor))
	{
		strider = Strider(shape);
		indexer = Indexer(shape, strider);
		checkDimensionality();
	}

	Ndarray(T* data_, vector<size_t> shape_, function<void(T*)> destructor=_deleteNothing<T>):
		ndim(shape_.size()),
		shape(shape_),
		data(shared_ptr<T>(data_, destructor))
	{
		strider = Strider(shape_);
		indexer = Indexer(shape, strider);
		checkDimensionality();
	}

	Ndarray(shared_ptr<T> data_, vector<size_t> shape_, size_t offset=0):
		ndim(shape_.size()),
		shape(shape_),
		data(data_)
	{
		strider = Strider(shape);
		indexer = Indexer(shape, strider, offset);
		checkDimensionality();
	}


	Ndarray(shared_ptr<T> data_, vector<size_t> shape_, vector<size_t> stride_, size_t offset=0):
		ndim(shape_.size()),
		shape(shape_),
		data(data_)
	{
		strider = Strider(shape, stride_);
		indexer = Indexer(shape, strider, offset);
		checkDimensionality();
	}

	Ndarray(shared_ptr<T> data_, vector<size_t> shape_, Strider strider_, Indexer indexer_):
		ndim(shape_.size()),
		shape(shape_),
		data(data_),
		strider(strider_),
		indexer(indexer_)
	{
		checkDimensionality();
	}

	Ndarray(const Ndarray<T,N>& other):
		/* copy constructor */
		ndim(other.ndim),
		shape(other.shape),
		data(other.data),
		strider(other.strider),
		indexer(other.indexer)
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
		return iterator(&data.get()[0], indexer);
	}
	iterator end(){
		// cout << "end" << endl;
		// cout << "end->last: " << data.get()[indexer.lastIndex()] << endl;
		return iterator(&data.get()[indexer.lastIndex()], indexer);
	}

	const_iterator cbegin(){
		return iterator(&data.get()[0], indexer);
	}
	const_iterator cend(){
		// cout << "cend" << endl;
		return iterator(&data.get()[indexer.lastIndex()], indexer);
	}

	
	template<typename U, int M=-1>
	operator Ndarray<U,M>(){
		/*
		  Allows to convert in both directions:
		  1. Ndarray<T>   -> Ndarray<T,N>
		  2. Ndarray<T,N> -> Ndarray<T>
		 */
		return Ndarray<U,M>(this->data,
							this->shape,
							this->strider,
							this->indexer);
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
		swap(first.data, second.data);		
		swap(first.strider, second.strider);
		swap(first.indexer, second.indexer);
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
		auto newshape = shape;
		auto newstrides = strider.getStrides();
		// cout << "strides: "<< strider.getStrides() << endl;
		// cout << "shape: " << shape << endl;
		int64_t start = slc.start * newstrides[0]; 
		newshape[0] = ceil((slc.stop - slc.start) / static_cast<double>(slc.step));
		newstrides[0] = newstrides[0] * slc.step;
		// cout << "newstrides: "<< newstrides << endl;
		// cout << "newshape: " << newshape << endl;
		return Ndarray<T>(shared_ptr<T>(data, data.get()+start),
						  newshape, newstrides, start + indexer.getOffset() // is start+data accesibly from the smart pointer?
						  );
	}

	template<typename U=T, int M=-1>
	const Ndarray<U,M> operator[](Slice slc) const{
		return operator[](slc);
	}
	
	template<typename U=T, int M=-1>
	Ndarray<U,M> operator[](int64_t idx){
		checkIndex(idx);
		auto strides = strider.getStrides();
		vector<size_t> newshape (&shape[1], &shape[ndim]);
		int64_t start = idx * strides[0];
		// vector<size_t> newshape (&shape[1], &shape[ndim]);
		return Ndarray<U, M>(shared_ptr<T>(data, data.get()+start),
							 newshape, start + indexer.getOffset()
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
