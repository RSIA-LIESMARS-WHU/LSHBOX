//////////////////////////////////////////////////////////////////////////////
/// Copyright (C) 2014 Gefu Tang <tanggefu@gmail.com>. All Rights Reserved.
///
/// This file is part of LSHBOX.
///
/// LSHBOX is free software: you can redistribute it and/or modify it under
/// the terms of the GNU General Public License as published by the Free
/// Software Foundation, either version 3 of the License, or(at your option)
/// any later version.
///
/// LSHBOX is distributed in the hope that it will be useful, but WITHOUT
/// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
/// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
/// more details.
///
/// You should have received a copy of the GNU General Public License along
/// with LSHBOX. If not, see <http://www.gnu.org/licenses/>.
///
/// @version 0.1
/// @author Gefu Tang & Zhifeng Xiao
/// @date 2014.6.30
//////////////////////////////////////////////////////////////////////////////
#define BOOST_PYTHON_SOURCE
#include <lshbox.h>
#include <boost/python.hpp>
#include <boost/python/stl_iterator.hpp>
namespace lshbox
{
boost::python::list reshape(const boost::python::list &source)
{
    boost::python::list vec;
    for (unsigned i = 0; i != unsigned(boost::python::len(source)); ++i)
    {
        vec.extend(source[i]);
    }
    return vec;
}
class pyRbsLsh
{
public:
    typedef unsigned DATATYPE;
    void init_file(
        const std::string &source,
        const std::string &index,
        unsigned M = 521,
        unsigned L = 5,
        unsigned N = 20,
        unsigned C = 5)
    {
        data.load(source);
        std::ifstream is(index.c_str(), std::ios_base::binary);
        lshbox::timer timer;
        if (is)
        {
            std::cout << "LOADING INDEX ..." << std::endl;
            lsh.load(index);
            std::cout << "LOADING TIME: " << timer.elapsed() << "s." << std::endl;
        }
        else
        {
            std::cout << "CONSTRUCTING INDEX ..." << std::endl;
            rbsLsh::Parameter param;
            param.M = M;
            param.L = L;
            param.D = data.getDim();
            param.N = N;
            param.C = C;
            lsh.reset(param);
            lsh.hash(data);
            lsh.save(index);
            std::cout << "CONSTRUCTING TIME: " << timer.elapsed() << "s." << std::endl;
        }
    }
    void init_mat(
        const boost::python::list &source,
        const std::string &index,
        unsigned M = 521,
        unsigned L = 5,
        unsigned N = 20,
        unsigned C = 5)
    {
        std::vector<DATATYPE> vec = std::vector<DATATYPE>(
            boost::python::stl_input_iterator<DATATYPE>(reshape(source)),
            boost::python::stl_input_iterator<DATATYPE>()
        );
        data.load(vec, int(boost::python::len(source)), int(boost::python::len(source[0])));
        std::ifstream is(index.c_str(), std::ios_base::binary);
        lshbox::timer timer;
        if (is)
        {
            std::cout << "LOADING INDEX ..." << std::endl;
            lsh.load(index);
            std::cout << "LOADING TIME: " << timer.elapsed() << "s." << std::endl;
        }
        else
        {
            std::cout << "CONSTRUCTING INDEX ..." << std::endl;
            rbsLsh::Parameter param;
            param.M = M;
            param.L = L;
            param.D = data.getDim();
            param.N = N;
            param.C = C;
            lsh.reset(param);
            lsh.hash(data);
            lsh.save(index);
            std::cout << "CONSTRUCTING TIME: " << timer.elapsed() << "s." << std::endl;
        }
    }
    boost::python::list query(boost::python::list &quy, unsigned type, unsigned K)
    {
        std::vector<DATATYPE> quy_vec = std::vector<DATATYPE>(
            boost::python::stl_input_iterator<DATATYPE>(quy),
            boost::python::stl_input_iterator<DATATYPE>()
        );
        Matrix<DATATYPE>::Accessor accessor(data);
        Metric<DATATYPE> metric(data.getDim(), type);
        Scanner<lshbox::Matrix<DATATYPE>::Accessor> scanner(
            accessor,
            metric,
            K
        );
        lsh.query(&quy_vec[0], scanner);
        boost::python::list key;
        boost::python::list dist;
        std::vector<std::pair<float, unsigned> > tmp = scanner.topk().getTopk();
        for (auto it = tmp.begin(); it != tmp.end(); ++it)
        {
            key.append(it->second);
            dist.append(it->first);
        }
        boost::python::list result;
        result.append(key);
        result.append(dist);
        return result;
    }
private:
    Matrix<DATATYPE> data;
    rbsLsh lsh;
};
class pyRhpLsh
{
public:
    typedef float DATATYPE;
    void init_file(
        const std::string &source,
        const std::string &index,
        unsigned M = 521,
        unsigned L = 5,
        unsigned N = 6)
    {
        data.load(source);
        std::ifstream is(index.c_str(), std::ios_base::binary);
        lshbox::timer timer;
        if (is)
        {
            std::cout << "LOADING INDEX ..." << std::endl;
            lsh.load(index);
            std::cout << "LOADING TIME: " << timer.elapsed() << "s." << std::endl;
        }
        else
        {
            std::cout << "CONSTRUCTING INDEX ..." << std::endl;
            rhpLsh<DATATYPE>::Parameter param;
            param.M = M;
            param.L = L;
            param.D = data.getDim();
            param.N = N;
            lsh.reset(param);
            lsh.hash(data);
            lsh.save(index);
            std::cout << "CONSTRUCTING TIME: " << timer.elapsed() << "s." << std::endl;
        }
    }
    void init_mat(
        const boost::python::list &source,
        const std::string &index,
        unsigned M = 521,
        unsigned L = 5,
        unsigned N = 6)
    {
        std::vector<DATATYPE> vec = std::vector<DATATYPE>(
            boost::python::stl_input_iterator<DATATYPE>(reshape(source)),
            boost::python::stl_input_iterator<DATATYPE>()
        );
        data.load(vec, int(boost::python::len(source)), int(boost::python::len(source[0])));
        std::ifstream is(index.c_str(), std::ios_base::binary);
        lshbox::timer timer;
        if (is)
        {
            std::cout << "LOADING INDEX ..." << std::endl;
            lsh.load(index);
            std::cout << "LOADING TIME: " << timer.elapsed() << "s." << std::endl;
        }
        else
        {
            std::cout << "CONSTRUCTING INDEX ..." << std::endl;
            rhpLsh<DATATYPE>::Parameter param;
            param.M = M;
            param.L = L;
            param.D = data.getDim();
            param.N = N;
            lsh.reset(param);
            lsh.hash(data);
            lsh.save(index);
            std::cout << "CONSTRUCTING TIME: " << timer.elapsed() << "s." << std::endl;
        }
    }
    boost::python::list query(boost::python::list &quy, unsigned type, unsigned K)
    {
        std::vector<DATATYPE> quy_vec = std::vector<DATATYPE>(
            boost::python::stl_input_iterator<DATATYPE>(quy),
            boost::python::stl_input_iterator<DATATYPE>()
        );
        Matrix<DATATYPE>::Accessor accessor(data);
        Metric<DATATYPE> metric(data.getDim(), type);
        Scanner<lshbox::Matrix<DATATYPE>::Accessor> scanner(
            accessor,
            metric,
            K
        );
        lsh.query(&quy_vec[0], scanner);
        boost::python::list key;
        boost::python::list dist;
        std::vector<std::pair<float, unsigned> > tmp = scanner.topk().getTopk();
        for (auto it = tmp.begin(); it != tmp.end(); ++it)
        {
            key.append(it->second);
            dist.append(it->first);
        }
        boost::python::list result;
        result.append(key);
        result.append(dist);
        return result;
    }
private:
    Matrix<DATATYPE> data;
    rhpLsh<DATATYPE> lsh;
};
class pyThLsh
{
public:
    typedef float DATATYPE;
    void init_file(
        const std::string &source,
        const std::string &index,
        unsigned M = 521,
        unsigned L = 5,
        unsigned N = 12)
    {
        data.load(source);
        std::ifstream is(index.c_str(), std::ios_base::binary);
        lshbox::timer timer;
        if (is)
        {
            std::cout << "LOADING INDEX ..." << std::endl;
            lsh.load(index);
            std::cout << "LOADING TIME: " << timer.elapsed() << "s." << std::endl;
        }
        else
        {
            std::cout << "CONSTRUCTING INDEX ..." << std::endl;
            thLsh<DATATYPE>::Parameter param;
            param.M = M;
            param.L = L;
            param.D = data.getDim();
            param.N = N;
            param.Max = std::numeric_limits<float>::max();
            param.Min = - std::numeric_limits<float>::max();
            for (unsigned i = 0; i != data.getSize(); ++i)
            {
                for (unsigned j = 0; j != data.getDim(); ++j)
                {
                    if (data[i][j] > param.Max)
                    {
                        param.Max = data[i][j];
                    }
                    if (data[i][j] < param.Min)
                    {
                        param.Min = data[i][j];
                    }
                }
            }
            lsh.reset(param);
            lsh.hash(data);
            lsh.save(index);
            std::cout << "CONSTRUCTING TIME: " << timer.elapsed() << "s." << std::endl;
        }
    }
    void init_mat(
        const boost::python::list &source,
        const std::string &index,
        unsigned M = 521,
        unsigned L = 5,
        unsigned N = 12)
    {
        std::vector<DATATYPE> vec = std::vector<DATATYPE>(
            boost::python::stl_input_iterator<DATATYPE>(reshape(source)),
            boost::python::stl_input_iterator<DATATYPE>()
        );
        data.load(vec, int(boost::python::len(source)), int(boost::python::len(source[0])));
        std::ifstream is(index.c_str(), std::ios_base::binary);
        lshbox::timer timer;
        if (is)
        {
            std::cout << "LOADING INDEX ..." << std::endl;
            lsh.load(index);
            std::cout << "LOADING TIME: " << timer.elapsed() << "s." << std::endl;
        }
        else
        {
            std::cout << "CONSTRUCTING INDEX ..." << std::endl;
            thLsh<DATATYPE>::Parameter param;
            param.M = M;
            param.L = L;
            param.D = data.getDim();
            param.N = N;
            param.Max = std::numeric_limits<float>::max();
            param.Min = - std::numeric_limits<float>::max();
            for (unsigned i = 0; i != data.getSize(); ++i)
            {
                for (unsigned j = 0; j != data.getDim(); ++j)
                {
                    if (data[i][j] > param.Max)
                    {
                        param.Max = data[i][j];
                    }
                    if (data[i][j] < param.Min)
                    {
                        param.Min = data[i][j];
                    }
                }
            }
            lsh.reset(param);
            lsh.hash(data);
            lsh.save(index);
            std::cout << "CONSTRUCTING TIME: " << timer.elapsed() << "s." << std::endl;
        }
    }
    boost::python::list query(boost::python::list &quy, unsigned type, unsigned K)
    {
        std::vector<DATATYPE> quy_vec = std::vector<DATATYPE>(
            boost::python::stl_input_iterator<DATATYPE>(quy),
            boost::python::stl_input_iterator<DATATYPE>()
        );
        Matrix<DATATYPE>::Accessor accessor(data);
        Metric<DATATYPE> metric(data.getDim(), type);
        Scanner<lshbox::Matrix<DATATYPE>::Accessor> scanner(
            accessor,
            metric,
            K
        );
        lsh.query(&quy_vec[0], scanner);
        boost::python::list key;
        boost::python::list dist;
        std::vector<std::pair<float, unsigned> > tmp = scanner.topk().getTopk();
        for (auto it = tmp.begin(); it != tmp.end(); ++it)
        {
            key.append(it->second);
            dist.append(it->first);
        }
        boost::python::list result;
        result.append(key);
        result.append(dist);
        return result;
    }
private:
    Matrix<DATATYPE> data;
    thLsh<DATATYPE> lsh;
};
class pyPsdLsh
{
public:
    typedef float DATATYPE;
    void init_file(
        const std::string &source,
        const std::string &index,
        unsigned M = 521,
        unsigned L = 5,
        unsigned T = GAUSSIAN,
        float W = 0.5)
    {
        data.load(source);
        std::ifstream is(index.c_str(), std::ios_base::binary);
        lshbox::timer timer;
        if (is)
        {
            std::cout << "LOADING INDEX ..." << std::endl;
            lsh.load(index);
            std::cout << "LOADING TIME: " << timer.elapsed() << "s." << std::endl;
        }
        else
        {
            std::cout << "CONSTRUCTING INDEX ..." << std::endl;
            psdLsh<DATATYPE>::Parameter param;
            param.M = M;
            param.L = L;
            param.D = data.getDim();
            param.T = T;
            param.W = W;
            lsh.reset(param);
            lsh.hash(data);
            lsh.save(index);
            std::cout << "CONSTRUCTING TIME: " << timer.elapsed() << "s." << std::endl;
        }
    }
    void init_mat(
        const boost::python::list &source,
        const std::string &index,
        unsigned M = 521,
        unsigned L = 5,
        unsigned T = GAUSSIAN,
        float W = 0.5)
    {
        std::vector<DATATYPE> vec = std::vector<DATATYPE>(
            boost::python::stl_input_iterator<DATATYPE>(reshape(source)),
            boost::python::stl_input_iterator<DATATYPE>()
        );
        data.load(vec, int(boost::python::len(source)), int(boost::python::len(source[0])));
        std::ifstream is(index.c_str(), std::ios_base::binary);
        lshbox::timer timer;
        if (is)
        {
            std::cout << "LOADING INDEX ..." << std::endl;
            lsh.load(index);
            std::cout << "LOADING TIME: " << timer.elapsed() << "s." << std::endl;
        }
        else
        {
            std::cout << "CONSTRUCTING INDEX ..." << std::endl;
            psdLsh<DATATYPE>::Parameter param;
            param.M = M;
            param.L = L;
            param.D = data.getDim();
            param.T = T;
            param.W = W;
            lsh.reset(param);
            lsh.hash(data);
            lsh.save(index);
            std::cout << "CONSTRUCTING TIME: " << timer.elapsed() << "s." << std::endl;
        }
    }
    boost::python::list query(boost::python::list &quy, unsigned type, unsigned K)
    {
        std::vector<DATATYPE> quy_vec = std::vector<DATATYPE>(
            boost::python::stl_input_iterator<DATATYPE>(quy),
            boost::python::stl_input_iterator<DATATYPE>()
        );
        Matrix<DATATYPE>::Accessor accessor(data);
        Metric<DATATYPE> metric(data.getDim(), type);
        Scanner<lshbox::Matrix<DATATYPE>::Accessor> scanner(
            accessor,
            metric,
            K
        );
        lsh.query(&quy_vec[0], scanner);
        boost::python::list key;
        boost::python::list dist;
        std::vector<std::pair<float, unsigned> > tmp = scanner.topk().getTopk();
        for (auto it = tmp.begin(); it != tmp.end(); ++it)
        {
            key.append(it->second);
            dist.append(it->first);
        }
        boost::python::list result;
        result.append(key);
        result.append(dist);
        return result;
    }
private:
    Matrix<DATATYPE> data;
    psdLsh<DATATYPE> lsh;
};
class pyShLsh
{
public:
    typedef float DATATYPE;
    void init_file(
        const std::string &source,
        const std::string &index,
        unsigned M = 521,
        unsigned L = 5,
        unsigned N = 4,
        unsigned S = 100)
    {
        data.load(source);
        std::ifstream is(index.c_str(), std::ios_base::binary);
        lshbox::timer timer;
        if (is)
        {
            std::cout << "LOADING INDEX ..." << std::endl;
            lsh.load(index);
            std::cout << "LOADING TIME: " << timer.elapsed() << "s." << std::endl;
        }
        else
        {
            std::cout << "CONSTRUCTING INDEX ..." << std::endl;
            shLsh<DATATYPE>::Parameter param;
            param.M = M;
            param.L = L;
            param.D = data.getDim();
            param.N = N;
            param.S = S;
            lsh.reset(param);
            lsh.train(data);
            lsh.hash(data);
            lsh.save(index);
            std::cout << "CONSTRUCTING TIME: " << timer.elapsed() << "s." << std::endl;
        }
    }
    void init_mat(
        const boost::python::list &source,
        const std::string &index,
        unsigned M = 521,
        unsigned L = 5,
        unsigned N = 4,
        unsigned S = 100)
    {
        std::vector<DATATYPE> vec = std::vector<DATATYPE>(
            boost::python::stl_input_iterator<DATATYPE>(reshape(source)),
            boost::python::stl_input_iterator<DATATYPE>()
        );
        data.load(vec, int(boost::python::len(source)), int(boost::python::len(source[0])));
        std::ifstream is(index.c_str(), std::ios_base::binary);
        lshbox::timer timer;
        if (is)
        {
            std::cout << "LOADING INDEX ..." << std::endl;
            lsh.load(index);
            std::cout << "LOADING TIME: " << timer.elapsed() << "s." << std::endl;
        }
        else
        {
            std::cout << "CONSTRUCTING INDEX ..." << std::endl;
            shLsh<DATATYPE>::Parameter param;
            param.M = M;
            param.L = L;
            param.D = data.getDim();
            param.N = N;
            param.S = S;
            lsh.reset(param);
            lsh.train(data);
            lsh.hash(data);
            lsh.save(index);
            std::cout << "CONSTRUCTING TIME: " << timer.elapsed() << "s." << std::endl;
        }
    }
    boost::python::list query(boost::python::list &quy, unsigned type, unsigned K)
    {
        std::vector<DATATYPE> quy_vec = std::vector<DATATYPE>(
            boost::python::stl_input_iterator<DATATYPE>(quy),
            boost::python::stl_input_iterator<DATATYPE>()
        );
        Matrix<DATATYPE>::Accessor accessor(data);
        Metric<DATATYPE> metric(data.getDim(), type);
        Scanner<lshbox::Matrix<DATATYPE>::Accessor> scanner(
            accessor,
            metric,
            K
        );
        lsh.query(&quy_vec[0], scanner);
        boost::python::list key;
        boost::python::list dist;
        std::vector<std::pair<float, unsigned> > tmp = scanner.topk().getTopk();
        for (auto it = tmp.begin(); it != tmp.end(); ++it)
        {
            key.append(it->second);
            dist.append(it->first);
        }
        boost::python::list result;
        result.append(key);
        result.append(dist);
        return result;
    }
private:
    Matrix<DATATYPE> data;
    shLsh<DATATYPE> lsh;
};
class pyItqLsh
{
public:
    typedef float DATATYPE;
    void init_file(
        const std::string &source,
        const std::string &index,
        unsigned M = 521,
        unsigned L = 5,
        unsigned N = 8,
        unsigned S = 100,
        unsigned I = 50)
    {
        data.load(source);
        std::ifstream is(index.c_str(), std::ios_base::binary);
        lshbox::timer timer;
        if (is)
        {
            std::cout << "LOADING INDEX ..." << std::endl;
            lsh.load(index);
            std::cout << "LOADING TIME: " << timer.elapsed() << "s." << std::endl;
        }
        else
        {
            std::cout << "CONSTRUCTING INDEX ..." << std::endl;
            itqLsh<DATATYPE>::Parameter param;
            param.M = M;
            param.L = L;
            param.D = data.getDim();
            param.N = N;
            param.S = S;
            param.I = I;
            lsh.reset(param);
            lsh.train(data);
            lsh.hash(data);
            lsh.save(index);
            std::cout << "CONSTRUCTING TIME: " << timer.elapsed() << "s." << std::endl;
        }
    }
    void init_mat(
        const boost::python::list &source,
        const std::string &index,
        unsigned M = 521,
        unsigned L = 5,
        unsigned N = 8,
        unsigned S = 100,
        unsigned I = 50)
    {
        std::vector<DATATYPE> vec = std::vector<DATATYPE>(
            boost::python::stl_input_iterator<DATATYPE>(reshape(source)),
            boost::python::stl_input_iterator<DATATYPE>()
        );
        data.load(vec, int(boost::python::len(source)), int(boost::python::len(source[0])));
        std::ifstream is(index.c_str(), std::ios_base::binary);
        lshbox::timer timer;
        if (is)
        {
            std::cout << "LOADING INDEX ..." << std::endl;
            lsh.load(index);
            std::cout << "LOADING TIME: " << timer.elapsed() << "s." << std::endl;
        }
        else
        {
            std::cout << "CONSTRUCTING INDEX ..." << std::endl;
            itqLsh<DATATYPE>::Parameter param;
            param.M = M;
            param.L = L;
            param.D = data.getDim();
            param.N = N;
            param.S = S;
            param.I = I;
            lsh.reset(param);
            lsh.train(data);
            lsh.hash(data);
            lsh.save(index);
            std::cout << "CONSTRUCTING TIME: " << timer.elapsed() << "s." << std::endl;
        }
    }
    boost::python::list query(boost::python::list &quy, unsigned type, unsigned K)
    {
        std::vector<DATATYPE> quy_vec = std::vector<DATATYPE>(
            boost::python::stl_input_iterator<DATATYPE>(quy),
            boost::python::stl_input_iterator<DATATYPE>()
        );
        Matrix<DATATYPE>::Accessor accessor(data);
        Metric<DATATYPE> metric(data.getDim(), type);
        Scanner<lshbox::Matrix<DATATYPE>::Accessor> scanner(
            accessor,
            metric,
            K
        );
        lsh.query(&quy_vec[0], scanner);
        boost::python::list key;
        boost::python::list dist;
        std::vector<std::pair<float, unsigned> > tmp = scanner.topk().getTopk();
        for (auto it = tmp.begin(); it != tmp.end(); ++it)
        {
            key.append(it->second);
            dist.append(it->first);
        }
        boost::python::list result;
        result.append(key);
        result.append(dist);
        return result;
    }
private:
    Matrix<DATATYPE> data;
    itqLsh<DATATYPE> lsh;
};
}

