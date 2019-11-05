%%
diskRadius = 500;
%% Generating MBS PPP
randNumb_MBS = poissrnd(5);
theta = rand(randNumb_MBS,1)*(2*pi);
r = diskRadius*sqrt(rand(randNumb_MBS,1));
x =  r.*cos(theta);  
y =  r.*sin(theta);   
MBS_location=[x,y];

%% Generating MBS fixed
randNumb_MBS = 6;
d = 2*pi/6;
a = linspace(0,2*pi,randNumb_MBS+1);
theta = a(1:randNumb_MBS)';
r = diskRadius/2*ones(randNumb_MBS,1);
x =  r.*cos(theta);  
y =  r.*sin(theta);   
MBS_location=[x,y];
 %% Generating SBS PPP
randNumb_SBS = poissrnd(100);
theta = rand(randNumb_SBS,1)*(2*pi);
r = diskRadius*sqrt(rand(randNumb_SBS,1));
x =  r.*cos(theta);
y =  r.*sin(theta);
SBS_location=[x,y];
%%
hold on;
grid on;
box on;

plot(SBS_location(:,1), SBS_location(:,2), '^k', 'MarkerFaceColor','k');
plot(MBS_location(:,1), MBS_location(:,2), 'sr', 'MarkerFaceColor','r', 'MarkerSize', 10);

% title('Example of mmWave BSs distribution','FontSize',14, 'FontWeight','bold');
xlim([-500,500]);
ylim([-500,500]);
xlabel('x-axis','FontSize',14, 'FontWeight','bold');
ylabel('y-axis','FontSize',14, 'FontWeight','bold');
legend({'SBS', 'MBS'},'FontSize',14, 'FontWeight','bold');
%%
h = open('deployment_1.fig');
set(h,'Units','Inches');
pos = get(h,'Position');
set(h,'PaperPositionMode','Auto','PaperUnits','Inches','PaperSize',[pos(3), pos(4)])
print(h,'deployment_1.pdf','-dpdf','-r0');