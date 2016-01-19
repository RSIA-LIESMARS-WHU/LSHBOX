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
 * @file create_test_data.cpp
 *
 * @brief Create unsigned test data for rbslsh.
 */
#include <lshbox.h>
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: ./create_test_data data_file" << std::endl;
        return -1;
    }
    std::cout << "CREATE UNSIGNED TEST DATA ..." << std::endl;
    lshbox::timer timer;
    std::mt19937 rng(unsigned(std::time(0)));
    std::ofstream out_u(argv[1], std::ios::binary);
    unsigned bytes_u(sizeof(unsigned)), size_u(1000000), dim_u(10);
    out_u.write((char *)&bytes_u, bytes_u);
    out_u.write((char *)&size_u, bytes_u);
    out_u.write((char *)&dim_u, bytes_u);
    std::uniform_int_distribution<unsigned> ui(0, 4);
    unsigned test1[10] = {1, 3, 2, 0, 1, 4, 4, 1, 3, 4};
    unsigned test2[10] = {1, 2, 3, 0, 0, 4, 3, 1, 3, 3};
    unsigned test3[10] = {0, 3, 3, 0, 0, 4, 4, 1, 4, 4};
    unsigned test4[10] = {0, 2, 3, 0, 0, 4, 4, 0, 3, 4};
    unsigned test5[10] = {3, 0, 0, 4, 3, 0, 1, 2, 1, 0};
    unsigned test6[10] = {3, 0, 0, 3, 2, 0, 1, 3, 0, 0};
    unsigned test7[10] = {3, 0, 0, 3, 3, 1, 1, 2, 0, 0};
    unsigned test8[10] = {4, 0, 1, 4, 3, 0, 1, 2, 0, 0};
    out_u.write((char *)&test1, dim_u * bytes_u);
    out_u.write((char *)&test2, dim_u * bytes_u);
    out_u.write((char *)&test3, dim_u * bytes_u);
    out_u.write((char *)&test4, dim_u * bytes_u);
    out_u.write((char *)&test5, dim_u * bytes_u);
    out_u.write((char *)&test6, dim_u * bytes_u);
    out_u.write((char *)&test7, dim_u * bytes_u);
    out_u.write((char *)&test8, dim_u * bytes_u);
    for (int i = 0; i != size_u * dim_u - 8; ++i)
    {
        unsigned target = ui(rng);
        out_u.write((char *)&target, bytes_u);
    }
    out_u.close();
    std::cout << "WASTE TIME: " << timer.elapsed() << "s." << std::endl;
}