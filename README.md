LSHBOX-0.9
==========
### *A C++ Toolbox of Locality-Sensitive Hashing for Large Scale Image Retrieval, Also Support Python and MATLAB.*
-----------------------------------------------------------------------------------------------------------------

#### Change Log

* 2015.07.04

A new LSH method, K-means Based Double-Bit Quantization for Hashing (KDBQ), was added into LSHBOX-0.9 on July 4th, 2015. We implement KDBQ by C++ but also provide MATLAB interface. And the Python interface will be added into LSHBOX-0.9 later. Other files related to KDBQ have been updated synchronously.

* 2015.06.04

A new LSH method, Double-Bit Quantization Hashing (DBQ), was added into LSHBOX-0.9 on June 4th, 2015. We implement DBQ by C++ but also provide MATLAB interface. And the Python interface will be added into LSHBOX-0.9 later. Other files related to DBQ have been updated synchronously.

### Chapter 1 - Introduction

Locality-Sensitive Hashing (LSH) is an efficient method for large scale image retrieval, and it achieves great performance in approximate nearest neighborhood searching.

LSHBOX is a simple but robust C++ toolbox that provides several LSH algrithms, in addition, it can be integrated into Python and MATLAB languages. The following LSH algrithms have been implemented in LSHBOX, they are:

