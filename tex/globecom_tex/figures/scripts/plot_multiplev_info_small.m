close all;
clear;
load('multiplev_3_info_query.mat');
%load('multiplev_2_info_query.mat');

num_graphs = 5;
num_columns = 5;
num_rows = 1;

title_string = {
    'Skitter' ...
    'Sinaweibo' ...
    'Orkut' ...        
    'Bio'...
    'Hollywood'};
% Sequence on file:
% facebook, wiki, skitter, baidu, Livejournal, 
% orkut, sinaweibo, hollywood, bio
sequence = [3 7 6 9 8];
legend_string = {'MultipleV K info Query'...
    'MultipleV Max-K info Query'...    
    'MultipleV Any-K info Query'...
    'MultipleV K search Query'...
    'MultipleV Max-K search Query'...
    'MultipleV Any-K search Query'};

figure(1);
for idx = 1:numel(sequence)
    graph_i = sequence(idx);
    subplot(num_rows, num_columns, idx);
    plot(k_info(graph_i,1:10) / (1000 * 100), '-+', 'LineWidth', 2, 'Markers', 7); hold on;
    plot(maxk_info(graph_i,1:10) / (1000 * 100), '-*', 'LineWidth', 2, 'Markers', 7); hold on;
    plot(anyk_info(graph_i,1:10) / (1000 * 100), '-x', 'LineWidth', 2, 'Markers', 7); hold on;
    plot((k_exact(graph_i,1:10) + k_info(graph_i,1:10)) / (1000 * 100), '-s', 'LineWidth', 2, 'Markers', 7); hold on;
    plot((maxk_exact(graph_i,1:10) + maxk_info(graph_i,1:10)) / (1000 * 100), '-^', 'LineWidth', 2, 'Markers', 7); hold on;
    plot((anyk_exact(graph_i,1:10) + anyk_info(graph_i,1:10)) / (1000 * 100), '-o', 'LineWidth', 2, 'Markers', 7);
    
    xlim([1 10]);
    set(gca,'XTick',(1:2:10));
    set(gca,'XTickLabel',{'10'; '30'; '50'; '70'; '90'});
    set(gca, 'Fontname', 'Times New Roman', 'Fontsize', 20);
    set(gca, 'YScale', 'log');
    
    data = [k_info(graph_i,1:10) / (1000 * 100)...
        maxk_info(graph_i,1:10) / (1000 * 100)...
        anyk_info(graph_i,1:10) / (1000 * 100)...
        (k_exact(graph_i,1:10) + k_info(graph_i,1:10)) / (1000 * 100)...
        (maxk_exact(graph_i,1:10) + maxk_info(graph_i,1:10)) / (1000 * 100)...
        (anyk_exact(graph_i,1:10) + anyk_info(graph_i,1:10)) / (1000 * 100)];
    y_max = max(data);
    y_min = min(data);
    ylim([y_min/3, y_max*3]);
    xlabel_hand=xlabel({'Degree rank (%)';' '});
    set(xlabel_hand,'Fontname', 'Times New Roman', 'Fontsize', 20);
    ylabel_hand=ylabel('Average time (s)');
    set(ylabel_hand,'Fontname', 'Times New Roman', 'Fontsize', 20);
    title(title_string(idx));
end

legend_hand = legend(legend_string,...
    'Orientation', 'horizontal',...
    'Position', [0.1 -0.08 0.8 0.1]);
set(legend_hand,'Fontname', 'Times New Roman', ...
    'Fontsize', 20, 'NumColumns', 3, 'box', 'off');

fig = gcf;
fig.PaperUnits = 'inches';
fig.PaperSize = [16 4.6];
fig.PaperPosition = [-1.5 0.6 19 4];
print('multiplev_3_info_query_small.pdf', '-dpdf')