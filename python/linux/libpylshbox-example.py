#!/usr/bin/env python
# -*- coding: utf-8 -*-
# libpylshbox_example.py
import libpylshbox
import numpy as np
print 'prepare test data'
float_mat = np.random.randn(100000, 192)
float_query = float_mat[0]
unsigned_mat = np.uint32(float_mat * 5)
unsigned_query = unsigned_mat[0]
print ''
print 'Test rbsLsh'
rbs_mat = libpylshbox.rbslsh()
rbs_mat.init_mat(unsigned_mat.tolist(), '', 521, 5, 20, 5)
result = rbs_mat.query(unsigned_query.tolist(), 1)
indices, dists = result[0], result[1]
for i in range(len(indices)):
    print indices[i], '\t', dists[i]
print ''
print 'Test rhpLsh'
rhp_mat = libpylshbox.rhplsh()
rhp_mat.init_mat(float_mat.tolist(), '', 521, 5, 6)
result = rhp_mat.query(float_query.tolist(), 2, 10)
indices, dists = result[0], result[1]
for i in range(len(indices)):
    print indices[i], '\t', dists[i]
print ''
print 'Test thLsh'
th_mat = libpylshbox.thlsh()
th_mat.init_mat(float_mat.tolist(), '', 521, 5, 12)
result = th_mat.query(float_query.tolist(), 2, 10)
indices, dists = result[0], result[1]
for i in range(len(indices)):
    print indices[i], '\t', dists[i]
print ''
print 'Test psdlsh with param.T = 1'
psdL1_mat = libpylshbox.psdlsh()
psdL1_mat.init_mat(float_mat.tolist(), '', 521, 5, 1, 5)
result = psdL1_mat.query(float_query.tolist(), 2, 10)
indices, dists = result[0], result[1]
for i in range(len(indices)):
    print indices[i], '\t', dists[i]
print ''
print 'Test psdlsh with param.T = 2'
psdL2_mat = libpylshbox.psdlsh()
psdL2_mat.init_mat(float_mat.tolist(), '', 521, 5, 2, 0.5)
result = psdL2_mat.query(float_query.tolist(), 2, 10)
indices, dists = result[0], result[1]
for i in range(len(indices)):
    print indices[i], '\t', dists[i]
print ''
print 'Test shLsh'
sh_mat = libpylshbox.shlsh()
sh_mat.init_mat(float_mat.tolist(), '', 521, 5, 4, 100)
result = sh_mat.query(float_query.tolist(), 2, 10)
indices, dists = result[0], result[1]
for i in range(len(indices)):
    print indices[i], '\t', dists[i]
print ''
print 'Test itqLsh'
itq_mat = libpylshbox.itqlsh()
itq_mat.init_mat(float_mat.tolist(), '', 521, 5, 8, 100, 50)
result = itq_mat.query(float_query.tolist(), 2, 10)
indices, dists = result[0], result[1]
for i in range(len(indices)):
    print indices[i], '\t', dists[i]
