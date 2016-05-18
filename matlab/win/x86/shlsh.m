function [indices, dists] = shlsh(data, testset, params, index, P, K)
if nargin < 6
    K = 10;
    if nargin < 5
        P = 2;
        if nargin < 4
            index = '';
        end
    end
end
params = [P, K, params.M, params.L, params.N, params.S];
[indices, dists] = shlsh_search(data, testset, params, index);
indices = indices + 1;
end