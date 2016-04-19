
load = 0:10:100;
rpm = 0:1000:16000;

slope = (30-13)/(6000-1300)

map = 13 + slope.*(rpm-1300).*heaviside(rpm-1300) -slope.*(rpm-1300).*heaviside(rpm-6000) + 17.*heaviside(rpm-6000);
map = repmat(map, [ size(load,2) ,1 ]);

figure(1)
clf
surf(rpm, load, map);
xlabel('RPM');
ylabel('Load [%]');
zlabel('Deg BTDC');
grid on;
title('Basic ignition map');


% Based on the example from http://www.mathworks.com/matlabcentral/newsreader/view_thread/48182


fd=fopen('ignitionMap.h','wt');

fprintf(fd, 'typedef struct {\n');
fprintf(fd, ' uint16_t rpm_min;\n');
fprintf(fd, ' uint16_t rpm_max;\n');
fprintf(fd, ' uint16_t rpm_step;\n');
fprintf(fd, ' uint16_t load_min;\n');
fprintf(fd, ' uint16_t load_max;\n');
fprintf(fd, ' uint16_t load_step;\n');
fprintf(fd, ' uint16_t values[][];\n');
fprintf(fd, '} ignitionMap_t;\n');

fprintf(fd, '\n\nignitionMap_t myMap ={');
fprintf(fd, ' %d,\n', min(rpm))
fprintf(fd, ' %d,\n', max(rpm))
fprintf(fd, ' %d,\n', rpm(2)-rpm(1))
fprintf(fd, ' %d,\n', min(load))
fprintf(fd, ' %d,\n', max(load))
fprintf(fd, ' %d,\n', load(2)-load(1))
fprintf(fd, ' { {2}, {1} }')
% fprintf(fd, ' uint8_t values[%d][%d] = {\n', size(rpm,2), size(load,2))
% for i=1:size(map,2)
%     fprintf(fd, '{');
%     for j=1:size(map,1)
%         fprintf(fd, '%.2f, ', map(j,i));
%     end
%     fprintf(fd, '},\n');
% end
% fprintf(fd, ' };\n')


fprintf(fd, '};')

%fprintf(fd,'float h[%d]={%.9g',length(h),h(1));
%fprintf(fd,',\n %.9f',h(2:end));
%fprintf(fd,'};\n');
fclose(fd)

