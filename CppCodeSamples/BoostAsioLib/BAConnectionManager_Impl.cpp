#include "stdafx.h"
#include "BAConnectionManager.h"
#include <BoostAsioLib/BAConnection.h>
#include <map>

namespace BoostAsioLib
{
	class BAConnectionManager_Impl : public IBAConnectionManager
	{
	public:
		BAConnectionManager_Impl() { }
		virtual ~BAConnectionManager_Impl() { }

		IBAConnectionPtr Get(long long socket_id) override
		{
			CONN_MAP::iterator itr = _conn_map.find(socket_id);
			if (itr == _conn_map.end())
			{
				return nullptr;
			}
			else
			{
				return itr->second;
			}
		}

		bool Register(IBAConnectionPtr connection) override
		{
			return _conn_map.insert(CONN_MAP::value_type(connection->GetSocketID(), connection)).second;
		}

		void Unregister(long long socket_id) override
		{
			_conn_map.erase(socket_id);
		}

	private:
		void Destroy() override { };		// noop

	private:
		typedef std::map<long long, IBAConnectionPtr> CONN_MAP;

		CONN_MAP _conn_map;
	};

	/////////////////////

	IBAConnectionManagerPtr IBAConnectionManager::Create()
	{
		return IBAConnectionManagerPtr(new BAConnectionManager);
	}

	/////////////////////

	BAConnectionManager::BAConnectionManager()
		: _p_impl(new BAConnectionManager_Impl)
	{
	}

	BAConnectionManager::~BAConnectionManager()
	{
	}

	IBAConnectionPtr BAConnectionManager::Get(long long socket_id)
	{
		ScopedReadLock auto_lock(&_lock);
		return _p_impl->Get(socket_id);
	}

	bool BAConnectionManager::Register(IBAConnectionPtr connection)
	{
		ScopedWriteLock auto_lock(&_lock);
		return _p_impl->Register(connection);
	}

	void BAConnectionManager::Unregister(long long socket_id)
	{
		ScopedWriteLock auto_lock(&_lock);
		_p_impl->Unregister(socket_id);
	}
}