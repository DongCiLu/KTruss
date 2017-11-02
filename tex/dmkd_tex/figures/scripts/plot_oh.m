k_all=x;
h=bar(k_all);
set(gca,'Xlim',[0 10],'XTick',1:1:9,'XTickLabel',{'Google' 'Wiki' 'Skitter' 'Gene' 'Baidu' 'Facebook' 'Livejournal' 'hollywood' 'friendster'});
set(gca,'Ylim',[0 1.2])
set(gca, 'Fontname', 'Times New Roman', 'Fontsize', 20);
ylabel_hand=ylabel('Overhead ratio compared to baseline');
set(ylabel_hand,'Fontname', 'Times New Roman', 'Fontsize', 20);
legend_hand = legend('Space overhead','Time overhead');
set(legend_hand,'Fontname', 'Times New Roman', 'Fontsize', 20);
xtickangle(-45)

fig = gcf;
fig.PaperUnits = 'inches';
fig.PaperSize = [8 6];
fig.PaperPosition = [0 0 8 6];
print('bar.pdf', '-dpdf')