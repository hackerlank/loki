#pragma once
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <string>
#include "type.h"

/**
* \brief 配置文件解析器声明
*/
/**
* \brief zXMLParser定义
* 
* 主要提供了节点的浏览,和其属性的得到.
*/
class zXMLParser
{
public:
	zXMLParser();
	~zXMLParser();

	bool initFile(const std::string &xmlFile);
	bool initFile(const char *xmlFile);
	bool initStr(const std::string &xmlStr);
	bool initStr(const char *xmlStr);
	bool init();
	void final();
	std::string & dump(std::string & s,bool format=false);
	std::string & dump(xmlNodePtr dumpNode,std::string & s,bool head=true);
	xmlNodePtr getRootNode(const char *rootName);
	xmlNodePtr getChildNode(const xmlNodePtr parent,const char *childName);
	xmlNodePtr getNextNode(const xmlNodePtr node,const char *nextName);
	DWORD getChildNodeNum(const xmlNodePtr parent,const char *childName);

	xmlNodePtr newRootNode(const char *rootName);
	xmlNodePtr newChildNode(const xmlNodePtr parent,const char *childName,const char *content);
	bool newNodeProp(const xmlNodePtr node,const char *propName,const char *prop);

	bool getNodePropNum(const xmlNodePtr node,const char *propName,void *prop,int propSize);
	bool getNodePropStr(const xmlNodePtr node,const char *propName,void *prop,int propSize);
	bool getNodePropStr(const xmlNodePtr node,const char *propName,std::string &prop);
	bool getNodeContentNum(const xmlNodePtr node,void *content,int contentSize);
	bool getNodeContentStr(const xmlNodePtr node,void *content,int contentSize);
	bool getNodeContentStr(const xmlNodePtr node,std::string &content);
	bool getNodeContentStr(const xmlNodePtr node,std::string &content,bool head );
private:
	BYTE *charConv(BYTE *in,const char *fromEncoding,const char *toEncoding);
	xmlDocPtr doc;
};
