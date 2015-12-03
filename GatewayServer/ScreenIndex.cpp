#include "ScreenIndex.h"
#include "GatewayTask.h"

ScreenIndex::ScreenIndex(const DWORD x,const DWORD y):screenx(x),screeny(y),screenMax(x*y)
{
  const int adjust[9][2] = { {0,-1},{1,-1},{1,0},{1,1},{0,1},{-1,1},{-1,0},{-1,-1},{0,0} };
  for(DWORD j=0; j < screenMax ; j ++)
  {
    index[j];
    int nScreenX = j % screenx;
    int nScreenY = j / screenx;
    {
      zPosIVector pv;
      for(int i = 0; i < 9; i++) {
        int x = nScreenX + adjust[i][0];
        int y = nScreenY + adjust[i][1];
        if (x >= 0 && y >= 0 && x < (int)screenx && y < (int)screeny) {
          pv.push_back(y * screenx + x);
        }
      }
      ninescreen.insert(NineScreen_map_value_type(j,pv));
    }
    for(int dir = 0; dir < 8; dir++)
    {
      int start,end;
      zPosIVector pv;

      if (1 == dir % 2) {
        start = 6;
        end = 10;
      }
      else {
        start = 7;
        end = 9;
      }
      for(int i = start; i <= end; i++) {
        int x = nScreenX + adjust[(i + dir) % 8][0];
        int y = nScreenY + adjust[(i + dir) % 8][1];
        if (x >= 0 && y >= 0 && x < (int)screenx && y < (int)screeny) {
          pv.push_back(y * screenx + x);
        }
      }
      direct_screen[dir].insert(NineScreen_map_value_type(j,pv));
    }
    for(int dir = 0; dir < 8; dir++)
    {
      int start,end;
      zPosIVector pv;

      if (1 == dir % 2) {
        start = 2;
        end = 6;
      }
      else {
        start = 3;
        end = 5;
      }
      for(int i = start; i <= end; i++) {
        int x = nScreenX + adjust[(i + dir) % 8][0];
        int y = nScreenY + adjust[(i + dir) % 8][1];
        if (x >= 0 && y >= 0 && x < (int)screenx && y < (int)screeny) {
          pv.push_back(y * screenx + x);
        }
      }
      reversedirect_screen[dir].insert(NineScreen_map_value_type(j,pv));
    }
  }
} 

template <class YourNpcEntry>
void ScreenIndex::execAllOfScreen(const DWORD screen,execEntry<YourNpcEntry> &callback)
{
  SceneEntry_SET &set = index[screen];
  wrlock.rdlock();
  SceneEntry_SET::iterator it = set.begin(),ed = set.end();
  for(; it != ed; ++it)
  {
    callback.exec(*it);
  }
  wrlock.unlock();
}

//TODO:
struct SendNineExec : public execEntry<GatewayTask>
{
	const google::protobuf::Message* msg_{nullptr};
	unsigned short targetDupIndex;
	SendNineExec(unsigned short _targetDupIndex,const google::protobuf::Message* msg): targetDupIndex(_targetDupIndex), msg_(msg)
	{
	}
	bool exec(GatewayTask *pUser)
	{

		if(pUser->dupIndex == targetDupIndex)
		{
			pUser->sendCmd(msg_);

		}
		return true;
	}
};

void ScreenIndex::sendCmdToNine(const DWORD posi,const google::protobuf::Message* msg, unsigned short dupIndex)
{
  SendNineExec exec(dupIndex,msg);
  const zPosIVector &pv = getNineScreen(posi);
  zPosIVector::const_iterator it = pv.begin(),ed = pv.end();
  for(; it != ed; ++it)
  {
    execAllOfScreen(*it,exec);
  }
}

void ScreenIndex::sendCmdToDirect(const zPosI posi,const int direct,const google::protobuf::Message* msg,unsigned short dupIndex)
{
  SendNineExec exec(dupIndex, msg);
  const zPosIVector &pv = getDirectScreen(posi,direct);
  zPosIVector::const_iterator it = pv.begin(),ed = pv.end();
  for(; it != ed; ++it)
  {
    execAllOfScreen(*it,exec);
  }
}

