#pragma once

#include "connection.h"
#include "context.h"
#include "record.pb.h"
#include <memory>

using namespace loki;

class GatewayTask;
using GatewayTaskPtr = std::shared_ptr<GatewayTask>;

enum class Systemstate
{
	SYSTEM_STATE_INITING,
	SYSTEM_STATE_CREATING,
	SYSTEM_STATE_SELECT,
	SYSTEM_STATE_PLAY,
	SYSTEM_STATE_WAIT_PLAY,
	SYSTEM_STATE_WAIT_UNREG,
};


class GatewayTask : public loki::context
{
private:
	//connection_ptr conn_;
	uint32_t accid_{0};
	uint64_t logintempid_{0};
	uint32_t tempid_{0};
	std::string name_;
	uint32_t country_{0};
	std::string ip_;

	std::shared_ptr<Record::t_Ret_SelectInfo_GateRecord> user_info_;

	Systemstate systemstate_{Systemstate::SYSTEM_STATE_INITING};
public:
	GatewayTask(connection_ptr conn);
	~GatewayTask();

	int dupIndex{0};
	bool inserted{false};
	uint32_t mapScreenIndex{0};

	void setIndexKey(const uint32_t screen) { mapScreenIndex = screen; }
	const uint32_t getIndexKey() const { return mapScreenIndex;  }

	void setSelectUserInfo(const std::shared_ptr<Record::t_Ret_SelectInfo_GateRecord>& a) { user_info_ = a; }

	void selectState() { systemstate_ = Systemstate::SYSTEM_STATE_SELECT; }
	Systemstate getSystemstate() const { return systemstate_; }
	void setState(Systemstate state) { systemstate_ = state; }

	//发送玩家角色信息到客户端，供客户端选择
	void refreshCharInfo();

	//角色数是否已达上限
	bool charInfoFull();
	std::shared_ptr<Record::t_CreateChar_GateRecord> createCharCmd;
	void addSelectUserInfo(const Gate::SelectUserInfo& info)
	{
		if (user_info_)
		{
			auto entry = user_info_->add_info();
			entry->CopyFrom(info);
		}
	}

	const Gate::SelectUserInfo* getSelectUserInfo(const uint32_t no)
	{
		if (user_info_ && user_info_->info_size() > no)
		{
			return &(user_info_->info(no));
		}
		return nullptr;
	}
	void 				set_accid(const uint32_t accid) { accid_ = accid; }
	uint32_t 			accid() const { return accid_; }

	void 				set_logintempid(const uint64_t n) { logintempid_ = n; }
	uint64_t			logintempid() const { return logintempid_; } 

	const uint32_t		tempid() const { return tempid_; }
	void				set_tempid(const uint32_t id) { tempid_ = id; }

	const std::string	name() const { return name_; }
	void				set_name(const std::string& n) { name_ = n; }
	uint32_t			country() const { return country_; }
	void				set_country(const uint32_t c) { country_ = c; }

	void				reg(const uint32_t no);
	const std::string   ip() const { return ip_; }
};
