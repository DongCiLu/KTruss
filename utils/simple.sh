# small networks
# ./main index datasets/facebook_wccnse.txt &> log_facebook_index.txt
# ./main index datasets/wiki_wccnse.txt &> log_wiki_index.txt
# ./main index datasets/skitter_wccnse.txt &> log_skitter_index.txt
# ./main index datasets/Livejournal_wccnse.txt &> log_Livejournal_index.txt
# ./main index datasets/baidu_wccnse.txt &> log_baidu_index.txt

./main testcase datasets/facebook_wccnse.txt 1 &> log_facebook_testcase1.txt
./main testcase datasets/wiki_wccnse.txt 1 &> log_wiki_testcase1.txt
./main testcase datasets/skitter_wccnse.txt 1 &> log_skitter_testcase1.txt
./main testcase datasets/Livejournal_wccnse.txt 1 &> log_Livejournal_testcase1.txt
./main testcase datasets/baidu_wccnse.txt 1 &> log_baidu_testcase1.txt

./main testcase datasets/facebook_wccnse.txt 2 &> log_facebook_testcase2.txt
./main testcase datasets/wiki_wccnse.txt 2 &> log_wiki_testcase2.txt
./main testcase datasets/skitter_wccnse.txt 2 &> log_skitter_testcase2.txt
./main testcase datasets/Livejournal_wccnse.txt 2 &> log_Livejournal_testcase2.txt
./main testcase datasets/baidu_wccnse.txt 2 &> log_baidu_testcase2.txt

./main testcase datasets/facebook_wccnse.txt 3 &> log_facebook_testcase3.txt
./main testcase datasets/wiki_wccnse.txt 3 &> log_wiki_testcase3.txt
./main testcase datasets/skitter_wccnse.txt 3 &> log_skitter_testcase3.txt
./main testcase datasets/Livejournal_wccnse.txt 3 &> log_Livejournal_testcase3.txt
./main testcase datasets/baidu_wccnse.txt 3 &> log_baidu_testcase3.txt

./main query datasets/facebook_wccnse.txt 1 10 1000 &> log_facebook_query_1_k.txt
./main query datasets/wiki_wccnse.txt 1 10 1000 &> log_wiki_query_1_k.txt
./main query datasets/skitter_wccnse.txt 1 10 1000 &> log_skitter_query_1_k.txt
./main query datasets/Livejournal_wccnse.txt 1 10 1000 &> log_Livejournal_query_1_k.txt
./main query datasets/baidu_wccnse.txt 1 10 1000 &> log_baidu_query_1_k.txt

./main query datasets/facebook_wccnse.txt 2 10 1000 &> log_facebook_query_2_k.txt
./main query datasets/wiki_wccnse.txt 2 10 1000 &> log_wiki_query_2_k.txt
./main query datasets/skitter_wccnse.txt 2 10 1000 &> log_skitter_query_2_k.txt
./main query datasets/Livejournal_wccnse.txt 2 10 1000 &> log_Livejournal_query_2_k.txt
./main query datasets/baidu_wccnse.txt 2 10 1000 &> log_baidu_query_2_k.txt

./main query datasets/facebook_wccnse.txt 3 10 1000 &> log_facebook_query_3_k.txt
./main query datasets/wiki_wccnse.txt 3 10 1000 &> log_wiki_query_3_k.txt
./main query datasets/skitter_wccnse.txt 3 10 1000 &> log_skitter_query_3_k.txt
./main query datasets/Livejournal_wccnse.txt 3 10 1000 &> log_Livejournal_query_3_k.txt
./main query datasets/baidu_wccnse.txt 3 10 1000 &> log_baidu_query_3_k.txt

./main query datasets/facebook_wccnse.txt 1 -1 1000 &> log_facebook_query_1_maxk.txt
./main query datasets/wiki_wccnse.txt 1 -1 1000 &> log_wiki_query_1_maxk.txt
./main query datasets/skitter_wccnse.txt 1 -1 1000 &> log_skitter_query_1_maxk.txt
./main query datasets/Livejournal_wccnse.txt 1 -1 1000 &> log_Livejournal_query_1_maxk.txt
./main query datasets/baidu_wccnse.txt 1 -1 1000 &> log_baidu_query_1_maxk.txt

./main query datasets/facebook_wccnse.txt 2 -1 1000 &> log_facebook_query_2_maxk.txt
./main query datasets/wiki_wccnse.txt 2 -1 1000 &> log_wiki_query_2_maxk.txt
./main query datasets/skitter_wccnse.txt 2 -1 1000 &> log_skitter_query_2_maxk.txt
./main query datasets/Livejournal_wccnse.txt 2 -1 1000 &> log_Livejournal_query_2_maxk.txt
./main query datasets/baidu_wccnse.txt 2 -1 1000 &> log_baidu_query_2_maxk.txt

