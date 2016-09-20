#! /usr/bin/env python
# -*- coding: utf-8 -*-

import numpy as np
from copy import deepcopy

def arrayStrides(array):
    return np.array(array.strides)/array.dtype.itemsize

def elementOffset(array):
    shape = np.array(array.shape)
    strides = arrayStrides(array)

    # print "shape: ", shape
    # print "strides: ", strides

    out = deepcopy(strides) 
    for i in xrange(len(shape)-1,0,-1):
        out[i-1] = strides[i-1] - np.sum(strides[i:]*(shape[i:]-1)) 
    return out

def elementOffsetImplement(array):
    shape = np.array(array.shape)
    strides = arrayStrides(array)
    ndim = len(shape)

    tmp = [0] * ndim
    for i in xrange(ndim-1, -1, -1):
        tmp[i] = strides[i] * (shape[i]-1)
        if i < ndim-1:
            tmp[i] += tmp[i+1]

    out = deepcopy(strides)
    for i in xrange(ndim-1, 0, -1):
        out[i-1] = strides[i-1] - tmp[i]

    return out

def indexDimension(array, idx):
    offsets = elementOffset(array)
    shape = np.array(array.shape)
   
    tmp = idx+1
    i = len(shape)-1
    while (i>0) and (tmp%shape[i] == 0):
        tmp = tmp//shape[i]
        i -= 1
    return offsets[i]
   

if __name__== "__main__":

    shape = (4, 4)
    x = np.arange(np.prod(shape)).reshape(shape)
    print arrayStrides(x[slice(0,4,2)])
    print x[slice(0,4,2)]
    print x
    # print elementOffset(x[::2,::2])
    # print elementOffsetImplement(x[::2, ::2,::2])
    # for i in xrange(10):
    #     y = indexDimension(x,i+1)
    #     print "results:", i+1, y
    # print indexDimension(x[::2, ::2,::2], 3)