#ifdef WIN32
    BOOST_PYTHON_MODULE(pylshbox)
#else
    BOOST_PYTHON_MODULE(libpylshbox)
#endif
{
    using namespace boost::python;
    class_<lshbox::pyRbsLsh>("rbslsh", "Locality-Sensitive Hashing Scheme Based on Random Bits Sampling.")
        .def("init_file", &lshbox::pyRbsLsh::init_file, (arg("source"), arg("index"), arg("M") = 521, arg("L") = 5, arg("N") = 20, arg("C") = 5))
        .def("init_mat", &lshbox::pyRbsLsh::init_mat, (arg("source"), arg("index"), arg("M") = 521, arg("L") = 5, arg("N") = 20, arg("C") = 5))
        .def("query", &lshbox::pyRbsLsh::query, (arg("quy"), arg("type") = 2, arg("K") = 10));
    class_<lshbox::pyRhpLsh>("rhplsh", "Locality-Sensitive Hashing Scheme Based on Random Hyperplane.")
        .def("init_file", &lshbox::pyRhpLsh::init_file, (arg("source"), arg("index"), arg("M") = 521, arg("L") = 5, arg("N") = 6))
        .def("init_mat", &lshbox::pyRhpLsh::init_mat, (arg("source"), arg("index"), arg("M") = 521, arg("L") = 5, arg("N") = 6))
        .def("query", &lshbox::pyRhpLsh::query, (arg("quy"), arg("type") = 2, arg("K") = 10));
    class_<lshbox::pyThLsh>("thlsh", "Locality-Sensitive Hashing Scheme Based on Thresholding.")
        .def("init_file", &lshbox::pyThLsh::init_file, (arg("source"), arg("index"), arg("M") = 521, arg("L") = 5, arg("N") = 12))
        .def("init_mat", &lshbox::pyThLsh::init_mat, (arg("source"), arg("index"), arg("M") = 521, arg("L") = 5, arg("N") = 12))
        .def("query", &lshbox::pyThLsh::query, (arg("quy"), arg("type") = 2, arg("K") = 10));
    class_<lshbox::pyPsdLsh>("psdlsh", "Locality-Sensitive Hashing Scheme Based on p-Stable Distributions.")
        .def("init_file", &lshbox::pyPsdLsh::init_file, (arg("source"), arg("index"), arg("M") = 521, arg("L") = 5, arg("T") = 2, arg("W") = 0.5))
        .def("init_mat", &lshbox::pyPsdLsh::init_mat, (arg("source"), arg("index"), arg("M") = 521, arg("L") = 5, arg("T") = 2, arg("W") = 0.5))
        .def("query", &lshbox::pyPsdLsh::query, (arg("quy"), arg("type") = 2, arg("K") = 10));
    class_<lshbox::pyShLsh>("shlsh", "Locality-Sensitive Hashing Scheme Based on Spectral Hashing.")
        .def("init_file", &lshbox::pyShLsh::init_file, (arg("source"), arg("index"), arg("M") = 521, arg("L") = 5, arg("N") = 4, arg("S") = 100))
        .def("init_mat", &lshbox::pyShLsh::init_mat, (arg("source"), arg("index"), arg("M") = 521, arg("L") = 5, arg("N") = 4, arg("S") = 100))
        .def("query", &lshbox::pyShLsh::query, (arg("quy"), arg("type") = 2, arg("K") = 10));
    class_<lshbox::pyItqLsh>("itqlsh", "Locality-Sensitive Hashing Scheme Based on Iterative Quantization.")
		.def("init_file", &lshbox::pyItqLsh::init_file, (arg("source"), arg("index"), arg("M") = 521, arg("L") = 5, arg("N") = 8, arg("S") = 100, arg("I") = 50))
        .def("init_mat", &lshbox::pyItqLsh::init_mat, (arg("source"), arg("index"), arg("M") = 521, arg("L") = 5, arg("N") = 8, arg("S") = 100, arg("I") = 50))
		.def("query", &lshbox::pyItqLsh::query, (arg("quy"), arg("type") = 2, arg("K") = 10));
}