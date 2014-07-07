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
 * @file metric.h
 *
 * @brief Common distance measures.
 */
#pragma once
#include <cmath>
namespace lshbox
{
#define L1_DIST 1
#define L2_DIST 2
/**
 * The calculation of square.
 */
template <typename DATATYPE>
DATATYPE sqr(const DATATYPE &x)
{
    return x * x;
}
/**
 * Use for common distance functions.
 */
template <typename DATATYPE>
class Metric
{
    unsigned type_;
    unsigned dim_;
public:
    /**
     * Constructor for this class.
     *
     * @param dim  Dimension of each vector
     * @param type The way to measure the distance, you can choose 1(L1_DIST) or 2(L2_DIST)
     */
    Metric(unsigned dim, unsigned type): dim_(dim), type_(type) {}
    ~Metric() {}
    /**
     * Get the dimension of the vectors
     */
    unsigned dim() const
    {
        return dim_;
    }
    /**
     * measure the distance.
     *
     * @param  vec1 The first vector
     * @param  vec2 The second vector
     * @return      The distance
     */
    float dist(const DATATYPE *vec1, const DATATYPE *vec2) const
    {
        float dist_ = 0.0;
        switch (type_)
        {
        case L1_DIST:
        {
            for (unsigned i = 0; i != dim_; ++i)
            {
                dist_ += float(std::abs(vec1[i] * 1.0 - vec2[i]));
            }
            return dist_;
        }
        case L2_DIST:
        {
            for (unsigned i = 0; i != dim_; ++i)
            {
                dist_ += sqr(vec1[i] - vec2[i]);
            }
            return std::sqrt(dist_);
        }
        default:
        {
            return -1;
        }
        }
    }
};
}