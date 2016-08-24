#pragma once

#include <BoostAsioLib/RefCount.h>
#include <boost/intrusive_ptr.hpp>

namespace BoostAsioLib
{
	class IBAConnection : public RefCount
	{
	public:
		virtual long long GetSocketID() const = 0;
	};

	typedef boost::intrusive_ptr<IBAConnection> IBAConnectionPtr;
}