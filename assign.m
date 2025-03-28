%% 
fs = 8e3;
% genenerate some wgn
x = single(wgn(1, 1e6, 2^32, 1, 11,'linear'));

%% Filter design
analysis = true;

lowpass_f1 = fs/6;

bandpass_f1 = fs/6;
bandpass_f2 = fs/3;

highpass_f1 = fs/3;
highpass_f2 = fs/2;

order = 7;

% elliptic filters for all why not
% lowpass
lpfilt = designfilt("lowpassiir", ...
    FilterOrder=order, ...
    PassbandFrequency=lowpass_f1, ...
    StopbandAttenuation=60, PassbandRipple=1, ...
    SampleRate=fs);
% bandpass
bpfilt = designfilt("bandpassiir", ...
    FilterOrder=order-1, ...
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
if analysis
filterAnalyzer(lpfilt, bpfilt, hpfilt, ...
            FilterNames=["Lowpass", "Bandpass", "Highpass"], ...
            Analysis="magnitude", ...
            SampleRates=[fs, fs, fs]);
end
% get transfer functions
[lp_b, lp_a] = tf(lpfilt);
[bp_b, bp_a] = tf(bpfilt);
[hp_b, hp_a] = tf(hpfilt);

% pad the BP with a 0
bp_a = [bp_a 0];
bp_b = [bp_b 0];

% analyze the transfer functions
if analysis
filterAnalyzer(lp_b, lp_a, bp_b, bp_a, hp_b, hp_a, ...
            FilterNames=["Lowpass_tf", "Bandpass_tf", "Highpass_tf"], ...
            SampleRates=[fs, fs, fs]);
end
% quantize the transfer functions
lp_b_q = single(lp_b);
lp_a_q = single(lp_a);
bp_b_q = single(bp_b);
bp_a_q = single(bp_a);
hp_b_q = single(hp_b);
hp_a_q = single(hp_a);

% analyze quantization
if analysis
filterAnalyzer(lp_b_q, lp_a_q, bp_b_q, bp_a_q, hp_b_q, hp_a_q, ...
            FilterNames=["Lowpass_tf_q", "Bandpass_tf_q", "Highpass_tf_q"], ...
            SampleRates=[fs, fs, fs]);
end
%% Filter the signal
% lowpass
y_lp = filter(lp_b_q, lp_a_q, x);
% bandpass
y_bp = filter(bp_b_q, bp_a_q, x);
% highpass
y_hp = filter(hp_b_q, hp_a_q, x);

%% Plot frequency content before and after filtering
fft_n = 2048;
fspace = linspace(0, fs, fft_n);
% before
X = fft(x, fft_n)/fft_n;
figure
subplot(3, 3, 1)
plot(fspace(1:fft_n/2), abs(X(1:fft_n/2)))
title("Original signal")
% after

Y_lp = fft(y_lp, fft_n);
subplot(3, 3, 2)
plot(fspace(1:fft_n/2), abs(Y_lp(1:fft_n/2)))
title("Lowpass filtered signal")

Y_bp = fft(y_bp, fft_n);
subplot(3, 3, 5)
plot(fspace(1:fft_n/2), abs(Y_bp(1:fft_n/2)))
title("Bandpass filtered signal")

Y_hp = fft(y_hp, fft_n);
subplot(3, 3, 8)
plot(fspace(1:fft_n/2), abs(Y_hp(1:fft_n/2)))
title("Highpass filtered signal")

Y_lpbp = fft(y_lp + y_bp, fft_n);
subplot(3, 3, 3)
plot(fspace(1:fft_n/2), abs(Y_lpbp(1:fft_n/2)))
title("Lowpass + Bandpass filtered signal")

Y_lphp = fft(y_lp + y_hp, fft_n);
subplot(3, 3, 6)
plot(fspace(1:fft_n/2), abs(Y_lphp(1:fft_n/2)))
title("Lowpass + Highpass filtered signal")

Y_bphp = fft(y_bp + y_hp, fft_n);
subplot(3, 3, 9)
plot(fspace(1:fft_n/2), abs(Y_bphp(1:fft_n/2)))
title("Bandpass + Highpass filtered signal")

Y_lpbphp = fft(y_lp + y_bp + y_hp, fft_n);
subplot(3, 3, 7)
plot(fspace(1:fft_n/2), abs(Y_lpbphp(1:fft_n/2)))
title("Lowpass + Bandpass + Highpass filtered signal")

%% Plot individially and save
Y_lp_plot = figure(Visible="off");
plot(fspace(1:fft_n/2), abs(Y_lp(1:fft_n/2)),LineWidth=2)
title("Lowpass filtered WGN")
xlabel("Frequency (Hz)")
ylabel("|Magnitude|")
saveas(Y_lp_plot, "plots/wgn/Y_lp.png")

Y_bp_plot = figure(Visible="off");
plot(fspace(1:fft_n/2), abs(Y_bp(1:fft_n/2)),LineWidth=2)
title("Bandpass filtered WGN")
xlabel("Frequency (Hz)")
ylabel("|Magnitude|")
saveas(Y_bp_plot, "plots/wgn/Y_bp.png")

Y_hp_plot = figure(Visible="off");
plot(fspace(1:fft_n/2), abs(Y_hp(1:fft_n/2)),LineWidth=2)
title("Highpass filtered WGN")
xlabel("Frequency (Hz)")
ylabel("|Magnitude|")
saveas(Y_hp_plot, "plots/wgn/Y_hp.png")

Y_lpbp_plot = figure(Visible="off");
plot(fspace(1:fft_n/2), abs(Y_lpbp(1:fft_n/2)),LineWidth=2)
title("Lowpass + Bandpass filtered WGN")
xlabel("Frequency (Hz)")
ylabel("|Magnitude|")
saveas(Y_lpbp_plot, "plots/wgn/Y_lpbp.png")

Y_lphp_plot = figure(Visible="off");
plot(fspace(1:fft_n/2), abs(Y_lphp(1:fft_n/2)),LineWidth=2)
title("Lowpass + Highpass filtered WGN")
xlabel("Frequency (Hz)")
ylabel("|Magnitude|")
saveas(Y_lphp_plot, "plots/wgn/Y_lphp.png")

Y_bphp_plot = figure(Visible="off");
plot(fspace(1:fft_n/2), abs(Y_bphp(1:fft_n/2)),LineWidth=2)
title("Bandpass + Highpass filtered WGN")
xlabel("Frequency (Hz)")
ylabel("|Magnitude|")
saveas(Y_bphp_plot, "plots/wgn/Y_bphp.png")

Y_lpbphp_plot = figure(Visible="off");
plot(fspace(1:fft_n/2), abs(Y_lpbphp(1:fft_n/2)),LineWidth=2)
title("Lowpass + Bandpass + Highpass filtered WGN")
xlabel("Frequency (Hz)")
ylabel("|Magnitude|")
saveas(Y_lpbphp_plot, "plots/wgn/Y_lpbphp.png")

%% export to a header
% lowpass
save_data('lp', lp_b, lp_a)
% bandpass
save_data('bp', bp_b, bp_a)
% highpass
save_data('hp', hp_b, hp_a)