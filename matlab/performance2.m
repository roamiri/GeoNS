
%% SNR Data
node_1 = [10, 1e2, 1e3, 2e3, 3e3, 1e4, 2e4, 3e4, 5e4, 1e5];
snr_tree_computation = [4.32E-04,8.96E-04,0.02689,0.08,0.18,1.836,6.63,14.29,37.83,130];
snr_tree_search = [1.30E-04, 1.50e-4, 7.13e-4, 2.07e-3, 4.63e-3, 0.0359, 0.1240, 0.2687, 0.6765, 2.3076];
% tree_load = [3.16e-4, 4.93e-4, 8.57e-3, 0.03, 0.0614, 0.594, 2.28, 5.100, 13.766, 54.45];

node_2 = [10, 1e2, 1e3, 2e3, 3e3, 1e4];
% snr_array_computation =[2.50E-03,1.50E-01,15.83,67.94,138.319,1500];
snr_array_computation =[0.0025, 0.1891, 17.3616, 69.45, 157.944, 1500];
snr_array_search = [1.68e-4, 33.21e-4, 0.2743, 1.092, 2.439, 27.42];
% array_load = [3.08e-4, 4.07e-4, 12.83e-4, 0.0023, 0.0032, 0.0123 ];

% snr_ratio=snr_array./snr_tree(1:size(snr_array,2));
%% Loadting time
node = [10, 1e2, 1e3, 2e3, 3e3, 1e4, 2e4, 3e4, 5e4, 1e5];
tree_load = [3.16e-4, 4.93e-4, 8.57e-3, 0.03, 0.0614, 0.594, 2.28, 5.100, 13.766, 54.45];
array_load = [3.08e-4, 4.07e-4, 12.83e-4, 0.0023, 0.0032, 0.0123, 2.15e-2, 3.07e-2, 4.97e-2, 9.7e-2];
figure;
loglog(node, tree_load , '--dr', 'LineWidth',1.2,'MarkerSize',8, 'MarkerFaceColor','r');
hold on;
loglog(node, array_load , '--ob', 'LineWidth',1.2,'MarkerSize',8, 'MarkerFaceColor','b');
grid on;
box on;
xlim([1e1,1e5]);
ylim([1e-4,1e4]);
xlabel('number of nodes','FontSize',14);%, 'FontWeight','bold');
ylabel('Loading time (s)','FontSize',14);%, 'FontWeight','bold');
legend({'Spatial indexing', 'Array indexing'}, 'Location','northwest', 'FontSize',14);
%% SNR
figure;
% loglog(node_1, snr_tree_computation , '--ob', 'LineWidth',1.2,'MarkerSize',8, 'MarkerFaceColor','b');

loglog(node_1, snr_tree_search , '--dr', 'LineWidth',1.2,'MarkerSize',8, 'MarkerFaceColor','r');
hold on;
% loglog(node_1, tree_load , '--sk', 'LineWidth',1.2,'MarkerSize',8, 'MarkerFaceColor','k');
grid on;
box on;

% loglog(node_2, snr_array_computation , '--sr', 'LineWidth',1.2,'MarkerSize',8, 'MarkerFaceColor','r');
loglog(node_2, snr_array_search , '--ob', 'LineWidth',1.2,'MarkerSize',8, 'MarkerFaceColor','b');
% loglog(node_2, array_load , '--dk', 'LineWidth',1.2,'MarkerSize',8, 'MarkerFaceColor','k');

xlim([1e1,1e5]);
ylim([1e-4,1e4]);
xlabel('number of nodes','FontSize',12);%, 'FontWeight','bold');
ylabel('Processing time (s)','FontSize',12);%, 'FontWeight','bold');
% legend({'Computation time','Search time', 'Loading time'}, 'Location','northwest');
legend({'Spatial indexing search time', 'Array indexing search time'}, 'Location','northwest');
% legend({'Spatial indexing computation time','Spatial indexing search time', 'Array indexing computation time','Array indexing search time'}, 'Location','northwest');
%% SNR
figure;
loglog(node_2, snr_array_computation , '--ob', 'LineWidth',1.2,'MarkerSize',8, 'MarkerFaceColor','b');
hold on;
loglog(node_2, snr_array_search , '--dr', 'LineWidth',1.2,'MarkerSize',8, 'MarkerFaceColor','r');
loglog(node_2, array_load , '--sk', 'LineWidth',1.2,'MarkerSize',8, 'MarkerFaceColor','k');
grid on;
box on;
xlim([1e1,1e5]);
ylim([1e-4,1e4]);
xlabel('number of nodes','FontSize',12);%, 'FontWeight','bold');
ylabel('Processing Time (s)','FontSize',12);%, 'FontWeight','bold');
legend({'Computation time','Search time', 'Loading time'}, 'Location','northwest');
%%
% figure;
loglog(node_1, snr_tree_computation , '--ob', 'LineWidth',1.2,'MarkerSize',8, 'MarkerFaceColor','b');
hold on;
loglog(node_2, snr_array , '--dr', 'LineWidth',1.2,'MarkerSize',8, 'MarkerFaceColor','r');
grid on;
box on;
xlabel('number of nodes','FontSize',12);%, 'FontWeight','bold');
ylabel('Processing Time (s)','FontSize',12);%, 'FontWeight','bold');
legend({'spatial indexing','array indexing'}, 'Location','northwest');
%% SINR Data

