#!/usr/bin/env python
# -*- coding: utf-8 -*-
# pylshbox_example2.py
import pylshbox
import numpy
import time
print 'prepare test data'
float_file = 'audio.data'
float_query = numpy.random.rand(192)
print ''
print 'Test itqLsh'
print ''
print 'First time, need to constructing index.' #About 1.5s.
start = time.time()
itq_file = pylshbox.itqlsh()
itq_file.init_file(float_file, 'pyitq.lsh', 521, 5, 8, 100, 50)
result = itq_file.query(float_query.tolist(), 2, 10)
indices, dists = result[0],result[1]
for i in range(len(indices)):
	print indices[i], '\t', dists[i]
print 'Elapsed time is %f seconds.' % (time.time() - start)
print ''
print 'Second time, no need to re-indexing.' #About 0.05s.
start = time.time()
itq_file2 = pylshbox.itqlsh()
itq_file2.init_file(float_file, 'pyitq.lsh', 521, 5, 8, 100, 50)
result = itq_file2.query(float_query.tolist(), 2, 10)
indices, dists = result[0],result[1]
for i in range(len(indices)):
	print indices[i], '\t', dists[i]
print 'Elapsed time is %f seconds.' % (time.time() - start)
input()