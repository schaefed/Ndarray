
This template library provides a numpy ndarray inspired multi dimensional array class for C++.

# Design

A Ndarray objects effectively wraps a pointer to data, holds some meta data and allows to use that contiguous block
of data in an manner of a multidimensional array object. The wrapped data is shared by design, and manged using shared_ptr.
Subscripting and slicing return views into the original data. Discontinuous slicing is available,
following the same strided approach as numpy does.

If an existing pointer is wrapped with an Ndarray object, the memory will not be freed by default. There is however
the possibility to pass a destructor function, which will be called if the reference count reaches zero.
If no pointer is passed to the constructor, an array of the paramterized datatype will be used to store data and is
freed when no other Ndarray objects points into the data.

# Status

This library is work in progress, the following things are implemented:
- Data sharing
- Subsripting and slicing (including discontinuous) along an axis and/or elements 
- Assignment to Ndarrays and elements
- Broadcasting

# Future Work
The most urgent things to implemented are in this order:
- Basic arithmetic operators.
- A usefull cout function.
- Parallel map versions.
- Possibly parallelized math functions.
