#pragma once

#include <boost/intrusive_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <windows.h>

namespace BoostAsioLib
{
#if( _WIN32_WINNT >= 0x0600 )
	class SyncObject
	{
	public:
		SyncObject()
		{
			InitializeSRWLock(&m_lock);
		}
		virtual ~SyncObject()
		{
		}

		void LockExclusive() { AcquireSRWLockExclusive(&m_lock); };
		void UnlockExclusive() { ReleaseSRWLockExclusive(&m_lock); };
		void LockShared() { AcquireSRWLockShared(&m_lock); };
		void UnlockShared() { ReleaseSRWLockShared(&m_lock); };

	private:
		SRWLOCK m_lock;
	};
#else
	class SyncObject
	{
	public:
		SyncObject()
		{
			InitializeCriticalSectionAndSpinCount(&m_lock, 4000);	// for multi-core cpu
		}
		virtual ~SyncObject()
		{
			DeleteCriticalSection(&m_lock);
		}

		void LockExclusive() { EnterCriticalSection(&m_lock); };
		void UnlockExclusive() { LeaveCriticalSection(&m_lock); };
		void LockShared() { EnterCriticalSection(&m_lock); };
		void UnlockShared() { LeaveCriticalSection(&m_lock); };

	private:
		CRITICAL_SECTION m_lock;
	};
#endif


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//

	class ScopedWriteLock
	{
	public:
		ScopedWriteLock(SyncObject* pSyncObject)
			: _sync_object_ptr(pSyncObject)
		{
			_sync_object_ptr->LockExclusive();
		};

		~ScopedWriteLock()
		{
			_sync_object_ptr->UnlockExclusive();
		};

	private:
		SyncObject* _sync_object_ptr;
	};

	class ScopedReadLock
	{
	public:
		ScopedReadLock(SyncObject* pSyncObject)
			: m_sync_object_ptr(pSyncObject)
		{
			m_sync_object_ptr->LockShared();
		};

		~ScopedReadLock()
		{
			m_sync_object_ptr->UnlockShared();
		};

	private:
		SyncObject* m_sync_object_ptr;
	};


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//! TBB의 accessor/con accessor를 흉내낸 이름입니다만, 매우 특별한 패턴(Blackbox, PIMPL for ThreadSafe)일 때만 사용해야 합니다.

	// 아래는 위 패턴을 사용할 때 상속받아야 할 객체이다.
	template <typename T>
	class IAccessible
	{
	public:
		virtual SyncObject* GetLockObj() = 0;
		virtual T* GetImplement() = 0;
	};


	// Accessor for read-only, -> operator를 사용할 함수들은 모두 type function( ... ) const; 형태로 정의되어야 한다.
	template <typename T >
	class ConstAccessor : public boost::noncopyable {
	public:
		typedef boost::intrusive_ptr<T> TSafePtr;

	public:
		ConstAccessor(const TSafePtr ptr)
			: m_value(ptr)
			, m_sync_object_ptr(ptr->GetLockObj())
		{
			m_sync_object_ptr->LockShared();
		}

		~ConstAccessor()
		{
			if (m_sync_object_ptr)
				m_sync_object_ptr->UnlockShared();
		}

		const T* operator -> () const
		{
			return static_cast <const T* > (m_value->GetImplement());
		}

		TSafePtr GetObject()
		{
			return m_value;
		}

	private:
		SyncObject* m_sync_object_ptr;
		TSafePtr m_value;
	};


	// Accessor for exclusive lock
	template <typename T >
	class Accessor : public boost::noncopyable {
	public:
		typedef boost::intrusive_ptr<T> TSafePtr;

	public:
		Accessor(const TSafePtr ptr)
			: m_value(ptr)
			, m_sync_object_ptr(ptr->GetLockObj())
		{
			m_sync_object_ptr->LockExclusive();
		}

		~Accessor()
		{
			if (m_sync_object_ptr)
				m_sync_object_ptr->UnlockExclusive();
		}

		T* operator -> ()
		{
			return static_cast <T* > (m_value->GetImplement());
		}

		TSafePtr GetObject()
		{
			return m_value;
		}

	private:
		SyncObject* m_sync_object_ptr;
		TSafePtr m_value;
	};
}