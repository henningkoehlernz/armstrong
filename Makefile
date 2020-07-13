#LINK = -static -lboost_log -lboost_system -lboost_thread -lpthread
LINK = -DBOOST_LOG_DYN_LINK -DBOOST_TEST_DYN_LINK \
-DBOOST_LIB_SUFFIX=-mt \
-lboost_log -lboost_log_setup \
-lboost_system \
-lboost_thread \
-lpthread \
-lboost_unit_test_framework \
-lboost_program_options
CC = g++ -std=c++2a -O2 -Wall -g
#Ubunto: sudo apt install clang libc++-dev libc++abi-dev
#CC = clang++ -std=c++17 -stdlib=libc++ -O2 -Wall
armstrong:
	$(CC) -o armstrong Armstrong.cpp AgreeSetGraph.cpp $(LINK)
informative:
	$(CC) -o informative InformativeArmstrong.cpp InformativeGraph.cpp $(LINK)
miner:
	$(CC) -o miner AgreeSetMinerCSV.cpp CSVUtil.cpp AgreeSetUtil.cpp AgreeSetMiner.cpp $(LINK)
edgeminer:
	$(CC) -o edgeMiner AgreeSetEdgeMinerCSV.cpp CSVUtil.cpp AgreeSetUtil.cpp AgreeSetMiner.cpp AgreeSetEdgeMiner.cpp $(LINK)
random:
	$(CC) -o random RandomArmstrong.cpp AgreeSetUtil.cpp AgreeSetMiner.cpp AgreeSetGraph.cpp $(LINK)
test: testASG testASM testASEM testTrie testIG
# add this to generate core dumps: --catch_system_errors=no
testASG:
	$(CC) -o testASG TestAgreeSetGraph.cpp AgreeSetGraph.cpp $(LINK)
	./testASG
testASM:
	$(CC) -o testASM TestAgreeSetMiner.cpp AgreeSetUtil.cpp AgreeSetMiner.cpp $(LINK)
	./testASM
testASEM:
	$(CC) -o testASEM TestAgreeSetEdgeMiner.cpp AgreeSetUtil.cpp AgreeSetMiner.cpp AgreeSetEdgeMiner.cpp $(LINK)
	./testASEM
testTrie:
	$(CC) -o testTrie TestOrderedTrie.cpp $(LINK)
	./testTrie
testIG:
	$(CC) -o testIG TestInformativeGraph.cpp InformativeGraph.cpp DominanceGraph.cpp $(LINK)
	./testIG
clean:
	rm armstrong informative miner edgeMiner random testASG testASM testASEM testTrie testIG
.PHONY: armstrong informative miner edgeMiner random testASG testASM testASEM testIG
