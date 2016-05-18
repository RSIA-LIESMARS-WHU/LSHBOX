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

/**
 * @file matrix.h
 *
 * @brief Dataset management class.
 */
#pragma once
#include <fstream>
#include <vector>
#include <assert.h>
#include <string.h>
namespace lshbox
{
/**
 * Dataset management class. A dataset is maintained as a matrix in memory.
 *
 * The file contains N D-dimensional vectors of single precision floating point numbers.
 *
 * Such binary files can be accessed using lshbox::Matrix<double>.
 */
template <class T>
class Matrix
{
    int dim;
    int N;
    T *dims;
public:
    /**
     * Reset the size.
     *
     * @param _dim Dimension of each vector
     * @param _N   Number of vectors
     */
    void reset(int _dim, int _N)
    {
        dim = _dim;
        N = _N;
        if (dims != NULL)
        {
            delete [] dims;
        }
        dims = new T[dim * N];
    }
    Matrix(): dim(0), N(0), dims(NULL) {}
    Matrix(int _dim, int _N): dims(NULL)
    {
        reset(_dim, _N);
    }
    ~Matrix()
    {
        if (dims != NULL)
        {
            delete [] dims;
        }
    }
    /**
     * Access the ith vector.
     */
    const T *operator [] (int i) const
    {
        return dims + i * dim;
    }
    /**
     * Access the ith vector.
     */
    T *operator [] (int i)
    {
        return dims + i * dim;
    }
    /**
     * Get the dimension.
     */
    int getDim() const
    {
        return dim;
    }
    /**
     * Get the size.
     */
    int getSize() const
    {
        return N;
    }
    /**
     * Get the data.
     */
    T * getData() const
    {
        return dims;
    }
    /**
     * Load the Matrix from a binary file.
     */
    void load(const std::string &path)
    {
        std::ifstream is(path.c_str(), std::ios::binary);
        unsigned header[3];
        assert(sizeof header == 3 * 4);
        is.read((char *)header, sizeof(header));
        reset(header[2], header[1]);
        is.read((char *)dims, sizeof(T) * dim * N);
        is.close();
    }
    /**
     * Load the Matrix from std::vector<T>.
     *
     * @param vec  The reference of std::vector<T>.
     * @param _N   Number of vectors
     * @param _dim Dimension of each vector
     */
    void load(std::vector<T> &vec, int _N, int _dim)
    {
        reset(_dim, _N);
        memcpy(dims, (void*)&vec[0], sizeof(T) * dim * N);
    }
    /**
     * Load the Matrix from T*.
     *
     * @param source The pointer to T*.
     * @param _N     Number of vectors
     * @param _dim   Dimension of each vector
     */
    void load(T *source, int _N, int _dim)
    {
        reset(_dim, _N);
        memcpy(dims, source, sizeof(T) * dim * N);
    }
    /**
     * Save the Matrix as a binary file.
     */
    void save(const std::string &path)
    {
        std::ofstream os(path.c_str(), std::ios::binary);
        unsigned header[3];
        header[0] = sizeof(T);
        header[1] = N;
        header[2] = dim;
        os.write((char *)header, sizeof header);
        os.write((char *)dims, sizeof(T) * dim * N);
        os.close();
    }
    Matrix(const std::string &path): dims(NULL)
    {
        load(path);
    }
    Matrix(const Matrix& M): dims(NULL)
    {
        reset(M.getDim(), M.getSize());
        memcpy(dims, M.getData(), sizeof(T) * dim * N);
    }
    Matrix& operator = (const Matrix& M)
    {
        dims = NULL;
        reset(M.getDim(), M.getSize());
        memcpy(dims, M.getData(), sizeof(T) * dim * N);
        return *this;
    }
    /**
     * An accessor class to be used with LSH index.
     */
    class Accessor
    {
        const Matrix &matrix_;
        std::vector<bool> flags_;
    public:
        typedef unsigned Key;
        typedef const T *Value;
        typedef T DATATYPE;
        Accessor(const Matrix &matrix): matrix_(matrix)
        {
            flags_.resize(matrix_.getSize());
        }
        void reset()
        {
            flags_.clear();
            flags_.resize(matrix_.getSize());
        }
        bool mark(unsigned key)
        {
            if (flags_[key])
            {
                return false;
            }
            flags_[key] = true;
            return true;
        }
        const T *operator () (unsigned key)
        {
            return matrix_[key];
        }
    };
};
}