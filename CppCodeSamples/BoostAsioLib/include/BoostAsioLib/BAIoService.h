#pragma once

#include <BoostAsioLib/RefCount.h>
#include <boost/asio.hpp>
#include <boost/intrusive_ptr.hpp>

namespace BoostAsioLib
{
	class IBAIoService;
	typedef boost::intrusive_ptr<IBAIoService> IBAIoServicePtr;

	class IBAIoService : public RefCount
	{
	public:
		static IBAIoServicePtr Create();

	public:
		virtual boost::asio::io_service& GetIoService(OUT long long* io_service_id = nullptr) = 0;
	};
}