* LSH Based on Random Bits Sampling
* Random Hyperplane Hashing
* LSH Based on Thresholding
* LSH Based on p-Stable Distributions
* [Spectral Hashing](http://www.cs.huji.ac.il/~yweiss/SpectralHashing/) (SH)
* [Iterative Quantization](http://www.unc.edu/~yunchao/itq.htm) (ITQ)
* Double-Bit Quantization Hashing (DBQ)
* K-means Based Double-Bit Quantization Hashing (KDBQ)

There are two repositories for compilation and performance tests, they are:
* [LSHBOX-3rdparty](https://github.com/RSIA-LIESMARS-WHU/LSHBOX-3rdparty): 3rdparty of LSHBOX, it is for compilation
* [LSHBOX-sample datasets](https://github.com/RSIA-LIESMARS-WHU/LSHBOX-sample-data): a dataset for performance tests

In addition, [File-Based-ITQ](https://github.com/RSIA-LIESMARS-WHU/File-Based-ITQ) is an File Based ITQ example for LSHBOX.

Part of the code depends on the C++11, So I think your compiler should support this feature. We tested LSHBOX with VS2010 in Windows 7/8 32bit/64bit and with g++ in Linux, Mac test will be done in the future. We hope that there are more people that join in the test or contribute more algrithms.

##### Please feel free to contact us [tanggefu@gmail.com, 20288ly@sina.cn or xiaozf@gmail.com] if you have any questions.

### Chapter 2 - Compilation

LSHBOX is written by C++. And it also can be easily used in many contexts through the Python and MATLAB bindings provided with this toolbox.

LSHBOX is simple and easy to use. If you want to integrate LSHBOX into you application, it don't need compile. You only need to add the include directory or modify the program search path, then you can use this library directly in C, C++, Python or MATLAB.

If you want to test or contribute, [CMAKE](http://www.cmake.org), a cross-platform, open-source build system, is usded to build some tools for the purpose. CMake can be downloaded from [CMake' website](http://www.cmake.org/cmake/resources/software.html).

In some cases, if you want or need to compile it by yourself with Python and MATLAB, please delete the comment of the last two lines in file `CMakeLists.txt`, and you will find the compiling progress of python must rely on Boost library or some part of this library. For more detailed information, you can view the document [`./python/README.md`](https://github.com/RSIA-LIESMARS-WHU/LSHBOX/blob/master/python/README.md).

During compilation, create a new directory named `build` in the main directory, then choose a appropriate compiler and switch to the `build` directory, finally, execute the following command according to your machine:

* Windows

```cpp
cmake -DCMAKE_BUILD_TYPE=Release .. -G"NMake Makefiles"
nmake
```

* Linux & OS X

```cpp
cmake ..
make
```

### Chapter 3 - Usage

This chapter contains small examples of how to use the LSHBOX library from different programming languages (C++, Python and MATLAB).

#### For C++

```cpp
/**
 * @file itqlsh_test.cpp
 *
 * @brief Example of using Iterative Quantization LSH index for L2 distance.
 */
#include <lshbox.h>
int main(int argc, char const *argv[])
{
    if (argc != 4)
    {
        std::cerr << "Usage: ./itqlsh_test data_file lsh_file benchmark_file" << std::endl;
        return -1;
    }
    std::cout << "Example of using Iterative Quantization" << std::endl << std::endl;
    typedef float DATATYPE;
    std::cout << "LOADING DATA ..." << std::endl;
    lshbox::timer timer;
    lshbox::Matrix<DATATYPE> data(argv[1]);
    std::cout << "LOAD TIME: " << timer.elapsed() << "s." << std::endl;
    std::cout << "CONSTRUCTING INDEX ..." << std::endl;
    timer.restart();
    std::string file(argv[2]);
    bool use_index = false;
    lshbox::itqLsh<DATATYPE> mylsh;
    if (use_index)
    {
        mylsh.load(file);
    }
    else
    {
        lshbox::itqLsh<DATATYPE>::Parameter param;
        param.M = 521;
        param.L = 5;
        param.D = data.getDim();
        param.N = 8;
        param.S = 100;
        param.I = 50;
        mylsh.reset(param);
        mylsh.train(data);
        mylsh.hash(data);
    }
    mylsh.save(file);
    std::cout << "CONSTRUCTING TIME: " << timer.elapsed() << "s." << std::endl;
    std::cout << "LOADING BENCHMARK ..." << std::endl;
    timer.restart();
    lshbox::Matrix<DATATYPE>::Accessor accessor(data);
    lshbox::Metric<DATATYPE> metric(data.getDim(), L1_DIST);
    lshbox::Benchmark bench;
    std::string benchmark(argv[3]);
    bench.load(benchmark);
    unsigned K = bench.getK();
    lshbox::Scanner<lshbox::Matrix<DATATYPE>::Accessor> scanner(
        accessor,
        metric,
        K
    );
    std::cout << "LOADING TIME: " << timer.elapsed() << "s." << std::endl;
    std::cout << "RUNING QUERY ..." << std::endl;
    timer.restart();
    lshbox::Stat cost, recall;
    lshbox::progress_display pd(bench.getQ());
    for (unsigned i = 0; i != bench.getQ(); ++i)
    {
        mylsh.query(data[bench.getQuery(i)], scanner);
        recall << bench.getAnswer(i).recall(scanner.topk());
        cost << float(scanner.cnt()) / float(data.getSize());
        ++pd;
    }
    std::cout << "MEAN QUERY TIME: " << timer.elapsed() / bench.getQ() << "s." << std::endl;
    std::cout << "RECALL   : " << recall.getAvg() << " +/- " << recall.getStd() << std::endl;
    std::cout << "COST     : " << cost.getAvg() << " +/- " << cost.getStd() << std::endl;

    // mylsh.query(data[0], scanner);
    // std::vector<std::pair<float, unsigned> > res = scanner.topk().getTopk();
    // for (std::vector<std::pair<float, unsigned> >::iterator it = res.begin(); it != res.end(); ++it)
    // {
    //     std::cout << it->second << ": " << it->first << std::endl;
    // }
    // std::cout << "DISTANCE COMPARISON TIMES: " << scanner.cnt() << std::endl;
}
```
You can get the sample dataset `audio.data` from [http://www.cs.princeton.edu/cass/audio.tar.gz](http://www.cs.princeton.edu/cass/audio.tar.gz), if the link is invalid, you can also get it from [LSHBOX-sample-data](https://github.com/RSIA-LIESMARS-WHU/LSHBOX-sample-data).

FOR EXAMPLE, YOU CAN RUN THE FOLLOWING CODE IN COMMAND LINE AFTER BUILD ALL THE TOOLS:

```
> create_benchmark audio.data audio.ben 200 50
> itqlsh_test audio.data audio.itq audio.ben
```

##### NOTE1:

In our project, the format of the input file (such as `audio.data`, which is in `float` data type) is a binary file but not a text file, because binary file has many advantages. In `LSHBOX/tools/create_test_data.cpp`, we create a binary file with `unsigned` data type, from the process, you will find that the binary file is organized as the following format:

>{Bytes of the data type} {The size of the vectors} {The dimension of the vectors} {All of the binary vector, arranged in turn}

For your application, you should also transform your dataset into this binary format.

##### NOTE2:

In addition, the dataset should be zero-centered, IT IS VERY IMPORTANT!

#### For Python

```python
#!/usr/bin/env python
# -*- coding: utf-8 -*-
# pylshbox_example.py
import pylshbox
import numpy as np
print 'prepare test data'
float_mat = np.random.randn(100000, 192)
float_query = float_mat[0]
unsigned_mat = np.uint32(float_mat * 5)
unsigned_query = unsigned_mat[0]
print ''
print 'Test rbsLsh'
rbs_mat = pylshbox.rbslsh()
rbs_mat.init_mat(unsigned_mat.tolist(), '', 521, 5, 20, 5)
result = rbs_mat.query(unsigned_query.tolist(), 1)
indices, dists = result[0], result[1]
for i in range(len(indices)):
    print indices[i], '\t', dists[i]
print ''
print 'Test rhpLsh'
rhp_mat = pylshbox.rhplsh()
rhp_mat.init_mat(float_mat.tolist(), '', 521, 5, 6)
result = rhp_mat.query(float_query.tolist(), 2, 10)
indices, dists = result[0], result[1]
for i in range(len(indices)):
    print indices[i], '\t', dists[i]
print ''
print 'Test thLsh'
th_mat = pylshbox.thlsh()
th_mat.init_mat(float_mat.tolist(), '', 521, 5, 12)
result = th_mat.query(float_query.tolist(), 2, 10)
indices, dists = result[0], result[1]
for i in range(len(indices)):
    print indices[i], '\t', dists[i]
print ''
print 'Test psdlsh with param.T = 1'
psdL1_mat = pylshbox.psdlsh()
psdL1_mat.init_mat(float_mat.tolist(), '', 521, 5, 1, 5)
result = psdL1_mat.query(float_query.tolist(), 2, 10)
indices, dists = result[0], result[1]
for i in range(len(indices)):
    print indices[i], '\t', dists[i]
print ''
print 'Test psdlsh with param.T = 2'
psdL2_mat = pylshbox.psdlsh()
psdL2_mat.init_mat(float_mat.tolist(), '', 521, 5, 2, 0.5)
result = psdL2_mat.query(float_query.tolist(), 2, 10)
indices, dists = result[0], result[1]
for i in range(len(indices)):
    print indices[i], '\t', dists[i]
print ''
print 'Test shLsh'
sh_mat = pylshbox.shlsh()
sh_mat.init_mat(float_mat.tolist(), '', 521, 5, 4, 100)
result = sh_mat.query(float_query.tolist(), 2, 10)
indices, dists = result[0], result[1]
for i in range(len(indices)):
    print indices[i], '\t', dists[i]
print ''
print 'Test itqLsh'
itq_mat = pylshbox.itqlsh()
itq_mat.init_mat(float_mat.tolist(), '', 521, 5, 8, 100, 50)
result = itq_mat.query(float_query.tolist(), 2, 10)
indices, dists = result[0], result[1]
for i in range(len(indices)):
    print indices[i], '\t', dists[i]
```

##### <font color="#4279F6">NOTE:

In Windows, the py module name is pylshbox, but in linux, it will be libpylshbox.</font>

#### For MATLAB

```matlab
% matlab_example.m
disp('prepare test data ...')
dataset = randn(128,100000);
dataset = dataset - repmat(mean(dataset), size(dataset, 1), 1);
testset = dataset(:,1:10);
disp('ok')
input('Test rhplsh, Press any key to continue ...')
param_rhp.M = 521;
param_rhp.L = 5;
param_rhp.N = 6;
[indices, dists] = rhplsh(dataset, testset, param_rhp, '', 2, 10)
input('Test thlsh, Press any key to continue ...')
param_th.M = 521;
param_th.L = 5;
param_th.N = 12;
[indices, dists] = thlsh(dataset, testset, param_th, '', 2, 10)
input('Test psdlsh with param_psdL1.T = 1, Press any key to continue ...')
param_psdL1.M = 521;
param_psdL1.L = 5;
param_psdL1.T = 1;
param_psdL1.W = 5;
[indices, dists] = psdlsh(dataset, testset, param_psdL1, '', 1, 10)
input('Test psdlsh with param_psdL2.T = 2, Press any key to continue ...')
param_psdL2.M = 521;
param_psdL2.L = 5;
param_psdL2.T = 2;
param_psdL2.W = 0.5;
[indices, dists] = psdlsh(dataset, testset, param_psdL2, '', 2, 10)
input('Test shlsh, Press any key to continue ...')
param_sh.M = 521;
param_sh.L = 5;
param_sh.N = 4;
param_sh.S = 100;
[indices, dists] = shlsh(dataset, testset, param_sh, '', 2, 10)
disp('Test itqlsh, Press any key to continue.')
param_itq.M = 521;
param_itq.L = 5;
param_itq.N = 8;
param_itq.S = 100;
param_itq.I = 50;
[indices, dists] = itqlsh(dataset, testset, param_itq, '', 2, 10)
disp('Test dbqlsh, Press any key to continue.')
param_dbq.M = 521;
param_dbq.L = 5;
param_dbq.N = 4;
param_dbq.I = 5;
[indices, dists] = dbqlsh(dataset, testset, param_dbq, '', 2, 10)
disp('Test kdbqlsh, Press any key to continue.')
param_kdbq.M = 521;
param_kdbq.L = 5;
param_kdbq.N = 4;
param_kdbq.I = 50;
[indices, dists] = kdbqlsh(dataset, testset, param_kdbq, '', 2, 10)
```

Have you ever find the empty string used in the Python and MATLAB code? In fact, they can be used to save the index through pass a file name. Like the following, you will find the next query speed faster than the first, because there is no re-indexing.

#### In Python

```python
#!/usr/bin/env python
# -*- coding: utf-8 -*-
# pylshbox_example2.py
import pylshbox
import numpy as np
import time
print 'prepare test data'
float_mat = np.random.randn(100000, 192)
float_query = float_mat[0]
print ''
print 'Test itqLsh'
print ''
print 'First time, need to constructing index.'  # About 7s.
start = time.time()
itq_mat = pylshbox.itqlsh()
itq_mat.init_mat(float_mat.tolist(), 'pyitq.lsh', 521, 5, 8, 100, 50)
result = itq_mat.query(float_query.tolist(), 2, 10)
indices, dists = result[0], result[1]
for i in range(len(indices)):
    print indices[i], '\t', dists[i]
print 'Elapsed time is %f seconds.' % (time.time() - start)
print ''
print 'Second time, no need to re-indexing.'  # About 3s.
start = time.time()
itq_mat2 = pylshbox.itqlsh()
itq_mat2.init_mat(float_mat.tolist(), 'pyitq.lsh')
result = itq_mat2.query(float_query.tolist(), 2, 10)
indices, dists = result[0], result[1]
for i in range(len(indices)):
    print indices[i], '\t', dists[i]
print 'Elapsed time is %f seconds.' % (time.time() - start)
```

#### In MATLAB

```matlab
% matlab_example2.m
dataset = randn(128,500000);
testset = dataset(:,1:10);
disp('Test itqlsh')
param_itq.M = 521;
param_itq.L = 5;
param_itq.N = 8;
param_itq.S = 100;
param_itq.I = 50;
disp('First time, need to constructing index') % About 13s.
tic;
[indices, dists] = itqlsh(dataset, testset, param_itq, 'itq.lsh', 2, 10);
toc;
disp('Second time, no need to re-indexing') % About 0.5s.
tic;
[indices, dists] = itqlsh(dataset, testset, param_itq, 'itq.lsh', 2, 10);
toc;
```

### Chapter 4 - Algorithm

LSHBOX is based on many approximate nearest neighbor schemes, and the following is a brief description of each algorithm and its parameters.


#### 4.1 - Locality-Sensitive Hashing Scheme Based on Random Bits Sampling

##### Reference

```
P. Indyk and R. Motwani. Approximate Nearest Neighbor - Towards Removing the Curse of Dimensionality. In Proceedings of the 30th Symposium on Theory of Computing, 1998, pp. 604-613.

A. Gionis, P. Indyk, and R. Motwani. Similarity search in high dimensions via hashing. Proceedings of the 25th International Conference on Very Large Data Bases (VLDB), 1999.
```

##### Parameters

```cpp
struct Parameter
{
	/// Hash table size
	unsigned M;
	/// Number of hash tables
	unsigned L;
	/// Dimension of the vector
	unsigned D;
	/// Binary code bytes
	unsigned N;
	/// The Difference between upper and lower bound of each dimension
	unsigned C;
};
```

##### Implementation

```cpp
#include <lshbox/lsh/rbslsh.h>
```

According to the second assumption in the paper, all coordinates of points in P are positive integer. Although we can convert all coordinates to integers by multiplying them by a suitably large number and rounding to the nearest integer, but I think it is very fussy, What's more, it often gets criticized for using too much memory when in a larger range of data. Therefore, it is recommended to use other algorithm.


#### 4.2 - Locality-Sensitive Hashing Scheme Based on Random Hyperplane

##### Reference

```
Charikar, M. S. 2002. Similarity estimation techniques from rounding algorithms. In Proceedings of the Thiry-Fourth Annual ACM Symposium on theory of Computing (Montreal, Quebec, Canada, May 19 - 21, 2002). STOC '02. ACM, New York, NY, 380-388. DOI= http://doi.acm.org/10.1145/509907.509965
```

##### Parameters

```cpp
struct Parameter
{
	/// Hash table size
	unsigned M;
	/// Number of hash tables
	unsigned L;
	/// Dimension of the vector
	unsigned D;
	/// Binary code bytes
	unsigned N;
};
```

##### Implementation

```cpp
#include <lshbox/lsh/rhplsh.h>
```

#### 4.3 - Locality-Sensitive Hashing Scheme Based on Thresholding

##### Reference

```
Zhe Wang, Wei Dong, William Josephson, Qin Lv, Moses Charikar, Kai Li. Sizing Sketches: A Rank-Based Analysis for Similarity Search. In Proceedings of the 2007 ACM SIGMETRICS International Conference on Measurement and Modeling of Computer Systems . San Diego, CA, USA. June 2007.

Qin Lv, Moses Charikar, Kai Li. Image Similarity Search with Compact Data Structures. In Proceedings of ACM 13th Conference on Information and Knowledge Management (CIKM), Washington D.C., USA. November 2004.
```

##### Parameters

```cpp
struct Parameter
{
	/// Hash table size
	unsigned M;
	/// Number of hash tables
	unsigned L;
	/// Dimension of the vector
	unsigned D;
	/// Binary code bytes
	unsigned N;
	/// Upper bound of each dimension
	float Max;
	/// Lower bound of each dimension
	float Min;
};
```

##### Implementation

```cpp
#include <lshbox/lsh/thlsh.h>
```

#### 4.4 - Locality-Sensitive Hashing Scheme Based on p-Stable Distributions

##### Reference

```
Mayur Datar , Nicole Immorlica , Piotr Indyk , Vahab S. Mirrokni, Locality-sensitive hashing scheme based on p-stable distributions, Proceedings of the twentieth annual symposium on Computational geometry, June 08-11, 2004, Brooklyn, New York, USA.
```

##### Parameters

```cpp
struct Parameter
{
	/// Hash table size
	unsigned M;
	/// Number of hash tables
	unsigned L;
	/// Dimension of the vector
	unsigned D;
	/// Index mode, you can choose 1(CAUCHY) or 2(GAUSSIAN)
	unsigned T;
	/// Window size
	float W;
};
```

##### Implementation

```cpp
#include <lshbox/lsh/psdlsh.h>
```

#### 4.5 - Spectral Hashing

##### Reference

```
Y. Weiss, A. Torralba, R. Fergus. Spectral Hashing. Advances in Neural Information Processing Systems, 2008.
```

##### Parameters

```cpp
struct Parameter
{
	/// Hash table size
	unsigned M;
	/// Number of hash tables
	unsigned L;
	/// Dimension of the vector
	unsigned D;
	/// Binary code bytes
	unsigned N;
	/// Size of vectors in train
	unsigned S;
};
```

##### Implementation

```cpp
#include <lshbox/lsh/shlsh.h>
```

#### 4.6 - Iterative Quantization

##### Reference

```
Gong Y, Lazebnik S, Gordo A, et al. Iterative quantization: A procrustean approach to learning binary codes for large-scale image retrieval[J]. Pattern Analysis and Machine Intelligence, IEEE Transactions on, 2013, 35(12): 2916-2929.
```

##### Parameters

```cpp
struct Parameter
{
	/// Hash table size
	unsigned M;
	/// Number of hash tables
	unsigned L;
	/// Dimension of the vector
	unsigned D;
	/// Binary code bytes
	unsigned N;
	/// Size of vectors in train
	unsigned S;
	/// Training iterations
	unsigned I;
};
```

##### Implementation

```cpp
#include <lshbox/lsh/itqlsh.h>
```

#### 4.7 - Double-Bit Quantization Hashing

##### Reference

```
Kong W, Li W. Double-Bit Quantization for Hashing. In AAAI, 2012.

Gong Y, Lazebnik S, Gordo A, et al. Iterative quantization: A procrustean approach to learning binary codes for large-scale image retrieval[J]. Pattern Analysis and Machine Intelligence, IEEE Transactions on, 2013, 35(12): 2916-2929.
```

##### Parameters

```cpp
struct Parameter
{
	/// Hash table size
	unsigned M;
	/// Number of hash tables
	unsigned L;
	/// Dimension of the vector
	unsigned D;
	/// Number of projection dimensions,corresponding to 2*N binary code bytes for each vector
	unsigned N;
	/// Training iterations
	unsigned I;
};
```

##### Implementation

```cpp
#include <lshbox/lsh/dbqlsh.h>
```

#### 4.8 - K-means Based Double-Bit Quantization Hashing

##### Reference

```
Zhu, H. K-means based double-bit quantization for hashing.Computational Intelligence for Multimedia, Signal and Vision Processing (CIMSIVP),2014 IEEE Symposium on (pp.1-5). IEEE.
```

##### Parameters

```cpp
struct Parameter
{
	/// Hash table size
	unsigned M;
	/// Number of hash tables
	unsigned L;
	/// Dimension of the vector
	unsigned D;
	/// Number of projection dimensions,corresponding to 2*N binary code bytes for each vector
	unsigned N;
	/// Training iterations
	unsigned I;
};
```

##### Implementation

```cpp
#include <lshbox/lsh/kdbqlsh.h>
```

According to the test, Double-Bit Quantization Hashing and Iterative Quantization performance very good and are superior to other schemes. Iterative Quantization can get high query accuracy with minimum cost while Double-Bit Quantization Hashing can achieve better query accuracy.