void ScreenIndex::sendCmdToReverseDirect(const zPosI posi,const int direct,const google::protobuf::Message* msg,unsigned short dupIndex)
{
  SendNineExec exec(dupIndex,msg);
  const zPosIVector &pv = getReverseDirectScreen(posi,direct);
  zPosIVector::const_iterator it = pv.begin(),ed = pv.end();
  for(; it != ed; ++it)
  {
    execAllOfScreen(*it,exec);
  }
}

void ScreenIndex::sendCmdToAll(const google::protobuf::Message* msg)
{
  wrlock.rdlock();
  SceneEntry_SET::iterator it=all.begin(),ed = all.end();
  for(; it != ed; ++it)
  {
    (*it)->sendCmd(msg);
  }
  wrlock.unlock();
}

struct SendNineExecExceptMe : public execEntry<GatewayTask>
{
  const DWORD _exceptme_id;
  const google::protobuf::Message  *_cmd;
  SendNineExecExceptMe(const DWORD exceptme_id,const google::protobuf::Message* msg):_exceptme_id(exceptme_id),_cmd(msg)
  {
  }
  bool exec(GatewayTask *pUser)
  {
    if (_exceptme_id != pUser->id())
      pUser->sendCmd(_cmd);
    return true;
  }
};

void ScreenIndex::sendCmdToNineExceptMe(const DWORD posi,const DWORD exceptme_id,const google::protobuf::Message* msg)
{
  SendNineExecExceptMe exec(exceptme_id,msg);
  const zPosIVector &pv = getNineScreen(posi);
  zPosIVector::const_iterator it = pv.begin(),ed = pv.end();
  for(; it != ed; ++it)
  {
    execAllOfScreen(*it,exec);
  }
}

bool ScreenIndex::refresh(GatewayTask *e,const DWORD newIndex)
{
  zRWLock_scope_wrlock scope_wrlock(wrlock);
  if (e==NULL) return false;
  //-2 present delete state, can't be added to scene
  //-1 preset waiting for add state
  if (e->getIndexKey() == (DWORD)-2 && newIndex != (DWORD)-1) return false;

  if (e->inserted)
  {
    bool ret=false;

    SceneEntry_SET &pimi = index[e->getIndexKey()];
    SceneEntry_SET::iterator it = pimi.find(e);
    if (it != pimi.end())
    {
      ret=true;
      pimi.erase(it);
      index[newIndex].insert(e);
      e->setIndexKey(newIndex);
    }

    return ret;
  }
  else if (newIndex != (DWORD)-1)
  {
    if (all.insert(e).second)
    {
      index[newIndex].insert(e);
    }
    e->inserted=true;
  }

  e->setIndexKey(newIndex);
  return e->inserted;
}

void ScreenIndex::removeGateUser(GatewayTask *e)
{
  zRWLock_scope_wrlock scope_wrlock(wrlock);
  if (e==NULL || !e->inserted) return;

  SceneEntry_SET::iterator it = all.find(e);
  if (it != all.end())
  {
    SceneEntry_SET &pimi = index[e->getIndexKey()];
    e->setIndexKey((DWORD)-2);
    all.erase(it);
    pimi.erase(e);
    e->inserted=false;
  }
}

const zPosIVector &ScreenIndex::getNineScreen(const zPosI &posi)
{
  NineScreen_map_iter iter = ninescreen.find((DWORD)posi);
  if (iter != ninescreen.end())
  {
    return iter->second;
  }
  return ninescreen[(DWORD)-1];
}

const zPosIVector &ScreenIndex::getDirectScreen(const zPosI &posi,const int dir)
{
  NineScreen_map_iter iter = direct_screen[dir].find((DWORD)posi);
  if (iter != direct_screen[dir].end())
  {
    return iter->second;
  }
  return direct_screen[dir][(DWORD)-1];
}

const zPosIVector &ScreenIndex::getReverseDirectScreen(const zPosI &posi,const int dir)
{
  NineScreen_map_iter iter = reversedirect_screen[dir].find((DWORD)posi);
  if (iter != reversedirect_screen[dir].end())
  {
    return iter->second;
  }
  return reversedirect_screen[dir][(DWORD)-1];
}

