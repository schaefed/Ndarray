#ifndef NDARRAY_H
#define NDARRAY_H

#include <iostream>
#include <array>
#include <functional>
#include <memory>
#include <math.h>

#include "utils.hpp"
#include "slice.hpp"
#include "iterator.hpp"
#include "exceptions.hpp"

using namespace std;

template<typename T, size_t N> class Ndarray;

template<typename T, size_t N> class NdarrayBase {
		
private:

public:
	
	typedef DataIterator<T, N>	iterator;
	typedef DataIterator<const T, N> const_iterator;
	
	size_t size;
	size_t offset;
	shared_ptr<T> data;
	array<size_t, N> shape;
	array<size_t, N> strides;

	NdarrayBase(size_t size_, size_t offset_, shared_ptr<T> data_,
				array<size_t, N> shape_, array<size_t, N> strides_):
		size(size_),
		offset(offset_),
		data(data_),
		shape(shape_),
		strides(strides_)
	{}

	NdarrayBase():
		size(0),
		offset(0),
		data(shared_ptr<T>(nullptr)),
		shape(array<size_t, N>()),
		strides(array<size_t, N>())
	{}

	NdarrayBase(const NdarrayBase<T,N>& other):
		/* copy constructor */
		size(other.size),
		offset(other.offset),
		data(other.data),
		shape(other.shape),
		strides(other.strides)
	{}
	
	NdarrayBase(NdarrayBase<T,N>&& other):
		/* move constructor */
		NdarrayBase<T,N>()
	{
		swap(*this, other);
	}

	iterator begin(){
		return iterator(&data.get()[0], shape, strides);
	}
	iterator end(){
		auto end = shape[0] * filterZeros(strides)[0];
		return iterator(&data.get()[end], shape, strides);
	}

	const_iterator cbegin(){
		return iterator(&data.get()[0], shape, strides);
	}
	const_iterator cend(){
		auto end = shape[0] * filterZeros(strides)[0];
		return iterator(&data.get()[end], shape, strides);
	}

	NdarrayBase<T,N>& operator=(NdarrayBase<T,N> that){
		/* copy asignment operator
		   obsolete when array value assignment is implemeted
		 */
		swap(*this, that);
		return *this;
	}
		
	void operator=(const T& other){
		/*
		  Asignment is handled here and not in operator[]. This seems to
		  be an easy way to allow asignments of higher complexity
		  (i.e. broadcasting)
		 */
		map([other](T& e) {
				e = other;
			});
	}

	void map(function<void(T&)> func){
		for (auto& e: (*this)){
			func(e);
		}
	}
	
	friend void swap(NdarrayBase<T,N>& first, NdarrayBase<T,N>& second){
		swap(first.size, second.size);
		swap(first.offset, second.offset);
		swap(first.shape, second.shape);
		swap(first.data, second.data);		
		swap(first.strides, second.strides);
	}

	void checkIndex(int64_t idx, uint64_t dim){
		if (N < 1){
			throw IndexError("Too many indices!");
		}
		if (dim > (N-1)){
			throw IndexError("Invalid index dimension!");
		}
		if ((idx < 0) or (idx >= static_cast<int64_t>(shape[dim]))) {
			throw IndexError("Index out of bounds");
		}
	}

	template<size_t DIM>
	void checkIndex(Slice<DIM> slc){
		checkIndex(slc.start, DIM);
		// stop can be larger than the dimension length,
		// at least numpy handles it that way
		// checkIndex(slc.stop, DIM);
	}

	size_t updateIndex(int64_t idx, size_t dim){
		if (idx < 0){
			idx += this->shape[dim];
		}
		return idx;
	}
	

	template<size_t M>
	Ndarray<T, M> broadcastTo(array<size_t, M> newshape){
		static_assert(N < M, "Can only broadcast to higher dimensionality!");
		
	 	array<size_t, M> newstrides = filledArray<size_t, M>(0);
		
		auto offset = newshape.size() - shape.size();

		for (int64_t i=shape.size()-1; i>=0; --i){
			if (newshape[i+offset] == shape[i]){
				newstrides[i+offset] = strides[i];
			} else if (shape[i] != 1){
				throw DimensionError("Operands could not be braodcast together");
			}
		}
		return Ndarray<T, M>(product(newshape), this->offset, this->data, newshape, newstrides);
	}
};


