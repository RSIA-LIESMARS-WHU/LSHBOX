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
 * @file topk.h
 *
 * @brief Top-K data structures.
 */
#pragma once
#include <vector>
#include <iostream>
#include <algorithm>
namespace lshbox
{
/**
 * Top-K heap.
 *
 * At this point topk should contain the nearest k query keys and distances.
 */
class Topk
{
private:
    unsigned K;
    float R;
    std::vector<std::pair<unsigned, float> > heapv;
public:
    Topk(): K(0), R(std::numeric_limits<float>::max()) {};
    ~Topk() {};
    /**
     * Reset the heap
     *
     * @param k The number of nearest query keys
     * @param r The initialization distance
     */
    void reset(unsigned k, float r = std::numeric_limits<float>::max())
    {
        R = r;
        K = k;
        heapv.resize(K);
        for (std::vector<std::pair<unsigned, float> >::iterator it = heapv.begin();
                it != heapv.end(); ++it)
        {
            *it = std::make_pair(0, R);
        }
    }
    /**
     * Get the std::vector<std::pair<unsigned, float> > instance which contains
     * the nearest keys and distances.
     */
    const std::vector<std::pair<unsigned, float> > &getTopk() const
    {
        return heapv;
    }
    /**
     * Get the std::vector<std::pair<unsigned, float> > instance which contains
     * the nearest keys and distances.
     */
    std::vector<std::pair<unsigned, float> > &getTopk()
    {
        return heapv;
    }
    /**
     * Add an element to the heap and then update it.
     *
     * @param key  The key of the element
     * @param dist The distance of the element
     */
    void push(unsigned key, float dist)
    {
        if (dist < heapv[K - 1].second)
        {
            heapv.pop_back();
            heapv.push_back(std::make_pair(key, dist));
            std::sort(heapv.begin(), heapv.end(), ascend);
        }
    }
    /**
     * Get the most nearest distance in the heap.
     */
    float getMin() const
    {
        return heapv[K - 1].second;
    }
    /**
     * Calculate the recall vale with another heap.
     */
    float recall(const Topk &topk) const
    {
        unsigned matched = 0;
        for (auto i = heapv.begin(); i != heapv.end(); ++i)
        {
            for (auto j = topk.getTopk().begin(); j != topk.getTopk().end(); ++j)
            {
                if (i->first == j ->first)
                {
                    matched++;
                    break;
                }
            }
        }
        return float(matched + 1) / float(K + 1);
    }
};
/**
 * Top-K scanner.
 *
 * Scans keys for top-K query, this is the object passed into the LSH query interface.
 */
template <typename ACCESSOR>
class Scanner
{
public:
    typedef typename ACCESSOR::Value Value;
    typedef typename ACCESSOR::DATATYPE DATATYPE;
    /**
     * Constructor for this class.
     *
     * @param accessor The scanner use accessor to retrieva values from keys.
     * @param metric The distance metric.
     * @param K Value used to reset internal Topk class.
     * @param R Value used to reset internal Topk class.
     */
    Scanner(
        const ACCESSOR &accessor,
        const Metric<DATATYPE> &metric,
        unsigned K,
        float R = std::numeric_limits<float>::max()
    ): accessor_(accessor), metric_(metric), K_(K), R_(R), cnt_(0) {}
    /**
      * Reset the query, this function should be invoked before each query.
      */
    void reset(Value query)
    {
        query_ = query;
        accessor_.reset();
        topk_.reset(K_, R_);
        cnt_ = 0;
    }
    /**
     * Number of points scanned for the current query.
     */
    unsigned cnt() const
    {
        return cnt_;
    }
    /**
     * TopK results.
     */
    const Topk &topk() const
    {
        return topk_;
    }
    /**
     * TopK results.
     */
    Topk &topk()
    {
        return topk_;
    }
    /**
     * Update the current query by scanning key, this is normally invoked by the LSH
     * index structure.
     */
    void operator () (unsigned key)
    {
        if (accessor_.mark(key))
        {
            ++cnt_;
            topk_.push(key, metric_.dist(query_, accessor_(key)));
        }
    }
private:
    ACCESSOR accessor_;
    Metric<DATATYPE> metric_;
    Topk topk_;
    Value query_;
    unsigned K_;
    float R_;
    unsigned cnt_;
};
}