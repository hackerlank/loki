#pragma once
#include <unordered_map>
#include "zSceneEntry.h"
#include <set>
#include <map>

#define MAX_NPC_GROUP 15

class SceneNpc;
class SceneUser;

struct zSceneEntryCallBack
{
	virtual bool exec(zSceneEntry *entry)=0;
	virtual ~zSceneEntryCallBack(){};
};

class zSceneEntryIndex 
{

  protected:
    zPos sceneWH;
    uint32_t screenMax;
    uint32_t screenx;
    uint32_t screeny;

  private:

    typedef std::set<zSceneEntry *,std::less<zSceneEntry *> > SceneEntry_SET;
    typedef std::set<SceneNpc *,std::less<SceneNpc *> > Npc_Index;

    typedef std::unordered_map<zPosI,SceneEntry_SET> PosIMapIndex;
    typedef std::unordered_map<uint32_t,Npc_Index> SpecialNpc_Index;

    PosIMapIndex index[zSceneEntry::SceneEntryType::SceneEntry_MAX];
    //all object
    SceneEntry_SET all[zSceneEntry::SceneEntryType::SceneEntry_MAX];
    SpecialNpc_Index special_index;
    Npc_Index functionNpc;

    typedef std::map<uint32_t,int,std::less<uint32_t> > PosiEffectMap;

    typedef PosiEffectMap::iterator PosiEffectMap_iter;
    PosiEffectMap posiEffect[MAX_NPC_GROUP];

    //nine screen relation
    typedef std::unordered_map<uint32_t,zPosIVector> NineScreen_map;
    typedef NineScreen_map::iterator NineScreen_map_iter;
    typedef NineScreen_map::const_iterator NineScreen_map_const_iter;
    typedef NineScreen_map::value_type NineScreen_map_value_type;
    NineScreen_map ninescreen;
    NineScreen_map direct_screen[8];
    NineScreen_map reversedirect_screen[8];

    void freshEffectPosi(const zPosI oldposi,const zPosI newposi);

  public:
    zSceneEntryIndex() {}
    virtual ~zSceneEntryIndex() {}

    virtual void freshGateScreenIndex(SceneUser *pUser,const uint32_t screen){}

    bool refresh(zSceneEntry *e,const zPos & newPos);
    bool removeSceneEntry(zSceneEntry *e);
    void execAllOfScreen(const zPosI screen,zSceneEntryCallBack &callback);
    void execAllOfScreen(const zSceneEntry::SceneEntryType type,const zPosI screen,zSceneEntryCallBack &callback);
    void execAllOfScene(zSceneEntryCallBack &callback);
    void execAllOfScene(const zSceneEntry::SceneEntryType type,zSceneEntryCallBack &callback);
    void execAllOfScene_npc(const uint32_t id,zSceneEntryCallBack &callback);
    void execAllOfScene_functionNpc(zSceneEntryCallBack &callback);

    void setSceneWH(const zPos sceneWH,const uint32_t screenx,const uint32_t screeny,const uint32_t screenMax);
	zSceneEntry *getSceneEntryByPos(zSceneEntry::SceneEntryType type,const zPos &pos,const bool bState = true,const zSceneEntry::SceneEntryState byState = zSceneEntry::SceneEntryState::SceneEntry_Normal);

    void execAllOfEffectNpcScreen(const uint32_t group,zSceneEntryCallBack &callback);

