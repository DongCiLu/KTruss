./main query datasets/facebook_wcc.txt datasets/testcases_truss/facebook_wcc.txt_testcase_single_q 10 1000 &> log_facebook 
./main query datasets/wiki_wcc.txt datasets/testcases_truss/wiki_wcc.txt_testcase_single_q 10 1000 &> log_wiki 
./main query datasets/Livejournal_wcc.txt datasets/testcases_truss/Livejournal_wcc.txt_testcase_single_q 10 1000 &> log_Livejournal 
./main query datasets/baidu_wcc.txt datasets/testcases_truss/baidu_wcc.txt_testcase_single_q 10 1000 &> log_baidu 
./main query datasets/orkut_wcc.txt datasets/testcases_truss/orkut_wcc.txt_testcase_single_q 10 1000 &> log_orkut 
./main query datasets/sinaweibo_wcc.txt datasets/testcases_truss/sinaweibo_wcc.txt_testcase_single_q 10 1000 &> log_sinaweibo 

./main query datasets/hollywood_wcc.txt datasets/testcases_truss/hollywood_wcc.txt_testcase_single_q 10 1000 &> log_hollywood 
# ./main query datasets/bio_wcc.txt datasets/testcases_truss/bio_wcc.txt_testcase_single_q 10 1000 &> log_bio 

# ./main index datasets/facebook_wcc.txt > log_facebook
# ./main index datasets/wiki_wcc.txt > log_wiki
# ./main index datasets/skitter_wcc.txt > log_skitter
# ./main index datasets/Livejournal_wcc.txt > log_Livejournal
# ./main index datasets/baidu_wcc.txt > log_baidu
# ./main index datasets/orkut_wcc.txt > log_orkut
# ./main index datasets/sinaweibo_wcc.txt > log_sinaweibo

# ./main index datasets/hollywood_wcc.txt > log_hollywood
# ./main index datasets/bio_wcc.txt > log_bio
