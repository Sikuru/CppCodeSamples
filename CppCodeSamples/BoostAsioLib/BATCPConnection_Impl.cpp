#include "stdafx.h"
#include "BATCPConnection.h"
#include <BoostAsioLib/BATCPConnection.h>
#include <BoostAsioLib/BAEvent.h>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <iostream>

namespace BoostAsioLib
{
	class BATCPConnection_Impl : public IBATCPConnection
	{
	public:
		BATCPConnection_Impl(boost::asio::io_service& iosvr, long long socket_id)
			: _socket(iosvr)
			, _socket_id(socket_id)
		{
			std::wcout << L"new BATCPConnection(" << socket_id << L")" << std::endl;
		}

		virtual ~BATCPConnection_Impl()
		{
			_socket.close();
			std::wcout << L"delete BATCPConnection(" << _socket_id << L")" << std::endl;
		}

		void SetOnReceived(OnReceivedCallback recv_callback) override
		{
			_recv_callback = recv_callback;
		}

		boost::asio::ip::tcp::socket& GetSocket() override { return _socket; };
		long long GetSocketID() const override { return _socket_id; };

		void Start() override
		{
			Receive();
		}

		void Stop() override
		{
			_socket.close();
		}

	private:
		void Destroy() override { };		// noop
		SyncObject* GetLockObj() override { return nullptr; };
		IBATCPConnection* GetImplement() override { return nullptr; };

		void Receive()
		{
			AddRef();
			_socket.async_read_some(boost::asio::buffer(_buffer), boost::bind(&BATCPConnection_Impl::OnReceived, this,
				boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
		}

		void OnReceived(const boost::system::error_code& errcode, size_t bytes_transferred)
		{
			if (_recv_callback)
			{
				_recv_callback(_socket_id, errcode, bytes_transferred);
			}

			if (!errcode && bytes_transferred > 0)
			{
				Receive();
			}

			ReleaseRef();
		}

		void OnSent(const boost::system::error_code& errcode)
		{
		}

	private:
		long long int _socket_id;
		boost::asio::ip::tcp::socket _socket;
		boost::array<unsigned char, 8192> _buffer;

		OnReceivedCallback _recv_callback;
	};

	/////////////////////

	IBATCPConnectionPtr IBATCPConnection::Create(boost::asio::io_service& iosvr, long long socket_id)
	{
		return IBATCPConnectionPtr(new BATCPConnection(iosvr, socket_id));
	}

	/////////////////////

	BATCPConnection::BATCPConnection(boost::asio::io_service& iosvr, long long socket_id)
		: _p_impl(new BATCPConnection_Impl(iosvr, socket_id))
	{
	}

	BATCPConnection::~BATCPConnection()
	{
	}

	SyncObject* BATCPConnection::GetLockObj()
	{
		return &_lock;
	}

	IBATCPConnection* BATCPConnection::GetImplement()
	{
		return _p_impl.get();
	}

	void BATCPConnection::SetOnReceived(OnReceivedCallback recv_callback)
	{
		ScopedWriteLock auto_lock(&_lock);
		_p_impl->SetOnReceived(recv_callback);
	}

	boost::asio::ip::tcp::socket& BATCPConnection::GetSocket()
	{
		return _p_impl->GetSocket();
	}

	long long BATCPConnection::GetSocketID() const
	{
		return _p_impl->GetSocketID();
	}

	void BATCPConnection::Start()
	{
		ScopedWriteLock auto_lock(&_lock);
		_p_impl->Start();
	}

	void BATCPConnection::Stop()
	{
		ScopedWriteLock auto_lock(&_lock);
		_p_impl->Stop();
	}
}