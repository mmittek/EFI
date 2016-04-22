% Data collected in the garage on Apr 21 2016
% using DENSO 079800-5710's knockoff and a $20 vaccum synchronizer
data = [ 1.32 46.67
         1.64 60
         2.03 73.33
         2.41 86.67
         2.82 100
         3.01 106.9
         3.17 114
         3.3 120.68];
     
     
v = data(:,1);
kpa = data(:,2);
figure(1)
clf;
plot(v,kpa, 'b-x');
grid on;
title('kpa as a function of output voltage');
xlabel('volts');
ylabel('pressure [kpa]');
