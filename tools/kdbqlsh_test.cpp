//////////////////////////////////////////////////////////////////////////////
/// Copyright (C) 2015 Gefu Tang <tanggefu@gmail.com> & Yang Long <20288ly@sina.cn>.
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
/// @author Gefu Tang, Yang Long & Zhifeng Xiao
/// @date 2015.6.30
//////////////////////////////////////////////////////////////////////////////

/**
 * @file kdbqlsh_test.cpp
 *
 * @brief Example of using K-means Based Double-Bit Quantization LSH index for L2 distance.
 */

#include<lshbox.h>
int main(int argc, char const *argv[])
{
    if (argc != 4)
    {
        std::cerr << "Usage: ./kdbqlsh_test data_file lsh_file benchmark_file" << std::endl;
        return -1;
    }
    std::cout << "Example of Using K-means Double-Bit Quantization for Hashing" << std::endl << std::endl;
    typedef float DATATYPE;
    std::cout << "LOADING DATA ..." << std::endl;
    lshbox::timer timer;
    lshbox::Matrix <DATATYPE> data(argv[1]);
    std::cout << "LOAD TIME:" << timer.elapsed () << "s." << std::endl;
    std::cout << "CONSTRUCTING INDEX ..." << std::endl;
    timer.restart();
    std::string file(argv[2]);
    bool use_index = false;
    lshbox::kdbqLsh<DATATYPE> mylsh;
    if (use_index)
    {
        mylsh.load(file);
    }
    else
    {
        lshbox::kdbqLsh<DATATYPE>::Parameter param;
        param.M = 521 ;
        param.L = 5;
        param.D = data.getDim();
        param.N = 4;
        param.I = 50;
        mylsh.reset(param);
        mylsh.train(data);
        mylsh.save(file);
    }
    std::cout << "CONSTRUCTING TIME: " << timer.elapsed() << "s." << std::endl;
    std::cout << "LOADING BENCHMARK ..." << std::endl;

    timer.restart();
    lshbox::Matrix<DATATYPE>::Accessor accessor(data);
    lshbox::Metric<DATATYPE> metric(data.getDim(), L2_DIST);

    lshbox::Benchmark bench;
    std::string benchmark(argv[3]);
    bench.load(benchmark);
    unsigned K = 50;
    lshbox::Scanner<lshbox::Matrix<DATATYPE>::Accessor> scanner(
        accessor,
        metric,
        K
    );
    std::cout << "LOADING TIME: " << timer.elapsed() << "s." << std::endl;
    std::cout << "RUNING QUERY ..." << std::endl;
    timer.restart();
    lshbox::Stat cost, recall;
    lshbox::progress_display pd(bench.getQ());
    for (unsigned i = 0; i != bench.getQ(); ++i)
    {
        mylsh.query(data[bench.getQuery(i)], scanner);
        recall << bench.getAnswer(i).recall(scanner.topk());
        cost << float(scanner.cnt()) / float(data.getSize());
        ++pd;
    }
    std::cout << "MEAN QUERY TIME: " << timer.elapsed() / bench.getQ() << "s." << std::endl;
    std::cout << "RECALL   : " << recall.getAvg() << " +/- " << recall.getStd() << std::endl;
    std::cout << "COST     : " << cost.getAvg() << " +/- " << cost.getStd() << std::endl;

    // mylsh.query(data[0], scanner);
    // std::vector<std::pair<float, unsigned> > res = scanner.topk().getTopk();
    // for (std::vector<std::pair<float, unsigned> >::iterator it = res.begin(); it != res.end(); ++it)
    // {
    //     std::cout << it->second << ": " << it->first << std::endl;
    // }
    // std::cout << "DISTANCE COMPARISON TIMES: " << scanner.cnt() << std::endl;
}