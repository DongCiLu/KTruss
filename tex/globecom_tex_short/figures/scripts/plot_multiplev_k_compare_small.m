close all;
clear;
load('multiplev_3_info_compare.mat');
num_graphs = 5;
num_columns = 5;
num_rows = 1;

title_string = {
    'Skitter' ...
    'Sinaweibo' ...
    'Orkut' ...        
    'Bio' ...
    'Hollywood'};
% Sequence on file:
% facebook, wiki, skitter, baidu, Livejournal, 
% orkut, sinaweibo, hollywood, bio
sequence = [3 7 6 9 8];
legend_string = {'MultipleV Community-level Query on 2-level index'...
    'MultipleV Community-level Query on Equitruss index'};
ytickarray = [0.00001 1; 0.00001 10; 0.00001 100; 0.00001 100; 0.00001 100;];

figure(1);
for idx = 1:numel(sequence)
    graph_i = sequence(idx);
    subplot(num_rows, num_columns, idx);
    plot(k_info(graph_i,1:10) / (1000 * 100), '-+', 'LineWidth', 2, 'Markers', 7); hold on;
    plot(k_equi_info(graph_i,1:10) / (1000 * 100), '--o', 'LineWidth', 2, 'Markers', 7);
    
    xlim([1 10]);
    set(gca,'XTick',(1:2:10));
    set(gca,'XTickLabel',{'10'; '30'; '50'; '70'; '90'});
    set(gca, 'Fontname', 'Times New Roman', 'Fontsize', 20);
    set(gca, 'YScale', 'log');
    
    data = [k_equi_info(graph_i,1:10) / (1000 * 100), ...
        k_info(graph_i,1:10) / (1000 * 100)];
    y_max = max(data);
    y_min = min(data);
    %y_min = max(y_min, 0.002);
    %ylim([y_min/3, y_max*3]);
    ylim(ytickarray(idx,:));
    [y_min, y_max]
    set(gca, 'YTick', ytickarray(idx,:));
    xlabel_hand=xlabel({'Degree rank (%)';' '});
    set(xlabel_hand,'Fontname', 'Times New Roman', 'Fontsize', 20);
    ylabel_hand=ylabel('Average time (s)');
    set(ylabel_hand,'Fontname', 'Times New Roman', 'Fontsize', 20);
    title(title_string(idx));
end

legend_hand = legend(legend_string,...
    'Orientation', 'horizontal',...
    'Position', [0.1 0 0.8 0.07]);
set(legend_hand,'Fontname', 'Times New Roman', ...
    'Fontsize', 20, 'box', 'off');

fig = gcf;
fig.PaperUnits = 'inches';
fig.PaperSize = [16 3.5];
fig.PaperPosition = [-1.5 0 19 3.5];
print('multiplev_k_compare_small.pdf', '-dpdf')