./main query datasets/facebook_wccnse.txt 3 -1 1000 &> log_facebook_query_3_maxk.txt
./main query datasets/wiki_wccnse.txt 3 -1 1000 &> log_wiki_query_3_maxk.txt
./main query datasets/skitter_wccnse.txt 3 -1 1000 &> log_skitter_query_3_maxk.txt
./main query datasets/Livejournal_wccnse.txt 3 -1 1000 &> log_Livejournal_query_3_maxk.txt
./main query datasets/baidu_wccnse.txt 3 -1 1000 &> log_baidu_query_3_maxk.txt

./main query datasets/facebook_wccnse.txt 1 0 1000 &> log_facebook_query_1_anyk.txt
./main query datasets/wiki_wccnse.txt 1 0 1000 &> log_wiki_query_1_anyk.txt
./main query datasets/skitter_wccnse.txt 1 0 1000 &> log_skitter_query_1_anyk.txt
./main query datasets/Livejournal_wccnse.txt 1 0 1000 &> log_Livejournal_query_1_anyk.txt
./main query datasets/baidu_wccnse.txt 1 0 1000 &> log_baidu_query_1_anyk.txt

./main query datasets/facebook_wccnse.txt 2 0 1000 &> log_facebook_query_2_anyk.txt
./main query datasets/wiki_wccnse.txt 2 0 1000 &> log_wiki_query_2_anyk.txt
./main query datasets/skitter_wccnse.txt 2 0 1000 &> log_skitter_query_2_anyk.txt
./main query datasets/Livejournal_wccnse.txt 2 0 1000 &> log_Livejournal_query_2_anyk.txt
./main query datasets/baidu_wccnse.txt 2 0 1000 &> log_baidu_query_2_anyk.txt

./main query datasets/facebook_wccnse.txt 3 0 1000 &> log_facebook_query_3_anyk.txt
./main query datasets/wiki_wccnse.txt 3 0 1000 &> log_wiki_query_3_anyk.txt
./main query datasets/skitter_wccnse.txt 3 0 1000 &> log_skitter_query_3_anyk.txt
./main query datasets/Livejournal_wccnse.txt 3 0 1000 &> log_Livejournal_query_3_anyk.txt
./main query datasets/baidu_wccnse.txt 3 0 1000 &> log_baidu_query_3_anyk.txt

# large networks
# ./main index datasets/orkut_wccnse.txt &> log_orkut_index.txt
# ./main index datasets/sinaweibo_wccnse.txt &> log_sinaweibo_index.txt
# ./main index datasets/hollywood_wccnse.txt &> log_hollywood_index.txt
# ./main index datasets/bio_wccnse.txt &> log_bio_index.txt

./main testcase datasets/orkut_wccnse.txt 1 &> log_orkut_testcase1.txt
./main testcase datasets/sinaweibo_wccnse.txt 1 &> log_sinaweibo_testcase1.txt
./main testcase datasets/hollywood_wccnse.txt 1 &> log_hollywood_testcase1.txt
./main testcase datasets/bio_wccnse.txt 1 &> log_bio_testcase1.txt

./main testcase datasets/orkut_wccnse.txt 2 &> log_orkut_testcase2.txt
./main testcase datasets/sinaweibo_wccnse.txt 2 &> log_sinaweibo_testcase2.txt
./main testcase datasets/hollywood_wccnse.txt 2 &> log_hollywood_testcase2.txt
./main testcase datasets/bio_wccnse.txt 2 &> log_bio_testcase2.txt

./main testcase datasets/orkut_wccnse.txt 3 &> log_orkut_testcase3.txt
./main testcase datasets/sinaweibo_wccnse.txt 3 &> log_sinaweibo_testcase3.txt
./main testcase datasets/hollywood_wccnse.txt 3 &> log_hollywood_testcase3.txt
./main testcase datasets/bio_wccnse.txt 3 &> log_bio_testcase3.txt

./main query datasets/orkut_wccnse.txt 1 10 1000 &> log_orkut_query_1_k.txt
./main query datasets/sinaweibo_wccnse.txt 1 10 1000 &> log_sinaweibo_query_1_k.txt
./main query datasets/hollywood_wccnse.txt 1 10 1000 &> log_hollywood_query_1_k.txt
./main query datasets/bio_wccnse.txt 1 10 1000 &> log_bio_query_1_k.txt

