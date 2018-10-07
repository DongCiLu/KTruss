# small networks
./main index datasets/facebook_wccnse.txt &> log_facebook_index.txt
./main index datasets/wiki_wccnse.txt &> log_wiki_index.txt
./main index datasets/skitter_wccnse.txt &> log_skitter_index.txt
./main index datasets/Livejournal_wccnse.txt &> log_Livejournal_index.txt
./main index datasets/baidu_wccnse.txt &> log_baidu_index.txt

./main testcase datasets/facebook_wccnse.txt &> log_facebook_testcase.txt
./main testcase datasets/wiki_wccnse.txt &> log_wiki_testcase.txt
./main testcase datasets/skitter_wccnse.txt &> log_skitter_testcase.txt
./main testcase datasets/Livejournal_wccnse.txt &> log_Livejournal_testcase.txt
./main testcase datasets/baidu_wccnse.txt &> log_baidu_testcase.txt

./main query datasets/facebook_wccnse.txt datasets/testcases_truss/facebook_wccnse.txt_testcase_single_q 10 1000 &> log_facebook_query.txt
./main query datasets/wiki_wccnse.txt datasets/testcases_truss/wiki_wccnse.txt_testcase_single_q 10 1000 &> log_wiki_query.txt
./main query datasets/skitter_wccnse.txt datasets/testcases_truss/skitter_wccnse.txt_testcase_single_q 10 1000 &> log_skitter_query.txt
./main query datasets/Livejournal_wccnse.txt datasets/testcases_truss/Livejournal_wccnse.txt_testcase_single_q 10 1000 &> log_Livejournal_query.txt
./main query datasets/Livejournal_wccnse.txt datasets/testcases_truss/Livejournal_wccnse.txt_testcase_single_q 10 1000 &> log_Livejournal_query.txt
./main query datasets/baidu_wccnse.txt datasets/testcases_truss/baidu_wccnse.txt_testcase_single_q 10 1000 &> log_baidu_query.txt

# large networks
./main index datasets/orkut_wccnse.txt &> log_orkut_index.txt
./main index datasets/sinaweibo_wccnse.txt &> log_sinaweibo_index.txt
# ./main index datasets/hollywood_wccnse.txt &> log_hollywood_index.txt
# ./main index datasets/bio_wccnse.txt &> log_bio_index.txt

./main testcase datasets/orkut_wccnse.txt &> log_orkut_testcase.txt
./main testcase datasets/sinaweibo_wccnse.txt &> log_sinaweibo_testcase.txt
# ./main index datasets/hollywood_wccnse.txt &> log_hollywood_testcase.txt
# ./main index datasets/bio_wccnse.txt &> log_bio_testcase.txt

./main query datasets/orkut_wccnse.txt datasets/testcases_truss/orkut_wccnse.txt_testcase_single_q 10 1000 &> log_orkut_query.txt
./main query datasets/sinaweibo_wccnse.txt datasets/testcases_truss/sinaweibo_wccnse.txt_testcase_single_q 10 1000 &> log_sinaweibo_query.txt
# ./main query datasets/hollywood_wccnse.txt datasets/testcases_truss/hollywood_wccnse.txt_testcase_single_q 10 1000 &> log_hollywood_query.txt
# ./main query datasets/bio_wccnse.txt datasets/testcases_truss/bio_wccnse.txt_testcase_single_q 10 1000 &> log_bio_query.txt
