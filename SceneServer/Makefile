LIBSEAL= ../base/libseal.a

#-Wall
CXXFLAGS = $(SEAL_COMPILE_OPT)  -D_GNU_SOURCE -D_REENTRANT $(COMMON_HEAD_DIR)
CXXLFLAGS = $(SEAL_COMPILE_OPT) -Wall $(COMMON_LIB_DIR) $(COMMON_LIBS)

objects = Main.o SceneServer.o SceneTask.o LoadMap.o SceneManager.o 
objects += SceneNpc.o
objects += zSceneEntryIndex.o Scene.o WayPoint.o SceneEntryPk.o

SRCS = $(objects:%.o=%.cpp)
DEPS = $(objects:%.o=.%.d)

ALL_TARGETS = SceneServer

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
