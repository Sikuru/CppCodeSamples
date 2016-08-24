#pragma once

#include <BoostAsioLib/BAConnectionManager.h>
#include <BoostAsioLib/Sync.h>
#include <boost/scoped_ptr.hpp>

namespace BoostAsioLib
{
	class BAConnectionManager_Impl;

	class BAConnectionManager : public IBAConnectionManager
	{
	public:
		BAConnectionManager();
		virtual ~BAConnectionManager();

		IBAConnectionPtr Get(long long socket_id) override;
		bool Register(IBAConnectionPtr connection) override;
		void Unregister(long long socket_id) override;

	private:
		void Destroy() override { delete this; };

	private:
		boost::scoped_ptr<BAConnectionManager_Impl>	_p_impl;
		SyncObject _lock;
	};

}