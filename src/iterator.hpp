#ifndef ITERATOR_H
#define ITERATOR_H

#include <vector>

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
	vector<size_t> index_;
	size_t ndim_;
	
	T* getPtr() const{
		return ptr_;
	}

	const T* getConstPtr() const{
		return ptr_;
	}

	ptrdiff_t nextIndex(){
		// Not correct!
		size_t out = 0;
		// int64_t i;
		// int64_t idx = ndim_ - 1;
		// index_[idx] += 1;
		// for (int64_t i=ndim_-1; i>=0 ; --i){ 
		// // // 	// cout << "looping\n"; 
		// 	if (index_[i] == shape_[i]){
		// 	// if ((index_[i] == shape_[i]) and (i > 0)){
		// 	// 	out += (stride_[i-1] - stride_[i]*shape_[i]);
		// 	// 	index_[i] = 0;
		// 	// 	index_[i-1] += 1;
		// 	} else {
		// 		out += stride_[i];
		// 		break;
		// 	}
			
		// }
		return out;
		// // cout << "next: " << stride_[i] << endl;
		// return stride_[i];
	}

public:

	DataIterator(T* ptr, vector<size_t> shape, vector<size_t> stride):
		ptr_(ptr), shape_(shape), stride_(stride){
		ndim_ = shape_.size();
		index_ = vector<size_t>(ndim_, 0);
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
