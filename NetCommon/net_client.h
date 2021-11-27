#pragma once

#include "net_common.h"
#include "net_message.h"
#include "net_tsqueue.h"
#include "net_connection.h"

namespace olc
{
	namespace net
	{
		template <typename T>
		class client_interface
		{
			client_interface() : m_socket(m_context)
			{
				// init the socket with the io context
			}

			virtual ~client_interface()
			{
				// if client is destroyed then disconnect from server
				Disconnect();
			}

		public:
			// connect to server with hostname/ip-address and port
			bool Connect(const std::string& host, const unit16_t port)
			{
				try
				{
					// create connection
					m_connection = std::make_unique<connection<T>>();

					// resolve hostname/ip address into tangiable physical address
					asio::ip::tcp::resolver resolver(m_context);
					m_endpoints = resolver.resolve(host, std::to_string(port));

					// pass connection to connect to server
					m_connection->ConnectToServer(m_endpoints);

					// start context thread
					thrContext = std::thread([this]() { m_context.run(); });
				}
				catch (std::exception& e)
				{
					std::cerr << "Client Exception: " << e.what() << "\n";
					return false;
				}

				return false;
			}

			// disconnect from server
			void Disconnect()
			{
				// if connection exists and its connected
				if (IsConnected())
				{
					// disaconnect from server
					m_connection->Disconnect();
				}

				// close asio context
				m_context.stop();

				if (thrContext.joinable())
					thrContext.join();

				// destory connection object
				m_connection.release();
			}

			// check if client is connected to the server
			bool IsConnected()
			{
				if (m_connection)
					return m_connection->IsConnected();
				else
					return false;
			}

			// retrieve queue of messages from server
			tsqueue<owned_message<T>>& Incoming()
			{
				return m_qMessagesIn;
			}

		protected:
			// asio context handles data tansfer
			asio::io_context m_context;

			// spawn a thread to excute its work commands
			std::thread thrContext;

			// hardware socket that is connected to the server
			asio::ip::tcp::socket m_socket;

			// Client has a single instance of the connection obj for data transfer
			std::unique_ptr<connection<T>> m_connection;
		
		private:
			// thread safe queue of incoming messages from server
			tsqueue<owned_message<T>> m_qMessagesIn;
		};


	}
}