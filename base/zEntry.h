#pragma once
#include "type.h"
#include <strings.h>

#pragma pack(1)
struct zEntryC
{
	/**
	* \brief entry的数据ID，不同类型的Entry可能会重复,此时不能实现从ID查找entry
	*/
	DWORD id;
	/**
	* \brief entry的临时id,建议在实现EntryManager时，保证分配唯一
	*/
	DWORD tempid;
	/**
	* \brief entry的名字，不同类型的Entry可能会重复,此时不能实现从名字查找entry
	*/
	char name[MAX_NAMESIZE+1];
	zEntryC()
	{
		id=0;
		tempid=0;
		bzero(name,sizeof(name));
	};
};

/**
* \brief 回调函数类模板
*/
template <typename T,typename RTValue = bool>
struct zEntryCallback
{
	virtual RTValue exec(T *e)=0;
	virtual ~zEntryCallback(){};
};

struct zEntry:public zEntryC
{
	virtual ~zEntry(){};
	zEntry():zEntryC()
	{
	};
};
#pragma pack()
