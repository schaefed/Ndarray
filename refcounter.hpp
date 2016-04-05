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
	T* ptr;   // points into the managed data, default ptr=start, using operator+ might change that value
	T* start; // points to the start of the managed data
	size_t* count;
	std::function<void(T*)> destructor;
	
public:

	SharedPointer():
		ptr(nullptr),
		start(nullptr),
		count(new size_t(0))
		// destructor([](int64_t* pointer)->void{})
		{
			destructor = [](int64_t* pointer)->void{};
			
		}

	SharedPointer(T* ptr_):
		ptr(ptr_),
		start(ptr_),
		count(new size_t(1)){
		destructor = [](int64_t* pointer)->void{
			delete pointer;
		};
	}

	SharedPointer(T* ptr_, std::function<void(T*)> destructor_):
		ptr(ptr_), start(ptr_), count(new size_t(1)), destructor(destructor_){
	}


	SharedPointer(const SharedPointer<T>& other):
		ptr(other.ptr),
		start(other.start),
		count(other.count),
		destructor(other.destructor)
	{
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
		swap(first.destructor, second.destructor);
		
	}
	
	
	~SharedPointer(){
		if (--(*count) == 0){
			destructor(ptr);
			// delete start;
			// start = nullptr;
			// ptr = nullptr;
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
