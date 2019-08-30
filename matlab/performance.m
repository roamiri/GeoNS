X = [1, 4];
A = [13.64 , 8.85];
B = [136.27, 89.46];
%%
figure;
hold on;
grid on;
box on;
% plot( ones(1,16)*2.0, '--k', 'LineWidth',1 );
plot(X, A , '--or', 'LineWidth',1.2,'MarkerSize',8, 'MarkerFaceColor','r', 'MarkerEdgeColor','b');
plot(X, B,'--ob', 'LineWidth',1.2,'MarkerSize',8, 'MarkerFaceColor','b', 'MarkerEdgeColor','b');
% % title('Running time for Async learning','FontSize',14, 'FontWeight','bold');
xlabel('number of processors','FontSize',12);%, 'FontWeight','bold');
ylabel('Seconds','FontSize',12);%, 'FontWeight','bold');
xlim([0 10]);
%  ylim([0 100]);
legend({'1e4 iterations','1e5 iterations'});%, 'Wired First', 'Position Aware'},'FontSize',12);%, 'FontWeight','bold');