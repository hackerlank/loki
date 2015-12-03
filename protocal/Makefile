
.PHONY: clear

SRCS = $(wildcard *.proto)

middle_login: $(SRCS)
	protoc --cpp_out=. $(SRCS)
	@mv login.pb.h ../base/
	@mv login.pb.cc ../base/login.pb.cpp
	@mv super.pb.h ../base/
	@mv super.pb.cc ../base/super.pb.cpp
	@mv record.pb.h ../base/
	@mv record.pb.cc ../base/record.pb.cpp
	@mv session.pb.h ../base/
	@mv session.pb.cc ../base/session.pb.cpp
	@mv scene.pb.h ../base/
	@mv scene.pb.cc ../base/scene.pb.cpp
	@mv gate.pb.h ../base/
	@mv gate.pb.cc ../base/gate.pb.cpp
	@mv Cmd.pb.h ../base/
	@mv Cmd.pb.cc ../base/Cmd.pb.cpp
	@touch middle_login

clear:
	@rm middle_login ../*/*.pb.h ../*/*.pb.cpp
