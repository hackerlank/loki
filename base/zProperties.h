#pragma once
#include <iostream>
#include <unordered_map>
#include "type.h"

/**
* \brief 属性关联类容器，所有属性关键字和值都使用字符串代表，关键字不区分大小写
*
*/
class zProperties
{

public:
	const std::string &getProperty(const std::string &key)
	{
		return properties[key];
	}

	void setProperty(const std::string &key,const std::string &value)
	{
		properties[key] = value;
	}

	std::string & operator[] (const std::string &key)
	{
		//fprintf(stderr,"properties operator[%s]\n",key.c_str());
		return properties[key];
	}

	void dump(std::ostream &out)
	{
		property_hashtype::const_iterator it;
		for(it = properties.begin(); it != properties.end(); it++)
			out << it->first << " = " << it->second << std::endl;
	}

	DWORD parseCmdLine(const std::string &cmdLine);
	DWORD parseCmdLine(const char *cmdLine);

protected:

	/**
	* \brief hash函数
	*
	*/
	/*struct key_hash : public std::unary_function<const std::string,size_t>
	{
	size_t operator()(const std::string &x) const
	{
	std::string s = x;
	hash<string> H;
	// _Hash<string> H;
	//转化字符串为小写
	to_lower(s);
	//tolower(s);
	//return H(s);
	return 0;
	}
	};*/
	/**
	* \brief 判断两个字符串是否相等
	*
	*/
	/*struct key_equal : public std::binary_function<const std::string,const std::string,bool>
	{
	bool operator()(const std::string &s1,const std::string &s2) const
	{
	// return strcasecmp(s1.c_str(),s2.c_str()) == 0;
	return (s1==s2);
	}
	};*/

	/**
	* \brief 字符串的hash_map
	*
	*/
	//typedef hash_map<std::string,std::string,key_hash,key_equal> property_hashtype;

	//typedef hash_map<std::string,std::string,key_hash,key_equal> property_hashtype;
	typedef std::unordered_map<std::string,std::string> property_hashtype;
	property_hashtype properties;      /**< 保存属性的键值对 */
};
