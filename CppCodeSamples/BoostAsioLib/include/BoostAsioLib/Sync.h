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
	//! TBB�� accessor/con accessor�� �䳻�� �̸��Դϴٸ�, �ſ� Ư���� ����(Blackbox, PIMPL for ThreadSafe)�� ���� ����ؾ� �մϴ�.

	// �Ʒ��� �� ������ ����� �� ��ӹ޾ƾ� �� ��ü�̴�.
	template <typename T>
	class IAccessible
	{
	public:
		virtual SyncObject* GetLockObj() = 0;
		virtual T* GetImplement() = 0;
	};


	// Accessor for read-only, -> operator�� ����� �Լ����� ��� type function( ... ) const; ���·� ���ǵǾ�� �Ѵ�.
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