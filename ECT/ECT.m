data = [ 20 -4
    5 32
    3 68
    1 104
    0.6 149
    0.3 176
    0.2 212
    0.1 248];
   
   x = data(:,1).*1000;
   y = data(:,2);
   
   figure(1)
   clf;
   plot(x,y, 'b--o');
   xlabel('R[ohm]');
   ylabel('Temp [F]');
   grid on;