#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <initializer_list>
#include <type_traits> // use for Ndarray!

using namespace std;

template<typename T>
void _deleteArray(T* pointer){
	delete[] pointer;
}

template<typename T>
void _deleteNothing(T* pointer){
}

template<typename T>
ostream& operator<< (ostream& stream, vector<T>arg ) {
	stream << '(';
	for (size_t i = 0; i < arg.size() ; i++){
		stream << arg[i];
		if (i < arg.size() - 1){
			stream << ", ";
		}
	}
	stream << ')';
	return stream;
}

template<typename T, size_t N>
ostream& operator<< (ostream& stream, array<T, N>arg ) {
	stream << '(';
	for (size_t i = 0; i < arg.size() ; i++){
		stream << arg[i];
		if (i < arg.size() - 1){
			stream << ", ";
		}
	}
	stream << ')';
	return stream;
}

vector<int64_t> range(int64_t stop){
	vector<int64_t> out(stop);
	int64_t i = 0;
	for (int64_t x=0 ; x < stop ; x++){
		out[i++] = x;
	}
	return out;
}

vector<int64_t> range(int64_t start, int64_t stop, int64_t step=1){

	if ((start > stop) and (step > 0)){
		start = stop;
	}

	int64_t i = 0;
	vector<int64_t> out((stop-start)/step);
	for (int64_t x=start ; x < stop ; x = x + step){
		out[i++] = x;
	}
	return out;
}


template<typename T, size_t N, size_t M>
array<T, N> subarray(array<T, M> arr, size_t offset=0){
	array<T, N> out;
	for (size_t i=0; i<N; i++){
		out[i] = arr[offset+i];
	}
	return out;
}

template<typename T>
vector<T> concat(const initializer_list<vector<T>> args){
	vector<T> out (vector<T>(0));
	for (auto a: args){
		out.insert(out.end(), a.begin(), a.end());
	}
	return out;
}

template<typename T, size_t N>
vector<T> concat(const initializer_list<array<T, N>> args){
	array<T, N> out (array<T, N>(0));
	for (auto a: args){
		out.insert(out.end(), a.begin(), a.end());
	}
	return out;
}

template<
	typename T,
	typename = typename enable_if<is_arithmetic<T>::value, T>::type
	>
T product(const vector<T> arg){
	T out = 1;
	for (auto x: arg){
		out *= x;
	}
	return out;
}
template<
	typename T, size_t N,
	typename = typename enable_if<is_arithmetic<T>::value, T>::type
	>
T product(const array<T, N> arg){
	T out = 1;
	for (auto x: arg){
		out *= x;
	}
	return out;
}


template<
	typename T,
	typename = typename enable_if<is_integral<T>::value, T>::type
	>
vector<T> filterZeros(const vector<T>& arg) {
	vector<T> out; // = vector<T>();
	for (auto e: arg){
		if (e != 0){
			out.push_back(e);
		}
	}
	return out;
}

template<
	typename T, size_t N,
	typename = typename enable_if<is_integral<T>::value, T>::type
	>
vector<T> filterZeros(const array<T, N>& arg) {
	vector<T> out; // = vector<T>();
	for (auto e: arg){
		if (e != 0){
			out.push_back(e);
		}
	}
	return out;
}

template<
	typename T, size_t N,
	typename = typename enable_if<is_arithmetic<T>::value, T>::type
	>
array<T, N> filledArray(const T value){
	array<T,N> out;
	out.fill(value);
	return out;
}


#endif /* UTILS_H */
