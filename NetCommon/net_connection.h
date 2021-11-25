#pragma once

#include "net_common.h"
#include "net_tsqueue.h"
#include "net_message.h"

namespace olc
{
	namespace net
	{
		template<typename T>
		class connection : public std::enable_shared_from_this<connection<T>>
		{
		public:
			connection()
			{

			}

			virtual ~connection()
			{

			}

		public:
			bool ConnectToServer();
			bool Disconnect();
			bool IsConnected() const;

		public:
			bool Send(const message<T>& msg);

		protected:
			// Each connection has a unique socket to a remote
			asio::ip::tcp::socket m_socket;

			// Shared context for asio instance
			asio::io_context& m_asioContext;

			// Queue holds all messages to be sent to the remote side of the current connection
			tsqueue<message<T>> m_qMessagesOut;

			// Queue holds all messages have been received from the remote side of the current connection
			tsqueue<owned_message<T>>& m_qMessagesIn;
		};
	}
}