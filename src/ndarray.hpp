#ifndef NDARRAY_H
#define NDARRAY_H

#include <iostream>
#include <vector>
#include <functional>
#include <memory>

#include "exceptions.hpp"

using namespace std;

// template<typename T>
// void print(T elem){
// 	cout << elem << " ";
// }

// template<typename T>
// void println(T elem){
// 	cout << elem << endl;
// }

class Slice {

private:
public:
	int64_t start;
	int64_t stop;
	int64_t step;
	
	Slice(){};
	Slice(int64_t start_, int64_t stop_, int64_t step_=1):
		start(start_),
		stop(stop_),
		step(step_)
	{}

	// void update(size_t length){
	// 	stop = length;
	// }
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
		// Not correct!
		size_t out = 0;
		// int64_t i;
		// int64_t idx = ndim_ - 1;
		// index_[idx] += 1;
		for (int64_t i=ndim_-1; i>=0 ; --i){ 
		// // 	// cout << "looping\n"; 
			if ((index_[i] == shape_[i]) and (i > 0)){
				out += (stride_[i-1] - stride_[i]*shape_[i]);
				index_[i] = 0;
				index_[i-1] += 1;
			} else {
				out += stride_[i];
				break;
			}
			
		}
		return out;
		// // cout << "next: " << stride_[i] << endl;
		// return stride_[i];
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
	// DataIterator<T> operator++(ptrdiff_t){
	// 	auto temp(*this);
	// 	ptr_ += nextIndex();
	// 	return temp;
	// }
	DataIterator<T> operator++(int){
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

	const T* operator->() const{
		// Is a const version needed??
		return ptr_;
	}

	// T* last(){
		
	// }
};

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
		
public:

	typedef DataIterator<T>	iterator;
	typedef DataIterator<const T> const_iterator;

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
		// stride = vector<size_t>(ndim, 1);
		stride = initStrides();
		checkDimensionality();
	}

	Ndarray(T* data_, vector<size_t> shape_, std::function<void(T*)> destructor=_deleteNothing<T>):
		ndim(shape_.size()),
		shape(shape_),
		data(shared_ptr<T>(data_, destructor))
	{
	 	// stride = vector<size_t>(ndim, 1);
		stride = initStrides();
		checkDimensionality();
	}

	Ndarray(shared_ptr<T> data_, vector<size_t> shape_):
		ndim(shape_.size()),
		shape(shape_),
		data(data_)
	{
		// stride = vector<size_t>(ndim,1);
		stride = initStrides();
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

	iterator begin(){
		return iterator(&data.get()[0], shape, stride);
	}
	iterator end(){
		// Not correct!
		size_t idx = 1;
		for (uint i=0; i<shape.size(); i++){
			idx *= (shape[i] * stride[i]);
		}
		return iterator(&data.get()[idx], shape, stride);
	}

	const_iterator cbegin(){
		return iterator(&data.get()[0], shape, stride);
	}
	const_iterator cend(){
		size_t idx = 1;
		for (uint i=0; i<shape.size(); i++){
			idx *= (shape[i] * stride[i]);
		}
		// cout << "end: " << idx << endl;
		return iterator(&data.get()[idx], shape, stride);
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
		int64_t idx = slc.start * stride[0]; 
		vector<size_t> newshape = shape;
		vector<size_t> newstride = stride;
		newshape[0] = (slc.stop - slc.start) / slc.step;
		newstride[0] = newstride[0] * slc.step;
	
		return Ndarray<T>(std::shared_ptr<T>(data, data.get()+idx),
						  newshape, newstride
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

		// if (shape.size() > 1){
		// 	cout << "in: " << idx << endl;
		// 	// cout << "ndim: " << start << endl;
		// 	cout << "start: " << start << endl;
		// 	// cout << "stride: " << stride[0] << endl;
		// 	start*=shape[0];
		// 	cout << "start: " << start << endl;
		// }

		// cout << "idx: " << idx << endl;
		// cout << "start: " << start << endl;
		// cout << "stride[0]: " << stride[0] << endl;
		// cout << "data: " << data.get()[0] << endl;
		// // if more than one dimension we are indexing Ndarrays
		// if (ndim - 1 > 0){
		// 	// start *= shape[1] * stride[1];
		// 	start *= shape[1] * stride[1];
		// }
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



