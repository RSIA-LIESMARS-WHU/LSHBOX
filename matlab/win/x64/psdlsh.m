function [indices, dists] = psdlsh(data, testset, params, index, P, K)
if nargin < 6
    K = 10;
    if nargin < 5
        P = 2;
        if nargin < 4
            index = '';
        end
    end
end
params = [P, K, params.M, params.L, params.T, params.W];
[indices, dists] = psdlsh_search(data, testset, params, index);
indices = indices + 1;
end