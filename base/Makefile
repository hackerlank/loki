
compile_debug = -g -Wall 
compile_relase = -g -Wall -O1

compile_opt = $(compile_debug) $(SEAL_COMPILE_OPT)

HEADER_DIR = -I./ -I/usr/include/lua5.2 -I../lua_tinker -I/usr/local/include `xml2-config --cflags` 
LIB_DIR = -L../lua_tinker
#LIBS = -lboost_system -lpthread -lboost_thread -lmysqlcppconn -lrt
LIBS = -lpthread -lboost_system  `xml2-config --libs` -llua_tinker

#CXX = g++
CXXFLAGS = $(compile_opt) $(HEADER_DIR) -D_REENTRANT -D_GNU_SOURCE
CXXLFLAGS = $(compile_opt) $(LIB_DIR) $(LIBS)

objects = db.o ServerList.o context.o logger.o
objects += TcpConn.o SubService.o
objects += ServerManager.o
objects += util.o

objects += Login.pb.o Super.pb.o Record.pb.o Session.pb.o Scene.pb.o Gate.pb.o Cmd.pb.o
objects += zMisc.o zProperties.o zDatabaseManager.o
objects += Service.o

sources = $(objects:%.o=%.cpp)
deps = $(objects:%.o=.%.d)

#libseal = libseal.a

all_targets = libseal.a

.PHONY: all clean

all: $(all_targets)

include $(deps)
$(deps): .%.d: %.cpp
	@$(CXX) -M $(CXXFLAGS) $< > $@.$$$$; \
		sed 's,\($*\).o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
		rm -f $@.$$$$

$(objects): %.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

#因为这个导致libseal.a 产生不出来
#$(CXX) $(objects) -o $@ $(CXXLFLAGS)
$(all_targets): $(objects) Makefile
	@rm -rf $(all_targets)
	@ar cru $(all_targets) $(objects)
	@ranlib $(all_targets)

clean:
	@rm -rf *.o .*.d $(all_targets) core *~
