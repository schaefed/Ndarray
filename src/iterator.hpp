#ifndef ITERATOR_H
#define ITERATOR_H

#include <vector>
#include <valarray>

#include "indexer.hpp"

using namespace std;

template<typename T>
class DataIterator : public iterator<forward_iterator_tag,
									 T,
									 ptrdiff_t,
									 T*,
									 T&>{
private:
	T* ptr_;
	const vector<size_t> shape_;
	Indexer indexer_;
	
	T* getPtr() const{
		return ptr_;
	}

	const T* getConstPtr() const{
		return ptr_;
	}


public:

	DataIterator(T* ptr, Indexer indexer):
		ptr_(ptr), indexer_(indexer){
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
		ptr_ += indexer_.nextIndex();
		return (*this);
	}
	// DataIterator<T> operator++(ptrdiff_t){
	// 	auto temp(*this);
	// 	ptr_ += nextIndex();
	// 	return temp;
	// }
	DataIterator<T> operator++(int){
		auto temp(*this);
		ptr_ += indexer_.nextIndex();
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

#endif /* ITERATOR_H */
