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
 * @file rbslsh.h
 *
 * @brief Locality-Sensitive Hashing Scheme Based on Random Bits Sampling.
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
 * Locality-Sensitive Hashing Scheme Based on Random Bits Sampling.
 *
 *
 * For more information on random bits sampling based LSH, see the following reference.
 *
 *     P. Indyk and R. Motwani. Approximate Nearest Neighbor - Towards Removing
 *     the Curse of Dimensionality. In Proceedings of the 30th Symposium on Theory
 *     of Computing, 1998, pp. 604-613.
 *
 *     A. Gionis, P. Indyk, and R. Motwani. Similarity search in high dimensions
 *     via hashing. Proceedings of the 25th International Conference on Very Large
 *     Data Bases (VLDB), 1999.
 */
class rbsLsh
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
        /// The Difference between upper and lower bound of each dimension
        unsigned C;
    };
    rbsLsh() {}
    rbsLsh(const Parameter &param_)
    {
        reset(param_);
    }
    ~rbsLsh() {}
    /**
     * Reset the parameter setting
     *
     * @param param_ A instance of rbsLsh::Parametor, which contains the necessary
     * parameters
     */
    void reset(const Parameter &param_);
    /**
     * Hash the dataset.
     *
     * @param data A instance of Matrix<unsigned>, it is the search dataset.
     */
    void hash(Matrix<unsigned> &data);
    /**
     * Insert a vector to the index.
     *
     * @param key   The sequence number of vector
     * @param domin The pointer to the vector
     */
    void insert(unsigned key, const unsigned *domin);
    /**
     * Query the approximate nearest neighborholds.
     *
     * @param domin   The pointer to the vector
     * @param scanner Top-K scanner, use for scan the approximate nearest neighborholds
     */
    template <typename SCANNER>
    void query(const unsigned *domin, SCANNER &scanner);
    /**
     * get the hash value of a vector.
     *
     * @param k     The idx of the table
     * @param domin The pointer to the vector
     * @return      The hash value
     */
    unsigned getHashVal(unsigned k, const unsigned *domin);
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
    std::vector<std::vector<unsigned> > rndBits;
    std::vector<std::vector<unsigned> > rndArray;
    std::vector<std::map<unsigned, std::vector<unsigned> > > tables;
};
}

// ------------------------- implementation -------------------------
void lshbox::rbsLsh::reset(const Parameter &param_)
{
    param = param_;
    tables.resize(param.L);
    rndBits.resize(param.L);
    rndArray.resize(param.L);
    std::mt19937 rng(unsigned(std::time(0)));
    std::uniform_int_distribution<unsigned> usBits(0, param.D * param.C - 1);
    for (std::vector<std::vector<unsigned> >::iterator iter = rndBits.begin(); iter != rndBits.end(); ++iter)
    {
        while (iter->size() != param.N)
        {
            unsigned target = usBits(rng);
            if (std::find(iter->begin(), iter->end(), target) == iter->end())
            {
                iter->push_back(target);
            }
        }
        std::sort(iter->begin(), iter->end());
    }
    std::uniform_int_distribution<unsigned> usArray(0, param.M - 1);
    for (std::vector<std::vector<unsigned> >::iterator iter = rndArray.begin(); iter != rndArray.end(); ++iter)
    {
        for (unsigned i = 0; i != param.N; ++i)
        {
            iter->push_back(usArray(rng));
        }
    }
}
void lshbox::rbsLsh::hash(Matrix<unsigned> &data)
{
    progress_display pd(data.getSize());
    for (unsigned i = 0; i != data.getSize(); ++i)
    {
        insert(i, data[i]);
        ++pd;
    }
}
void lshbox::rbsLsh::insert(unsigned key, const unsigned *domin)
{
    for (unsigned k = 0; k != param.L; ++k)
    {
        unsigned hashVal = getHashVal(k, domin);
        tables[k][hashVal].push_back(key);
    }
}
template<typename SCANNER>
void lshbox::rbsLsh::query(const unsigned *domin, SCANNER &scanner)
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
unsigned lshbox::rbsLsh::getHashVal(unsigned k, const unsigned *domin)
{
    unsigned sum(0), seq(0);
    for (std::vector<unsigned>::iterator it = rndBits[k].begin(); it != rndBits[k].end(); ++it)
    {
        if ((*it % param.C) <= unsigned(domin[*it / param.C]))
        {
            sum += rndArray[k][seq];
        }
        ++seq;
    }
    unsigned hashVal = sum % param.M;
    return hashVal;
}
void lshbox::rbsLsh::load(const std::string &file)
{
    std::ifstream in(file, std::ios::binary);
    in.read((char *)&param.M, sizeof(unsigned));
    in.read((char *)&param.L, sizeof(unsigned));
    in.read((char *)&param.D, sizeof(unsigned));
    in.read((char *)&param.C, sizeof(unsigned));
    in.read((char *)&param.N, sizeof(unsigned));
    tables.resize(param.L);
    rndBits.resize(param.L);
    rndArray.resize(param.L);
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
void lshbox::rbsLsh::save(const std::string &file)
{
    std::ofstream out(file, std::ios::binary);
    out.write((char *)&param.M, sizeof(unsigned));
    out.write((char *)&param.L, sizeof(unsigned));
    out.write((char *)&param.D, sizeof(unsigned));
    out.write((char *)&param.C, sizeof(unsigned));
    out.write((char *)&param.N, sizeof(unsigned));
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