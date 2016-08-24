#include "stdafx.h"
#include <BoostAsioLib/BAIoServiceManager.h>
#include <BoostAsioLib/BAIoService.h>
#include <atomic>

namespace BoostAsioLib
{
	class BAIoServiceManager_Impl : public IBAIoServiceManager, private boost::noncopyable
	{
	public:
		BAIoServiceManager_Impl(short thread_count)
			: _ios_seed(0)
			, _thread_count(static_cast<long long>(thread_count))
		{
			for (int i = 0; i < thread_count; ++i)
			{
				IBAIoServicePtr iosvr_ptr(IBAIoService::Create());
				_vx_iosvr.push_back(iosvr_ptr);
			}
		}

		virtual ~BAIoServiceManager_Impl()
		{
			for (std::vector<IBAIoServicePtr>::iterator itr = _vx_iosvr.begin(); itr != _vx_iosvr.end(); ++itr)
			{
				IBAIoServicePtr iosvr_ptr = *itr;
				iosvr_ptr->GetIoService().stop();
			}

			_vx_iosvr.clear();
		}

		boost::asio::io_service& GetIoService(OUT long long* io_service_id = nullptr) override
		{
			long long int ios_seed = ++_ios_seed;
			if (io_service_id != nullptr)
			{
				*io_service_id = ios_seed;
			}
			return _vx_iosvr[static_cast<size_t>(ios_seed % _thread_count)]->GetIoService();
		}
	private:
		void Destroy() override { delete this; };

	private:
		std::vector<IBAIoServicePtr> _vx_iosvr;
		long long int _thread_count;

		std::atomic<long long int> _ios_seed;
	};

	IBAIoServiceManagerPtr IBAIoServiceManager::Create(short thread_count)
	{
		return IBAIoServiceManagerPtr(new BAIoServiceManager_Impl(thread_count));
	}
}