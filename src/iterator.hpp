#ifndef ITERATOR_H
#define ITERATOR_H

#include <vector>
#include <valarray>

using namespace std;

template<typename T>
class DataIterator : public iterator<forward_iterator_tag,
									 T,
									 ptrdiff_t,
									 T*,
									 T&>{
private:
	T* ptr;
	size_t index;
	const vector<size_t> shape;
	const vector<size_t> strides;
	
	const T* getConstPtr() const{
		return ptr;
	}

	size_t nextIndex(){

		index++;
		auto idx = index;
		auto i = shape.size()-1;
		auto accum = 0;
		while ((i>0) and (idx%shape[i] == 0)){
			idx /= shape[i];
			accum += ((shape[i]-1) * strides[i]);
			i--;
		}
		return strides[i] - accum;
	}


public:

	DataIterator(T* ptr_, vector<size_t> shape_, vector<size_t> strides_):
		index(0), ptr(ptr_), shape(shape_), strides(strides_){
	}
	
	DataIterator(const DataIterator<T>& iter) = default; // Maybe I should implement the constructor...

	~DataIterator(){}


	// void swap(DataIterator<T>& other) noexcept
	// {
	// 	swap(ptr_, other.ptr_);
	// 	swap(shape_, other.shape_);
	// 	swap(indexer_, other.indexer_);
	// }

	DataIterator<T>& operator=(const DataIterator<T>& iter) = default;

	bool operator==(const DataIterator<T>& iter) const{
		return (ptr == iter.getConstPtr());
	}
	bool operator!=(const DataIterator<T>& iter) const{
		return (ptr != iter.getConstPtr());
	}
	DataIterator<T>& operator++(){
		ptr += nextIndex();
		return (*this);
	}

	DataIterator<T> operator++(int){
		auto temp(*this);
		ptr += nextIndex();
		return temp;
	}
	T& operator*(){
		return *ptr;
	}

	const T& operator*() const{
		return *ptr;
	}

	T* operator->(){
		// Is a const version needed??
		return ptr;
	}

	const T* operator->() const{
		// Is a const version needed??
		return ptr;
	}

};

#endif /* ITERATOR_H */
