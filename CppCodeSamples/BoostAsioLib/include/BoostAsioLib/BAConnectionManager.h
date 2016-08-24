#pragma once

#include <BoostAsioLib/RefCount.h>
#include <boost/intrusive_ptr.hpp>

namespace BoostAsioLib
{
	class IBAConnectionManager;
	typedef boost::intrusive_ptr<IBAConnectionManager> IBAConnectionManagerPtr;

	class IBAConnection;
	typedef boost::intrusive_ptr<IBAConnection> IBAConnectionPtr;

	class IBAConnectionManager : public RefCount
	{
	public:
		static IBAConnectionManagerPtr Create();

	public:
		virtual IBAConnectionPtr Get(long long socket_id) = 0;
		virtual bool Register(IBAConnectionPtr connection) = 0;
		virtual void Unregister(long long socket_id) = 0;
	};
}