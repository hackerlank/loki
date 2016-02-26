
.PHONY: clear

SRCS = $(wildcard *.proto)

middle_login: $(SRCS)
	protoc --cpp_out=. $(SRCS)
	@mv Login.pb.h ../base/
	@mv Login.pb.cc ../base/Login.pb.cpp
	@mv Super.pb.h ../base/
	@mv Super.pb.cc ../base/Super.pb.cpp
	@mv Record.pb.h ../base/
	@mv Record.pb.cc ../base/Record.pb.cpp
	@mv Session.pb.h ../base/
	@mv Session.pb.cc ../base/Session.pb.cpp
	@mv Scene.pb.h ../base/
	@mv Scene.pb.cc ../base/Scene.pb.cpp
	@mv Gate.pb.h ../base/
	@mv Gate.pb.cc ../base/Gate.pb.cpp
	@mv Cmd.pb.h ../base/
	@mv Cmd.pb.cc ../base/Cmd.pb.cpp
	@touch middle_login

clear:
	@rm middle_login ../*/*.pb.h ../*/*.pb.cpp
