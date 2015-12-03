#include "SceneTask.h"
#include "script.h"
#include "compressor_zlib.h"
#include "encryptor_aes.h"
#include "SceneDefine.h"

SceneTask::SceneTask(connection_ptr conn)
	:context(conn)
{
	char key[32]= {'a', 'b', 'c'};
	std::shared_ptr<loki::encryptor_aes> aes(new loki::encryptor_aes());
	aes->setkey((const unsigned char*)key,256);

	conn->set_compressor(std::shared_ptr<loki::compressor>(new loki::compressor_zlib()));
	conn->set_encryptor(aes);
}

SceneTask::~SceneTask()
{
}
