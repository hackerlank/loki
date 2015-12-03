#pragma once
#include "zSceneEntry.h"
#include "zXMLParser.h"
#include "zMisc.h"
#include "string.h"
#include "strings.h"

/**
 * \brief 点结构
 */
struct Point
{
  /// 名称
  char name[MAX_NAMESIZE];

  /// 位置
  zPos pos;

  /**
   * \brief 构造函数初始化
   */
  Point() : pos()
  {
    bzero(name,sizeof(name));
  }

  /**
   * \brief 拷贝构造
   * \param point 点
   */
  Point(const Point &point)
  {
    bcopy(point.name,name,sizeof(name));
    pos = point.pos;
  }

  /**
   * \brief 重载操作符,拷贝点
   * \param point 被拷贝的点
   */
  Point & operator= (const Point &point)
  {
    bcopy(point.name,name,sizeof(name));
    pos = point.pos;
	return *this;
  }
};

// [ranqd] 区域类型定义
struct zZone{
	zPos pos;// 左上角的点
	int  width; // 区域宽度
	int  height; // 区域高度

	zZone()
	{
		pos = zPos(0,0);
		width = 0;
		height = 0;
	}
	bool IsInZone( zPos p ) // 判断点p是否在此区域内
	{
		return  p.x >= pos.x && p.x < pos.x + width && p.y >= pos.y && p.y < pos.y + height;
	}
	zPos GetRandPos() // 返回该区域内随机一点
	{
		zPos p;
		p.x = randBetween(pos.x, pos.x + width - 1);
		p.y = randBetween(pos.y, pos.y + height - 1);
		return p;
	}
	bool IsEmpty()
	{
		return width == 0 && height == 0;
	}
};
// [ranqd] 带地图名称的区域
struct zPoint
{
	/// 地图名称
	char name[MAX_NAMESIZE];

	/// 地图区域
	zZone pos;

	/**
	* \brief 构造函数初始化
	*/
	zPoint() : pos()
	{
		bzero(name,sizeof(name));
	}

	/**
	* \brief 拷贝构造
	* \param point 点
	*/
	zPoint(const zPoint &point)
	{
		bcopy(point.name,name,sizeof(name));
		pos = point.pos;
	}

	/**
	* \brief 重载操作符,拷贝点
	* \param point 被拷贝的点
	*/
	zPoint & operator= (const zPoint &point)
	{
		bcopy(point.name,name,sizeof(name));
		pos = point.pos;
		return *this;
	}
};
/**
 * \brief 传点定义
 */
struct WayPoint
{
  friend class WayPointM;
  private:

    /// 目的坐标点集合
 //   std::vector<Point> dest;
    // [ranqd] 传送点更改为区域型
	std::vector<zPoint> dest;

    /// 目的坐标点数目
    WORD destC;
  public:
    /// 传送坐标点集合
 // std::vector<zPos> point;
	// [ranqd] 传送点更改为区域型
	std::vector<zZone> point;
    /// 传送坐标点数目
    WORD pointC;

    bool init(zXMLParser *parser,const xmlNodePtr node,DWORD countryid);
    WayPoint();
    const Point getRandDest();
    const zPos getRandPoint();
};

class WayPointM
{
  private:
    /// 传点集合
    std::vector<WayPoint> wps;
  public:
    bool addWayPoint(const WayPoint &wp);
    WayPoint *getWayPoint(const zPos &pos);
    WayPoint *getWayPoint(const char *filename);
    WayPoint *getRandWayPoint();
};
