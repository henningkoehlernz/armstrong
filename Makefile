#LINK = -static -lboost_log -lboost_system -lboost_thread -lpthread
LINK = -DBOOST_LOG_DYN_LINK -DBOOST_TEST_DYN_LINK -lboost_log -lboost_system -lboost_thread -lpthread -lboost_unit_test_framework
CC = g++ -std=c++11 -O2 -Wall
default:
	$(CC) -o armstrong Armstrong.cpp AgreeSetGraph.cpp $(LINK)
miner:
	$(CC) -o miner AgreeSetMiner.cpp $(LINK)
test:
	$(CC) -o test TestAgreeSetGraph.cpp AgreeSetGraph.cpp $(LINK)
	./test
clean:
	rm armstrong test
