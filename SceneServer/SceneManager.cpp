#include "SceneManager.h"
#include "SceneServer.h"
#include <boost/lexical_cast.hpp>
#include "session.pb.h"
#include "protobuf_codec.h"

using namespace loki;

SceneManager::SceneManager()
{
}

SceneManager::~SceneManager()
{
}

Scene* SceneManager::loadScene(Scene::SceneType type, const uint32_t country, const uint32_t mapid)
{
	Scene* scene = nullptr;
	switch (type)
	{
		case Scene::SceneType::STATIC:
			{
				scene = new StaticScene();
			}
			break;
			/*
		case Scene::SceneType::DYNAMIC:
			{
				scene.reset(new Scene());
			}
			break;
			*/
	}
	bool ret = scene->init(country, mapid);
	if (ret)
	{
		ret = addEntry(scene);
	}
	LOG(INFO)<<__func__<<", scene mapid="<<mapid<<", ret="<<ret?"ok":"fail";
	return scene;
}

bool SceneManager::getUniqeID(DWORD &tempid)       
{                                                  
	tempid=sceneUniqeID->get();                      
	return (tempid!=sceneUniqeID->invalid());        
}                                                  

void SceneManager::putUniqeID(const DWORD &tempid) 
{                                                  
	sceneUniqeID->put(tempid);                       
}                                                  

bool SceneManager::init()
{
	DWORD firstTempID=10000+(SceneServer::getInstance().get_id()%100)*50000;
	sceneUniqeID=new zUniqueDWORDID(firstTempID, firstTempID+49998);                

	//loadMapInfo(map_info);
	//load mapinfo in lua script
	//load scenesinfo by serverid in lua script
	//load country info
	lua_tinker::table scenesinfo = SceneServer::instance().get_script()->get<lua_tinker::table>("ScenesInfo");
	//lua_tinker::table scenes = scenesinfo.get<lua_tinker::table>(boost::lexical_cast<std::string>(SceneServer::getInstance().get_id()).c_str());
	const uint32_t country = scenesinfo.get<uint32_t>("count");
	LOG(INFO)<<__func__<<", count="<<country;
	return true;

}

void SceneManager::registerScript(lua_State* L)
{
	using namespace lua_tinker;
	def(L, "getSceneManager", SceneManager::getSingletonPtr);

	class_add<SceneManager>(L, "SceneManager");
	class_def<SceneManager>(L, "loadScene", &SceneManager::loadScene);
	class_def<SceneManager>(L, "loadedSceneNotifySession", &SceneManager::loadedSceneNotifySession);
}

void SceneManager::loadedSceneNotifySession(Scene* scene)
{
	LOG(INFO)<<__func__;

	Session::t_regScene_SceneSession send;
	send.set_id(scene->id);
	send.set_tempid(scene->tempid);
	send.set_name(scene->name);
	send.set_filename(scene->getFileName());
	send.set_level(scene->getLevel());
	send.set_country(scene->getCountryID());
	protobuf_codec::send(SceneServer::instance().sessionclient_, &send);
}
