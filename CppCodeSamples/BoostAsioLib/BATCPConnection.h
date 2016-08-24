#include "stdafx.h"
#include <BoostAsioLib/BATCPConnection.h>
#include <BoostAsioLib/BAEvent.h>
#include <BoostAsioLib/Sync.h>
#include <boost/scoped_ptr.hpp>

namespace BoostAsioLib
{
	class BATCPConnection_Impl;

	class BATCPConnection : public IBATCPConnection
	{
	public:
		BATCPConnection(boost::asio::io_service& iosvr, long long socket_id);
		virtual ~BATCPConnection();

		void SetOnReceived(OnReceivedCallback recv_callback) override;
		boost::asio::ip::tcp::socket& GetSocket() override;
		long long GetSocketID() const override;
		void Start() override;
		void Stop() override;

	private:
		void Destroy() override { delete this; };
		SyncObject* GetLockObj() override;
		IBATCPConnection* GetImplement() override;

	private:
		boost::scoped_ptr<BATCPConnection_Impl>	_p_impl;
		SyncObject _lock;
	};
}