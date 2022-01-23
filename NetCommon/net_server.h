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
				: m_asioAcceptor(m_asioContext, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
			{

			}

			virtual ~server_interface()
			{
				Stop();
			}

			bool Start()
			{
				try
				{
					WaitForClientConnection();

					m_threadContext = std::thread([this]() { m_asioContext.run(); });
				}
				catch (std::exception& e)
				{
					// server error  from listening
					std::cerr << "[SERVER] Exception: " << e.what() << "\n";
					return false;
				}

				std::cout << "[SERVER] Started!\n";
				return true;
			}

			void Stop()
			{
				// close the context request
				m_asioContext.stop();

				// tidy up the context thread
				if (m_threadContext.joinable()) m_threadContext.join();

				// inform client
				std::cout << "[SERVER] Stopped!\n";
			}

			// async insctruct asio to wait for connection
			void WaitForClientConnection()
			{
				m_asioAcceptor.async_accept(
					[this](std::error_code ec, asio::ip::tcp::socket socket)
					{
						if (!ec)
						{
							std::cout << "[SERVER] New Connection: " << socket.remote_endpoint() << "\n";

							std::shared_ptr<connection<T>> newconn =
								std::make_shared<connection<T>>(connection<T>::owner::server,
									m_asioContext, std::move(socket), m_qMessagesIn);

							// client can deny the connection
							if (OnClientConnect(newconn))
							{
								// connection established, add to container of new connections
								m_deqConnections.push_back(std::move(newconn));

								m_deqConnections.back()->ConnectToClient(nIDCounter++);

								std::cout << "[" << m_deqConnections.back()->GetID() << "] Connection Approved\n";
							}
							else
							{
								std::cout << "[-----] Connection Denied\n";
							}
						}
						else
						{
							// error has occured during acceptance
							std::cout << "[SERVER] New Connection Error: " << ec.message() << "\n";
						}

						// reconnect again
						WaitForClientConnection();;
					});
			}

			// send a message to the client
			void MessageClient(std::shared_ptr<connection<T>> client, const message<T>& msg)
			{
				if (client && client->IsConnected())
				{
					client->Send(msg);
				}
				else
				{
					OnClientDisconnect(client);
					client.reset();

					// remove client
					m_deqConnections.erase(
						std::remove(m_deqConnections.begin(), m_deqConnections.end(), client, m_deqConnections.end());
				}
			}

			// send a message to all clients
			void MessageAllClients(const message<T>& msg, std::shared_ptr<connection<T>> pIgnoreClient = nullprt)
			{
				bool bInvalidClientExists = false;

				for (auto& client : m_deqConnections)
				{
					// check if client is connected
					if (client && client->IsConnected())
					{
						if (client != pIgnoreClient)
							client->Send(msg);
					}
					else
					{
						// client is disconnected
						OnClientDisconnect(client);
						client.reset();
						bInvalidClientExists = true;
					}
				}

				if (bInvalidClientExists)
					// remove clients
					m_deqConnections.erase(
						std::remove(m_deqConnections.begin(), m_deqConnections.end(), nullprt, m_deqConnections.end());
			}

			void Update(size_t nMaxMessages = -1)
			{
				sizr_t nMessageCount = 0;
				while (nMessageCount < nMessages && !m_qMessagesIn.empty())
				{
					// grab the front message
					auto msg = m_qMessagesIn.pop_front();

					// pass to message handler
					OnMessage(msg.remote, mas.msg);

					nMessageCount++;
				}
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

		protected:
			// thread safe queue for incoming message packets
			tsqueue<owned_message<T>> m_qMessagesIn;

			// container of active validated connections
			std::deque::<std::shared_ptr<connection<T>>> m_deqConnections;

			// declaration order / oder of initialisation
			asio::io_context m_asioContext;
			std::thread m_threadContext;

			// asio context
			asio::ip::tcp::acceptor m_asioAcceptor;

			// clients will be ifentified via an ID
			uint32_t nIDCounter = 10000;
		};
	}
}