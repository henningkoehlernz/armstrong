#LINK = -static -lboost_log -lboost_system -lboost_thread -lpthread
LINK = -DBOOST_LOG_DYN_LINK -DBOOST_TEST_DYN_LINK -lboost_log -lboost_system -lboost_thread -lpthread -lboost_unit_test_framework
default:
	g++ -o armstrong -std=c++11 -O2 Armstrong.cpp AgreeSetGraph.cpp $(LINK)
test:
	g++ -o test -std=c++11 -O2 TestAgreeSetGraph.cpp AgreeSetGraph.cpp $(LINK)
	./test
