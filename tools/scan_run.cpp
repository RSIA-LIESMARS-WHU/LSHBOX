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
 * @file scan-test.cpp
 *
 * @brief Linear scan dataset and construct benchmark.
 */
#include <lshbox.h>
int main(int argc, char *argv[])
{
    unsigned K = 100, Q = 1000, seed = 2;
    // create benchmark of audio.data
    std::cout << "CREATE BENCHMARK OF AUDIO.DATA ..." << std::endl;
    lshbox::timer timer;
    std::string audio_file = "audio.data";
    std::string audio_query_file = "audio.ben";
    lshbox::Matrix<float> data_audio(audio_file);
    lshbox::Benchmark bench_audio;
    bench_audio.init(Q, K, data_audio.getSize(), seed);
    lshbox::Metric<float> metric_audio(data_audio.getDim(), L1_DIST);
    lshbox::progress_display pd_audio(Q);
    for (unsigned i = 0; i != Q; ++i)
    {
        unsigned q = bench_audio.getQuery(i);
        lshbox::Topk &topk = bench_audio.getAnswer(i);
        for (unsigned j = 0; j != data_audio.getSize(); ++j)
        {
            if (q == j)
            {
                continue;
            }
            topk.push(j, metric_audio.dist(data_audio[q], data_audio[j]));
        }
        ++pd_audio;
    }
    bench_audio.save(audio_query_file);
    std::cout << "WASTE TIME: " << timer.elapsed() << "s." << std::endl;
    // create benchmark of unsigned.data
    std::cout << "CREATE BENCHMARK OF UNSIGNED.DATA ..." << std::endl;
    timer.restart();
    std::string unsigned_file = "unsigned.data";
    std::string unsigned_query_file = "unsigned.ben";
    lshbox::Matrix<unsigned> data_unsigned(unsigned_file);
    lshbox::Benchmark bench_unsigned;
    bench_unsigned.init(Q, K, data_unsigned.getSize(), seed);
    lshbox::Metric<unsigned> metric_unsignbed(data_unsigned.getDim(), L1_DIST);
    lshbox::progress_display pd_unsigned(Q);
    for (unsigned i = 0; i != Q; ++i)
    {
        unsigned q = bench_unsigned.getQuery(i);
        lshbox::Topk &topk = bench_unsigned.getAnswer(i);
        for (unsigned j = 0; j != data_unsigned.getSize(); ++j)
        {
            if (q == j)
            {
                continue;
            }
            topk.push(j, metric_unsignbed.dist(data_unsigned[q], data_unsigned[j]));
        }
        ++pd_unsigned;
    }
    bench_unsigned.save(unsigned_query_file);
    std::cout << "WASTE TIME: " << timer.elapsed() << "s." << std::endl;
}