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
 * @file rhplsh.h
 *
 * @brief Locality-Sensitive Hashing Scheme Based on Random Hyperplane.
 */
#pragma once
#include <map>
#include <vector>
#include <random>
#include <iostream>
#include <functional>
namespace lshbox
{
/**
 * Locality-Sensitive Hashing Scheme Based on Random Hyperplane.
 *
 *
 * For more information on random hyperplane based LSH, see the following reference.
 *
 *     Charikar, M. S. 2002. Similarity estimation techniques from rounding
 *     algorithms. In Proceedings of the Thiry-Fourth Annual ACM Symposium on
 *     theory of Computing (Montreal, Quebec, Canada, May 19 - 21, 2002). STOC '02.
 *     ACM, New York, NY, 380-388. DOI= http://doi.acm.org/10.1145/509907.509965
 */
template<typename DATATYPE = float>
class rhpLsh
{
public:
    struct Parameter
    {
        /// Hash table size
        unsigned M;
        /// Number of hash tables
        unsigned L;
        /// Dimension of the vector, it can be obtained from the instance of Matrix
        unsigned D;
        /// Binary code bytes
        unsigned N;
    };
    rhpLsh() {}
    rhpLsh(const Parameter &param_)
    {
        reset(param_);
    }
    ~rhpLsh() {}
    /**
     * Reset the parameter setting
     *
     * @param param_ A instance of rhpLsh<DATATYPE>::Parametor, which contains
     * the necessary parameters
     */
    void reset(const Parameter &param_);
    /**
     * Hash the dataset.
     *
     * @param data A instance of Matrix<DATATYPE>, it is the search dataset.
     */
    void hash(Matrix<DATATYPE> &data);
    /**
     * Insert a vector to the index.
     *
     * @param key   The sequence number of vector
     * @param domin The pointer to the vector
     */
    void insert(unsigned key, const DATATYPE *domin);
    /**
     * Query the approximate nearest neighborholds.
     *
     * @param domin   The pointer to the vector
     * @param scanner Top-K scanner, use for scan the approximate nearest neighborholds
     */
    template<typename SCANNER>
    void query(const DATATYPE *domin, SCANNER &scanner);
    /**
     * get the hash value of a vector.
     *
     * @param k     The idx of the table
     * @param domin The pointer to the vector
     * @return      The hash value
     */
    unsigned getHashVal(unsigned k, const DATATYPE *domin);
    /**
     * Load the index from binary file.
     *
     * @param file The path of binary file.
     */
    void load(const std::string &file);
    /**
     * Save the index as binary file.
     *
     * @param file The path of binary file.
     */
    void save(const std::string &file);
private:
    Parameter param;
    std::vector<std::vector<unsigned> > rndArray;
    std::vector<std::vector<std::vector<float> > > uosArray;
    std::vector<std::map<unsigned, std::vector<unsigned> > > tables;
};
}

