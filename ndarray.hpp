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
private:
	class SliceProxy{
		/*
		  If this class would be implemented as a child of
		  Ndarray<T>, it would be poosible to acces all 
		  member functions and things like 
		  cout << array[0].shape should work.
		*/
	private:
		Ndarray<T> owner;  
	public:
		SliceProxy(Ndarray<T> owner_ ) : owner( owner_ ) {}

		operator Ndarray<T>(){
			return owner;
		}
    
		operator T(){
			if (owner.ndim > 0){
				throw range_error("Cannot Convert Ndarray to scalar!");
			}
			return owner.data.get()[0];
		}
		
		SliceProxy operator[](int idx){
			return owner[idx];
		}

		// check for different semantics
		void operator=(const T& other){
			if (owner.ndim > 1){
				throw range_error("Assigning to a Ndarray is not supported yet!");
			}
			owner.data.get()[0] = other;
		}
		
	};

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
  
public:

	std::vector<size_t> shape;
	size_t  ndim;
	SharedPointer<T> data;

	Ndarray(): shape(nullptr), ndim(0), data(nullptr) {}
	

	/* Create an empty Ndarray */
	Ndarray(std::vector<size_t> shape_)
	{
		ndim = shape_.size();		
		shape = shape_;
		data = SharedPointer<T>(new T[size()]);
	}

	/* Wrap pointer to existing data */
	Ndarray(std::vector<T> &data_){
		ndim = 1;
		shape = {data_.size()};
		data = SharedPointer<T>(&data_[0]);
	}


	/* Wrap pointer to existing data */
	Ndarray(std::vector<T> &data_, std::vector<size_t> shape_){
		ndim = shape_.size();
		shape = shape_;
		data = SharedPointer<T>(&data_[0]);
	}


	/* Wrap pointer to existing data */
	Ndarray(std::vector<T> &data_, std::vector<size_t> shape_, size_t refcount_){
		ndim = shape_.size();
		shape = shape_;
		data = SharedPointer<T>(&data_[0], refcount_);
	}

	
	/* Wrap pointer to existing data */
	Ndarray(T* data_, std::vector<size_t> shape_){
		ndim = shape_.size();
		shape = shape_;
		data = SharedPointer<T>(data_);
	}

	/* Wrap pointer to existing data, with given refcount */
	Ndarray(T* data_, std::vector<size_t> shape_, size_t refcount_) {
		ndim = shape_.size();
		shape = shape_;
		data = SharedPointer<T>(data_,refcount_);
	}

	Ndarray(SharedPointer<T> data_, std::vector<size_t> shape_, size_t ndim_):
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

	friend void swap(Ndarray<T>& first, Ndarray<T>& second) 
	{
		
		// enable ADL (not necessary in our case, but good practice)
		using std::swap;
		
		swap(first.shape, second.shape);
		swap(first.ndim, second.ndim);
		swap(first.data, second.data);		
	}

	
	SliceProxy operator[](int idx){
		// cout << "IN: operator[](int idx)\n";
		checkIndex(idx);
		int start = idx;
		if (ndim - 1 > 0){
			start *= shape[1];
		}
		// SliceProxy out(Ndarray<T>(data+start,shape+1,ndim-1));
		// cout << "OUT: operator[](int idx)\n";
				
		// return out;
		std::vector<size_t> newshape (&shape[1],&shape[ndim]);
		return SliceProxy(Ndarray<T>(data+start,newshape,ndim-1));
		// return SliceProxy(Ndarray<T>(data+start,shape+1,ndim-1));
	}

	const SliceProxy operator[](int idx) const{
		return operator[](idx);
	}

};


// not compiling yet
// template<typename T> NdarraySlice : Ndarray<T>{
// }


std::ostream& operator<< (std::ostream& stream, std::vector<size_t>shape ) {
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



