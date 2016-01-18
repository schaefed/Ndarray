// #ifndef NDARRAY_H
// #define NDARRAY_H

#pragma once

#include <iostream>
#include <stdexcept>
#include <vector>
#include <assert.h>
#include <memory>

using namespace std;

template <typename T>
class SharedPointer{
private:
	// size_t start;
	size_t* count;
	T* ptr;   // points into the managed data, default ptr=start, using operator+ might change that value
	T* start; // points to the start of the managed data
	
public:

	SharedPointer(): ptr(nullptr), start(nullptr), count(new size_t(0)){
	}

	SharedPointer(T* ptr_):
		ptr(ptr_),start(ptr_),count(new size_t(1)){
	}

	// init with raw pointer and non-zero refcount
	SharedPointer(T* ptr_, size_t count_):
		ptr(ptr_),start(ptr_),count(new size_t(count_ + 1)){
	}
	
	SharedPointer(const SharedPointer<T>& other):
		count(other.count), ptr(other.ptr), start(other.start){
		++*count;
	}
	
	SharedPointer operator=(SharedPointer<T> that){
	// SharedPointer operator=(const SharedPointer<T>& that){

		/* copy-and-swap: create a copy (that is passed by value)
		   swap values of this and that and take the former
		   content of this down with that
		   source: http://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
		*/
		swap(*this, that);
		return *this;
	}

	

	friend void swap(SharedPointer<T>& first, SharedPointer<T>& second) 
	{

		// enable ADL (not necessary in our case, but good practice)
		using std::swap;
		
		swap(first.count, second.count); 
		swap(first.ptr, second.ptr);
		swap(first.start, second.start);
		
	}
	
	
	~SharedPointer(){
		if (--(*count) == 0){
			delete start;
			start = nullptr;
			ptr = nullptr;
		}
	}


	T* get(){
		return ptr;
	}

	SharedPointer operator+(long value){
		SharedPointer<T> out(*this);
		out.ptr += value;
		return out;
	}

	size_t getCount(){
		return *count;
	}
	
};


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
	
	size_t *shape;
	size_t  ndim;
	SharedPointer<T> data;

	Ndarray(): shape(nullptr), ndim(0), data(nullptr) {}
	

	/* Create an empty Ndarray */
	Ndarray(std::vector<size_t> shape_)
	{
		ndim = shape_.size();		
		shape = new size_t[ndim];
		std::copy(shape_.begin(), shape_.end(), shape);		
		data = SharedPointer<T>(new T[size()]);
	}
  
	/* Wrap pointer to existing data */
	Ndarray(T* data_, std::vector<size_t> shape_){
		ndim = shape_.size();
		shape = new size_t[ndim];
		std::copy(shape_.begin(), shape_.end(), shape);
		data = SharedPointer<T>(data_);
	}

	/* Wrap pointer to existing data, with given refcount */
	Ndarray(T* data_, std::vector<size_t> shape_, size_t refcount_) {
		ndim = shape_.size();
		shape = new size_t[ndim];
		std::copy(shape_.begin(), shape_.end(), shape);
		data = SharedPointer<T>(data_,refcount_);
	}

	Ndarray(SharedPointer<T> data_, size_t* shape_, size_t ndim_) :
		data(data_), shape(shape_), ndim(ndim_){
	}


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
		return SliceProxy(Ndarray<T>(data+start,shape+1,ndim-1));
	}

	const SliceProxy operator[](int idx) const{
		return operator[](idx);
	}

};


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



