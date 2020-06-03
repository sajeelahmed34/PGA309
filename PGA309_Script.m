% Calculation for the Gain and Register values for Programmable Gain
% Amplifier PGA309. The calculation is based on the formulas listed in
% PGA309 User Guide. Example 2.1 on page 23 in user guide is followed in
% this MatLab script. 

%clc command will clear the command window
clc

% Constant parameters
FSS = 1.5e-3; %Full scale sensitivity of the bridge sensor
Vos = 0; %offset voltage in mV
Vref = 3.3; %Reference voltage applied across pga309. 
Vb = 2.7; %bridge voltage across sensor
Rbrg = 120; %Bridge resistance in ohms
Vout_min = 0.5; %Desired minimum output voltage 
Vout_max = 2.5; %Desired maximum output voltage

%Maximum sensor output
Vbr_max = FSS*Vb; %maximum bridge sensor voltage
fprintf('Maximum sensor output = %gV\n', Vbr_max);

%Total Desired Gain
GT = (Vout_max - Vout_min)/Vbr_max; 
fprintf('Total Desired Gain = GT = %f\n', GT);

%Partitioning the Gain
count = 1;
front_end_gain = [4 8 16 23.27 32 42.67 64 128]; %Array containing front end gain values 
output_gain = [2 2.4 3 3.6 4.5 6 9]; %Array containing output amplifier gain values 
min_value(3, 56) = 0; %initializing a matrix of 3x56 with all zeros 
for i = 1:1:8
    for j = 1:1:7
        temp = front_end_gain(i)*output_gain(j);
        if ((temp - GT)>=0)
            min_value(1, count) = i;
            min_value(2, count) = j;
            min_value(3, count) = temp - GT;
            count = count + 1;
        end
    end
end

temp = min(min_value(3, 1:count-1));
[r, c] = find(min_value == temp);
r = min(r);
c = max(c);
i_index = min_value(1, c);
j_index = min_value(2, c);
FEG = front_end_gain(1, i_index);
GO = output_gain(1, j_index);
fprintf('Front end gain = %0.2f\n', FEG);
fprintf('Output gain = %0.2f\n', GO);

%Calculation for Gain DAC
Gain_DAC = GT/(FEG*GO);
%Converting Gain_DAC value into Decimal counts
Gain_DAC_dec = round((Gain_DAC-(1/3))*1.5*65536);
%Converting decimal value to Hexadecimal format
Gain_DAC_hexDec = dec2hex(Gain_DAC_dec);
fprintf('Gain DAC Hexadecimal value = 0x%s\n', Gain_DAC_hexDec);

%Calculation for Zero DAC - Register 1 settings 
Vzero_DAC = Vout_min/(Gain_DAC*GO);
%Converting Vzero_DAC value into Decimal counts
Vzero_DAC_dec = round(Vzero_DAC/(Vref/65536));
%Converting decimal value to Hexadecimal format
Vzero_DAC_hexDec = dec2hex(Vzero_DAC_dec);
fprintf('Zero DAC Hexadecimal value = 0x%s\n', Vzero_DAC_hexDec);

%new functions will be added soon


