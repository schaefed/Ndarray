
class OutOfBoundsException : public std::exception {
public:
  OutOfBoundsException(){};
  ~OutOfBoundsException() throw() {};
  const char* what() const throw(){
    return "Index out of bounds!";
  };
};


// template<typename T> 
// Ndarray<T>::~Ndarray(){
//   if (own_data){
//     cout << "freeing\n";
//     free(data);
//   }
// }

template<typename T, const int N> 
NdarrayBase<T, N>::NdarrayBase(const std::vector<size_t> shape_){
  shape = shape_;
  size = 1;
  for (auto d: shape){
    size = size * d;
  }
  data = (T*) (malloc(size * sizeof(T)));
  own_data = true;
}

template<typename T, const int N> 
NdarrayBase<T, N>::NdarrayBase(T* data_, const std::vector<size_t> shape_, bool own_data_){
  shape = shape_;
  size = 1;
  for (auto d: shape){
    size = size * d;
  }  
  data = data_;
  own_data = own_data_;
}

// template<typename T, const int N> 
// const T& Ndarray<T, N>::operator[](std::vector<size_t> idxv) const{

//   int i;
//   size_t idx = 0;
    
//   for (i=0 ; i < this->shape.size() ; i++){
//     if (idxv[i] < this->shape[i]){
//       if (i < this->shape.size() - 1){
// 	idx += idxv[i]*this->shape[i];
//       }
//     } else {
//       throw OutOfBoundsException();
//     }
//   }
//   if (i < idxv.size()){
//     throw OutOfBoundsException();
//   }
//   idx += idxv[i];
//   return this->data[idx];
// }


// template<typename T, const int N> 
// T& Ndarray<T, N>::operator[](std::vector<size_t> idxv){

//   int i;
//   size_t idx = 0;
    
//   for (i=0 ; i < this->shape.size() ; i++){
//     if (idxv[i] < this->shape[i]){
//       if (i < this->shape.size() - 1){
// 	idx += idxv[i]*this->shape[i];
//       }
//     } else {
//       throw OutOfBoundsException();
//     }
//   }
//   if (i < idxv.size()){
//     throw OutOfBoundsException();
//   }
//   idx += idxv[i];
//   return this->data[idx];
// }




template<typename T, const int N> 
const Ndarray<T, N-1> Ndarray<T, N>::operator[](size_t idx) const{
  if (idx < 0 || idx >= this->shape[0]){
    throw OutOfBoundsException();
  }
  std::vector<size_t> new_shape(this->shape.begin()+1, this->shape.end());
  size_t start = idx * new_shape[0];
  return Ndarray<T, N-1>(this->data+start,new_shape);
}

template<typename T, const int N> 
Ndarray<T, N-1> Ndarray<T, N>::operator[](size_t idx) {
  if (idx < 0 || idx >= this->shape[0]){
    throw OutOfBoundsException();
  }
  std::vector<size_t> new_shape(this->shape.begin()+1, this->shape.end());
  size_t start = idx * new_shape[0];
  return Ndarray<T, N-1>(this->data+start,new_shape);
}


template<typename T> 
T& Ndarray<T, 1>::operator[](size_t idx){
  if (idx < 0 || idx >= this->shape[0]){
    throw OutOfBoundsException();
  }
  return this->data[idx];
}


template<typename T> 
const T& Ndarray<T, 1>::operator[](size_t idx) const {
  if (idx < 0 || idx >= this->shape[0]){
    throw OutOfBoundsException();
  }
  return this->data[idx];
}


template<typename T, const int N> 
void NdarrayBase<T,N>::fill(T value){
//   size_t ndim = 3;
//   for (int i=0; i<ndim; i++){
//     const int d = N-i;
//     Ndarray<T,d> tmp = this[i];
//   }
}
