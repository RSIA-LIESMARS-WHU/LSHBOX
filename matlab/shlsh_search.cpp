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
#include <mex.h>
#include <lshbox.h>
namespace lshbox
{
class matShLsh
{
public:
    typedef double DATATYPE;
    void init(
        DATATYPE *source,
        unsigned dim,
        unsigned size,
        const std::string &index,
        unsigned M = 521,
        unsigned L = 5,
        unsigned N = 4,
        unsigned S = 100)
    {
        data.load(source, size, dim);
        std::ifstream is(index.c_str(), std::ios_base::binary);
        if (is)
        {
            lsh.load(index);
        }
        else
        {
            shLsh<DATATYPE>::Parameter param;
            param.M = M;
            param.L = L;
            param.D = data.getDim();
            param.N = N;
            param.S = S;
            lsh.reset(param);
            lsh.train(data);
            lsh.hash(data);
            lsh.save(index);
        }
    }
    void query(DATATYPE *quy, unsigned size, DATATYPE *indices, DATATYPE *dists, unsigned type, unsigned K)
    {
        Matrix<DATATYPE>::Accessor accessor(data);
        Metric<DATATYPE> metric(data.getDim(), type);
        Scanner<lshbox::Matrix<DATATYPE>::Accessor> scanner(
            accessor,
            metric,
            K
        );
        for (unsigned i = 0; i != size; ++i)
        {
            lsh.query(quy + i * data.getDim(), scanner);
            std::vector<std::pair<float, unsigned> > tmp = scanner.topk().getTopk();
            for (unsigned j = 0; j != tmp.size(); ++j)
            {
                indices[i * K + j] = tmp[j].second;
                dists[i * K + j] = tmp[j].first;
            }
        }
    }
private:
    Matrix<DATATYPE> data;
    shLsh<DATATYPE> lsh;
};
}
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    double *data_set = mxGetPr(prhs[0]);
    unsigned dim = mxGetM(prhs[0]);
    unsigned size = mxGetN(prhs[0]);
    double *query_set = mxGetPr(prhs[1]);
    unsigned query_size = mxGetN(prhs[1]);
    double *params = mxGetPr(prhs[2]);
    std::string index = mxArrayToString(prhs[3]);
    plhs[0] = mxCreateDoubleMatrix(int(params[1]), mxGetN(prhs[1]), mxREAL);
    plhs[1] = mxCreateDoubleMatrix(int(params[1]), mxGetN(prhs[1]), mxREAL);
    double *indices = mxGetPr(plhs[0]);
    double *dists = mxGetPr(plhs[1]);
    lshbox::matShLsh mylsh;
    mylsh.init(data_set, dim, size, index, unsigned(params[2]),
               unsigned(params[3]), unsigned(params[4]), unsigned(params[5]));
    mylsh.query(query_set, query_size, indices, dists,
                unsigned(params[0]), unsigned(params[1]));
}