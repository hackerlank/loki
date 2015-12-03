#include "logger.h"

void InitLog(const char* cmd)
{
	FLAGS_logbufsecs = 0;
	google::InitGoogleLogging(cmd);
}
