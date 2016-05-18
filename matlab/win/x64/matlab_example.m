% matlab_example.m
disp('prepare test data ...')
dataset = randn(128,100000);
dataset = dataset - repmat(mean(dataset), size(dataset, 1), 1);
testset = dataset(:,1:10);
disp('ok')
input('Test rhplsh, Press any key to continue ...')
param_rhp.M = 521;
param_rhp.L = 5;
param_rhp.N = 6;
[indices, dists] = rhplsh(dataset, testset, param_rhp, '', 2, 10)
input('Test thlsh, Press any key to continue ...')
param_th.M = 521;
param_th.L = 5;
param_th.N = 12;
[indices, dists] = thlsh(dataset, testset, param_th, '', 2, 10)
input('Test psdlsh with param_psdL1.T = 1, Press any key to continue ...')
param_psdL1.M = 521;
param_psdL1.L = 5;
param_psdL1.T = 1;
param_psdL1.W = 5;
[indices, dists] = psdlsh(dataset, testset, param_psdL1, '', 1, 10)
input('Test psdlsh with param_psdL2.T = 2, Press any key to continue ...')
param_psdL2.M = 521;
param_psdL2.L = 5;
param_psdL2.T = 2;
param_psdL2.W = 0.5;
[indices, dists] = psdlsh(dataset, testset, param_psdL2, '', 2, 10)
input('Test shlsh, Press any key to continue ...')
param_sh.M = 521;
param_sh.L = 5;
param_sh.N = 4;
param_sh.S = 100;
[indices, dists] = shlsh(dataset, testset, param_sh, '', 2, 10)
disp('Test itqlsh, Press any key to continue.')
param_itq.M = 521;
param_itq.L = 5;
param_itq.N = 8;
param_itq.S = 100;
param_itq.I = 50;
[indices, dists] = itqlsh(dataset, testset, param_itq, '', 2, 10)
disp('Test dbqlsh, Press any key to continue.')
param_dbq.M = 521;
param_dbq.L = 5;
param_dbq.N = 4;
param_dbq.I = 5;
[indices, dists] = dbqlsh(dataset, testset, param_dbq, '', 2, 10)
disp('Test kdbqlsh, Press any key to continue.')
param_kdbq.M = 521;
param_kdbq.L = 5;
param_kdbq.N = 4;
param_kdbq.I = 50;
[indices, dists] = kdbqlsh(dataset, testset, param_kdbq, '', 2, 10)