#pragma once

#include <windows.h>

namespace BoostAsioLib
{
	class RefCount
	{
	public:
		RefCount()
			: _ref_count(0)
		{
		}

	protected:
		virtual inline void Destroy() = 0;

	protected:
		inline void AddRef()
		{
			InterlockedIncrement(&_ref_count);
		}

		inline void ReleaseRef()
		{
			if (!InterlockedDecrement(&_ref_count))
			{
				Destroy();
			}
		}

		template <typename T> friend void intrusive_ptr_add_ref(T*);
		template <typename T> friend void intrusive_ptr_release(T*);

	private:
		volatile long _ref_count;
	};

	template <typename T> inline void intrusive_ptr_add_ref(T* p)
	{
		p->AddRef();
	}

	template <typename T> inline void intrusive_ptr_release(T* p)
	{
		p->ReleaseRef();
	}
}