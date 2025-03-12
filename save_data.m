function save_data(name,b,a)
    filename = strcat('MiniProject/IIR_coeffs/',name,'.h');
    fid = fopen(filename,'w');
    
    if length(a) ~= length(b)
        error('a has different length than b')
    end
    % FIR FILTER
    % B
    fprintf(fid,['#define N_IIR_' upper(name) ' %d' char([13 10])], ...
        length(a));
    fwrite(fid,char([13 10]),'uchar');
    
    %fwrite(fid,['float b_iir_' name '[] = { '],'uchar');
    arrayfun(@(dIn) fwrite(fid,dIn, 'uchar'),['float b_iir_' name '[] = { ']);
    for ct = 1:length(b)-1
        fprintf(fid,'%.7ff, ', single(b(ct)));
    end
    fprintf(fid,'%.7ff', single(b(end)));
    fwrite(fid,[' };' char([13 10])],'uchar');
    %fwrite(fid,char([13 10]),'uchar');
    % A
    %fprintf(fid,['#define N_FIR_A %d' char([13 10])], length(a_fir));
    fwrite(fid,char([13 10]),'uchar');
    fwrite(fid,['float a_iir_' name '[] = { '],'uchar');
    for ct = 1:length(a)-1
        fprintf(fid,'%.7ff, ', single(a(ct)));
    end
    fprintf(fid,'%.7ff', single(a(end)));
    fwrite(fid,[' };' char([13 10])],'uchar');
    %fwrite(fid,char([13 10]),'uchar');
    
    
    fclose(fid);
end