BUILD_DIR_ROOT = $(PWD)

LOKI_OPTIONS = -std=c++11 -fno-omit-frame-pointer -fno-optimize-sibling-calls
SEAL_DEBUG_OPT = -g -Werror -Wno-unused-local-typedefs $(LOKI_OPTIONS)
SEAL_DIST_OPT = -g -O1 -Werror $(LOKI_OPTIONS) 

start_compile_time = $(shell date +%s)

export COMMON_HEAD_DIR = -I../base -I./  -I/usr/include/lua5.2 -I../lua_tinker -I/usr/local/include `xml2-config --cflags`
export COMMON_LIB_DIR = -L/usr/local/lib -L../base `xml2-config --libs`
export COMMON_LIBS = -lseal -lpthread -lboost_system -lboost_chrono \
			  -L../lua_tinker -llua_tinker \
			  -llua5.2 -lssl -lcrypto -lprotobuf -lmysqlcppconn -lz -lglog


SUB_DIRS = protocal base lua_tinker Super Login
#SUB_DIRS += LoginServer SuperServer RecordServer SessionServer SceneServer GatewayServer 

#TEST_SUB_DIRS = base/EncDec base tools
ALL_SUB_DIRS = $(SUB_DIRS) 

NJOBS = `if [ ""=="$(NCPUS)" ]; then echo ""; else echo "-j$(NCPUS)"; fi`
#DMUCS_HOST_IP=`hostname -i | sed s,.$$$$,,`"0"
DMUCS_HOST_IP=172.17.102.30
export CXX=g++ -std=c++11

.PHONY: all debug ctags doc distclean clean $(ALL_SUB_DIRS) ChangeLog client cleanproto proto

all: debug 

super:
	@export CXX='gethost -s $(DMUCS_HOST_IP) distcc /usr/bin/g++'; \
				for dir in $(SUB_DIRS); \
				do \
				SEAL_COMPILE_OPT='$(SEAL_DEBUG_OPT) -DBUILD_STRING=$(BUILD_STRING)' PIC_FLAG='-fDIC' $(MAKE) $(NJOBS) -C $$dir || exit 1; \
				done

debug:
	@for dir in $(SUB_DIRS); \
		do \
		SEAL_COMPILE_OPT='$(SEAL_DEBUG_OPT) ' $(MAKE) $(NJOBS) -C $$dir || exit 1; \
		done
	@echo "compiletimeelapsed:" `echo \`date +%s\` $(start_compile_time) | awk '{printf("%g",$$1-$$2)}'`s

client:
	SEAL_COMPILE_OPT='$(SEAL_DEBUG_OPT) -DBUILD_STRING=$(BUILD_STRING)' $(MAKE) $(NJOBS) -C ./client/ || exit 1;

debug_dist:
	@for dir in $(SUB_DIRS); \
		do \
		SEAL_COMPILE_OPT='$(SEAL_DIST_OPT) -DBUILD_STRING=$(BUILD_STRING)' $(MAKE) $(NJOBS) -C $$dir || exit 1; \
		done


test:
	@for dir in $(TEST_SUB_DIRS); \
		do \
		SEAL_COMPILE_OPT='$(SEAL_DEBUG_OPT) -DBUILD_STRING=$(BUILD_STRING)' $(MAKE) $(NJOBS) -C $$dir || exit 1; \
		done

ChangeLog:
	@rm ChangeLog
	@svn log -v | ./gnuifiy-changelog.pl > ChangeLog

doc:
	@doxygen Doxyfile.pub

alldoc:
	@doxygen

distclean:
	@find . -iname .\*.d -exec rm \{\} \;

clean:
	@for dir in $(ALL_SUB_DIRS); \
		do \
		$(MAKE) -C $$dir clean; \
		done

proto:
	@cd protocal;make

cleanproto:
	@cd protocal;make clear

#ctags:
#	@find . -type f -name "*.cpp" -o -name ".c" -o -name "*.h" -o -name "*.hpp" | xargs ctags

ctags:
	@ctags-exuberant -R 

#@ctags -R "*.c" "*.cpp" "*.h"
#@ctags  */*.c */*.cpp */*.h


