#ifndef UTILS_H
#define UTILS_H

#include <iostream>

using namespace std;

template<typename T>
void _deleteArray(T* pointer){
	delete[] pointer;
}

template<typename T>
void _deleteNothing(T* pointer){
}

template<typename T>
ostream& operator<< (ostream& stream, vector<T>shape ) {
	stream << '(';
	for (size_t i = 0; i < shape.size() ; i++){
		stream << shape[i];
		if (i < shape.size() - 1){
			stream << ", ";
		}
	}
	stream << ')';
	return stream;
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

#endif /* UTILS_H */
