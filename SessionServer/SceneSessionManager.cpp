#include "SceneSessionManager.h"

using namespace std;

SceneSessionManager::SceneSessionManager():zSceneManager()
{
  inited=false;
}

SceneSessionManager::~SceneSessionManager()
{
  final();
}

bool SceneSessionManager::getUniqeID(DWORD &tempid)
{
  return true;
}

void SceneSessionManager::putUniqeID(const DWORD &tempid)
{
}

bool SceneSessionManager::init()
{
  if (inited)
  {
    return true;
  }
  inited=true;
  LOG(INFO)<<__func__;
  return inited;
}

void SceneSessionManager::final()
{
  if (inited)
  {
    inited=false;
  }
}

bool SceneSessionManager::addScene(SceneSession *scene)
{
  rwlock.wrlock();
  bool ret= addEntry(scene);
  rwlock.unlock();
  return ret;
}

SceneSession * SceneSessionManager::getSceneByName(const char *name)
{
  return (SceneSession *)zSceneManager::getSceneByName(name);
}

SceneSession * SceneSessionManager::getSceneByTempID(DWORD tempid)
{
  return (SceneSession *)zSceneManager::getSceneByTempID(tempid);
}

SceneSession * SceneSessionManager::getSceneByID(DWORD id)
{
  return (SceneSession *)zSceneManager::getSceneByID(id);
}

void SceneSessionManager::removeScene(SceneSession *scene)
{
  rwlock.wrlock();
  removeEntry(scene);
  rwlock.unlock();
}

void SceneSessionManager::removeAllSceneByTask(SessionTask* task)
{
  struct removeAllSceneByTask :public removeEntry_Pred<SceneSession>
  {
    SessionTask* task;
    bool isIt(SceneSession *ss)
    {
      return (ss->getTask() == task);
    }
  };
  if (task==NULL || task->type()!=SCENESERVER) return;
  removeAllSceneByTask rust;
  rust.task=task;
  removeScene_if (rust);
  for(DWORD i=0,n=rust.removed.size();i<n;i++)
  {
    SAFE_DELETE(rust.removed[i]);
  }
}

SceneSession * SceneSessionManager::getSceneByFile(const char *name)
{
  struct GetSceneByFileName: public execEntry<SceneSession>
  {
    SceneSession *ret;
    const char *_name;

    GetSceneByFileName(const char* name) : ret(NULL),_name(name)
    { }
    
    bool exec(SceneSession *scene)
    {
      if (scene->filename() == _name)
      {
        ret = scene;
        return false;
      }
      else
        return true;
    }
  };

  GetSceneByFileName gsfn(name);
  execEveryScene(gsfn);
  return gsfn.ret;
}

void SceneSessionManager::notifyCountryTax(DWORD dwCountry,BYTE byTax)
{
	//SessionTaskManager::instance().broadcastScene();
}
SceneSession* createSceneSession(SessionTask* task)
{
	return new SceneSession(task);
}
