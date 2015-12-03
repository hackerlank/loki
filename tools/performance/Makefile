COMMON_HEAD_DIR = -I./ -I/usr/local/include -I$(BOOST_PATH)
COMMON_LIB_DIR = -L/usr/local/lib -L$(BOOST_PATH)/stage/lib 
COMMON_LIBS = -lpthread -lboost_system 

COMPILE_OPT = -g -Werror -O2 -Wno-unused-local-typedefs 

CXX = g++ -std=c++11

CXXFLAGS = $(COMPILE_OPT) $(COMMON_HEAD_DIR)
CXXLFLAGS = $(COMPILE_OPT) -Wall $(COMMON_LIB_DIR) $(COMMON_LIBS)

server: server.cpp handler_allocator.hpp
	$(CXX) $(CXXFLAGS) server.cpp -o server $(CXXLFLAGS) 

client: client.cpp handler_allocator.hpp
	$(CXX) $(CXXFLAGS) client.cpp -o client $(CXXLFLAGS) 

clean:
	@rm -rf *.o .*.d* server client core *~
