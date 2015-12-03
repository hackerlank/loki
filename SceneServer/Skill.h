#pragma once
#include "zDatabaseManager.h"
#include "type.h"
#include "zTime.h"
#include "SkillBase.h"
#include "zUniqueID.h"
#include "zSceneEntry.h"
#include "Cmd.h"

/**
 * \brief 技能压缩结构
 */
struct ZlibSkill
{
  DWORD count;
  BYTE data[0];
  ZlibSkill()
  {
    count = 0;
  }
};

/**
 * \brief 技能存档单元
 */
struct SaveSkill
{
  DWORD type;
  DWORD level;
};

/**
 * \brief 技能分类
 *
 */
enum SkillType
{
  SKILL_TYPE_DAMAGE = 1, ///直接伤害
  SKILL_TYPE_RECOVER, ///恢复系
  SKILL_TYPE_BUFF, ///增益效果
  SKILL_TYPE_DEBUFF, ///负面效果
  SKILL_TYPE_SUMMON, ///召唤
  SKILL_TYPE_RELIVE  ///复活
};

//sky 新增技能特殊属性枚举
enum
{
	SPEC_NULL		= 0,	//sky 空	
	SPEC_MAGIC		= 1,	//sky 魔法类
	SPEC_PHYSICS	= 2,	//sky 物理类
	SPEC_IMMUNE		= 4,	//sky 可免疫
	SPEC_REBOUND	= 8,	//sky 可被反弹
	SPEC_TREATMENT	= 16,	//sky 治疗
	SPEC_BUFF		= 32,	//sky 有益BUFF
	SPEC_DEBUFF		= 64,	//sky 减益BUFF
	SPEC_DRUG		= 128,	//sky 毒
	SPEC_OTHER		= 256,	//sky 其他
};

/**
 * \brief 技能类,定义技能的基本特性及攻击执行方法
 */
struct zSkill : zEntry
{
  static const DWORD maxUniqueID = 100000;
  public:
    t_Skill data;
    const zSkillB *base;
    const zSkillB *actionbase;

    static zSkill *create(SceneEntryPk *pEntry,DWORD id,DWORD level);
    static zSkill *createTempSkill(SceneEntryPk *pEntry,DWORD id,DWORD level);
    static zSkill *load(SceneEntryPk *pEntry,const SaveSkill *s);
    bool canUpgrade(SceneEntryPk *pEntry);
    bool setupSkillBase(SceneEntryPk *pEntry);
    bool getSaveData(SaveSkill *save);
    bool canUse();
    bool checkSkillBook(bool nextbase=false);
    bool checkSkillStudy(bool nextbase=false);
    void resetUseTime();
    void clearUseTime();
    void refresh(bool ignoredirty=false);

	//sky 新增技能特殊属性检测函数
	bool IsMagicSkill();	//sky 检测技能是否是魔法系技能
	bool IsPhysicsSkill();	//sky 检测技能是否是物理系技能
	bool IsImmuneSkill();	//sky 检测技能是否能被免疫
	bool IsReboundSkill();	//sky 检测技能是否能被反射
	bool IsTreatmentSkill();//sky 检测技能是否是治疗系
	bool IsBuffSkill();		//sky 检测技能是否是增益BUFF系
	bool IsDeBuffSkill();	//sky 检测技能是否是减益BUFF系
	bool IsDrugSkill();		//sky 检测技能是否是毒系
	bool IsOtherSkill();	//sky 检测技能是否是其他系

    ~zSkill();
    QWORD lastUseTime;
  private:
    bool dirty;
    bool istemp;
    bool needSave;
    bool inserted;
    SceneEntryPk *_entry;
    const Cmd::stAttackMagicUserCmd *curRevCmd;
    DWORD curRevCmdLen;
    zSkill();

    //static DWORD uniqueID;

// 新增加的技能方法
  public:
    bool action(const Cmd::stAttackMagicUserCmd *rev,const DWORD cmdLen);
    inline void doOperation(const SkillStatus *pSkillStatus);
    inline void doPassivenessSkill();
    inline bool showMagicToAll();
    inline bool findAttackTarget(const SkillStatus *pSkillStatus,zPos &pd,DWORD &count,SWORD rangDamageBonus=0);
    inline const zSkillB *getNewBase();
};
