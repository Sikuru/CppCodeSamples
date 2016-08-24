#pragma once

namespace BoostAsioLib
{
	class IBATCPEvent
	{
	public:
		virtual void OnConnected() = 0;
		virtual void OnDisconnected() = 0;
		virtual void OnAccepted() = 0;
		virtual void OnReceived() = 0;
		virtual void OnSent() = 0;
	};
}