#include "zSceneEntryIndex.h"
#include "SceneNpc.h"
#include "SceneUser.h"
#include "logger.h"

void zSceneEntryIndex::setSceneWH(const zPos sceneWH,const DWORD screenx,const DWORD screeny,const DWORD screenMax)
{
  this->sceneWH=sceneWH;
  this->screenx=screenx;
  this->screeny=screeny;
  for(int i = 0; i < zSceneEntry::SceneEntryType::SceneEntry_MAX; i++)
  {
    for(DWORD j=0; j < screenMax; j ++)
    {
      index[i][j];
    }
  }
  const int adjust[9][2] = { {0,-1},{1,-1},{1,0},{1,1},{0,1},{-1,1},{-1,0},{-1,-1},{0,0} };
  for(DWORD j=0; j < screenMax ; j ++)
  {
    int nScreenX = j % screenx;
    int nScreenY = j / screenx;
    //calculate nice screen
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
        //oblique
        start = 6;
        end = 10;
      }
      else {
        //forward direction
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
    //5 screen or 3 screen
    for(int dir = 0; dir < 8; dir++)
    {
      int start,end;
      zPosIVector pv;

      if (1 == dir % 2) {
        //oblique
        start = 2;
        end = 6;
      }
      else {
        //forward direction
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

bool zSceneEntryIndex::refresh(zSceneEntry *e,const zPos & newPos)
{
	if (e==NULL) return false;
	zSceneEntry::SceneEntryType type = e->getType();
	if (e->inserted)
	{
		//already in map, change between screens
		bool ret=false;
		zPosI orgscreen=e->getPosI();

		SceneEntry_SET &pimi = index[type][orgscreen];
		SceneEntry_SET::iterator it = pimi.find(e);
		if (it != pimi.end() && e->setPos(sceneWH,newPos))
		{
			ret=true;
			if (orgscreen!=e->getPosI())
			{
				//Zebra::logger->debug("%s ÇÐÆÁ(%ld->%ld)",e->name,orgscreen,e->getPosI());
				pimi.erase(it);
				index[type][e->getPosI()].insert(e);
				if (type == zSceneEntry::SceneEntryType::SceneEntry_Player)
				{
					freshEffectPosi(orgscreen,e->getPosI());
					freshGateScreenIndex((SceneUser*)e,e->getPosI());
				}
			}
		}

		return ret;
	}
	else
	{
		//add to new map
		if (e->setPos(sceneWH,newPos))
		{
			index[type][e->getPosI()].insert(e);
			//add to global index
			all[type].insert(e);
			//npc index
			if (zSceneEntry::SceneEntryType::SceneEntry_NPC == type )
			{
				SceneNpc *npc = (SceneNpc *)e;
				special_index[npc->id].insert(npc);
				if (npc->isFunctionNpc())
				{
					functionNpc.insert(npc);
				}
			}
			if (type == zSceneEntry::SceneEntryType::SceneEntry_Player)
			{
				freshEffectPosi((zPosI)-1,e->getPosI());
				freshGateScreenIndex((SceneUser*)e,e->getPosI());
			}

			e->inserted=true;
		}
		else
			LOG(INFO)<<__func__<<" failed add to screen index "<<e->name<<",x="<<newPos.x<<",y"<<newPos.y;

		return e->inserted;
	}
}

bool zSceneEntryIndex::removeSceneEntry(zSceneEntry *e)
{
	if (e==NULL || !e->inserted) return false;

	zSceneEntry::SceneEntryType type = e->getType();
	SceneEntry_SET &pimi = index[type][e->getPosI()];
	SceneEntry_SET::iterator it = pimi.find(e);
	if (it != pimi.end())
	{
		pimi.erase(it);
		all[type].erase(e);
		if (zSceneEntry::SceneEntryType::SceneEntry_NPC == type )
		{
			SceneNpc *npc = (SceneNpc *)e;
			special_index[npc->id].erase(npc);
			if (npc->isFunctionNpc())
			{
				functionNpc.erase(npc);
			}
		}
		if (type == zSceneEntry::SceneEntryType::SceneEntry_Player)
		{
			freshEffectPosi(e->getPosI(),(zPosI)-1);
			freshGateScreenIndex((SceneUser*)e,(DWORD)-1); 
		}
		e->inserted=false;
		return true;
	}

	return false;
}

void zSceneEntryIndex::execAllOfScreen(const zPosI screen,zSceneEntryCallBack &callback)
{
  for(int i = 0; i < zSceneEntry::SceneEntryType::SceneEntry_MAX; i++)
  {
    SceneEntry_SET &pimi = index[i][screen];
    for(SceneEntry_SET::iterator it=pimi.begin();it!=pimi.end();)
    {
      SceneEntry_SET::iterator tmp = it;
      it++;
      zSceneEntry *eee = *tmp;
      if (!callback.exec(eee)) return;
    }
  }
}

void zSceneEntryIndex::execAllOfScreen(const zSceneEntry::SceneEntryType type,const zPosI screen,zSceneEntryCallBack &callback)
{
  SceneEntry_SET &pimi = index[type][screen];
  for(SceneEntry_SET::iterator it=pimi.begin();it!=pimi.end();)
  {
    SceneEntry_SET::iterator tmp = it;
    it++;
    zSceneEntry *eee = *tmp;
    if (!callback.exec(eee)) return;
  }
}

void zSceneEntryIndex::execAllOfScene(zSceneEntryCallBack &callback)
{
  for(int i = 0; i < zSceneEntry::SceneEntryType::SceneEntry_MAX; i++)
  {
    for(SceneEntry_SET::iterator it = all[i].begin(); it != all[i].end();)
    {
      SceneEntry_SET::iterator tmp = it;
      it++;
      zSceneEntry *eee = *tmp;
      if (!callback.exec(eee)) return;
    }
  }
}

void zSceneEntryIndex::execAllOfScene(const zSceneEntry::SceneEntryType type,zSceneEntryCallBack &callback)
{
  for(SceneEntry_SET::iterator it = all[type].begin(); it != all[type].end();)
  {
    SceneEntry_SET::iterator tmp = it;
    it++;
    zSceneEntry *eee = *tmp;
    if (!callback.exec(eee)) return;
  }
}

void zSceneEntryIndex::execAllOfScene_npc(const DWORD id,zSceneEntryCallBack &callback)
{
  SpecialNpc_Index::iterator my_it = special_index.find(id);
  if (special_index.end() != my_it)
  {
    for(Npc_Index::iterator it = my_it->second.begin(); it != my_it->second.end(); ++it)
    {
      if (!callback.exec(*it)) return;
    }
  }
}

void zSceneEntryIndex::execAllOfScene_functionNpc(zSceneEntryCallBack &callback)
{
  for(Npc_Index::iterator it = functionNpc.begin(); it != functionNpc.end(); ++it)
  {
    if (!callback.exec(*it)) return;
  }
}

zSceneEntry *zSceneEntryIndex::getSceneEntryByPos(zSceneEntry::SceneEntryType type,const zPos &pos,const bool bState,const zSceneEntry::SceneEntryState byState)
{
  class GetSceneEntryByPos : public zSceneEntryCallBack
  {
    public:
      const zPos &pos;
      zSceneEntry *sceneEntry;
      bool _bState;
	  zSceneEntry::SceneEntryState _byState;

      GetSceneEntryByPos(const zPos &pos,const bool bState,const zSceneEntry::SceneEntryState byState)
        : pos(pos),sceneEntry(NULL),_bState(bState),_byState(byState) {}
      bool exec(zSceneEntry *e)
      {
        if (e->getPos() == pos)
        {
          if (_bState && e->getState() != _byState)
            return true;
		  /*
          if (e->getType() == zSceneEntry::SceneEntry_NPC
              && ((SceneNpc *)e)->getPetType() == Cmd::PET_TYPE_TOTEM)
          {
            return true;
          }
		  */
          sceneEntry = e;
          return false;
        }
        else
          return true;
      }
  } ret(pos,bState,byState);
  zPosI screen;
  zSceneEntry::zPos2zPosI(sceneWH,pos,screen);
  execAllOfScreen(type,screen,ret);
  return ret.sceneEntry;
}


void zSceneEntryIndex::freshEffectPosi(const zPosI oldposi,const zPosI newposi)
{
  if (oldposi != (zPosI)-1)
  {
    const zPosIVector &pv = getNineScreen(oldposi);
    for(zPosIVector::const_iterator it = pv.begin(); it != pv.end(); it++)
    {
      PosiEffectMap_iter iter = posiEffect[(*it)%MAX_NPC_GROUP].find(*it);
      if (iter != posiEffect[(*it)%MAX_NPC_GROUP].end() && iter->second > 0)
      {
        iter->second--;
      }
    }
  }
  if (newposi != (zPosI)-1)
  {
    const zPosIVector &pv = getNineScreen(newposi);
    for(zPosIVector::const_iterator it = pv.begin(); it != pv.end(); it++)
    {
      posiEffect[(*it)%MAX_NPC_GROUP][*it]++;
    }
  }
}

void zSceneEntryIndex::execAllOfEffectNpcScreen(const DWORD group,zSceneEntryCallBack &callback)
{
  PosiEffectMap_iter iter = posiEffect[group%MAX_NPC_GROUP].begin();
  for( ;iter != posiEffect[group%MAX_NPC_GROUP].end() ; iter++)
  {
    SceneEntry_SET &pimi = index[zSceneEntry::SceneEntryType::SceneEntry_NPC][iter->first];
    for(SceneEntry_SET::iterator it=pimi.begin();it!=pimi.end();)
    {
      SceneEntry_SET::iterator tmp = it;
      it++;
      zSceneEntry *eee = *tmp;
      if (!callback.exec(eee)) return;
    }
  }
}

const zPosIVector &zSceneEntryIndex::getScreenByRange(const zPos &pos,const int range)
{
  static zPosIVector pv;
  pv.clear();

  uint32_t leftBoard = 0;
  uint32_t upBoard = 0;
  if (pos.x > range)
	  leftBoard = (pos.x - range) / SCREEN_WIDTH;
  if (pos.y > range)
	  upBoard = (pos.y - range) /SCREEN_HEIGHT;

  uint32_t rightBoard = (pos.x + range) /SCREEN_WIDTH;
  uint32_t downBoard = (pos.y + range) /SCREEN_HEIGHT;
  if (rightBoard >= screenx) rightBoard = screenx - 1;
  if (downBoard >= screeny) downBoard = screeny - 1;

  for (uint32_t y = upBoard; y <= downBoard; ++y)
  {
	  for (uint32_t x = leftBoard; x <= rightBoard; ++x)
		  pv.push_back(y*screenx + x);
  }
	
  return pv;
}

