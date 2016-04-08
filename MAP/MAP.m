% http://injector-rehab.com/shop/mapsensor.html

x = linspace(0, 5, 21);

y = 40.*x + 8;
t = 2.86

yt = 40*t + 8;
figure(1)
clf;
hold on;
plot(x,y, 'b-x');
plot( t, yt, 'ro');
hold off;
grid on;