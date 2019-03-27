close all;
clear;
load('boundary_outter_size.mat');

title_string = {'Wiki' ...
    'Baidu' ...
    'Skitter' ...
    'Sinaweibo' ...
    'Livejournal' ...
    'Orkut' ...        
    'Bio'...
    'Hollywood'};

figure(1);
subplot(2, 4, 1);
scatter(wiki_k,wiki_size);
xlim([0 60]);
%ylim([1 70]);
xlabel_hand=xlabel('Trussness');
ylabel_hand=ylabel('Boundary length');
title(title_string(1));
set(gca, 'Fontname', 'Times New Roman', 'Fontsize', 20);
set(gca, 'YScale', 'log');
subplot(2, 4, 2);
scatter(baidu_k,baidu_size);
%xlim([0 10]);
ylim([1 10000000]);
xlabel_hand=xlabel('Trussness');
ylabel_hand=ylabel('Boundary length');
title(title_string(2));
set(gca, 'Fontname', 'Times New Roman', 'Fontsize', 20);
set(gca, 'YScale', 'log');
subplot(2, 4, 3);
scatter(skitter_k,skitter_size);
xlim([0 70]);
ylim([1 10000000]);
xlabel_hand=xlabel('Trussness');
ylabel_hand=ylabel('Boundary length');
title(title_string(3));
set(gca, 'Fontname', 'Times New Roman', 'Fontsize', 20);
set(gca, 'YScale', 'log');
subplot(2, 4, 4);
scatter(sinaweibo_k,sinaweibo_size);
xlim([0 60]);
ylim([1 10000000]);
xlabel_hand=xlabel('Trussness');
ylabel_hand=ylabel('Boundary length');
title(title_string(4));
set(gca, 'Fontname', 'Times New Roman', 'Fontsize', 20);
set(gca, 'YScale', 'log');
subplot(2, 4, 5);
scatter(Livejournal_k,Livejournal_size);
xlim([0 70]);
ylim([1 10000000]);
xlabel_hand=xlabel('Trussness');
ylabel_hand=ylabel('Boundary length');
title(title_string(5));
set(gca, 'Fontname', 'Times New Roman', 'Fontsize', 20);
set(gca, 'YScale', 'log');
subplot(2, 4, 6);
scatter(orkut_k,orkut_size);
xlim([0 60]);
ylim([1 10000000]);
xlabel_hand=xlabel('Trussness');
ylabel_hand=ylabel('Boundary length');
title(title_string(6));
set(gca, 'Fontname', 'Times New Roman', 'Fontsize', 20);
set(gca, 'YScale', 'log');
subplot(2, 4, 7);
scatter(bio_k,bio_size);
xlim([0 800]);
%ylim([1 10000000]);
xlabel_hand=xlabel('Trussness');
ylabel_hand=ylabel('Boundary length');
title(title_string(7));
set(gca, 'Fontname', 'Times New Roman', 'Fontsize', 20);
set(gca, 'YScale', 'log');
subplot(2, 4, 8);
scatter(hollywood_k,hollywood_size);
xlim([0 200]);
%ylim([1 10000000]);
xlabel_hand=xlabel('Trussness');
ylabel_hand=ylabel('Boundary length');
title(title_string(8));
set(gca, 'Fontname', 'Times New Roman', 'Fontsize', 20);
set(gca, 'YScale', 'log');

fig = gcf;
fig.PaperUnits = 'inches';
fig.PaperSize = [16 8];
fig.PaperPosition = [0 0 16 8];
print('boundary_size.pdf', '-dpdf')