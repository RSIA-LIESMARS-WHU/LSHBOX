//////////////////////////////////////////////////////////////////////////////
/// Copyright (C) 2015 Yang Long <20288ly@sina.cn> & Gefu Tang <tanggefu@gmail.com> .
/// All Rights Reserved.
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
/// @author Yang Long, Gefu Tang & Zhifeng Xiao
/// @date 2015.6.30
//////////////////////////////////////////////////////////////////////////////

/**
 * @file kdbqlsh.h
 *
 * @brief Locality-Sensitive Hashing Scheme Based on K-means Double-Bit Quantization.
 */
#pragma once
#include <utility>
#include <map>
#include <vector>
#include <random>
#include <iostream>
#include <functional>
#include <eigen/Eigen/Dense>
#include <cmath>
#include <cstring>
namespace lshbox
{
/**
 * Locality-Sensitive Hashing Scheme Based on K-means Double-Bit Quantization.
 *
 *
 * For more information on Double-Bit Quantization based LSH, see the following reference.
 *
 *     Zhu, H. (2014). K-means based double-bit quantization for hashing.
 *     Computational Intelligence for Multimedia, Signal and Vision Processing (CIMSIVP),
 *     2014 IEEE Symposium on (pp.1-5). IEEE.
 *
 */
template<typename DATATYPE = float>
class kdbqLsh
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
        /// Number of projection dimensions,corresponding to 2*N binary code bytes for each vector
        unsigned N;
        /// Training iterations
        unsigned I;
    };
    kdbqLsh() {}
    kdbqLsh(const Parameter &param_)
    {
        reset(param_);
    }
    ~kdbqLsh() {}
    /**
     * Reset the parameter setting
     *
     * @param param_ A instance of dbqLsh<DATATYPE>::Parametor, which contains
     * the necessary parameters
     */
    void reset(const Parameter &param_);
    /**
     * Train the data to get several groups of suitable vector for index.
     *
     * @param data A instance of Matrix<DATATYPE>, most of the time, is the search library.
     */
    void train(Matrix<DATATYPE> &data);
    /**
     * Projection for the source vector
     */
    void DataProjectoin();
    /**
     * K-means clustering
     */
    void Cluster(unsigned k, Eigen::MatrixXf prjMatrix);
    /**
     * Insert a vector to the index.
     *
     * @param key Number of hash tables
     */
    void BitsAllocation(unsigned key);
    /**
     * Query the approximate nearest neighborhoods.
     *
     * @param domin   The pointer to the vector
     * @param scanner Top-K scanner, use for scan the approximate nearest neighborhoods
     */
    template<typename SCANNER>
    void query(const DATATYPE *domin, SCANNER &scanner);
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
    std::vector<std::vector<std::vector<float> > >  pcsAll;
    std::vector<std::vector<std::vector<float> > >  omegasAll;
    std::vector<std::vector<unsigned> > rndArray;
    std::vector<std::map<unsigned, std::vector<unsigned> > > tables;
    Eigen::VectorXf prjColMean;
    std::vector<std::vector<std::vector<std::pair<float, unsigned> > > > prjArray;
    std::vector<std::vector<std::vector<std::vector<std::pair<float, unsigned> > > > > S;
    Eigen::MatrixXf u0, u1, u2;
    Eigen::MatrixXf X;
};
}

