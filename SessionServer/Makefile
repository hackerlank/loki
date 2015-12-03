header_dir = $(COMMON_HEAD_DIR)
lib_dir = $(COMMON_LIB_DIR)
LIBS = $(COMMON_LIBS)

LIBSEAL= ../base/libseal.a

#-Wall
CXXFLAGS = -std=gnu++11 $(SEAL_COMPILE_OPT)  -D_GNU_SOURCE -D_REENTRANT $(header_dir)
CXXLFLAGS = -std=gnu++11 $(SEAL_COMPILE_OPT) -Wall $(lib_dir) $(LIBS)

objects = Main.o SessionServer.o SessionTask.o SceneSession.o SceneSessionManager.o

SRCS = $(objects:%.o=%.cpp)
DEPS = $(objects:%.o=.%.d)

ALL_TARGETS = SessionServer

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
