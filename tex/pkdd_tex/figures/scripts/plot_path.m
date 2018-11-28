close all;
clear;
load('path.mat');

legend_string = {'Wiki' ...
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
line_spec = ["-+", "-*", "-x", "-o", "-^", "-d", "-s", "-p"];
% pathinfo_seq = [path_info(2,:); path_info(4,:); path_info(3,:); path_info(7,:);...
%     path_info(5,:); path_info(6,:); path_info(9,:); path_info(8,:)];
% pathexact_seq = [path_exact(2,:); path_exact(4,:); path_exact(3,:); path_exact(7,:);...
%     path_exact(5,:); path_exact(6,:); path_exact(9,:); path_exact(8,:)];
% path_seq = pathinfo_seq + pathexact_seq;

figure(1);
%h=plot(path_seq', 'LineWidth', 2);
for idx = 1:numel(sequence)
    graph_i = sequence(idx);
    plot((path_info(graph_i,:) + path_exact(graph_i,:)) / (1000 * 100), ...
        line_spec(idx), 'LineWidth', 2, 'Markers', 7); hold on;
end
xlim([1 10]);
set(gca,'XTick',(1:2:10));
set(gca,'XTickLabel',{'10'; '30'; '50'; '70'; '90'});
set(gca, 'Fontname', 'Times New Roman', 'Fontsize', 20);
set(gca,'Ylim',[0.1 200])
set(gca, 'YScale', 'log');
set(gca, 'Fontname', 'Times New Roman', 'Fontsize', 20);
xlabel_hand=xlabel({'Degree rank (%)';' '});
set(xlabel_hand,'Fontname', 'Times New Roman', 'Fontsize', 20);
ylabel_hand=ylabel('Average time (s)');
set(ylabel_hand,'Fontname', 'Times New Roman', 'Fontsize', 20);
legend_hand = legend(legend_string);
set(legend_hand,'Fontname', 'Times New Roman', 'Fontsize', 14, ...
    "Location", "southwest", 'NumColumns', 2);

fig = gcf;
fig.PaperUnits = 'inches';
fig.PaperSize = [8 6];
fig.PaperPosition = [0 0 8 6];
print('path.pdf', '-dpdf')