lambda = @(x) 300*(x(:,1).^2 + x(:,2).^2);
lamstar = 120;
N=poissrnd(lamstar); x = rand(N,2)-0.5; % homogeneous PP
ind = find(rand(N,1) < lambda(x)/lamstar);
xa = x(ind,:); % thinned PP
plot(1000*x(:,1),1000*x(:,2), '^k');
xlim([-500 500]);
ylim([-500 500]);

hold on;
grid on;
box on;
% title('Example of mmWave BSs distribution','FontSize',14, 'FontWeight','bold');
xlabel('x-axis','FontSize',14, 'FontWeight','bold');
ylabel('y-axis','FontSize',14, 'FontWeight','bold');
legend({'mmWave BS'},'FontSize',14, 'FontWeight','bold');
%%
h = open('sysmodel1.fig');
set(h,'Units','Inches');
pos = get(h,'Position');
set(h,'PaperPositionMode','Auto','PaperUnits','Inches','PaperSize',[pos(3), pos(4)])
print(h,'sysmodel1.pdf','-dpdf','-r0');