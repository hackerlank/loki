
compile_debug = -g -Wall 
compile_relase = -g -Wall -O1

compile_opt = $(compile_debug) $(SEAL_COMPILE_OPT)

HEADER_DIR = -I/usr/include/lua5.2
LIB_DIR = -L.
LIBS = -llua5.2

CXXFLAGS = $(compile_opt) $(HEADER_DIR) -D_REENTRANT -D_GNU_SOURCE
CXXLFLAGS = $(compile_opt) $(LIB_DIR) $(LIBS)

objects = lua_tinker.o
sources = $(objects:%.o=%.cpp)
deps = $(objects:%.o=.%.d)

all_targets = liblua_tinker.a

.PHONY: all clean

all: $(all_targets)

include $(deps)
$(deps): .%.d: %.cpp
	@$(CXX) -M $(CXXFLAGS) $< > $@.$$$$; \
		sed 's,\($*\).o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
		rm -f $@.$$$$

$(objects): %.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

#$(CXX) $(objects) -o $@ $(CXXLFLAGS)
$(all_targets): $(objects) Makefile
	@rm -rf $(all_targets)
	@ar cru $(all_targets) $(objects)
	@ranlib $(all_targets)

clean:
	@rm -rf *.o .*.d $(all_targets) core *~