node_3 = [10, 100, 2e2, 3e2, 5e2, 1e3, 2e3, 3e3, 5e3, 1e4, 2e4];%, 3e4, 5e4, 1e5];

% sinr_tree_computation = [4.00E-04,9.800E-04,26e-4, 55e-4, 228e-4, 0.0488, 0.15,0.99,3.15,80,560,1605];

sinr_tree_computation = [4.38e-4, 1.03e-3, 2.88e-3, 6.01e-3, 2.35e-2, 0.169, 1.114, 3.41, 13.77, 90.84, 603.498];
sinr_tree_search = [1.25e-4, 2.897e-4, 1e-3, 2.52e-3, 1.37e-2 ,0.1298, 0.813, 2.468, 10.74, 70.67, 476.625];
sinr_tree_load = [3.16e-4, 4.93e-4, 7.79e-4, 1.3e-3, 2.65e-3 ,8.57e-3, 0.03, 0.0614, 0.154, 0.594, 2.28];%, 5.100, 13.766, 54.45];

node_4 = [10, 100, 2e2, 3e2, 5e2, 1e3];%, 2e3, 3e3, 5e3, 1e4, 2e4];

sinr_array_computation = [38.2e-4, 1.452, 9.6511, 36.38, 133.68 ,1171.79];%, 0.15,0.99,3.15,80,560,1605];
sinr_array_search = [3.6e-4, 0.2055, 1.646, 5.438, 24.42, 205.2062];
sinr_array_load = [2.76e-4, 3.85e-4, 5.15e-4, 6.38e-4, 8.67e-4, 2e-3];

% sinr_array = [0.0045,1.49,10,27,166,292, 1436];
% sinr_ratio=sinr_array./sinr_tree(1:size(sinr_array,2));
%% SINR
figure;
% loglog(node_3, sinr_tree_computation , '--ob', 'LineWidth',1.2,'MarkerSize',8, 'MarkerFaceColor','b');

loglog(node_3, sinr_tree_search , '--dr', 'LineWidth',1.2,'MarkerSize',8, 'MarkerFaceColor','r');
hold on;
% loglog(node_3, sinr_tree_load , '--sk', 'LineWidth',1.2,'MarkerSize',8, 'MarkerFaceColor','k');
% loglog(node_4, sinr_array_computation , '--sr', 'LineWidth',1.2,'MarkerSize',8, 'MarkerFaceColor','r');
loglog(node_4, sinr_array_search , '--ob', 'LineWidth',1.2,'MarkerSize',8, 'MarkerFaceColor','b');
grid on;
box on;
xlim([1e1,1e5]);
ylim([1e-4,1e4]);
xlabel('number of nodes','FontSize',12);%, 'FontWeight','bold');
ylabel('Processing time (s)','FontSize',12);%, 'FontWeight','bold');
% legend({'Computation time','Search time', 'Loading time'}, 'Location','northwest');
legend({'Spatial indexing search time', 'Array indexing search time'}, 'Location','northwest');
% legend({'Spatial indexing computation time','Spatial indexing search time', 'Array indexing computation time','Array indexing search time'}, 'Location','northwest');
%% SINR
figure;
loglog(node_4, sinr_array_computation , '--ob', 'LineWidth',1.2,'MarkerSize',8, 'MarkerFaceColor','b');
hold on;
loglog(node_4, sinr_array_search , '--dr', 'LineWidth',1.2,'MarkerSize',8, 'MarkerFaceColor','r');
loglog(node_4, sinr_array_load , '--sk', 'LineWidth',1.2,'MarkerSize',8, 'MarkerFaceColor','k');
grid on;
box on;
xlim([1e1,1e5]);
ylim([1e-4,1e4]);
xlabel('number of nodes','FontSize',12);%, 'FontWeight','bold');
ylabel('Processing Time (s)','FontSize',12);%, 'FontWeight','bold');
legend({'Computation time','Search time', 'Loading time'}, 'Location','northwest');
%% SINR
figure;
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
%% SINR  , markers = {'o' , '+','*','.','x','s','d','^','v','>','<','p','h'} ;
figure;
loglog(node_3, sinr_tree_computation , '--ob', 'LineWidth',1.2,'MarkerSize',8, 'MarkerFaceColor','b');
hold on;
loglog(node_3, sinr_tree_search , '--or', 'LineWidth',1.2,'MarkerSize',8, 'MarkerFaceColor','r');
loglog(node_3, sinr_tree_load , '--ok', 'LineWidth',1.2,'MarkerSize',8, 'MarkerFaceColor','k');

loglog(node_4, sinr_array_computation , '--*b', 'LineWidth',1.2,'MarkerSize',8, 'MarkerFaceColor','b');
loglog(node_4, sinr_array_search , '--dr', 'LineWidth',1.2,'MarkerSize',8, 'MarkerFaceColor','r');
loglog(node_4, sinr_array_load , '--dk', 'LineWidth',1.2,'MarkerSize',8, 'MarkerFaceColor','k');

grid on;
box on;
xlim([1e1,1e5]);
ylim([1e-4,1e4]);
xlabel('number of nodes','FontSize',12);%, 'FontWeight','bold');
ylabel('Processing Time (s)','FontSize',12);%, 'FontWeight','bold');
legend({'Computation time','Search time', 'Loading time'}, 'Location','northwest');