template<typename T, size_t N>
class Ndarray : public NdarrayBase<T,N> {

public:
	using NdarrayBase<T,N>::NdarrayBase ;
	using NdarrayBase<T,N>::operator= ;

	template<size_t DIM>
	Ndarray<T,N> operator[](Slice<DIM> slc){
		static_assert(DIM < N, "Invalid index dimension!");
		
		slc.update(this->shape[DIM]);
		this->checkIndex(slc);

		auto newshape = this->shape;
		auto newstrides = this->strides;
		int64_t start = slc.start * newstrides[DIM]; 
		newshape[DIM] = ceil((slc.stop - slc.start) / static_cast<double>(slc.step));
		newstrides[DIM] = newstrides[DIM] * slc.step;
		return Ndarray<T,N>(product(newshape),
							start + this->offset, 
							shared_ptr<T>(this->data, (this->data).get()+start),
							newshape,
							newstrides // is start+data accesibly from the smart pointer?
							);
	}

	template<size_t DIM>
	const Ndarray<T,N> operator[](Slice<DIM> slc) const{
		return operator[](slc);
	}

	
	Ndarray<T,N-1> operator[](int64_t idx){

		idx = this->updateIndex(idx, 0);
		this->checkIndex(idx, 0);
		
		array<size_t, N-1> newshape = subarray<size_t, N-1>(this->shape, 1);
		int64_t start = idx * this->strides[0];

		return Ndarray<T, N-1>(product(newshape),
							   start + this->offset,
							   shared_ptr<T>(this->data, this->data.get()+start),
							   newshape,
							   subarray<size_t, N-1>(this->strides, 1)
							   );
	}
};


template<typename T>
class Ndarray<T,1> : public NdarrayBase<T,1> {

public:
	using NdarrayBase<T,1>::NdarrayBase ;
	using NdarrayBase<T,1>::operator= ;

	Ndarray<T,1> operator[](int64_t idx){
		idx = this->updateIndex(idx, 0);
		this->checkIndex(idx, 0);
		int64_t start = idx * this->strides[0];
		return Ndarray<T, 1>(1,
							 start + this->offset,
							 shared_ptr<T>(this->data, this->data.get()+start),
			                 {1},
			                 {1}
							 );
	}

	const Ndarray<T,1> operator[](int64_t idx) const{
		return operator[](idx);
	}

	Ndarray<T,1> operator[](Slice<0> slc){

		slc.update(this->shape[0]);
		this->checkIndex(slc);

		auto newshape = this->shape;
		auto newstrides = this->strides;
		int64_t start = slc.start * newstrides[0]; 

		newshape[0] = ceil((slc.stop - slc.start) / static_cast<double>(slc.step));
		newstrides[0] = newstrides[0] * slc.step;

		return Ndarray<T,1>(1,
							start + this->offset, 
							shared_ptr<T>(this->data, (this->data).get()+start),
							newshape,
							newstrides // is start+data accesibly from the smart pointer?
							);
	}

	const Ndarray<T,1> operator[](Slice<0> slc) const{
		return operator[](slc);
	}
	
	operator T(){
		if (this->size != 1){
			throw range_error("Only length-1 arrays can be converted to scalars!");
		}
		return this->data.get()[0];
	}
	
};

template<typename T, size_t N>
Ndarray<T, N> ndarray(array<size_t, N> shape_, function<void(T*)> destructor_=_deleteArray<T>){
	auto size = product(shape_);
	return Ndarray<T,N>(size,
						0,
						shared_ptr<T>(new T[size], destructor_),
						shape_,
						cumulativeProduct(shape_)
						);
}

template<typename T, size_t N>
Ndarray<T, N> ndarray(T* data_, array<size_t, N> shape_, function<void(T*)> destructor_=_deleteNothing<T>){
	return Ndarray<T,N>(product(shape_),
						0,
						shared_ptr<T>(data_, destructor_),
						shape_,
						cumulativeProduct(shape_)
						);	
}

template<typename T, size_t N>
Ndarray<T, N> ndarray(T* data_, array<size_t, N> shape_, array<size_t, N> strides_, function<void(T*)> destructor_=_deleteNothing<T>){
	return Ndarray<T,N>(product(shape_),
						0,
						shared_ptr<T>(data_, destructor_),
						shape_,
						strides_
						);	
}

#endif /* NDARRAY_H */
