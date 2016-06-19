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
 * @file thlsh.h
 *
 * @brief Locality-Sensitive Hashing Scheme Based on Thresholding.
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
 * Locality-Sensitive Hashing Scheme Based on Thresholding.
 *
 *
 * For more information on thresholding based LSH, see the following reference.
 *
 *     Zhe Wang, Wei Dong, William Josephson, Qin Lv, Moses Charikar, Kai Li.
 *     Sizing Sketches: A Rank-Based Analysis for Similarity Search. In
 *     Proceedings of the 2007 ACM SIGMETRICS International Conference on
 *     Measurement and Modeling of Computer Systems . San Diego, CA, USA. June
 *     2007.
 *
 *     Qin Lv, Moses Charikar, Kai Li. Image Similarity Search with Compact
 *     Data Structures. In Proceedings of ACM 13th Conference on Information
 *     and Knowledge Management (CIKM), Washington D.C., USA. November 2004.
 */
template<typename DATATYPE = float>
class thLsh
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
        /// Upper bound of each dimension
        float Max;
        /// Lower bound of each dimension
        float Min;
    };
    thLsh() {}
    thLsh(const Parameter &param_)
    {
        reset(param_);
    }
    ~thLsh() {}
    /**
     * Reset the parameter setting
     *
     * @param param_ A instance of thLsh<DATATYPE>::Parametor, which contains
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
    std::vector<float> threadholds;
    std::vector<std::vector<unsigned> > rndBits;
    std::vector<std::vector<unsigned> > rndArray;
    std::vector<std::map<unsigned, std::vector<unsigned> > > tables;
};
}

// ------------------------- implementation -------------------------
template<typename DATATYPE>
void lshbox::thLsh<DATATYPE>::reset(const Parameter &param_)
{
    param = param_;
    tables.resize(param.L);
    rndBits.resize(param.L);
    rndArray.resize(param.L);
    std::mt19937 rng(unsigned(std::time(0)));
    std::uniform_int_distribution<unsigned> usBits(0, param.D - 1);
    std::uniform_int_distribution<unsigned> usArray(0, param.M - 1);
    std::uniform_real_distribution<float> urThreadholds(param.Min, param.Max);
    for (unsigned i = 0; i != param.L; ++i)
    {
        while (rndBits[i].size() != param.N)
        {
            unsigned target = usBits(rng);
            if (std::find(rndBits[i].begin(), rndBits[i].end(), target) == rndBits[i].end())
            {
                rndBits[i].push_back(target);
            }
        }
        std::sort(rndBits[i].begin(), rndBits[i].end());
        threadholds.push_back(urThreadholds(rng));
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
void lshbox::thLsh<DATATYPE>::hash(Matrix<DATATYPE> &data)
{
    progress_display pd(data.getSize());
    for (unsigned i = 0; i != data.getSize(); ++i)
    {
        insert(i, data[i]);
        ++pd;
    }
}
template<typename DATATYPE>
void lshbox::thLsh<DATATYPE>::insert(unsigned key, const DATATYPE *domin)
{
    for (unsigned k = 0; k != param.L; ++k)
    {
        unsigned hashVal = getHashVal(k, domin);
        tables[k][hashVal].push_back(key);
    }
}
template<typename DATATYPE>
template<typename SCANNER>
void lshbox::thLsh<DATATYPE>::query(const DATATYPE *domin, SCANNER &scanner)
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
unsigned lshbox::thLsh<DATATYPE>::getHashVal(unsigned k, const DATATYPE *domin)
{
    unsigned sum(0), seq(0);
    for (std::vector<unsigned>::iterator it = rndBits[k].begin(); it != rndBits[k].end(); ++it)
    {
        if (domin[*it] > threadholds[seq])
        {
            sum += rndArray[k][seq];
        }
        ++seq;
    }
    unsigned hashVal = sum % param.M;
    return hashVal;
}
template<typename DATATYPE>
void lshbox::thLsh<DATATYPE>::load(const std::string &file)
{
    std::ifstream in(file, std::ios::binary);
    in.read((char *)&param.M, sizeof(unsigned));
    in.read((char *)&param.L, sizeof(unsigned));
    in.read((char *)&param.D, sizeof(unsigned));
    in.read((char *)&param.N, sizeof(unsigned));
    in.read((char *)&param.Max, sizeof(float));
    in.read((char *)&param.Min, sizeof(float));
    tables.resize(param.L);
    rndBits.resize(param.L);
    rndArray.resize(param.L);
    threadholds.resize(param.L);
    in.read((char *)&threadholds[0], sizeof(float) * param.L);
    for (unsigned i = 0; i != param.L; ++i)
    {
        rndBits[i].resize(param.N);
        rndArray[i].resize(param.N);
        in.read((char *)&rndBits[i][0], sizeof(unsigned) * param.N);
        in.read((char *)&rndArray[i][0], sizeof(unsigned) * param.N);
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
void lshbox::thLsh<DATATYPE>::save(const std::string &file)
{
    std::ofstream out(file, std::ios::binary);
    out.write((char *)&param.M, sizeof(unsigned));
    out.write((char *)&param.L, sizeof(unsigned));
    out.write((char *)&param.D, sizeof(unsigned));
    out.write((char *)&param.N, sizeof(unsigned));
    out.write((char *)&param.Max, sizeof(float));
    out.write((char *)&param.Min, sizeof(float));
    out.write((char *)&threadholds[0], sizeof(float) * param.L);
    for (int i = 0; i != param.L; ++i)
    {
        out.write((char *)&rndBits[i][0], sizeof(unsigned) * param.N);
        out.write((char *)&rndArray[i][0], sizeof(unsigned) * param.N);
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