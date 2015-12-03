#pragma once
#include "type.h"
#include <string>
#include <vector>
#include <ctype.h>
#include <string.h>
#include "ServerType.h"
#include "zProperties.h"


namespace loki
{
	extern zProperties global;
}

struct odds_t
{
	DWORD upNum;
	DWORD downNum;
};

#if 0
typedef enum
{
	UNKNOWNSERVER  =  0, /** 未知服务器类型 */
	SUPERSERVER      =  1, /** 管理服务器 */
	LOGINSERVER     =  10, /** 登陆服务器 */
	RECORDSERVER  =  11, /** 档案服务器 */
	BILLSERVER      =  12, /** 计费服务器 */
	SESSIONSERVER  =  20, /** 会话服务器 */
	SCENESSERVER  =  21, /** 场景服务器 */
	GATEWAYSERVER  =  22, /** 网关服务器 */
	MINISERVER      =  23    /** 小游戏服务器 */
}ServerType;
#endif

//从字符串中查找第pos(从零开始)个数字，如果未找到返回defValue
template <typename T>
WORD getAllNum(const char *s,std::vector<T> & data)
{
	size_t i;
	int count = 0;
	if (s == NULL) return count;
	bool preIsD = false;
	for (i = 0; i < strlen(s); i++)
	{
		if (isdigit(*(s + i)))
		{
			if (!preIsD)
			{
				count++;
				data.push_back(atoi(s+i));
			}
			preIsD = true;
		}
		else
			preIsD = false;
	}
	return count;
}

//随机产生min~max之间的数字，包括min和max
int randBetween(int min,int max);

//获取几分之的几率
bool selectByOdds(const DWORD upNum,const DWORD downNum);

//获取几分之几的几率
bool selectByt_Odds(const odds_t &odds);

//获取百分之的几率
bool selectByPercent(const DWORD percent);

//获取万分之的几率
bool selectByTenTh(const DWORD tenth);


//获取十万分之的几率
bool selectByLakh(const DWORD lakh);

//获取亿分之之的几率
bool selectByOneHM(const DWORD lakh);

//获取当前时间字符串，需要给定格式
void getCurrentTimeString(char *buffer,const int bufferlen,const char *format);

char *getTimeString(time_t t,char *buffer,const int bufferlen,const char *format);

char *getTimeString(time_t t,char *buffer,const int bufferlen);


//手动调用构造函数，不分配内存               
	template<class _T1>                          
inline  void constructInPlace(_T1  *_Ptr)    
{                                            
	new (static_cast<void*>(_Ptr)) _T1();    
}                                            

template <typename V>
class Parse
{
public:
	V* operator () (const std::string& down,const std::string& separator_down)
	{
		std::string::size_type pos = 0;
		if  ( (pos = down.find(separator_down)) != std::string::npos ) {

			std::string first_element = down.substr(0,pos);
			std::string second_element = down.substr(pos+separator_down.length());
			return new V(first_element,second_element);
		}

		return NULL;
	}
};

template <typename V>
class Parse3
{
public:
	V* operator () (const std::string& down,const std::string& separator_down)
	{
		std::string::size_type pos = 0;
		if  ( (pos = down.find(separator_down)) != std::string::npos ) {

			std::string first_element = down.substr(0,pos);
			std::string::size_type npos = 0;
			if ((npos = down.find(separator_down,pos+separator_down.length())) != std::string::npos) {
				std::string second_element = down.substr(pos+separator_down.length(),npos-pos);
				std::string third_element = down.substr(npos+separator_down.length());
				return new V(first_element,second_element,third_element);
			}
		}

		return NULL;
	}
};

/**
* \brief  分隔由二级分隔符分隔的字符串
* \param list 待分隔的字符串
* \param dest 存储分隔结果，必须满足特定的语义要求
* \param separator_up 一级分隔符
* \param separator_down 二级分隔符     
*/
template <template <typename> class P = Parse>
class Split
{
public:

	template <typename T>
	void operator() (const std::string& list,T& dest,const std::string& separator_up = ";",const std::string& separator_down = ",")
	{  
		typedef typename T::value_type value_type;
		typedef typename T::pointer pointer;

		std::string::size_type lpos = 0;
		std::string::size_type pos = 0;
		P<value_type> p;


		while ( ( lpos = list.find(separator_up,pos)) != std::string::npos) {
			/*
			std::string down = list.substr(pos,lpos - pos);
			std::string::size_type dpos = 0;
			if  ( (dpos = down.find(separator_down)) != std::string::npos ) {

			std::string first_element = down.substr(0,dpos);
			std::string second_element = down.substr(dpos+separator_down.length());
			dest.push_back(typename T::value_type(first_element,second_element));
			}
			pos = lpos+1;
			*/
			std::string down = list.substr(pos,lpos - pos);
			pointer v = p(down,separator_down);
			if (v) {
				dest.push_back(*v);
				SAFE_DELETE(v);
			}
			pos = lpos+1;
		}

		std::string down = list.substr(pos,lpos - pos);
		pointer v = p(down,separator_down);
		if (v) {
			dest.push_back(*v);
			SAFE_DELETE(v);
		}
	}
};

