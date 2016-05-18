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
 * @file basis.h
 *
 * @brief A set of basic tools.
 */
#pragma once
#include <string>
#include <iostream>
#include <time.h>
namespace lshbox
{
#define CAUCHY   1
#define GAUSSIAN 2
#define M_PI 3.14159265358979323846
/**
 * Sort std::vector<std::pair<unsigned, float> > by the second value.
 */
struct ascend_sort
{
    bool operator()(const std::pair<unsigned, float> &lhs, const std::pair<unsigned, float> &rhs)
    {
        return lhs.second < rhs.second;
    }
};
/**
 * Displays an appropriate indication of progress at an appropriate place
 * in an appropriate form.
 *
 * If you are familiar with the Boost library, you should be very familiar
 * with this class.
 */
class progress_display
{
public:
    explicit progress_display(
        unsigned long expected_count,
        std::ostream &os = std::cout,
        const std::string &s1 = "\n",
        const std::string &s2 = "",
        const std::string &s3 = "")
        : m_os(os), m_s1(s1), m_s2(s2), m_s3(s3)
    {
        restart(expected_count);
    }
    void restart(unsigned long expected_count)
    {
        _count = _next_tic_count = _tic = 0;
        _expected_count = expected_count;
        m_os << m_s1 << "0%   10   20   30   40   50   60   70   80   90   100%\n"
             << m_s2 << "|----|----|----|----|----|----|----|----|----|----|"
             << std::endl
             << m_s3;
        if (!_expected_count)
        {
            _expected_count = 1;
        }
    }
    unsigned long operator += (unsigned long increment)
    {
        if ((_count += increment) >= _next_tic_count)
        {
            display_tic();
        }
        return _count;
    }
    unsigned long operator ++ ()
    {
        return operator += ( 1 );
    }
    unsigned long count() const
    {
        return _count;
    }
    unsigned long expected_count() const
    {
        return _expected_count;
    }
private:
    std::ostream &m_os;
    const std::string m_s1;
    const std::string m_s2;
    const std::string m_s3;
    unsigned long _count, _expected_count, _next_tic_count;
    unsigned _tic;
    void display_tic()
    {
        unsigned tics_needed = unsigned((double(_count) / _expected_count) * 50.0);
        do
        {
            m_os << '*' << std::flush;
        }
        while (++_tic < tics_needed);
        _next_tic_count = unsigned((_tic / 50.0) * _expected_count);
        if (_count == _expected_count)
        {
            if ( _tic < 51 ) m_os << '*';
            m_os << std::endl;
        }
    }
};
/**
 * A timer object measures elapsed time, and it is very similar to boost::timer.
 */
class timer
{
public:
    timer(): time(double(clock())) {};
    ~timer() {};
    /**
     * Restart the timer.
     */
    void restart()
    {
        time = double(clock());
    }
    /**
     * Measures elapsed time.
     *
     * @return The elapsed time
     */
    double elapsed()
    {
        return (double(clock()) - time) / CLOCKS_PER_SEC;
    }
private:
    double time;
};
}