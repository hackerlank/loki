#include "service.h"
#include "logger.h"

namespace loki
{
	service* service::instance_ = nullptr;

	service::service(io_service_pool& pool) :
		pool_(pool),
		signals_(pool_.get_io_service())
	{

		instance_ = this;
		LOG(INFO)<<"service create";
	}
};
