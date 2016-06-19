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
 * @file shlsh.h
 *
 * @brief Locality-Sensitive Hashing Scheme Based on Spectral Hashing.
 */
#pragma once
#include <map>
#include <vector>
#include <random>
#include <iostream>
#include <functional>
#include <algorithm>
#include <eigen/Eigen/Dense>
namespace lshbox
{
/**
 * Locality-Sensitive Hashing Scheme Based on Spectral Hashing.
 *
 *
 * For more information on spectral hashing based LSH, see the following reference.
 *
 *     Y. Weiss, A. Torralba, R. Fergus. Spectral Hashing.
 *     Advances in Neural Information Processing Systems, 2008.
 */
template<typename DATATYPE = float>
class shLsh
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
        /// Size of vectors in train
        unsigned S;
    };
    shLsh() {}
    shLsh(const Parameter &param_)
    {
        reset(param_);
    }
    ~shLsh() {}
    /**
     * Reset the parameter setting
     *
     * @param param_ A instance of shLsh<DATATYPE>::Parametor, which contains
     * the necessary parameters
     */
    void reset(const Parameter &param_);
    /**
     * Train the data to get several groups of suitable vector for index.
     *
     * @param data A instance of Matrix<DATATYPE>, most of the time, is the search dataset.
     */
    void train(Matrix<DATATYPE> &data);
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
    std::vector<std::vector<double> > minsAll;
    std::vector<std::vector<std::vector<float> > > pcsAll;
    std::vector<std::vector<std::vector<float> > > omegasAll;
    std::vector<std::vector<unsigned> > rndArray;
    std::vector<std::map<unsigned, std::vector<unsigned> > > tables;
};
}

