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

		public:
			server_interface(unit16_t port)
			{

			}

			virtual ~server_interface()
			{

			}

			bool Start()
			{

			}

			void Stop()
			{

			}

			// async insctruct asio to wait for connection
			void WaitForClientConnection()
			{

			}

			// send a message to the client
			void MessageClient(std::shared_ptr<connection<T>> client, const message<T>& msg)
			{

			}

			// send a message to all clients
			void MessageAllClients(const message<T>& msg, std::shared_ptr<connection<T>> pIgnoreClient = nullprt)
			{

			}

		protected:
			// called when a client connects
			virtual bool OnClientConnect(std::shared_ptr<connection<T>> client)
			{
				return false;
			}

			// called when a client disconnects
			virtual void OnClientDisconnect(std::shared_ptr<connection<T>> client)
			{

			}

			// called when receive a message
			virtual void OnMessage(std::shared_ptr<connection<T>> client, message<T>& msg)
			{

			}
		};
	}
}