#include "stdafx.h"
#include <BoostAsioLib/BAIoService.h>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

namespace BoostAsioLib
{
	class BAIoService_Impl : public IBAIoService
	{
	public:
		BAIoService_Impl()
			: _iosvr_work(_iosvr)
			, _thread(boost::bind(&boost::asio::io_service::run, &_iosvr))
		{
		}

		virtual ~BAIoService_Impl()
		{
			_thread.join();
		}

		boost::asio::io_service& GetIoService(OUT long long* io_service_id = nullptr) override { return _iosvr; };

	private:
		void Destroy() override { delete this; };

	private:
		boost::asio::io_service _iosvr;
		boost::asio::io_service::work _iosvr_work;
		boost::thread _thread;
	};

	IBAIoServicePtr IBAIoService::Create()
	{
		return IBAIoServicePtr(new BAIoService_Impl);
	}
}