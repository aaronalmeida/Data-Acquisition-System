%Aaron Almeida
%April 5th 2019
%MATLAB version 9.2

%delete all ports that are not being used
 if ~isempty(instrfind)
     fclose(instrfind);
      delete(instrfind);
 end
 
%initialize variables  
plotTitle = 'Serial Data Log';  % title
xLabel = 'Elapsed Time (s)';    % x label
yLabel = 'Angle in Degrees';      % y label
plotGrid = 'on';                % 'off' to turn off grid
min = 0;                     
max = 90;                      
scrollWidth = 10;               
delay = 0.01;                    

%Define Function Variables
time = 0;
data = 0;
count = 0;
counter = 0;


%plot graph 
plotGraph = plot(time,data);

             
title(plotTitle,'FontSize',25);
xlabel(xLabel,'FontSize',15);
ylabel(yLabel,'FontSize',15);
axis([0 10 min max]);
grid(plotGrid);
 
%open port COM2
s = serial('COM2');
set(s,'BaudRate',19200);
disp('Close Plot to End Session');
fopen(s);

tic

while ishandle(plotGraph)
    counter =counter + 1;
    dat = fscanf(s); %Read Data from Serial 
    count = count + 1;    
    time(count) = toc;    %Extract time
    data(count) = (str2double(dat(1:2))); %Extract data
         
        %Set Axis according to Scroll Width
        if(scrollWidth > 0)
        set(plotGraph,'XData',time(time > time(count)-scrollWidth),'YData',data(time > time(count)-scrollWidth));
        axis([time(count)-scrollWidth time(count) min max]);
        else
        set(plotGraph,'XData',time,'YData',data);
        axis([0 time(count) min max]);
        end
         
        %Allow MATLAB to Update Plot
        pause(delay);
    
     %empty the data, prevents overflow error
    if(counter == 4000)
        clear data;
        counter = 0;
    end
        
   
end

%Close port and deete variables 
fclose(s);
clear count dat delay max min plotGraph plotGrid plotTitle s ...
        scrollWidth serialPort xLabel yLabel;
 
 
disp('Session Terminated...');
   

fclose(s);
delete(s);
clear s
