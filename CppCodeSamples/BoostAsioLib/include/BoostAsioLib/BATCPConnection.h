#pragma once

#include <BoostAsioLib/BAConnection.h>
#include <BoostAsioLib/BAEvent.h>
#include <BoostAsioLib/RefCount.h>
#include <BoostAsioLib/Sync.h>
#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/intrusive_ptr.hpp>

namespace BoostAsioLib
{
	class IBATCPConnection;
	typedef boost::intrusive_ptr<IBATCPConnection> IBATCPConnectionPtr;

	typedef boost::function<void(long long, const boost::system::error_code&, size_t)> OnReceivedCallback;

	class IBATCPConnection : public IBAConnection, public IAccessible<IBATCPConnection>
	{
	public:
		static IBATCPConnectionPtr Create(boost::asio::io_service& iosvr, long long socket_id);

	public:
		virtual void SetOnReceived(OnReceivedCallback recv_callback) = 0;
		virtual boost::asio::ip::tcp::socket& GetSocket() = 0;

		virtual void Start() = 0;
		virtual void Stop() = 0;
	};
}