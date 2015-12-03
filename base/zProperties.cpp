#include "zProperties.h"
#include "zString.h"
#include <vector>

DWORD zProperties::parseCmdLine(const std::string &cmdLine)
{
	return parseCmdLine(cmdLine.c_str());
}

DWORD zProperties::parseCmdLine(const char *cmdLine)
{
  std::vector<std::string> sv;
  stringtok(sv,cmdLine);
  for(std::vector<std::string>::const_iterator it = sv.begin(); it != sv.end(); it++)
  {
    std::vector<std::string> ssv;
    stringtok(ssv,*it,"=",1);
    if (ssv.size() == 2)
    {
      properties[ssv[0]] = ssv[1];
    }
  }
  return properties.size();
}

