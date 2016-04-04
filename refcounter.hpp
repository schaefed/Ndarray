#ifndef REFCOUNTER_H
#define REFCOUNTER_H

// #pragma once

#include <iostream>
#include <vector>
#include <functional>

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
		ptr(ptr_), start(ptr_), count(new size_t(1)){
	}

	SharedPointer(T* ptr_, std::function<void(T*)> destructor):
		ptr(ptr_), start(ptr_), count(new size_t(1)){
	}

	
	// init with raw pointer and non-zero refcount
	SharedPointer(T* ptr_, size_t count_):
		ptr(ptr_),start(ptr_),count(new size_t(count_+1)){
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

#endif /* REFCOUNTER_H */
