#pragma once

#include <BoostAsioLib/RefCount.h>
#include <boost/asio.hpp>
#include <boost/intrusive_ptr.hpp>

namespace BoostAsioLib
{
	class IBAIoServiceManager;
	typedef boost::intrusive_ptr<IBAIoServiceManager> IBAIoServiceManagerPtr;

	class IBAIoServiceManager : public RefCount
	{
	public:
		static IBAIoServiceManagerPtr Create(short thread_count);
	public:
		virtual boost::asio::io_service& GetIoService(OUT long long* io_service_id = nullptr) = 0;
	};
}