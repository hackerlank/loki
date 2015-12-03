#pragma once

#include "type.h"
#include "zSceneEntry.h"
#include <vector>
#include <list>
#include <map>
#include "zString.h"

typedef std::vector<zPos> zPosIndex;

/**
 * \brief 区域的定义
 * 包括开头、中心、结束点的坐标和区域内非阻挡点索引
 *
 */
struct zRegion
{
	zPos s;        /// 左上点
	zPos c;        /// 中心点
	zPos e;        /// 右下点
	zPosIndex index;  /// 非阻挡点索引
	zRegion()
	{
	}
	const bool isIn(const zPos &pos) const
	{
		return pos >= s && pos <= e;
	}

};


/**
 * \brief 区域类型的定义
 * 包括类型、大小等
 *
 */
struct ZoneTypeDef
{
  enum
  {
    ZONE_NONE        =  0, //一般区
    ZONE_PK_SAFE      =  1,   // pk安全区
    ZONE_ABSOLUTE_SAFE    =  2,   // 绝对安全区
    ZONE_RELIVE        =  4,   // 死亡重生区
    ZONE_NEWBIE        =  8,   // 新手出生区
    ZONE_SPORTS        =  16,   // 竞技区
    ZONE_FOREIGN_RELIVE    =  32,   // 国外死亡重生区
    ZONE_PRIVATE_STORE     =   128,      // 摆摊区
    ZONE_PRIVATE_DARE     =   256,      // 国战跳转区
    ZONE_PRIVATE_RELIVE    =  512,    // 国战复活区
    ZONE_PRIVATE_UNION    =   1024,     // 帮会所属地跳转区
    ZONE_COUNTRY_WAR    =   2048,    // 边境的国战跳转区
    ZONE_PRIVATE_DARE_UNION    =  4096,    // 帮会战挑战方跳转区
    ZONE_DARE_SAFE      =   8192,    // 国战时为绝对安全区,平时为非安全区
    ZONE_PRIVATE_THIRD_UNION        =       16384,    // 帮会所属地第三方跳转区
    ZONE_EMPEROR_DEF_RELIVE    =   32768,    // 皇城战守方复活区
    ZONE_EMPEROR_ATT_RELIVE    =  65536,    // 皇城战攻方复活区  
  };

  zPos  pos;                //坐标
  WORD  width;              //矩形宽
  WORD  height;              //矩形长
  DWORD type;                //矩形区域的类型
  zSceneEntry::SceneEntryState initstate;  //初始状态,隐藏或者非隐藏
  zSceneEntry::SceneEntryState state;    //当前状态,隐藏或者非隐藏
  zRegion region;              //范围数据,包括范围内的可行走点数据索引

  /**
   * \brief 构造函数
   *
   */
  ZoneTypeDef() : pos(),region()
  {
    width = 0;
    height = 0;
    type = 0;
    initstate = zSceneEntry::SceneEntry_Normal;
    state = zSceneEntry::SceneEntry_Normal;
  }
};

typedef std::vector<ZoneTypeDef> ZoneTypeDefVector;

const DWORD NANJIAO_MAP_ID = 135;
const DWORD DONGJIAO_MAP_ID = 136;
const DWORD BIANJING_MAP_ID = 137;
const DWORD WANGCHENG_MAP_ID = 139;
const DWORD PK_MAP_ID = 213;

//配置文件中关于NPC的定义
/**
 * \brief npc定义结构
 * 包括编号,出生位置、范围、间隔等信息 
 *
 */
struct t_NpcDefine
{
	DWORD id;                //编号
	char  name[MAX_NAMESIZE+1];        //名称
	zPos  pos;                //坐标
	WORD  width;              //矩形宽
	WORD  height;              //矩形长
	DWORD num;                //数量
	DWORD interval;              //刷新时间
	zSceneEntry::SceneEntryState initstate;  //初始状态,隐藏或者非隐藏
	zRegion region;              //范围数据,包括范围内的可行走点数据索引

	//std::vector<zPos> path;//固定移动的路线

	DWORD rushID;//触发攻城ID
	BYTE rushRate;//触发攻城的几率
	DWORD rushDelay;//触发攻城的延时

	int scriptID;//智能脚本的id
	DWORD intervalAmendment;		//sky 复活时间修正值
	DWORD AmendmentFactor;		//sky 修正系数
	WORD Camp;					//sky 所属阵营索引值

	std::map<DWORD,std::pair<DWORD,DWORD> > petList;
	std::map<DWORD,std::pair<DWORD,DWORD> > summonList;
	std::map<DWORD,std::pair<DWORD,DWORD> > deathSummonList;
	std::list< std::pair<DWORD,zPos> > dieList;
  t_NpcDefine() : pos(),region()
  {
    id = 0;
    bzero(name,sizeof(name));
    width = 0;
    height = 0;
    num = 0;
    interval = 0;
    rushID = 0;
    rushRate = 0;
    rushDelay = 0;
    initstate = zSceneEntry::SceneEntry_Normal;
    scriptID = 0;
	intervalAmendment = 0;
	AmendmentFactor = 0;
	Camp = 0;
  }
  void fillNpcMap(const char * tempChar,std::map<DWORD,std::pair<DWORD,DWORD> > &map)
  {
    std::vector<std::string> vs;
    std::vector<std::string> sub_vs;
    vs.clear();
    stringtok(vs,tempChar,";");
    for (DWORD i=0; i<vs.size(); i++)
    {
      sub_vs.clear();
      stringtok(sub_vs,vs[i].c_str(),"-");
      if (sub_vs.size()==2)
        map[atoi(sub_vs[0].c_str())] = std::make_pair(atoi(sub_vs[1].c_str()),100);
      else if (sub_vs.size()==3) //npcID,num,rate
        map[atoi(sub_vs[0].c_str())] = std::make_pair(atoi(sub_vs[1].c_str()),atoi(sub_vs[2].c_str()));
    }
  }

  //sky 计算复活时间修改系数
  void InitIntervalAmendment()
  {
	  //sky 复活时间为0或者数量小于1的时候
	  if(interval == 0 || num <= 1)
		  return;

	  //sky 系数取复活时间与刷怪数量的平均值(时间先转换成毫秒级)
	  intervalAmendment = (interval * 1000)/num;
	  AmendmentFactor = num;

	  return;
  }

  //sky 获取复活时间修正值
  //Dead 生命状态 (true:重生 false:死亡)
  DWORD GetIntervalAmendmaent(bool Dead)
  {
	  if(Dead)
	  {
		  //sky 如果是重生,把重生的系数提高
		  AmendmentFactor++;
		  if(AmendmentFactor > num)
			  AmendmentFactor = num;
	  }
	  else
	  {
		  //sky 如果是死亡,就把重生的系数减少
		  AmendmentFactor--;
		  if(AmendmentFactor < 0)
			  AmendmentFactor = 0;
	  }

	  //sky 返回修正值
	  return intervalAmendment * AmendmentFactor;
  }
};

typedef std::vector<t_NpcDefine> NpcDefineVector;
