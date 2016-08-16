#ifndef NDARRAY_H
#define NDARRAY_H

#include <iostream>
#include <vector>
#include <functional>
#include <memory>

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
void _deleteArray(T* pointer){
	delete[] pointer;
}

template<typename T>
void _deleteNothing(T* pointer){
}



template<typename T>
class DataIterator : public std::iterator<std::forward_iterator_tag,
										   T,
										   ptrdiff_t,
										   T*,
										   T&>{
private:
	T* ptr_;
	const std::vector<size_t> stride_;
	const std::vector<size_t> shape_;
	std::vector<size_t> index_;
	std::size_t ndim_;
	
	T* getPtr() const{
		return ptr_;
	}

	const T* getConstPtr() const{
		return ptr_;
	}

	std::ptrdiff_t nextIndex(){
		size_t i = ndim_ - 1;
		index_[i] += 1;
		for (i; i--; -1){ 
			if ((index_[i] == shape_[i]) and (i > 0)){
				index_[i] = 0;
				index_[i-1] += 1;
			} else {
				break;
			}
		}
		return stride_[i];
	}

public:

	DataIterator(T* ptr, std::vector<size_t> shape, std::vector<size_t> stride):
		ptr_(ptr), shape_(shape), stride_(stride){
		ndim_ = shape_.size();
		index_ = std::vector<size_t>(ndim_, 0);
	}
	
	DataIterator(const DataIterator<T>& iter) = default; // Maybe I should implement the constructor...

	~DataIterator(){}

	DataIterator<T>& operator=(const DataIterator<T>& iter) = default;

	bool operator==(const DataIterator<T>& iter) const{
		return (ptr_ == iter.getConstPtr());
	}
	bool operator!=(const DataIterator<T>& iter) const{
		return (ptr_ != iter.getConstPtr());
	}
	DataIterator<T>& operator++(){
		ptr_ += nextIndex();
		return (*this);
	}
	DataIterator<T> operator++(ptrdiff_t){
		auto temp(*this);
		ptr_ += nextIndex();
		return temp;
	}
	T& operator*(){
		return *ptr_;
	}

	const T& operator*() const{
		return *ptr_;
	}

	T* operator->(){
		// Is a const version needed??
		return ptr_;
	}

	// T* last(){
		
	// }
};

template<typename T, int N=-1>
	class Ndarray {
 
public:
	std::size_t ndim;
	std::vector<size_t> shape;
	std::vector<size_t> stride;
	std::shared_ptr<T> data;

	Ndarray():
		ndim(0),
		shape(std::vector<size_t>()),
		stride(std::vector<size_t>()),
		data(shared_ptr<T>(nullptr))
		{}

	Ndarray(vector<size_t> shape_, std::function<void(T*)> destructor=_deleteArray<T>):
		ndim(shape_.size()),
		shape(shape_),
		data(shared_ptr<T>(new T[size()], destructor))
	{
		stride = vector<size_t>(ndim, 1);
		checkDimensionality();
	}

	Ndarray(T* data_, vector<size_t> shape_, std::function<void(T*)> destructor=_deleteNothing<T>):
		ndim(shape_.size()),
		shape(shape_),
		data(shared_ptr<T>(data_, destructor))
	{
	 	stride = vector<size_t>(ndim, 1);
		checkDimensionality();
	}

	Ndarray(shared_ptr<T> data_, vector<size_t> shape_):
		ndim(shape_.size()),
		shape(shape_),
		data(data_)
	{
		stride = vector<size_t>(ndim,1);
		checkDimensionality();
	}

	Ndarray(shared_ptr<T> data_, vector<size_t> shape_, vector<size_t> stride_):
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
		slc.update(shape[0]);
		// TODO: implement some sort of check
		int64_t start = slc.start * stride[0];

		vector<size_t> newshape = shape;//(&shape[1],&shape[ndim]);
		vector<size_t> newstride = stride;
		newshape[0] = (slc.stop - slc.start) / slc.step;
		newstride[0] = newstride[0] * slc.step;
		
		return Ndarray<T>(std::shared_ptr<T>(data, data.get()+start), newshape, newstride);
	}

	template<typename U=T, int M=-1>
	const Ndarray<U,M> operator[](Slice slc) const{
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
		return Ndarray<U, M>(std::shared_ptr<T>(data, data.get()+start), newshape);
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



