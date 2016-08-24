#pragma once
#include <BoostAsioLib/BAIoServiceManager.h>
#include <BoostAsioLib/BATCPConnection.h>
#include <boost/scoped_ptr.hpp>
#include <boost/bind.hpp>

class TcpServerSample
{
public:
	TcpServerSample()
		: _manager_ptr(BoostAsioLib::IBAIoServiceManager::Create(4))
		, _is_disposed(false)
	{
		_acceptor.reset(new boost::asio::ip::tcp::acceptor(_manager_ptr->GetIoService()));
		boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address_v4::any(), 9661);

		_acceptor->open(ep.protocol());
		_acceptor->bind(ep);
		_acceptor->listen();

		long long socket_id;
		boost::asio::io_service& ios = _manager_ptr->GetIoService(&socket_id);

		_accepted_conn_ptr = BoostAsioLib::IBATCPConnection::Create(ios, socket_id);
		_accepted_conn_ptr->SetOnReceived(boost::bind(&TcpServerSample::OnReceived, this, _1, _2, _3));

		_acceptor->async_accept(_accepted_conn_ptr->GetSocket(), boost::bind(&TcpServerSample::OnAccepted, this,
			_accepted_conn_ptr, boost::asio::placeholders::error));
	}

	~TcpServerSample()
	{

	}

private:
	void OnAccepted(BoostAsioLib::IBATCPConnectionPtr conn_ptr, const boost::system::error_code& errcode)
	{
		if (_is_disposed)
		{
			return;
		}

		if (!errcode)
		{
			_sockets.insert(CONN_MAP::value_type(conn_ptr->GetSocketID(), conn_ptr));
			conn_ptr->Start();
		}
		else
		{
			conn_ptr->Stop();
		}

		long long socket_id;
		boost::asio::io_service& ios = _manager_ptr->GetIoService(&socket_id);
		_accepted_conn_ptr = BoostAsioLib::IBATCPConnection::Create(ios, socket_id);
		_accepted_conn_ptr->SetOnReceived(boost::bind(&TcpServerSample::OnReceived, this, _1, _2, _3));

		_acceptor->async_accept(_accepted_conn_ptr->GetSocket(), boost::bind(&TcpServerSample::OnAccepted, this, _accepted_conn_ptr, boost::asio::placeholders::error));
	}

	void OnReceived(long long socket_id, const boost::system::error_code& errcode, size_t bytes_transferred)
	{
		if (bytes_transferred == 0 || errcode)
		{
			// disconnect ?
			_sockets.erase(socket_id);
			printf("OnDisconnected (OnReceived) ; id=%I64d, thread=%u\n", socket_id, GetCurrentThreadId());
		}
		else
		{
			// success ?
			printf("OnReceived ; id=%I64d, thread=%u\n", socket_id, GetCurrentThreadId());
		}
	}

private:
	BoostAsioLib::IBAIoServiceManagerPtr _manager_ptr;

	boost::scoped_ptr<boost::asio::ip::tcp::acceptor> _acceptor;
	BoostAsioLib::IBATCPConnectionPtr _accepted_conn_ptr;

	typedef std::map<long long, BoostAsioLib::IBATCPConnectionPtr> CONN_MAP;
	CONN_MAP _sockets;

	bool _is_disposed;
};

