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
	T* ptr_;
	const vector<size_t> stride_;
	const vector<size_t> shape_;
	// vector<size_t> index_;
	size_t index_;
	size_t ndim_;
	
	T* getPtr() const{
		return ptr_;
	}

	const T* getConstPtr() const{
		return ptr_;
	}

	vector<size_t> strideOffset(){

		// most likely not the most efficient way to handle this.
		// The loop should be replaced by Ndarray calculations one day...
		// cout << "entered strideOffset()\n";

		auto tmp = vector<size_t>(ndim_, 0);
		for (int64_t i=ndim_-1; i>=0; --i){
			tmp[i] = stride_[i] * (shape_[i]-1);
			if (i < ndim_-1){
				tmp[i] += tmp[i+1];
			}
		}

		auto out = stride_;
		for (int64_t i=ndim_-1; i>0; --i){
			out[i-1] = stride_[i-1] - tmp[i];
		}

		return out;
		
	}
	
	size_t nextIndex(){

		index_++;
		auto offsets = strideOffset();
		auto index = index_;
		auto i = ndim_-1;
		while ((i>0) and (index%shape_[i] == 0)){
			index /= shape_[i];
			i--;
		}
		return offsets[i];
	}

public:

	DataIterator(T* ptr, vector<size_t> shape, vector<size_t> stride):
		ptr_(ptr), shape_(shape), stride_(stride), index_(0){
		ndim_ = shape_.size();
		// index_ = vector<size_t>(ndim_, 0);
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

#endif /* ITERATOR_H */
