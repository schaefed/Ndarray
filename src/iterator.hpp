#ifndef ITERATOR_H
#define ITERATOR_H

#include <vector>
#include "utils.hpp"

using namespace std;

template<typename T, size_t N>
class DataIterator : public iterator<forward_iterator_tag, T, ptrdiff_t, T*, T&>{

private:

	T* buffer;
	T* pos;
	size_t iter;
	const array<size_t, N> &shape;
	const array<size_t, N> &strides;

	void increment(){
		size_t inc = 0;
		size_t tmp = ++iter;
		for (int64_t i=shape.size()-1; i>= 0; --i){
			auto n = tmp%(shape[i]);
			inc += (n*strides[i]);
			tmp /= shape[i];
		}
		if (tmp > 0){
			inc = shape[0] * filterZeros(strides)[0];
		}
		pos = buffer + inc;
	}


public:

	DataIterator(T* buffer_, const array<size_t, N> &shape_, const array<size_t, N> &strides_):
		buffer(buffer_), pos(&buffer_[0]), iter(0), shape(shape_), strides(strides_){
	}
	
	bool operator==(const DataIterator<T, N>& iter) const {
		return (pos == iter.pos);
	}

	bool operator!=(const DataIterator<T, N>& iter) const {
		return (pos != iter.pos);
	}

	DataIterator<T, N>& operator++(){
		increment();
		return (*this);
	}

	DataIterator<T, N> operator++(const int){
		auto out(*this);
		increment();
		return out;
	}

	T& operator*(){
		return *pos;
	}

	T* operator->(){
		return pos;
	}

	const T& operator*() const{
		return *pos;
	}

	const T* operator->() const{
		return pos;
	}
};

#endif /* ITERATOR_H */
