#include "zMisc.h"
#include <cstdlib>
#include <time.h>

namespace loki
{
	zProperties global;
}

int randBetween(int min,int max)
{
  if (min==max)
    return min;
  else if (min > max)
    return max + (int) (((double) min - (double)max + 1.0) * rand() / (RAND_MAX + 1.0));
  else
    return min + (int) (((double) max - (double)min + 1.0) * rand() / (RAND_MAX + 1.0));
}

bool selectByOdds(const DWORD upNum,const DWORD downNum)
{
  DWORD m_rand;
  if (downNum < 1) return false;
  if (upNum < 1) return false;
  if (upNum > downNum - 1) return true;
  m_rand = 1 + (DWORD) ((double)downNum * rand() / (RAND_MAX + 1.0));
  return (m_rand <= upNum);
}

bool selectByt_Odds(const odds_t &odds)
{
  return selectByOdds(odds.upNum,odds.downNum);
}

bool selectByPercent(const DWORD percent)
{
  return selectByOdds(percent,100);
}

bool selectByTenTh(const DWORD tenth)
{
  return selectByOdds(tenth,10000);
}

bool selectByLakh(const DWORD lakh)
{
  return selectByOdds(lakh,100000);
}

bool selectByOneHM(const DWORD lakh)
{
  return selectByOdds(lakh,100000000);
}

void getCurrentTimeString(char *buffer,const int bufferlen,const char *format)
{
  time_t now;
  time(&now);
  strftime(buffer,bufferlen,format,localtime(&now));
}

char *getTimeString(time_t t,char *buffer,const int bufferlen,const char *format)
{
  strftime(buffer,bufferlen,format,localtime(&t));
  return buffer;
}

char *getTimeString(time_t t,char *buffer,const int bufferlen)
{
  return getTimeString(t,buffer,bufferlen,"%C/%m/%d %T");
}

