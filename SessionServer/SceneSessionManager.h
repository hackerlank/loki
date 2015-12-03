#pragma once
#include "zSceneManager.h"
#include "SceneSession.h"
#include "singleton.h"
#include "SessionTask.h"

class SceneSessionManager:public zSceneManager, public Singleton<SceneSessionManager>
{
	private:
		bool inited;

		bool getUniqeID(DWORD &tempid);
		void putUniqeID(const DWORD &tempid);


	public:
		SceneSessionManager();
		~SceneSessionManager();
		bool init();
		void final();

		bool addScene(SceneSession *scene);
		SceneSession * getSceneByName(const char *name);
		SceneSession * getSceneByFile(const char *file);
		SceneSession * getSceneByID(DWORD id);
		SceneSession * getSceneByTempID(DWORD tempid);
		void removeAllSceneByTask(SessionTask* task);
		void removeScene(SceneSession *scene);
		void notifyCountryTax(DWORD dwCountry,BYTE byTax);

};

extern SceneSession* createSceneSession(SessionTask* task);