    const uint32_t width() const { return sceneWH.x; }
    const uint32_t height() const { return sceneWH.y; }
    const uint32_t getScreenX() const { return screenx; }
    const uint32_t getScreenY() const { return screeny; }
    zPosI &zPos2zPosI(const zPos &p,zPosI &pi) const
    {
      zSceneEntry::zPos2zPosI(sceneWH,p,pi);
      return pi;
    }
    const void zPosRevaluate(zPos &pos) const
    {
      if (pos.x & 0x80000000)
        pos.x = 0;
      if (pos.y & 0x80000000)
        pos.y = 0;
      if (pos.x >= sceneWH.x)
        pos.x = sceneWH.x - 1;
      if (pos.y >= sceneWH.y)
        pos.y = sceneWH.y - 1;
    }
    const bool zPosValidate(const zPos &pos) const { return pos.x < sceneWH.x && pos.y < sceneWH.y; }
    const bool zPosIValidate(const zPosI &posi) const { return posi < screenMax; }
    bool checkTwoPosIInNine(const zPosI one,const zPosI two) const
    {
      int oneX,oneY,twoX,twoY;
      oneX = one % screenx;
      oneY = one / screenx;
      twoX = two % screenx;
      twoY = two / screenx;
      if (abs(oneX - twoX) <=1 && abs(oneY - twoY) <=1)
      {
        return true;
      }
      return false;
    }
    const bool zPosShortRange(const zPos &pos1,const zPos &pos2,const int radius) const
    {
      if (abs((long)(pos1.x - pos2.x)) <= radius
          && abs((long)(pos1.y - pos2.y)) <= radius)
        return true;
      else
        return false;
    }
    const bool zPosShortRange(const zPos &pos1,const zPos &pos2,const int wide,const int height) const
    {
      if (abs((long)(pos1.x - pos2.x)) <= wide
          && abs((long)(pos1.y - pos2.y)) <= height)
        return true;
      else
        return false;
    }
    uint32_t getDistance(zPos pos1,zPos pos2) const { return abs((long)(pos1.x-pos2.x))+abs((long)(pos1.y-pos2.y)); }
    int getReverseDirect(const int direct) const { return (direct + 4) % 8; }
    int getScreenDirect(const zPosI posiorg,const zPosI posinew) const
    {
      using namespace Cmd;
      uint32_t orgX = posiorg % screenx;
      uint32_t orgY = posiorg / screenx;
      uint32_t newX = posinew % screenx;
      uint32_t newY = posinew / screenx;
      if (orgX == newX && orgY == newY+1)
      {
        return _DIR_UP;
      }
      else if (orgX + 1 == newX && orgY == newY +1)
      {
        return _DIR_UPRIGHT;
      }
      else if (orgX +1 == newX && orgY == newY)
      {
        return _DIR_RIGHT;
      }
      else if (orgX +1 == newX && orgY +1 == newY)
      {
        return _DIR_RIGHTDOWN;
      }
      else if (orgX == newX && orgY +1 == newY)
      {
        return _DIR_DOWN;
      }
      else if (orgX == 1 + newX && orgY +1 == newY)
      {
        return _DIR_DOWNLEFT;
      }
      else if (orgX == 1 + newX && orgY == newY)
      {
        return _DIR_LEFT;
      }
      else if (orgX == 1 + newX && orgY == 1 + newY)
      {
        return _DIR_LEFTUP;
      }
      else
        return _DIR_WRONG;
    }

    static int getCompDir(const zPos &myPos,const zPos &otherPos)
    {
      int x = otherPos.x - myPos.x;
      int y = otherPos.y - myPos.y;
      int absx = abs(x);
      int absy = abs(y);
      if (absx > absy && absy < absx/2)
        y = 0;
      else if (absx < absy && absx < absy/2)
        x = 0;

      int bkDir = 0;

      if (x < 0 &&  y < 0)
        bkDir = 7;
      else if (x < 0 && 0 == y)
        bkDir = 6;
      else if (x < 0 && y > 0)
        bkDir = 5;
      else if (0 == x && y > 0)
        bkDir = 4;
      else if (x > 0 && y > 0)
        bkDir = 3;
      else if (x > 0 && 0 == y)
        bkDir = 2;
      else if (x > 0 && y < 0)
        bkDir = 1;
      else if (0 == x && y < 0)
        bkDir = 0;
      return bkDir;
    }
    const zPosIVector &getScreenByRange(const zPos &pos,const int range);
    const zPosIVector &getNineScreen(const zPosI &posi)
    {
      NineScreen_map_const_iter iter = ninescreen.find((uint32_t)posi);
      if (iter != ninescreen.end())
      {
        return iter->second;
      }
      return ninescreen[(uint32_t)-1];
    }

    const zPosIVector &getDirectScreen(const zPosI &posi,const int dir)
    {
      NineScreen_map_const_iter iter = direct_screen[dir].find((uint32_t)posi);
      if (iter != direct_screen[dir].end())
      {
        return iter->second;
      }
      return direct_screen[dir][(uint32_t)-1];
    }

    const zPosIVector &getReverseDirectScreen(const zPosI &posi,const int dir)
    {
      NineScreen_map_const_iter iter = reversedirect_screen[dir].find((uint32_t)posi);
      if (iter != reversedirect_screen[dir].end())
      {
        return iter->second;
      }
      return reversedirect_screen[dir][(uint32_t)-1];
    }

};

