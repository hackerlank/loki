#pragma once

#include <unordered_map>
#include "Login.pb.h"
#include <memory>
#include "singleton.h"
#include "mutex.h"
#include "ACLZone.h"

typedef std::shared_ptr<Login::t_GYList_FL> t_GYList_FL_ptr;

class GYListManager : public Singleton<GYListManager>
{
public:
   bool put(const GameZone_t & gameZone, const t_GYList_FL_ptr& gy);
   void disableAll(const GameZone_t& );
   t_GYList_FL_ptr getAvl(const GameZone_t& gameZone);
   bool verifyVersion(const GameZone_t& gameZone, uint32_t verify_client_version, uint32_t & retcode);
   
   uint32_t getOnline();
   
private:
   std::mutex mutex_;
   std::unordered_multimap<GameZone_t, t_GYList_FL_ptr> gyData_;
   
};

