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

template<typename T, int N> class Ndarray{
		
private:

public:
	
	typedef DataIterator<T>	iterator;
	typedef DataIterator<const T> const_iterator;
	
	size_t ndim;
	size_t size;
	size_t offset;
	shared_ptr<T> data;
	vector<size_t> shape;
	vector<size_t> strides;

	Ndarray(size_t ndim_, size_t size_, size_t offset_, shared_ptr<T> data_,
			vector<size_t> shape_, vector<size_t> strides_):
		ndim(ndim_), size(size_), offset(offset_),
		data(data_), shape(shape_), strides(strides_)
	{
		checkDimensionality();
	}

	Ndarray():
		ndim(0), size(0), offset(0),
		data(shared_ptr<T>(nullptr)), shape(vector<size_t>()), strides(vector<size_t>())
	{}

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
		if (static_cast<int64_t>(ndim) != N){
			throw DimensionError("Invalid dimensions!");
		}
	}
		
	template<size_t DIM>
		Ndarray<T,N> operator[](Slice<DIM> slc){
		if (DIM > ndim){
			throw DimensionError("Invalid slicing dimension");
		}
		slc.update(shape[DIM]);
		auto newshape = shape;
		auto newstrides = strides;
		int64_t start = slc.start * newstrides[DIM]; 
		newshape[DIM] = ceil((slc.stop - slc.start) / static_cast<double>(slc.step));
		newstrides[DIM] = newstrides[DIM] * slc.step;
		return Ndarray<T,N>(N,
							product(newshape),
							start + offset, 
							shared_ptr<T>(data, data.get()+start),
							newshape,
							newstrides // is start+data accesibly from the smart pointer?
							);
	}

	template<size_t DIM>
		const Ndarray<T,N> operator[](Slice<DIM> slc) const{
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
		return Ndarray<T, N-1>(N-1,
							   product(newshape),
							   start + offset,
							   shared_ptr<T>(data, data.get()+start),
							   newshape,
							   newstrides 
							   );
	}

	const Ndarray<T,N> operator[](int64_t idx) const{
		return operator[](idx);

	}

};

template<typename T, size_t N>
class NdarrayTest : public Ndarray<T,N> {

public:
	using Ndarray<T,N>::Ndarray ;
	using Ndarray<T,N>::operator= ;
	NdarrayTest(size_t ndim_, size_t size_, size_t offset_,
				shared_ptr<T> data_,
				vector<size_t> shape_, vector<size_t> strides_):
		Ndarray<T,N>(ndim_, size_, offset_, data_, shape_, strides_){
		
		cout << "ND" << N << endl ;
	}


	// Ndarray<T, N-1> operator[](int64_t idx){
	// 	if (idx < 0){
	// 		idx += this->shape[0];
	// 	}
	// 	// checkIndex(idx);
	// 	vector<size_t> newshape (&(this->shape[1]), &(this->shape[this->ndim]));
	// 	vector<size_t> newstrides (&(this->strides[1]), &(this->strides[this->ndim]));
	// 	int64_t start = idx * this->strides[0];
	// 	return Ndarray<T, N-1>(N-1,
	// 						   product(newshape),
	// 						   start + this->offset,
	// 						   shared_ptr<T>(this->data, this->data.get()+start),
	// 						   newshape,
	// 						   newstrides 
	// 						   );
	// }

	// const Ndarray<T,N> operator[](int64_t idx) const{
	// 	return operator[](idx);

	// }

	// NdarrayTest(const Ndarray<T,N>& other):
	// 	Ndarray<T,N>(other) {}
	
	// NdarrayTest(Ndarray<T,N>&& other):
	// 	Ndarray<T,N>(other){}

};


template<typename T>
class NdarrayTest<T,1> : public Ndarray<T,1> {

public:
	using Ndarray<T,1>::Ndarray ;
	using Ndarray<T,1>::operator= ;
	NdarrayTest(size_t ndim_, size_t size_, size_t offset_,
				shared_ptr<T> data_,
				vector<size_t> shape_, vector<size_t> strides_):
		Ndarray<T,1>(ndim_, size_, offset_, data_, shape_, strides_){
		cout << "1D\n" ;
	}

	NdarrayTest<T,1> operator[](int64_t idx){
		if (idx < 0){
			idx += (this->shape)[0];
		}
		int64_t start = idx * this->strides[0];
		return NdarrayTest<T, 1>(1,
								 1,
								 start + this->offset,
								 shared_ptr<T>(this->data, this->data.get()+start),
			{1}, {1}
								);
	}

	const Ndarray<T,1> operator[](int64_t idx) const{
		return operator[](idx);

	}


};

template<typename T, size_t N>
NdarrayTest<T, N> ndarray(vector<size_t> shape_, function<void(T*)> destructor_=_deleteArray<T>){
	auto size = product(shape_);
	return NdarrayTest<T,N>(shape_.size(),
							size,
							0,
							shared_ptr<T>(new T[size], destructor_),
							shape_,
							cumulativeProduct(shape_)
							);
}

template<typename T, size_t N>
NdarrayTest<T, N> ndarray(T* data_, vector<size_t> shape_, function<void(T*)> destructor_=_deleteNothing<T>){
	return NdarrayTest<T,N>(shape_.size(),
							product(shape_),
							0,
							shared_ptr<T>(data_, destructor_),
							shape_,
							cumulativeProduct(shape_)
							);	
}

// template<typename T, size_t N>
// Ndarray<T, N> ndarray(vector<size_t> shape_, function<void(T*)> destructor_=_deleteArray<T>){
// 	auto size = product(shape_);
// 	return Ndarray<T,N>(shape_.size(),
// 						size,
// 						0,
// 						shared_ptr<T>(new T[size], destructor_),
// 						shape_,
// 						cumulativeProduct(shape_)
// 						);
// }

// template<typename T, size_t N>
// Ndarray<T, N> ndarray(T* data_, vector<size_t> shape_, function<void(T*)> destructor_=_deleteNothing<T>){
// 	return Ndarray<T,N>(shape_.size(),
// 						product(shape_),
// 						0,
// 						shared_ptr<T>(data_, destructor_),
// 						shape_,
// 						cumulativeProduct(shape_)
// 						);	
// }






#endif /* NDARRAY_H */
