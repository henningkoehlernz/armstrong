#LINK = -static -lboost_log -lboost_system -lboost_thread -lpthread
LINK = -DBOOST_LOG_DYN_LINK -DBOOST_TEST_DYN_LINK \
-DBOOST_LIB_SUFFIX=-mt \
-lboost_log -lboost_log_setup \
-lboost_system \
-lboost_thread \
-lpthread \
-lboost_unit_test_framework \
-lboost_program_options
CC = g++ -std=c++17 -O2 -Wall -g
#CC = clang -stdlib=libc++ -O2 -Wall
default:
	$(CC) -o armstrong Armstrong.cpp AgreeSetGraph.cpp $(LINK)
miner:
	$(CC) -o miner AgreeSetMiner.cpp AgreeSetMinerCSV.cpp $(LINK)
random:
	$(CC) -o random RandomArmstrong.cpp AgreeSetMiner.cpp AgreeSetGraph.cpp $(LINK)
test:
	$(CC) -o testASG TestAgreeSetGraph.cpp AgreeSetGraph.cpp $(LINK)
	$(CC) -o testASM TestAgreeSetMiner.cpp AgreeSetMiner.cpp $(LINK)
	./testASG
	./testASM
clean:
	rm armstrong miner test
.PHONY: default miner random testASG testASM