./main query datasets/orkut_wccnse.txt 2 10 1000 &> log_orkut_query_2_k.txt
./main query datasets/sinaweibo_wccnse.txt 2 10 1000 &> log_sinaweibo_query_2_k.txt
./main query datasets/hollywood_wccnse.txt 2 10 1000 &> log_hollywood_query_2_k.txt
./main query datasets/bio_wccnse.txt 2 10 1000 &> log_bio_query_2_k.txt

./main query datasets/orkut_wccnse.txt 3 10 1000 &> log_orkut_query_3_k.txt
./main query datasets/sinaweibo_wccnse.txt 3 10 1000 &> log_sinaweibo_query_3_k.txt
./main query datasets/hollywood_wccnse.txt 3 10 1000 &> log_hollywood_query_3_k.txt
./main query datasets/bio_wccnse.txt 3 10 1000 &> log_bio_query_3_k.txt

./main query datasets/orkut_wccnse.txt 1 -1 1000 &> log_orkut_query_1_maxk.txt
./main query datasets/sinaweibo_wccnse.txt 1 -1 1000 &> log_sinaweibo_query_1_maxk.txt
./main query datasets/hollywood_wccnse.txt 1 -1 1000 &> log_hollywood_query_1_maxk.txt
./main query datasets/bio_wccnse.txt 1 -1 1000 &> log_bio_query_1_maxk.txt

./main query datasets/orkut_wccnse.txt 2 -1 1000 &> log_orkut_query_2_maxk.txt
./main query datasets/sinaweibo_wccnse.txt 2 -1 1000 &> log_sinaweibo_query_2_maxk.txt
./main query datasets/hollywood_wccnse.txt 2 -1 1000 &> log_hollywood_query_2_maxk.txt
./main query datasets/bio_wccnse.txt 2 -1 1000 &> log_bio_query_2_maxk.txt

./main query datasets/orkut_wccnse.txt 3 -1 1000 &> log_orkut_query_3_maxk.txt
./main query datasets/sinaweibo_wccnse.txt 3 -1 1000 &> log_sinaweibo_query_3_maxk.txt
./main query datasets/hollywood_wccnse.txt 3 -1 1000 &> log_hollywood_query_3_maxk.txt
./main query datasets/bio_wccnse.txt 3 -1 1000 &> log_bio_query_3_maxk.txt

./main query datasets/orkut_wccnse.txt 1 0 1000 &> log_orkut_query_1_anyk.txt
./main query datasets/sinaweibo_wccnse.txt 1 0 1000 &> log_sinaweibo_query_1_anyk.txt
./main query datasets/hollywood_wccnse.txt 1 0 1000 &> log_hollywood_query_1_anyk.txt
./main query datasets/bio_wccnse.txt 1 0 1000 &> log_bio_query_1_anyk.txt

./main query datasets/orkut_wccnse.txt 2 0 1000 &> log_orkut_query_2_anyk.txt
./main query datasets/sinaweibo_wccnse.txt 2 0 1000 &> log_sinaweibo_query_2_anyk.txt
./main query datasets/hollywood_wccnse.txt 2 0 1000 &> log_hollywood_query_2_anyk.txt
./main query datasets/bio_wccnse.txt 2 0 1000 &> log_bio_query_2_anyk.txt

./main query datasets/orkut_wccnse.txt 3 0 1000 &> log_orkut_query_3_anyk.txt
./main query datasets/sinaweibo_wccnse.txt 3 0 1000 &> log_sinaweibo_query_3_anyk.txt
./main query datasets/hollywood_wccnse.txt 3 0 1000 &> log_hollywood_query_3_anyk.txt
./main query datasets/bio_wccnse.txt 3 0 1000 &> log_bio_query_3_anyk.txt

# path query
./main query datasets/facebook_wccnse.txt 2 -2 1000 &> log_facebook_query_2_path.txt
./main query datasets/wiki_wccnse.txt 2 -2 1000 &> log_wiki_query_2_path.txt
./main query datasets/skitter_wccnse.txt 2 -2 1000 &> log_skitter_query_2_path.txt
./main query datasets/Livejournal_wccnse.txt 2 -2 1000 &> log_Livejournal_query_2_path.txt
./main query datasets/baidu_wccnse.txt 2 -2 1000 &> log_baidu_query_2_path.txt

./main query datasets/orkut_wccnse.txt 2 -2 1000 &> log_orkut_query_2_path.txt
./main query datasets/sinaweibo_wccnse.txt 2 -2 1000 &> log_sinaweibo_query_2_path.txt
./main query datasets/hollywood_wccnse.txt 2 -2 1000 &> log_hollywood_query_2_path.txt
./main query datasets/bio_wccnse.txt 2 -2 1000 &> log_bio_query_2_path.txt
