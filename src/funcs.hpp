#ifndef FUNCS_H
#define FUNCS_H

using namespace std;

template<typename T, size_t N> class Ndarray;

template<
	typename T,
	typename = typename enable_if<is_arithmetic<T>::value, T>::type
	>
vector<T> calcStrides(const vector<T> arg){
	/* This name is misleading, as the function returns standard strides for a given shape */
	auto out = vector<T>(arg.size(), 1);
	for (int64_t i=arg.size()-2; i>=0 ; --i){
		out[i] = arg[i+1] * out[i+1]; 
	}
	return out;
}

template<
	typename T, size_t N,
	typename = typename enable_if<is_arithmetic<T>::value, T>::type
	>
array<T, N> calcStrides(const array<T, N> arg){
	/* This name is misleading, as the function returns standard strides for a given shape */
	array<T, N> out;
	out.back() = 1;
	for (int64_t i=arg.size()-2; i>=0 ; --i){
		out[i] = arg[i+1] * out[i+1]; 
	}
	return out;
}

template<typename T, size_t N>
Ndarray<T, N> ndarray(array<size_t, N> shape_, function<void(T*)> destructor_=_deleteArray<T>){
	auto size = product(shape_);
	return Ndarray<T,N>(size,
						0,
						shared_ptr<T>(new T[size], destructor_),
						shape_,
						calcStrides(shape_)
						);
}

template<typename T, size_t N>
Ndarray<T, N> ndarray(T* data_, array<size_t, N> shape_, function<void(T*)> destructor_=_deleteNothing<T>){
	return Ndarray<T,N>(product(shape_),
						0,
						shared_ptr<T>(data_, destructor_),
						shape_,
						calcStrides(shape_)
						);	
}

template<typename T, size_t N>
Ndarray<T, N> ndarray(T* data_, array<size_t, N> shape_, array<size_t, N> strides_, function<void(T*)> destructor_=_deleteNothing<T>){
	return Ndarray<T,N>(product(shape_),
						0,
						shared_ptr<T>(data_, destructor_),
						shape_,
						strides_
						);	
}

#endif /* FUNCS_H */
