#ifndef NDARRAY_H
#define NDARRAY_H

#include <iostream>
#include <array>
#include <functional>
#include <memory>
#include <math.h>

#include "utils.hpp"
#include "funcs.hpp"
#include "slice.hpp"
#include "iterator.hpp"
#include "exceptions.hpp"

using namespace std;

Ellipsis eee = Ellipsis();
Ellipsis __ = Ellipsis();

template<typename T, size_t N> class Ndarray;
template<typename T, size_t N> class NdarrayBase;

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

	NdarrayBase<T,N>& operator=(const T& other){
		map([other](T& e) {
				e = other;
			});
		return *this;
	}

	template<size_t M>
	NdarrayBase<T,N>& operator=(const Ndarray<T, M>& other){
		map(
			[](T& l, const T r){
				l = r;
			},
			other
			);
		return (*this);
	}
	
	void map(function<void(T&)> func){
		for (auto& e: (*this)){
			func(e);
		}
	}

	template<size_t M>
	void map(function<void(T&, const T)> func, const Ndarray<T,M> other){
		/*
		 */
		auto tmp = other.broadcastTo(shape);
		auto this_iter = this->begin();
		auto this_end = this->end();
		auto tmp_iter = tmp.begin();
		auto tmp_end = tmp.end();
		while ((this_iter++ != this_end) and (tmp_iter++ != tmp_end)){
			func(*this_iter, *tmp_iter);
		}
		assert((this_iter == this_end) and (tmp_iter == tmp_end));
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
	Ndarray<T, M> broadcastTo(array<size_t, M> newshape) const {
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

	Ndarray<T,N> operator[](Ellipsis){
		return Ndarray<T,N>(*this);
	}

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

	template<size_t DIM>
	const Ndarray<T,N> operator[](int64_t slc) const{
		return operator[](slc);
	}

};


template<typename T>
class Ndarray<T,1> : public NdarrayBase<T,1> {

public:
	using NdarrayBase<T,1>::NdarrayBase ;
	using NdarrayBase<T,1>::operator= ;

	Ndarray<T,1> operator[](Ellipsis){
		return Ndarray<T,1>(*this);
	}

	T& operator[](int64_t idx){
		idx = this->updateIndex(idx, 0);
		this->checkIndex(idx, 0);
		int64_t start = idx * this->strides[0];
		return this->data.get()[start];
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
};

#endif /* NDARRAY_H */
