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

# def nextIndex(array, idx):
#     idx +=1 
#     shape = array.shape
#     i = array.ndim - 1
#     while (i>0) and (idx%shape[i] == 0):
#         idx /= shape[i]
#         i -= 1
    
    
# def indexDimension(array, idx):
#     offsets = elementOffset(array)
#     strides = arrayStrides(array)
#     shape = np.array(array.shape)
   
#     tmp = idx+1
#     i = len(shape)-1
#     acc = 0
#     while (i>0) and (tmp%shape[i] == 0):
#         tmp = tmp//shape[i]
#         acc += ((shape[i]-1) * strides[i])
#         i -= 1
#     return strides[i] - acc

def indexDimension(array, idx):
    strides = arrayStrides(array)
    shape = np.array(array.shape)
    
    idx = idx+1
    tmp = 0
    for i in xrange(array.ndim-1,-1,-1):
        if idx%shape[i] != 0:
            break
        tmp += ((shape[i]-1) * strides[i])
        idx /= shape[i]
    return strides[i] - tmp
    
    # tmp = idx+1
    # i = len(shape)-1
    # acc = 0
    # while (i>0) and (tmp%shape[i] == 0):
    #     tmp = tmp//shape[i]
    #     acc += ((shape[i]-1) * strides[i])
    #     i -= 1
    # return strides[i] - acc
   

if __name__== "__main__":

    shape = (16, 16)
    x = np.arange(np.prod(shape)).reshape(shape)
    print x[-5][slice(-12,-3,2)]
    # print arrayStrides(x[slice(0,4,2)])
    # print x[slice(0,4,2)]
    # print x
    # print nextIndex(x[::2,::2],1)
    # print elementOffset(x[::2,::2])
    # print elementOffsetImplement(x[::2, ::2,::2])
    # for i in xrange(10):
    #     y = indexDimension(x,i+1)
    #     print "results:", i+1, y
    # idx = 15
    # print x[::2]
    # test = indexDimension(x[::2], idx)
    # print x.flatten()[test+idx] 
