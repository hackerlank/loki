export COMMON_HEAD_DIR = -I../base -I./  -I/usr/include/lua5.2 -I../lua_tinker -I/usr/local/include 
export COMMON_LIB_DIR = -L/usr/local/lib -L../base 
export COMMON_LIBS = -lseal -lpthread -lboost_system -lboost_chrono -lglog -lz -lprotobuf -llua5.2 -L../lua_tinker -llua_tinker \
	-lmysqlcppconn

LIBSEAL= ../base/libseal.a

header_dir = $(COMMON_HEAD_DIR)
lib_dir = $(COMMON_LIB_DIR)
LIBS = $(COMMON_LIBS) 

#-Wall
CXXFLAGS = -g -std=gnu++11 $(SEAL_COMPILE_OPT)  -D_GNU_SOURCE -D_REENTRANT $(header_dir)
CXXLFLAGS = -g -std=gnu++11 $(SEAL_COMPILE_OPT) -Wall $(lib_dir) $(LIBS)

objects = GateServer.o Main.o

SRCS = $(objects:%.o=%.cpp)
DEPS = $(objects:%.o=.%.d)

ALL_TARGETS = GateServer

all: $(ALL_TARGETS)

include $(DEPS)
$(DEPS): .%.d: %.cpp
	@$(CXX) -M $(CXXFLAGS) $< > $@.$$$$; sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; rm -f $@.$$$$

$(objects): %.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

$(ALL_TARGETS): $(objects) $(LIBSEAL)
	$(CXX) $(objects) -o $@ $(CXXLFLAGS)

clean:
	@rm -rf *.o .*.d* $(ALL_TARGETS) core *~
