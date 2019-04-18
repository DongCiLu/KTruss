close all;
clear;
load('singlev_k_compare.mat');
k_exact = k_exact_2;
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
legend_string = {'SingleV Query on 2-level index'...
    'SingleV Query on TCP index'...
    'SingleV Query on Equitruss index'};
ytickarray = [0.01 10; 0.000001 100; 1 100; 1 100; 1 1000;];

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
    %ylim([y_min/3, y_max*3]);
    ylim(ytickarray(idx,:));
    %[y_min, y_max]
    %ytickarray(idx,:)
    %set(gca, 'YTick', ytickarray(idx,:));
    
    Fontsize = 15;
    yl = get(gca,'YLabel');
    ylFontSize = get(yl,'FontSize');
    yAX = get(gca,'YAxis');
    set(yAX,'FontSize', Fontsize)
    set(yl, 'FontSize', ylFontSize);
    
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
print('singlev_k_compare_small.pdf', '-dpdf')