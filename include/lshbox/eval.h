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
 * @file eval.h
 *
 * @brief A set of classes for evaluation.
 */
#pragma once
#include <cmath>
#include <ctime>
#include <limits>
#include <vector>
#include <random>
#include <fstream>
#include <algorithm>
#include <functional>
namespace lshbox
{
/**
 * Use for access a benchmark file.
 *
 * We assume the feature vectors in the benchmark database are numbered from 0
 * to N. We sample Q queries as test examples and run K-NN search against the
 * database with linear scan.  The results are saved in a benchmark file for
 * evaluation purpose. A benchmark file is made up of Q lines, each line
 * represents a test query and is of the following format:
 *
 * [query ID] [K] [1st NN's ID] [distance] [2nd NN's ID] [distance] ... [Kth NN's ID] [distance]
 *
 * For all queries in the benchmark file, the K value should be the same.
 *
 * Because the query points are also sampled from the database, they should be
 * excluded from scanning when running this particular query.
 */
class Benchmark
{
public:
    Benchmark(): Q_(0), K_(0) {}
    /**
     * Change the query number for brenchmark and the result number for each query.
     *
     * @param Q The new query number for brenchmark
     * @param K The new result number for each query
     */
    void resize(unsigned Q, unsigned K = 0)
    {
        Q_ = Q;
        K_ = K;
        queries_.resize(Q);
        topks_.resize(Q);
        for (unsigned i = 0; i != topks_.size(); ++i)
        {
            topks_[i].reset(K);
        }
    }
    /**
     * Random initialization.
     *
     * @param Q     The query number for brenchmark
     * @param K     The result number for each query
     * @param maxID The number of vectors in the search library
     * @param seed  Seed some value for random to generate different query samples
     */
    void init(unsigned Q, unsigned K, unsigned maxID, unsigned seed = 0)
    {
        resize(Q, K);
        std::mt19937 rng(unsigned(std::time(0)));
        std::uniform_int_distribution<unsigned> ui(0, maxID - 1);
        if (seed != 0)
        {
            rng.seed(int(seed));
        }
        for (unsigned i = 0; i != queries_.size(); ++i)
        {
            while (true)
            {
                queries_[i] = ui(rng);
                unsigned j;
                for (j = 0; j != i; j++)
                {
                    if (queries_[i] == queries_[j])
                    {
                        break;
                    }
                }
                if (j >= i)
                {
                    break;
                }
            }
        }
    }
    ~Benchmark() {}
    /**
     * Save the benchmark to byte stream.
     */
    void save(std::ostream &os) const
    {
        os << Q_ << "\t" << K_ << std::endl;
        for (unsigned i = 0; i != Q_; ++i)
        {
            os << queries_[i] << "\t";
            for (unsigned j = 0; j != topks_[i].getTopk().size(); j++)
            {
                os << "\t" <<  topks_[i].getTopk()[j].second;
                os << "\t" <<  topks_[i].getTopk()[j].first;
            }
            os << std::endl;
        }
    }
    /**
     * Save the benchmark as a text file.
     */
    void save(const std::string &path) const
    {
        std::ofstream os(path.c_str());
        save(os);
        os.close();
    }
    /**
     * Load the benchmark from byte stream.
     */
    void load(std::istream &is)
    {
        queries_.clear();
        topks_.clear();
        is >> Q_ >> K_;
        resize(Q_, K_);
        for (unsigned i = 0; i != Q_; ++i)
        {
            unsigned q;
            is >> q;
            queries_[i] = q;
            for (unsigned j = 0; j != K_; ++j)
            {
                unsigned key;
                float dist;
                is >> key;
                is >> dist;
                topks_[i].push(key, dist);
            }
            topks_[i].genTopk();
        }
    }
    /**
     * Load the benchmark from a text file.
     */
    void load(const std::string &path)
    {
        std::ifstream is(path.c_str());
        load(is);
        is.close();
    }
    /**
     * Get the query number for brenchmark.
     */
    unsigned getQ() const
    {
        return Q_;
    }
    /**
     * Get the result number for each query.
     */
    unsigned getK() const
    {
        return K_;
    }
    /**
     * Get the ID of the nth query.
     */
    unsigned getQuery(unsigned n) const
    {
        return queries_[n];
    }
    /**
     * Get the nearest neighbors of the nth query.
     */
    const Topk &getAnswer(unsigned n) const
    {
        return topks_[n];
    }
    /**
     * Get the KNNs for modification.
     */
    Topk &getAnswer(unsigned n)
    {
        return topks_[n];
    }
private:
    unsigned Q_;
    unsigned K_;
    std::vector<unsigned> queries_;
    std::vector<Topk> topks_;
};
/**
 * Use for basic statistics, and the interface is self-evident.
 *
 * Usage:
 *
 * @code
 * Stat stat;
 *
 * stat << 1.0 << 2.0 << 3.0;
 *
 * Stat stat2;
 * stat2 << 3.0 << 5.0 << 6.0;
 *
 * stat.merge(stat2);
 *
 * stat.getCount();
 * stat.getSum();
 * stat.getMax();
 * stat.getMin();
 * stat.getStd();
 * @endcode
 *
 */
class Stat
{
    int count;
    float sum;
    float sum2;
    float min;
    float max;
public:
    Stat(): count(0), sum(0), sum2(0), min(std::numeric_limits<float>::max()), max(-std::numeric_limits<float>::max()) {}
    ~Stat() {}
    void reset()
    {
        count = 0;
        sum = sum2 = 0;
        min = std::numeric_limits<float>::max();
        max = -std::numeric_limits<float>::max();
    }
    void append(float r)
    {
        count++;
        sum += r;
        sum2 += r * r;
        if (r > max) max = r;
        if (r < min) min = r;
    }
    Stat &operator << (float r)
    {
        append(r);
        return *this;
    }
    int getCount() const
    {
        return count;
    }
    float getSum() const
    {
        return sum;
    }
    float getAvg() const
    {
        return sum / count;
    }
    float getMax() const
    {
        return max;
    }
    float getMin() const
    {
        return min;
    }
    float getStd() const
    {
        if (count > 1)
        {
            return std::sqrt((sum2 - (sum / count) * sum) / (count - 1));
        }
        else
        {
            return 0;
        }
    }
    void merge(const Stat &stat)
    {
        count += stat.count;
        sum += stat.sum;
        sum2 += stat.sum2;
        if (stat.min < min)
        {
            min = stat.min;
        }
        if (stat.max > max)
        {
            max = stat.max;
        }
    }
};
}