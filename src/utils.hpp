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

#endif /* UTILS_H */
