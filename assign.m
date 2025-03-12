%% Init
fs = 8e3;
load handel.mat
in_fs = 8192;
x = resample(y, fs, in_fs);
clear y
audiowrite("x.wav", x, fs)

%% Filter design
lowpass_f1 = fs/6;

bandpass_f1 = fs/6;
bandpass_f2 = fs/3;

highpass_f1 = fs/3;
highpass_f2 = fs/2;

order = 10;

% elliptic filters for all why not
% lowpass
lpfilt = designfilt("lowpassiir", ...
    FilterOrder=order, ...
    PassbandFrequency=lowpass_f1, ...
    StopbandAttenuation=60, PassbandRipple=1, ...
    SampleRate=fs);
% bandpass
bpfilt = designfilt("bandpassiir", ...
    FilterOrder=order, ...
    PassbandFrequency1=bandpass_f1, ...
    PassbandFrequency2=bandpass_f2, ...
    StopbandAttenuation1=60, PassbandRipple=1, StopbandAttenuation2=60, ...
    SampleRate=fs);
% highpass
hpfilt = designfilt("highpassiir", ...
    FilterOrder=order, ...
    PassbandFrequency=highpass_f1, ...
    StopbandAttenuation=60, PassbandRipple=1, ...
    SampleRate=fs);

% analyze the filters
filterAnalyzer(lpfilt, bpfilt, hpfilt, ...
            FilterNames=["Lowpass", "Bandpass", "Highpass"], ...
            Analysis="magnitude", ...
            SampleRates=[fs, fs, fs]);

% get transfer functions
[lp_b, lp_a] = tf(lpfilt);
[bp_b, bp_a] = tf(bpfilt);
[hp_b, hp_a] = tf(hpfilt);

% analyze the transfer functions
filterAnalyzer(lp_b, lp_a, bp_b, bp_a, hp_b, hp_a, ...
            FilterNames=["Lowpass_tf", "Bandpass_tf", "Highpass_tf"], ...
            SampleRates=[fs, fs, fs]);

% quantize the transfer functions
lp_b_q = single(lp_b);
lp_a_q = single(lp_a);
bp_b_q = single(bp_b);
bp_a_q = single(bp_a);
hp_b_q = single(hp_b);
hp_a_q = single(hp_a);

% analyze quantization
filterAnalyzer(lp_b_q, lp_a_q, bp_b_q, bp_a_q, hp_b_q, hp_a_q, ...
            FilterNames=["Lowpass_tf_q", "Bandpass_tf_q", "Highpass_tf_q"], ...
            SampleRates=[fs, fs, fs]);

%% Filter the signal
% lowpass
y_lp = filter(lp_b_q, lp_a_q, x);
audiowrite("y_lp.wav",y_lp,fs)
% bandpass
y_bp = filter(bp_b_q, bp_a_q, x);
audiowrite("y_bp.wav",y_bp,fs)
% highpass
y_hp = filter(hp_b_q, hp_a_q, x);
audiowrite("y_hp.wav",y_hp,fs)

%% Plot frequency content before and after filtering
% % before
% figure
% subplot(3, 2, 1)
% pwelch(x, [], [], [], fs)
% title("Original signal")
% % after
% subplot(3, 2, 2)
% pwelch(y_lp, [], [], [], fs)
% title("Lowpass filtered signal")
% subplot(3, 2, 4)
% pwelch(y_bp, [], [], [], fs)
% title("Bandpass filtered signal")
% subplot(3, 2, 6)
% pwelch(y_hp, [], [], [], fs)
% title("Highpass filtered signal")

fspace = linspace(0, fs, length(x));
half = ceil(length(x)/2);
% before
X = fft(x);
figure
subplot(3, 2, 1)
plot(fspace(1:half), abs(X(1:half)))
title("Original signal")
% after
Y_lp = fft(y_lp);
subplot(3, 2, 2)
plot(fspace(1:half), abs(Y_lp(1:half)))
title("Lowpass filtered signal")
Y_bp = fft(y_bp);
subplot(3, 2, 4)
plot(fspace(1:half), abs(Y_bp(1:half)))
title("Bandpass filtered signal")
Y_hp = fft(y_hp);
subplot(3, 2, 6)
plot(fspace(1:half), abs(Y_hp(1:half)))
title("Highpass filtered signal")


%% export to a header
% lowpass
save_data('lp', lp_b, lp_a)
% bandpass
save_data('bp', bp_b, bp_a)
% highpass
save_data('hp', hp_b, hp_a)