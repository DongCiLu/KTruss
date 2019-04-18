close all;
clear;
load('singlev_k_compare.mat');
k_exact = k_exact_2;
num_graphs = 8;
num_columns = 4;
num_rows = 2;

title_string = {'Wiki' ...
    'Baidu' ...
    'Skitter' ...
    'Sinaweibo' ...
    'Livejournal' ...
    'Orkut' ...        
    'Bio'...
    'Hollywood'};
% Sequence on file:
% facebook, wiki, skitter, baidu, Livejournal, 
% orkut, sinaweibo, hollywood, bio
sequence = [2 4 3 7 5 6 9 8];
legend_string = {'SingleV Query on 2-level index'...
    'SingleV Query on TCP index'...
    'SingleV Query on Equitruss index'};

figure(1);
for idx = 1:numel(sequence)
    graph_i = sequence(idx);
    subplot(num_rows, num_columns, idx);
    plot((k_info(graph_i,1:10) + k_exact(graph_i,1:10)) / (1000 * 100), '-+', 'LineWidth', 2, 'Markers', 7); hold on;
    plot(k_tcp(graph_i,1:10) / (1000 * 100), '--^', 'LineWidth', 2, 'Markers', 7); hold on;
    plot(k_equi(graph_i,1:10) / (1000 * 100), '--o', 'LineWidth', 2, 'Markers', 7);
    
    xlim([1 10]);
    set(gca,'XTick',(1:2:10));
    set(gca,'XTickLabel',{'10'; '30'; '50'; '70'; '90'});
    set(gca, 'Fontname', 'Times New Roman', 'Fontsize', 20);
    set(gca, 'YScale', 'log');
    
    data = [k_equi(graph_i,1:10) / (1000 * 100), ...
        k_tcp(graph_i,1:10) / (1000 * 100), ...
        (k_info(graph_i,1:10) + k_exact(graph_i,1:10)) / (1000 * 100)];
    y_max = max(data);
    y_min = min(data);
    %y_min = max(y_min, 0.002);
    ylim([y_min/3, y_max*3]);
    xlabel_hand=xlabel({'Degree rank (%)';' '});
    set(xlabel_hand,'Fontname', 'Times New Roman', 'Fontsize', 20);
    ylabel_hand=ylabel('Average time (s)');
    set(ylabel_hand,'Fontname', 'Times New Roman', 'Fontsize', 20);
    title(title_string(idx));
end

legend_hand = legend(legend_string,...
    'Orientation', 'horizontal',...
    'Position', [0.1 0 0.8 0.05]);
set(legend_hand,'Fontname', 'Times New Roman', ...
    'Fontsize', 20, 'box', 'off');

fig = gcf;
fig.PaperUnits = 'inches';
fig.PaperSize = [16 9];
fig.PaperPosition = [-1.5 0 19 9];
print('singlev_k_compare.pdf', '-dpdf')