// ------------------------- implementation -------------------------
template<typename DATATYPE>
void lshbox::kdbqLsh<DATATYPE>::reset(const Parameter &param_)
{
    param = param_;
    tables.resize(param.L);
    rndArray.resize(param.L);
    pcsAll.resize(param.L);
    omegasAll.resize(param.L);
    prjArray.resize(param.L);
    u0.resize(param.L, param.N);
    u1.resize(param.L, param.N);
    u2.resize(param.L, param.N);
    S.resize(param.L);
    std::mt19937 rng(unsigned(std::time(0)));
    std::uniform_int_distribution<unsigned> usArray(0, param.M - 1);
    for (std::vector<std::vector<unsigned> >::iterator iter = rndArray.begin(); iter != rndArray.end(); ++iter)
    {
        for (unsigned i = 0; i != 2 * param.N; ++i)
        {
            iter->push_back(usArray(rng));
        }
    }
}
template<typename DATATYPE>
void lshbox::kdbqLsh<DATATYPE>::train(Matrix<DATATYPE> &data)
{
    int npca = param.N;
    std::mt19937 rng(unsigned(std::time(0)));
    std::normal_distribution<float> nd;
    X.resize(data.getSize(), data.getDim());
    for (unsigned i = 0; i != X.rows(); ++i)
    {
        std::vector<float> vals(0);
        for (int j = 0; j != data.getDim(); ++j)
        {
            vals.push_back(data[i][j]);
        }
        X.row(i) = Eigen::Map<Eigen::VectorXf>(&vals[0], data.getDim());
    }
    Eigen::MatrixXf cov = X.transpose() * X;
    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXf> eig(cov);
    Eigen::MatrixXf mat_c = X * eig.eigenvectors().rightCols(npca);

    std::cout << std::endl;
    for (unsigned k = 0; k != param.L; ++k)
    {
        std::cout << "Computing the " << k + 1 << "th Hash Table Rotation-Matrix ..." << std::endl;
        Eigen::MatrixXf R(npca, npca);
        for (unsigned i = 0; i != R.rows(); ++i)
        {
            for (unsigned j = 0; j != R.cols(); ++j)
            {
                R(i, j) = nd(rng);
            }
        }
        Eigen::JacobiSVD<Eigen::MatrixXf> svd(R, Eigen::ComputeThinU | Eigen::ComputeThinV);
        R = svd.matrixU();
        for (unsigned iter = 0; iter != param.I; ++iter)
        {
            Eigen::MatrixXf Z = mat_c * R;
            Eigen::MatrixXf UX(Z.rows(), Z.cols());
            for (unsigned i = 0; i != Z.rows(); ++i)
            {
                for (unsigned j = 0; j != Z.cols(); ++j)
                {
                    if (Z(i, j) > 0)
                    {
                        UX(i, j) = 1;
                    }
                    else
                    {
                        UX(i, j) = -1;
                    }
                }
            }
            Eigen::JacobiSVD<Eigen::MatrixXf> svd_tmp(UX.transpose () * mat_c, Eigen::ComputeThinU | Eigen::ComputeThinV);
            R = svd_tmp.matrixV() * svd_tmp.matrixU().transpose ();
        }
        pcsAll[k].resize(npca);
        for (unsigned i = 0; i != pcsAll[k].size(); ++i)
        {
            pcsAll[k][i].resize(param.D);
            for (unsigned j = 0; j != pcsAll[k][i].size(); ++j)
            {
                pcsAll[k][i][j] = eig.eigenvectors().rightCols(npca).transpose ()(i, j);
            }
        }
        omegasAll[k].resize(npca);
        for (unsigned i = 0; i != omegasAll[k].size(); ++i)
        {
            omegasAll[k][i].resize(npca);
            for (unsigned j = 0; j != omegasAll[k][i].size(); ++j)
            {
                omegasAll[k][i][j] = R.transpose()(i, j);
            }
        }
    }
    std::cout << std::endl;
    std::cout << "Data Projection ..." << std::endl;
    DataProjectoin();
    std::cout << "Bits Allocating ..." << std::endl;
    progress_display pd(param.L);
    for (unsigned k = 0; k != param.L; ++k)
    {
        BitsAllocation(k);
        ++pd;
    }
}
template<typename DATATYPE>
void lshbox::kdbqLsh<DATATYPE>::DataProjectoin()
{
    for (unsigned k = 0; k != param.L; ++k)
    {
        Eigen::MatrixXf prj(X.rows(), param.N);
        for (unsigned i = 0; i != X.rows(); ++i)
        {
            for (unsigned q = 0; q != param.N; ++q)
            {
                float sum = 0.0;
                for (unsigned j = 0; j != X.cols(); ++j)
                {
                    sum += X(i, j) * pcsAll[k][q][j];
                }
                prj(i, q) = sum;
            }
        }

        Eigen::MatrixXf prjMat(X.rows(), param.N);
        for (unsigned i = 0; i != X.rows(); ++i)
        {
            for (unsigned q = 0; q != omegasAll[k].size(); ++q)
            {
                float sum = 0.0;
                for (unsigned j = 0; j != omegasAll[k].size(); ++j)
                {
                    sum += prj(i, j) * omegasAll[k][q][j];
                }
                prjMat(i, q) = sum;
            }
        }
        Cluster(k, prjMat);
    }
    std::cout << std::endl;
    std::cout << "The First Clustering Center-Matrix: u0(" << param.L << ","
              << param.N << ")" << std::endl << u0 << std::endl << std::endl;
    std::cout << "The Second Clustering Center-Matrix: u1(" << param.L << ","
              << param.N << ")" << std::endl << u1 << std::endl << std::endl;
    std::cout << "The Third Clustering Center-Matrix: u2(" << param.L << ","
              << param.N << ")" << std::endl << u2 << std::endl << std::endl;
}
template<typename DATATYPE>
void lshbox::kdbqLsh<DATATYPE>::Cluster(unsigned k, Eigen::MatrixXf prjMatrix)
{

    S[k].resize(param.N);
    for (unsigned i = 0; i != prjMatrix.cols() ; ++i)
    {
        S[k][i].resize(3);
        u0(k, i) = prjMatrix.row(i).minCoeff();
        u1(k, i) = prjMatrix.row(i).mean();
        u2(k, i) = prjMatrix.row(i).maxCoeff();

        float E[3] = { 0 };
        for (unsigned j = 0; j != prjMatrix.rows(); ++j)
        {
            float value[3];
            value[0] = fabsf(prjMatrix(j, i) - u0(k, i));
            value[1] = fabsf(prjMatrix(j, i) - u1(k, i));
            value[2] = fabsf(prjMatrix(j, i) - u2(k, i));

            float distance = value[0]; unsigned label = 0;
            for (unsigned t = 0; t != 3; ++t)
            {
                if (value[t] < distance)
                {
                    distance = value[t];
                    label = t;
                }
            }
            std::pair<float, unsigned> dot(prjMatrix(j, i), j);
            S[k][i][label].push_back(dot);
            E[label] += pow(value[label], 2);
        }

        float Var = E[0] + E[1] + E[2];
        float minVar = std::numeric_limits<float>::max();
        float sum[3] = { 0 };
        while (Var != minVar)
        {
            minVar = Var;
            for (std::vector<std::pair<float, unsigned > >::iterator it0 = S[k][i][0].begin(); it0 != S[k][i][0].end(); ++it0)
                sum[0] += (it0->first);

            for (std::vector<std::pair<float, unsigned > >::iterator it1 = S[k][i][1].begin(); it1 != S[k][i][1].end(); ++it1)
                sum[1] += (it1->first);

            for (std::vector<std::pair<float, unsigned > >::iterator it2 = S[k][i][2].begin(); it2 != S[k][i][2].end(); ++it2)
                sum[2] += (it2->first);

            u0(k, i) = sum[0] / ((int)S[k][i][0].size());
            u1(k, i) = sum[1] / ((int)S[k][i][1].size());
            u2(k, i) = sum[2] / ((int)S[k][i][2].size());

            S[k][i][0].clear(); S[k][i][1].clear(); S[k][i][2].clear();
            memset(E, 0, sizeof(float) * 3);
            memset(sum, 0, sizeof(float) * 3);
            for (unsigned j = 0; j != prjMatrix.rows(); ++j)
            {
                float val[3];
                val[0] = fabsf(prjMatrix(j, i) - u0(k, i));
                val[1] = fabsf(prjMatrix(j, i) - u1(k, i));
                val[2] = fabsf(prjMatrix(j, i) - u2(k, i));

                float dist = val[0]; unsigned lab = 0;
                for (unsigned t = 0; t != 3; t++)
                {
                    if (val[t] < dist)
                    {
                        dist = val[t];
                        lab = t;
                    }
                }
                std::pair<float, unsigned> dot(prjMatrix(j, i), j);
                S[k][i][lab].push_back(dot);
                E[lab] += pow(val[lab], 2);
            }
            Var = E[0] + E[1] + E[2];
        }
    }
}
template<typename DATATYPE>
void lshbox::kdbqLsh<DATATYPE>::BitsAllocation(unsigned key)
{
    std::vector<unsigned > sum(X.rows(), 0);
    for (unsigned i = 0; i != param.N; ++i)
    {
        for (std::vector<std::pair<float, unsigned > >::iterator it0 = S[key][i][0].begin(); it0 != S[key][i][0].end(); ++it0)
        {
            sum[it0->second] += rndArray[key][2 * i + 1];
        }
        for (std::vector<std::pair<float, unsigned > >::iterator it2 = S[key][i][2].begin(); it2 != S[key][i][2].end(); ++it2)
        {
            sum[it2->second] += rndArray[key][2 * i];
        }
    }

    for (unsigned a = 0; a != X.rows(); ++a)
    {
        unsigned halVal = sum[a] % param.M;
        tables[key][halVal].push_back(a);
    }
}
template<typename DATATYPE>
template<typename SCANNER>
void lshbox::kdbqLsh<DATATYPE>::query(const DATATYPE *domin, SCANNER &scanner)
{
    scanner.reset(domin);
    for (unsigned k = 0; k != param.L; ++k)
    {
        unsigned sum = 0;
        std::vector<float> domin_pc(param.N);
        for (unsigned i = 0; i != domin_pc.size(); ++i)
        {
            domin_pc[i] = 0.0;
            for (unsigned j = 0; j != param.D; ++j)
            {
                domin_pc[i] += domin[j] * pcsAll[k][i][j];
            }
        }

        for (unsigned i = 0; i != domin_pc.size(); ++i)
        {
            float product = 0.0;
            for (unsigned j = 0; j != omegasAll[k][i].size(); ++j)
            {
                product += float(domin_pc[j] * omegasAll[k][i][j]);
            }

            float value[3];
            value[0] = fabsf(product - u0(k, i));
            value[1] = fabsf(product - u1(k, i));
            value[2] = fabsf(product - u2(k, i));

            float min = value[0]; unsigned label;
            for (unsigned t = 0; t != 3; t++)
            {
                if (value[t] < min)
                {
                    min = value[t];
                    label = t;
                }
            }

            if (label == 0)
            {
                sum += rndArray[k][2 * i + 1];
            }
            if (label == 2)
            {
                sum += rndArray[k][2 * i];
            }

        }
        unsigned hashVal = sum % param.M;
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
void lshbox::kdbqLsh<DATATYPE>::load(const std::string &file)
{
    std::ifstream in(file, std::ios::binary);
    in.read((char *)&param.M, sizeof(unsigned));
    in.read((char *)&param.L, sizeof(unsigned));
    in.read((char *)&param.D, sizeof(unsigned));
    in.read((char *)&param.N, sizeof(unsigned));
    tables.resize(param.L);
    rndArray.resize(param.L);
    pcsAll.resize(param.L);
    omegasAll.resize(param.L);
    u0.resize(param.L, param.N);
    u1.resize(param.L, param.N);
    u2.resize(param.L, param.N);
    for (unsigned i = 0; i != param.L; ++i)
    {
        rndArray[i].resize(param.N * 2);
        in.read((char *)&rndArray[i][0], sizeof(unsigned) * param.N * 2);
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
        pcsAll[i].resize(param.N);
        omegasAll[i].resize(param.N);
        for (unsigned j = 0; j != param.N; ++j)
        {
            pcsAll[i][j].resize(param.D);
            omegasAll[i][j].resize(param.N);
            in.read((char *)&pcsAll[i][j][0], sizeof(float) * param.D);
            in.read((char *)&omegasAll[i][j][0], sizeof(float) * param.N);
        }
    }
    in.read((char *)&u0(0, 0), sizeof(float) * param.L * param.N);
    in.read((char *)&u1(0, 0), sizeof(float) * param.L * param.N); 
    in.read((char *)&u2(0, 0), sizeof(float) * param.L * param.N); 
    in.close();
}
template<typename DATATYPE>
void lshbox::kdbqLsh<DATATYPE>::save(const std::string &file)
{
    std::ofstream out(file, std::ios::binary);
    out.write((char *)&param.M, sizeof(unsigned));
    out.write((char *)&param.L, sizeof(unsigned));
    out.write((char *)&param.D, sizeof(unsigned));
    out.write((char *)&param.N, sizeof(unsigned));
    for (int i = 0; i != param.L; ++i)
    {
        out.write((char *)&rndArray[i][0], sizeof(unsigned) * param.N * 2);
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
        for (unsigned j = 0; j != param.N; ++j)
        {
            out.write((char *)&pcsAll[i][j][0], sizeof(float) * param.D);
            out.write((char *)&omegasAll[i][j][0], sizeof(float) * param.N);
        }
    }
    out.write((char *)&u0(0, 0), sizeof(float) * param.L * param.N);
    out.write((char *)&u1(0, 0), sizeof(float) * param.L * param.N); 
    out.write((char *)&u2(0, 0), sizeof(float) * param.L * param.N); 
    out.close();
}