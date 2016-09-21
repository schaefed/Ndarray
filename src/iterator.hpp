#ifndef ITERATOR_H
#define ITERATOR_H

#include <vector>

using namespace std;

template<typename T>
class DataIterator : public iterator<forward_iterator_tag, T, ptrdiff_t, T*, T&>{

private:
	T* ptr;
	size_t index;
	const vector<size_t> &shape;
	const vector<size_t> &strides;
	
	size_t nextIndex(){
		// is there a less complex solution?
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

	DataIterator(T* ptr_, const vector<size_t> &shape_, const vector<size_t> &strides_):
		ptr(ptr_), index(0), shape(shape_), strides(strides_){
	}
	
	bool operator==(const DataIterator<T>& iter) const {
		return (ptr == iter.ptr);
	}

	bool operator!=(const DataIterator<T>& iter) const {
		return (ptr != iter.ptr);
	}

	DataIterator<T>& operator++(){
		ptr += nextIndex();
		return (*this);
	}

	DataIterator<T> operator++(const int){
		auto out(*this);
		ptr += nextIndex();
		return out;
	}

	T& operator*(){
		return *ptr;
	}

	T* operator->(){
		return ptr;
	}

	const T& operator*() const{
		return *ptr;
	}

	const T* operator->() const{
		return ptr;
	}
};

#endif /* ITERATOR_H */