// ------------------------- implementation -------------------------
template<typename DATATYPE>
void lshbox::shLsh<DATATYPE>::reset(const Parameter &param_)
{
    param = param_;
    tables.resize(param.L);
    rndArray.resize(param.L);
    pcsAll.resize(param.L);
    minsAll.resize(param.L);
    omegasAll.resize(param.L);
    std::mt19937 rng(unsigned(std::time(0)));
    std::uniform_int_distribution<unsigned> usArray(0, param.M - 1);
    for (std::vector<std::vector<unsigned> >::iterator iter = rndArray.begin(); iter != rndArray.end(); ++iter)
    {
        for (unsigned i = 0; i != param.N; ++i)
        {
            iter->push_back(usArray(rng));
        }
    }
}
template<typename DATATYPE>
void lshbox::shLsh<DATATYPE>::train(Matrix<DATATYPE> &data)
{
    int npca = param.N;
    std::mt19937 rng(unsigned(std::time(0)));
    std::uniform_int_distribution<unsigned> usBits(0, data.getSize() - 1);
    for (unsigned k = 0; k != param.L; ++k)
    {
        std::vector<unsigned> seqs;
        while (seqs.size() != param.S)
        {
            unsigned target = usBits(rng);
            if (std::find(seqs.begin(), seqs.end(), target) == seqs.end())
            {
                seqs.push_back(target);
            }
        }
        std::sort(seqs.begin(), seqs.end());
        Eigen::MatrixXf tmp(param.S, data.getDim());
        for (unsigned i = 0; i != tmp.rows(); ++i)
        {
            std::vector<float> vals(0);
            for (int j = 0; j != data.getDim(); ++j)
            {
                vals.push_back(data[seqs[i]][j]);
            }
            tmp.row(i) = Eigen::Map<Eigen::VectorXf>(&vals[0], data.getDim());
        }
        Eigen::MatrixXf centered = tmp.rowwise() - tmp.colwise().mean();
        Eigen::MatrixXf cov = (centered.adjoint() * centered) / float(tmp.rows() - 1);
        Eigen::SelfAdjointEigenSolver<Eigen::MatrixXf> eig(cov);
        Eigen::MatrixXf mat_c = tmp * eig.eigenvectors().rightCols(npca);
        minsAll[k].resize(npca);
        std::vector<double> maxs(npca);
        std::vector<double> omega0(npca);
        double maxR = 0;
        for (unsigned i = 0; i != omega0.size(); ++i)
        {
            minsAll[k][i] = mat_c.colwise().minCoeff()(i);
            maxs[i] = mat_c.colwise().maxCoeff()(i);
            omega0[i] = M_PI / (maxs[i] - minsAll[k][i]);
            if ((maxs[i] - minsAll[k][i]) > maxR)
            {
                maxR = maxs[i] - minsAll[k][i];
            }
        }
        std::vector<double> maxMode(npca);
        int sum = 0;
        for (unsigned i = 0; i != maxMode.size(); ++i)
        {
            maxMode[i] = std::ceil((npca + 1) * (maxs[i] - minsAll[k][i]) / maxR);
            sum += int(maxMode[i]);
        }
        int nModes = sum - npca + 1;
        std::vector<float> modes_in(nModes, 1);
        std::vector<std::vector<float> > modes(npca, modes_in);
        int m = 1;
        for (unsigned i = 0; i != modes.size(); ++i)
        {
            for (unsigned j = 0; j != maxMode[i] - 1; ++j)
            {
                modes[i][m + j] = float(j + 2);
            }
            m = m + int(maxMode[i]) - 1;
        }
        std::vector<std::vector<float> > omegas(npca);
        for (unsigned i = 0; i != omegas.size(); ++i)
        {
            omegas[i].resize(nModes);
            for (unsigned j = 0; j != omegas[i].size(); ++j)
            {
                omegas[i][j] = float(modes[i][j] * omega0[i]);
            }
        }
        std::vector<std::pair<unsigned, float> > eigVal(nModes);
        for (unsigned i = 0; i != eigVal.size(); ++i)
        {
            float sum = 0;
            for (unsigned j = 0; j != omegas.size(); ++j)
            {
                sum += omegas[j][i] * omegas[j][i];
            }
            eigVal[i] = std::make_pair(i, sum);
        }
        std::sort(eigVal.begin(), eigVal.end(), ascend_sort());
        omegasAll[k].resize(npca);
        for (unsigned i = 0; i != omegasAll[k].size(); ++i)
        {
            omegasAll[k][i].resize(npca);
            for (unsigned j = 0; j != omegasAll[k][i].size(); ++j)
            {
                omegasAll[k][i][j] = omegas[i][eigVal[j + 1].first];
            }
        }
        pcsAll[k].resize(npca);
        for (unsigned i = 0; i != pcsAll[k].size(); ++i)
        {
            pcsAll[k][i].resize(param.D);
            for (unsigned j = 0; j != pcsAll[k][i].size(); ++j)
            {
                pcsAll[k][i][j] = eig.eigenvectors().rightCols(npca).adjoint()(i, j);
            }
        }
    }
}
template<typename DATATYPE>
void lshbox::shLsh<DATATYPE>::hash(Matrix<DATATYPE> &data)
{
    progress_display pd(data.getSize());
    for (unsigned i = 0; i != data.getSize(); ++i)
    {
        insert(i, data[i]);
        ++pd;
    }
}
template<typename DATATYPE>
void lshbox::shLsh<DATATYPE>::insert(unsigned key, const DATATYPE *domin)
{
    for (unsigned k = 0; k != param.L; ++k)
    {
        unsigned hashVal = getHashVal(k, domin);
        tables[k][hashVal].push_back(key);
    }
}
template<typename DATATYPE>
template<typename SCANNER>
void lshbox::shLsh<DATATYPE>::query(const DATATYPE *domin, SCANNER &scanner)
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
unsigned lshbox::shLsh<DATATYPE>::getHashVal(unsigned k, const DATATYPE *domin)
{
    unsigned sum = 0;
    std::vector<float> domin_pc(pcsAll[k].size());
    for (unsigned i = 0; i != domin_pc.size(); ++i)
    {
        for (unsigned j = 0; j != pcsAll[k][i].size(); ++j)
        {
            domin_pc[i] += domin[j] * pcsAll[k][i][j];
        }
        domin_pc[i] -= float(minsAll[k][i]);
    }
    for (unsigned i = 0; i != domin_pc.size(); ++i)
    {
        float product = 1;
        for (unsigned j = 0; j != omegasAll[k][i].size(); ++j)
        {
            product *= float(std::sin(domin_pc[j] * omegasAll[k][i][j] + M_PI / 2));
        }
        if (product > 0)
        {
            sum += rndArray[k][i];
        }
    }
    unsigned hashVal = sum % param.M;
    return hashVal;
}
template<typename DATATYPE>
void lshbox::shLsh<DATATYPE>::load(const std::string &file)
{
    std::ifstream in(file, std::ios::binary);
    in.read((char *)&param.M, sizeof(unsigned));
    in.read((char *)&param.L, sizeof(unsigned));
    in.read((char *)&param.D, sizeof(unsigned));
    in.read((char *)&param.N, sizeof(unsigned));
    in.read((char *)&param.S, sizeof(unsigned));
    tables.resize(param.L);
    rndArray.resize(param.L);
    minsAll.resize(param.L);
    pcsAll.resize(param.L);
    omegasAll.resize(param.L);
    for (unsigned i = 0; i != param.L; ++i)
    {
        rndArray[i].resize(param.N);
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
        minsAll[i].resize(param.N);
        pcsAll[i].resize(param.N);
        omegasAll[i].resize(param.N);
        in.read((char *)&minsAll[i][0], sizeof(double) * param.N);
        for (unsigned j = 0; j != param.N; ++j)
        {
            pcsAll[i][j].resize(param.D);
            omegasAll[i][j].resize(param.N);
            in.read((char *)&pcsAll[i][j][0], sizeof(float) * param.D);
            in.read((char *)&omegasAll[i][j][0], sizeof(float) * param.N);
        }
    }
    in.close();
}
template<typename DATATYPE>
void lshbox::shLsh<DATATYPE>::save(const std::string &file)
{
    std::ofstream out(file, std::ios::binary);
    out.write((char *)&param.M, sizeof(unsigned));
    out.write((char *)&param.L, sizeof(unsigned));
    out.write((char *)&param.D, sizeof(unsigned));
    out.write((char *)&param.N, sizeof(unsigned));
    out.write((char *)&param.S, sizeof(unsigned));
    for (int i = 0; i != param.L; ++i)
    {
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
        out.write((char *)&minsAll[i][0], sizeof(double) * param.N);
        for (unsigned j = 0; j != param.N; ++j)
        {
            out.write((char *)&pcsAll[i][j][0], sizeof(float) * param.D);
            out.write((char *)&omegasAll[i][j][0], sizeof(float) * param.N);
        }
    }
    out.close();
}