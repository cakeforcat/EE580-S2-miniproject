%% Init
fs = 8e3;
load laughter.mat
in_fs = 8192;
x = resample(y, fs, in_fs);
clear y

%% Filter design
lowpass_f1 = fs/6;

bandpass_f1 = fs/6;
bandpass_f2 = fs/3;

highpass_f1 = fs/3;
highpass_f2 = fs/2;