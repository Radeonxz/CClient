#pragma once

#include "net_common.h"
#include "net_tsqueue.h"
#include "net_message.h"
#include "net_connection.h"

namespace olc
{
	namespace net
	{
		template<typename T>
		class server_interface
		{

		protected:
			// thread safe queue for incoming message packets
			tsqueue<owned_message<T>> m_qMessagesIn;

			// container of active validated connections
			std::deque<std::shared_ptr<connection<T>>> m_deqConnections;

			// order of declaration is important - it is also the order of initialisation
			asio::io_context m_asioContext;
			std::thread m_threadContext;

			// these things need an asio context
			asio::ip::tcp::acceptor m_asioAcceptor; // Handles new incoming connection attempts...

			// clients will be identified in the "wider system" via an ID
			uint32_t nIDCounter = 10000;
		};
	}
}