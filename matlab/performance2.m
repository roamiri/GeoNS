
%% SNR Data
node_1 = [10, 1e2, 1e3, 2e3, 3e3, 1e4, 2e4, 3e4, 5e4, 1e5];
snr_tree = [4.00E-04,1.00E-03,0.026,0.08,0.18,1.87,6.63,14.29,37.83,70];
node_2 = [10, 1e2, 1e3, 2e3, 3e3, 1e4];
snr_array=[2.50E-03,1.50E-01,15.83,67.94,138.319,1500];

%%
% figure;
loglog(node_1, snr_tree , '--ob', 'LineWidth',1.2,'MarkerSize',8, 'MarkerFaceColor','b');
hold on;
loglog(node_2, snr_array , '--dr', 'LineWidth',1.2,'MarkerSize',8, 'MarkerFaceColor','r');
grid on;
box on;
xlabel('number of nodes','FontSize',12);%, 'FontWeight','bold');
ylabel('Processing Time (s)','FontSize',12);%, 'FontWeight','bold');
legend({'spatial indexing','array indexing'}, 'Location','northwest');
%% SINR Data
node_3 = [10,1.00E+02,2.00E+02,3.00E+02,5.00E+02,6.00E+02,1.00E+03,2.00E+03,3.00E+03,1.00E+04,2.00E+04,3.00E+04];
sinr_tree = [4.00E-04,9.800E-04,26e-4, 55e-4, 228e-4, 0.0488, 0.15,0.99,3.15,80,560,1605];
node_4 = [10,1.00E+02,2.00E+02,3.00E+02,5.00E+02,6.00E+02,1.00E+03];
sinr_array = [0.0056,1.49,10,27,166,292, 1436];
%%
loglog(node_3, sinr_tree , '--ob', 'LineWidth',1.2,'MarkerSize',8, 'MarkerFaceColor','b');
hold on;
loglog(node_4, sinr_array , '--dr', 'LineWidth',1.2,'MarkerSize',8, 'MarkerFaceColor','r');
grid on;
box on;
xlabel('number of nodes','FontSize',12);%, 'FontWeight','bold');
ylabel('Processing Time (s)','FontSize',12);%, 'FontWeight','bold');
legend({'spatial indexing','array indexing'}, 'Location','northwest');
%%
node_5 = [10,1.00E+02,2.00E+02,3.00E+02,5.00E+02,6.00E+02,1.00E+03,2.00E+03,3.00E+03,1.00E+04,2.00E+04,3.00E+04, 5e4, 1e5];
load_time = [3.00E-04,5.00E-04,0.0024,0.0012,0.0023,0.0054,7.70E-03,0.027,0.06,0.58,2.087,4.489,12.9,53.15];
snr_tree = [4.00E-04,1.00E-03,5.90E-03,3.70E-03,1.90E-02,1.00E-02,0.026,0.08,0.18,1.87,6.63,14.29,37.83,70];
ratio=zeros(size(node_5));
for i=1:size(node_5,2)
    ratio(1,i) = load_time(1,i)/snr_tree(1,i);
end
%%
semilogx(node_5, ratio, '--ob', 'LineWidth',1.2,'MarkerSize',8, 'MarkerFaceColor','b');
grid on;
box on;
xlabel('number of nodes','FontSize',12);%, 'FontWeight','bold');
ylabel('Processing Time (s)','FontSize',12);%, 'FontWeight','bold');
% legend({'spatial indexing','array indexing'}, 'Location','northwest');
