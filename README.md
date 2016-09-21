
This template library provides a numpy ndarray inspired multi dimensional array class for C++.
The library tries to bridge the gap between the safety that comes from an an explicitly templated dimensionality
and the flexibility of being able to change the view on the underlying data. 

# Design

A Ndarray objects effectively wraps a pointer to data, holds some meta data and allows to use that contiguous block
of data in an manner of a multidimensional array object. The wrapped data is shared by design, and manged using shared_ptr.
Subscripting and slicing return views into the original data. Discontinuous slicing is available,
following the same strided approach as numpy does.

If an existing pointer is wrapped with an Ndarray object, the memory will not be freed by default. There is however
the possibility to pass a destructor function, which will be called if the reference count reaches zero.
If no pointer is passed to the constructor, an array of the paramterized datatype will be used to store data and is
freed when no other Ndarray objects points into the data.

The Ndarray class is templated by the data type of the underlying data and the dimensionality of the array. The latter
parameter defaults to -1. This means, that the dimensionality is not fixed and can be changed through the interface.
Operations like flattening and arbitrary reshaping of objects are trivial operations in this state of an object. 
An implicit conversion function allows the easy transformation from one state (e.g. not fixed dimensionality) to the other
(i.e. fixed dimensionality).

# Status
This library is work in progress and only the most basic things are implemented, notably:
- Data sharing.
- Subsripting and slicing (including discontinuous) along an axis and/or elements. 
- Assignment to single Ndarray elements

# Future Work
The most urgent things to implemented are in this order:
- Basic arithmetic operators.

