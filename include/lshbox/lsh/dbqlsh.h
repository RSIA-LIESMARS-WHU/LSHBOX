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
/// @date 2015.5.18
//////////////////////////////////////////////////////////////////////////////

/**
 * @file dbqlsh.h
 *
 * @brief Locality-Sensitive Hashing Scheme Based on Double-Bit Quantization.
 */
#pragma once
#include <utility>
#include <map>
#include <vector>
#include <random>
#include <iostream>
#include <functional>
#include <eigen/Eigen/Dense>
namespace lshbox
{
/**
 * Locality-Sensitive Hashing Scheme Based on Double-Bit Quantization.
 *
 *
 * For more information on Double-Bit Quantization based LSH, see the following reference.
 *
 *     Kong W, Li W. Double-Bit Quantization for Hashing. In AAAI, 2012.
 *
 *     Gong Y, Lazebnik S, Gordo A, et al. Iterative quantization: A procrustean
 *     approach to learning binary codes for large-scale image retrieval[J].
 *     Pattern Analysis and Machine Intelligence, IEEE Transactions on, 2013,
 *     35(12): 2916-2929.
 */
template<typename DATATYPE = float>
class dbqLsh
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
        /// Number of projection dimensions, corresponding to 2*N binary code bytes for each vector
        unsigned N;
        /// Training iterations
        unsigned I;
    };
    dbqLsh() {}
    dbqLsh(const Parameter &param_)
    {
        reset(param_);
    }
    ~dbqLsh() {}
    /**
     * Reset the parameter setting
     *
     * @param param_ A instance of itqLsh<DATATYPE>::Parametor, which contains
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
     * Allocating the thresholding-position for each projection dimension
     */
    void Thresholds();
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
    Eigen::MatrixXf prjColMean;
    std::vector<std::vector<std::vector<std::pair<float, unsigned> > > > prjArray;
    std::vector<std::vector<std::vector<std::vector<std::pair<float, unsigned> > > > > S;
    Eigen::MatrixXf A;
    Eigen::MatrixXf B;
    Eigen::MatrixXf X;
};
}

