% matlab_example2.m
dataset = randn(128,500000);
testset = dataset(:,1:10);
disp('Test itqlsh')
param_itq.M = 521;
param_itq.L = 5;
param_itq.N = 8;
param_itq.S = 100;
param_itq.I = 50;
disp('First time, need to constructing index') % About 13s.
tic;
[indices, dists] = itqlsh(dataset, testset, param_itq, 'itq.lsh', 2, 10);
toc;
disp('Second time, no need to re-indexing') % About 0.5s.
tic;
[indices, dists] = itqlsh(dataset, testset, param_itq, 'itq.lsh', 2, 10);
toc;