// ------------------------- implementation -------------------------
template<typename DATATYPE>
void lshbox::rhpLsh<DATATYPE>::reset(const Parameter &param_)
{
    param = param_;
    tables.resize(param.L);
    uosArray.resize(param.L);
    rndArray.resize(param.L);
    std::mt19937 rng(unsigned(std::time(0)));
    std::normal_distribution<float> nd;
    std::uniform_int_distribution<unsigned> usArray(0, param.M - 1);
    for (std::vector<std::vector<std::vector<float> > >::iterator ithRb = uosArray.begin(); ithRb != uosArray.end(); ++ithRb)
    {
        ithRb->resize(param.N);
        for (std::vector<std::vector<float> >::iterator iter = ithRb->begin(); iter != ithRb->end(); ++iter)
        {
            for (unsigned k = 0; k != param.D; ++k)
            {
                iter->push_back(nd(rng));
            }
        }
    }
    for (std::vector<std::vector<unsigned> >::iterator iter = rndArray.begin(); iter != rndArray.end(); ++iter)
    {
        for (unsigned i = 0; i != param.N; ++i)
        {
            iter->push_back(usArray(rng));
        }
    }
}
template<typename DATATYPE>
void lshbox::rhpLsh<DATATYPE>::hash(Matrix<DATATYPE> &data)
{
    progress_display pd(data.getSize());
    for (unsigned i = 0; i != data.getSize(); ++i)
    {
        insert(i, data[i]);
        ++pd;
    }
}
template<typename DATATYPE>
void lshbox::rhpLsh<DATATYPE>::insert(unsigned key, const DATATYPE *domin)
{
    for (unsigned k = 0; k != param.L; ++k)
    {
        unsigned hashVal = getHashVal(k, domin);
        tables[k][hashVal].push_back(key);
    }
}
template<typename DATATYPE>
template<typename SCANNER>
void lshbox::rhpLsh<DATATYPE>::query(const DATATYPE *domin, SCANNER &scanner)
{
    scanner.reset(domin);
    for (unsigned k = 0; k != param.L; ++k)
    {
        unsigned hashVal = getHashVal(k, domin);
        if (tables[k].find(hashVal) != tables[k].end())
        {
            for (std::vector<unsigned>::iterator iter = tables[k][hashVal].begin(); iter != tables[k][hashVal].end(); ++iter)
            {
                scanner(*iter);
            }
        }
    }
    scanner.topk().genTopk();
}
template<typename DATATYPE>
unsigned lshbox::rhpLsh<DATATYPE>::getHashVal(unsigned k, const DATATYPE *domin)
{
    unsigned sum(0);
    for (unsigned i = 0; i != param.N; ++i)
    {
        float flag(0);
        for (unsigned j = 0; j != param.D; ++j)
        {
            flag += uosArray[k][i][j] * domin[j];
        }
        if (flag > 0)
        {
            sum += rndArray[k][i];
        }
    }
    unsigned hashVal = sum % param.M;
    return hashVal;
}
template<typename DATATYPE>
void lshbox::rhpLsh<DATATYPE>::load(const std::string &file)
{
    std::ifstream in(file, std::ios::binary);
    in.read((char *)&param.M, sizeof(unsigned));
    in.read((char *)&param.L, sizeof(unsigned));
    in.read((char *)&param.D, sizeof(unsigned));
    in.read((char *)&param.N, sizeof(unsigned));
    tables.resize(param.L);
    uosArray.resize(param.L);
    rndArray.resize(param.L);
    for (unsigned i = 0; i != param.L; ++i)
    {
        rndArray[i].resize(param.N);
        uosArray[i].resize(param.N);
        in.read((char *)&rndArray[i][0], sizeof(unsigned) * param.N);
        for (unsigned j = 0; j != param.N; ++j)
        {
            uosArray[i][j].resize(param.D);
            in.read((char *)&uosArray[i][j][0], sizeof(float) * param.D);
        }
        unsigned count;
        in.read((char *)&count, sizeof(unsigned));
        for (unsigned j = 0; j != count; ++j)
        {
            unsigned target;
            in.read((char *)&target, sizeof(unsigned));
            unsigned length;
            in.read((char *)&length, sizeof(unsigned));
            tables[i][target].resize(length);
            in.read((char *) & (tables[i][target][0]), sizeof(unsigned) * length);
        }
    }
    in.close();
}
template<typename DATATYPE>
void lshbox::rhpLsh<DATATYPE>::save(const std::string &file)
{
    std::ofstream out(file, std::ios::binary);
    out.write((char *)&param.M, sizeof(unsigned));
    out.write((char *)&param.L, sizeof(unsigned));
    out.write((char *)&param.D, sizeof(unsigned));
    out.write((char *)&param.N, sizeof(unsigned));
    for (unsigned i = 0; i != param.L; ++i)
    {
        out.write((char *)&rndArray[i][0], sizeof(unsigned) * param.N);
        for (unsigned j = 0; j != param.N; ++j)
        {
            out.write((char *)&uosArray[i][j][0], sizeof(float) * param.D);
        }
        unsigned count = unsigned(tables[i].size());
        out.write((char *)&count, sizeof(unsigned));
        for (std::map<unsigned, std::vector<unsigned> >::iterator iter = tables[i].begin(); iter != tables[i].end(); ++iter)
        {
            unsigned target = iter->first;
            out.write((char *)&target, sizeof(unsigned));
            unsigned length = unsigned(iter->second.size());
            out.write((char *)&length, sizeof(unsigned));
            out.write((char *) & ((iter->second)[0]), sizeof(unsigned) * length);
        }
    }
    out.close();
}