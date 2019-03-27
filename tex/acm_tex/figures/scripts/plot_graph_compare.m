close all;
clear;
load('graphsize.mat');

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
twolevelnodes_seq = [twolevelnodes(2), twolevelnodes(4), twolevelnodes(3), twolevelnodes(7),...
    twolevelnodes(5), twolevelnodes(6), twolevelnodes(9), twolevelnodes(8)];
equinodes_seq = [equinodes(2), equinodes(4), equinodes(3), equinodes(7),...
    equinodes(5), equinodes(6), equinodes(9), equinodes(8)];
twoleveledges_seq = [twoleveledges(2), twoleveledges(4), twoleveledges(3), twoleveledges(7),...
    twoleveledges(5), twoleveledges(6), twoleveledges(9), twoleveledges(8)];
equiedges_seq = [equiedges(2), equiedges(4), equiedges(3), equiedges(7),...
    equiedges(5), equiedges(6), equiedges(9), equiedges(8)];

figure(1);
h=bar([twolevelnodes_seq', equinodes_seq']);
%h(1).FaceColor = 'm';
% ybuff=2;
% for i=1:length(h)
%     XDATA=bsxfun(@plus, h(1).XData, [h.XOffset]');
%     YDATA=cat(1, h.YData);
%     for j=1:size(XDATA, 2)
%         x=XDATA(i,j);
%         y=YDATA(i,j);
%         if y > 1000000
%             scale=1000000;
%             unit='M';
%         elseif y > 1000
%             scale=1000;
%             unit='K';
%         else
%             scale=1;
%             unit='';
%         end
%         t=[num2str(y / scale, 3) ,unit];
%         text(x,y,t,'Color','k','HorizontalAlignment','left','Rotation',90,'FontSize',14)
%     end
% end

set(gca,'XTickLabel',title_string);
%set(gca,'Xlim',[0 10],'XTick',1:1:9,'XTickLabel',title_string);
set(gca,'Ylim',[1 300000000])
set(gca, 'YScale', 'log');
set(gca, 'Fontname', 'Times New Roman', 'Fontsize', 20);
ylabel_hand=ylabel('Number of super vertices');
set(ylabel_hand,'Fontname', 'Times New Roman', 'Fontsize', 20);
legend_hand = legend('2-level index','Equitruss');
set(legend_hand,'Fontname', 'Times New Roman', 'Fontsize', 20, 'Location', 'northwest');
xtickangle(45)

fig = gcf;
fig.PaperUnits = 'inches';
fig.PaperSize = [8 6];
fig.PaperPosition = [0 0 8 6];
print('super_node_compare.pdf', '-dpdf')

figure(2);
h=bar([twoleveledges_seq', equiedges_seq']);

% ybuff=2;
% for i=1:length(h)
%     XDATA=bsxfun(@plus, h(1).XData, [h.XOffset]');
%     YDATA=cat(1, h.YData);
%     for j=1:size(XDATA, 2)
%         x=XDATA(i,j);
%         y=YDATA(i,j);
%         if y > 1000000
%             scale=1000000;
%             unit='M';
%         elseif y > 1000
%             scale=1000;
%             unit='K';
%         else
%             scale=1;
%             unit='';
%         end
%         t=[num2str(y / scale, 3) ,unit];
%         text(x,y,t,'Color','k','HorizontalAlignment','left','Rotation',90,'FontSize',14)
%     end
% end

set(gca,'XTickLabel',title_string);
%set(gca,'Xlim',[0 10],'XTick',1:1:9,'XTickLabel',title_string);
set(gca,'Ylim',[1 1000000000])
set(gca, 'YScale', 'log');
set(gca, 'Fontname', 'Times New Roman', 'Fontsize', 20);
ylabel_hand=ylabel('Number of super edges');
set(ylabel_hand,'Fontname', 'Times New Roman', 'Fontsize', 20);
legend_hand = legend('2-level index','Equitruss');
set(legend_hand,'Fontname', 'Times New Roman', 'Fontsize', 20, 'Location', 'northwest');
xtickangle(45)

fig = gcf;
fig.PaperUnits = 'inches';
fig.PaperSize = [8 6];
fig.PaperPosition = [0 0 8 6];
print('super_edge_compare.pdf', '-dpdf')