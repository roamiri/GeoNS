X = [1, 8, 16, 32];
A = [0.816 , 0.554, 0.357]; % 1e4
B = [8.02, 5.35, 3.24, 3.3]; % 1e5
C = [83.04, 53.67, 33.04, 29.94]; % 1e6
%%
figure;
hold on;
grid on;
box on;
% plot( ones(1,16)*2.0, '--k', 'LineWidth',1 );
plot(X, B , '--or', 'LineWidth',1.2,'MarkerSize',8, 'MarkerFaceColor','r', 'MarkerEdgeColor','b');
plot(X, C,'--db', 'LineWidth',1.2,'MarkerSize',8, 'MarkerFaceColor','b', 'MarkerEdgeColor','b');
% % title('Running time for Async learning','FontSize',14, 'FontWeight','bold');
xlabel('number of processors','FontSize',12);%, 'FontWeight','bold');
ylabel('Seconds','FontSize',12);%, 'FontWeight','bold');
% xlim([1 32]);
%  ylim([0 100]);
xticks([1 8 16 32]);
legend({'1e5 iterations','1e6 iterations'});%, 'Wired First', 'Position Aware'},'FontSize',12);%, 'FontWeight','bold');