// ------------------------- implementation -------------------------
template<typename DATATYPE>
void lshbox::dbqLsh<DATATYPE>::reset(const Parameter &param_)
{
    param = param_;
    tables.resize(param.L);
    rndArray.resize(param.L);
    pcsAll.resize(param.L);
    omegasAll.resize(param.L);
    prjArray.resize(param.L);
    A.resize(param.L, param.N);
    B.resize(param.L, param.N);
    S.resize(param.L);
    prjColMean.resize(param.L, param.N);
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
void lshbox::dbqLsh<DATATYPE>::train(Matrix<DATATYPE> &data)
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
    std::cout << "Data Projectoin ..." << std::endl;
    DataProjectoin();
    std::cout << "Thresholding ..." << std::endl;
    Thresholds();
    std::cout << "Bits Allocating ..." << std::endl;
    progress_display pd(param.L);
    for (unsigned k = 0; k != param.L; ++k)
    {
        BitsAllocation(k);
        ++pd;
    }
}
template<typename DATATYPE>
void lshbox::dbqLsh<DATATYPE>::DataProjectoin()
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
        prjColMean.row(k) = prjMat.colwise().mean();
        Eigen::MatrixXf prjCentr = prjMat.rowwise() - prjMat.colwise().mean();
        prjArray[k].resize(param.N);
        S[k].resize(param.N);
        for (unsigned i = 0; i != prjCentr.cols(); ++i)
        {
            prjArray[k][i].resize(X.rows());
            S[k][i].resize(3);
            for (unsigned j = 0; j != prjCentr.rows(); ++j)
            {
                std::pair<float, unsigned> dot(prjCentr(j, i), j);
                prjArray[k][i].push_back(dot);
                if (prjCentr(j, i) <= 0)
                {
                    S[k][i][0].push_back(dot);
                }
                else
                {
                    S[k][i][2].push_back(dot);
                }
            }
            std::sort(prjArray[k][i].begin(), prjArray[k][i].end());
            std::sort(S[k][i][0].begin(), S[k][i][0].end());
            std::sort(S[k][i][2].begin(), S[k][i][2].end());
        }
    }
}
template<typename DATATYPE>
void lshbox::dbqLsh<DATATYPE>::Thresholds()
{
    for (unsigned k = 0; k != param.L; ++k)
    {
        for (unsigned i = 0; i != param.N; i++)
        {
            float Fmax = 0;
            float sums0 = 0, sums1 = 0, sums2 = 0, sum = 0;

            for (std::vector<std::pair<float, unsigned> >::iterator it0 = S[k][i][0].begin(); it0 != S[k][i][0].end(); ++it0)
            {
                sums0 += it0->first;
            }
            for (std::vector<std::pair<float, unsigned> >::iterator it1 = S[k][i][1].begin(); it1 != S[k][i][1].end(); ++it1)
            {
                sums1 += it1->first;
            }
            for (std::vector<std::pair<float, unsigned> >::iterator it2 = S[k][i][2].begin(); it2 != S[k][i][2].end(); ++it2)
            {
                sums2 += it2->first;
            }

            std::vector<std::pair<float, unsigned> >::size_type a = S[k][i][0].size(), b = S[k][i][2].size();
            Fmax = sums0 * sums0 / a + sums2 * sums2 / b;
            std::vector<std::pair<float, unsigned> >::reverse_iterator iter0 = S[k][i][0].rbegin();
            std::vector<std::pair<float, unsigned> >::iterator iter2 = S[k][i][2].begin();
            for (; iter0 != S[k][i][0].rend() || iter2 != S[k][i][2].end();)
            {
                if (sums1 > 0)
                {
                    sums1 += (iter0->first);
                    sums0 -= (iter0->first);
                    ++iter0;
                    --a;
                }
                else
                {
                    sums1 += (iter2->first);
                    sums2 -= (iter2->first);
                    ++iter2;
                    --b;
                }
                if (a > 0 && b > 0)
                {sum = sums0 * sums0 / a + sums2 * sums2 / b;}
                else if (a == 0 && b > 0)
                {sum = sums2 * sums2 / b;}
                else if (a > 0 && b == 0)
                {sum = sums0 * sums0 / a;}
                else {sum = 0;}
                if (sum > Fmax)
                {
                    Fmax = sum;
                    A(k, i) = iter0->first;
                    B(k, i) = iter2->first;
                }
            }
        }
    }
    std::cout << std::endl;
    std::cout << "The Left Thresholding-Matrix: A(" << param.L << "," << param.N << ")" << std::endl << A << std::endl << std::endl;
    std::cout << "The Right Thresholding-Matrix: B(" << param.L << "," << param.N << ")" << std::endl << B << std::endl << std::endl;
}
template<typename DATATYPE>
void lshbox::dbqLsh<DATATYPE>::BitsAllocation(unsigned key)
{
    std::vector<unsigned > sum(X.rows(), 0);
    for (unsigned i = 0; i != param.N; ++i)
    {
        for (std::vector<std::pair<float, unsigned> >::iterator iter = prjArray[key][i].begin(); iter != prjArray[key][i].end(); ++iter)
        {
            if ((iter->first) <= A(key, i))
            {
                sum[iter->second] += rndArray[key][2 * i + 1];
            }
            if ((iter->first) >= B(key, i))
            {
                sum[iter->second] += rndArray[key][2 * i];
            }
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
void lshbox::dbqLsh<DATATYPE>::query(const DATATYPE *domin, SCANNER &scanner)
{
    scanner.reset(domin);
    for (unsigned k = 0; k != param.L; ++k)
    {
        unsigned sum = 0;
        std::vector<float> domin_pc(param.N);
        for (unsigned i = 0; i != domin_pc.size(); ++i)
        {
            for (unsigned j = 0; j != param.D; ++j)
            {
                domin_pc[i] += domin[j] * pcsAll[k][i][j];
            }
        }
        for (unsigned i = 0; i != domin_pc.size(); ++i)
        {
            float product = 0;
            for (unsigned j = 0; j != omegasAll[k][i].size(); ++j)
            {
                product += float(domin_pc[j] * omegasAll[k][i][j]);
            }
            product -= prjColMean(k, i);
            if (product <= A(k, i))
            {
                sum += rndArray[k][2 * i + 1];
            }

            if (product >= B(k, i))
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
void lshbox::dbqLsh<DATATYPE>::load(const std::string &file)
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
    A.resize(param.L, param.N);
    B.resize(param.L, param.N);
    prjColMean.resize(param.L, param.N);
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
    in.read((char *)&A(0, 0), sizeof(float) * param.L * param.N);
    in.read((char *)&B(0, 0), sizeof(float) * param.L * param.N);
    in.read((char *)&prjColMean(0, 0), sizeof(float)* param.L * param.N);
    in.close();
}
template<typename DATATYPE>
void lshbox::dbqLsh<DATATYPE>::save(const std::string &file)
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
    out.write((char *)&A(0, 0), sizeof(float) * param.L * param.N);
    out.write((char *)&B(0, 0), sizeof(float) * param.L * param.N);
    out.write((char *)&prjColMean(0, 0), sizeof(float) * param.L * param.N);
    out.close();
}