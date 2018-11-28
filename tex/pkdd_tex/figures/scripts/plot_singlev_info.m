close all;
clear;
load('singlev_info_query.mat');

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
legend_string = {'SingleV K info Query'...
    'SingleV Max-K info Query'...    
    'SingleV Any-K info Query'...
    'SingleV K search Query'...
    'SingleV Max-K search Query'...
    'SingleV Any-K search Query'};

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
    'Position', [0.1 -0.032 0.8 0.1]);
set(legend_hand,'Fontname', 'Times New Roman', ...
    'Fontsize', 20, 'NumColumns', 3, 'box', 'off');

fig = gcf;
fig.PaperUnits = 'inches';
fig.PaperSize = [16 9];
fig.PaperPosition = [-1.5 0.3 19 9];
print('singlev_info_query.pdf', '-dpdf')