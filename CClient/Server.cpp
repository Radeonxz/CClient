//#include <chrono>
//#include <iostream>
//
//#ifdef _WIN32
//#define _WIN32_WINNT 0x0A00
//#endif
//#define ASIO_STANDALONE
//#include <asio.hpp>
//#include <asio/ts/buffer.hpp>
//#include <asio/ts/internet.hpp>
//
//std::vector<char> vBuffer(20 * 1024);
//
//void AsyncFetchData(asio::ip::tcp::socket& socket)
//{
//	socket.async_read_some(asio::buffer(vBuffer.data(), vBuffer.size()),
//		[&](std::error_code ec, std::size_t length)
//		{
//			if (!ec)
//			{
//				std::cout << "\n\nRead " << length << " bytes\n\n";
//
//				for (int i = 0; i < length; i++)
//					std::cout << vBuffer[i];
//
//				AsyncFetchData(socket);
//			}
//		}
//	);
//}
//
//int main()
//{
//	asio::error_code ec;
//
//	// Create the asio context
//	asio::io_context context;
//
//	//  Tasks for asio context when context doesn't finish
//	asio::io_context::work idleWork(context);
//
//	// Spawn context in threads
//	std::thread thrContext = std::thread([&]() { context.run(); });
//
//	// Get the address of the connection
//	std::string shortUri = "93.184.216.34";
//	std::string longUri = "51.38.81.49";
//	asio::ip::tcp::endpoint endpoint(asio::ip::make_address(longUri, ec), 80);
//
//	// Create a socket, associate the context
//	asio::ip::tcp::socket socket(context);
//
//	// Conect socket
//	socket.connect(endpoint, ec);
//
//	if (!ec)
//	{
//		std::cout << "Connected" << std::endl;
//	}
//	else
//	{
//		std::cout << "Failed to connect to address:\n" << ec.message() << std::endl;
//	}
//
//	if (socket.is_open())
//	{
//		AsyncFetchData(socket);
//
//		std::string sRequest =
//			"GET /index.html HTTP/1.1\r\n"
//			"Host: uri.com\r\n"
//			"Connection: close\r\n\r\n";
//
//		socket.write_some(asio::buffer(sRequest.data(), sRequest.size()), ec);
//
//		// Wait until recive response from the server then write
//		/*socket.wait(socket.wait_read);
//
//		size_t bytes = socket.available();
//		std::cout << "Bytes Available: " << bytes << std::endl;
//
//		if (bytes > 0)
//		{
//			std::vector<char> vBuffer(bytes);
//			socket.read_some(asio::buffer(vBuffer.data(), vBuffer.size()), ec);
//
//			for (auto c : vBuffer)
//				std::cout << c;
//		}*/
//
//		// Wait for 20000ms in the background to wait for data transfer
//		using namespace std::chrono_literals;
//		std::this_thread::sleep_for(20000ms);
//	}
//
//	system("pause");
//
//	return 0;
//}

#include <iostream>
#include <olc_net.h>

enum class CustomMsgTypes : uint32_t
{
	ServerAccept,
	ServerDeny,
	ServerPing,
	MessageAll,
	ServerMessage,
};



class CustomServer : public olc::net::server_interface<CustomMsgTypes>
{
public:
	CustomServer(uint16_t nPort) : olc::net::server_interface<CustomMsgTypes>(nPort)
	{

	}

protected:
	virtual bool OnClientConnect(std::shared_ptr<olc::net::connection<CustomMsgTypes>> client)
	{
		olc::net::message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::ServerAccept;
		client->Send(msg);
		return true;
	}

	// Called when a client appears to have disconnected
	virtual void OnClientDisconnect(std::shared_ptr<olc::net::connection<CustomMsgTypes>> client)
	{
		std::cout << "Removing client [" << client->GetID() << "]\n";
	}

	// Called when a message arrives
	virtual void OnMessage(std::shared_ptr<olc::net::connection<CustomMsgTypes>> client, olc::net::message<CustomMsgTypes>& msg)
	{
		switch (msg.header.id)
		{
		case CustomMsgTypes::ServerPing:
		{
			std::cout << "[" << client->GetID() << "]: Server Ping\n";

			// Simply bounce message back to client
			client->Send(msg);
		}
		break;

		case CustomMsgTypes::MessageAll:
		{
			std::cout << "[" << client->GetID() << "]: Message All\n";

			// Construct a new message and send it to all clients
			olc::net::message<CustomMsgTypes> msg;
			msg.header.id = CustomMsgTypes::ServerMessage;
			msg << client->GetID();
			MessageAllClients(msg, client);

		}
		break;
		}
	}
};

int main()
{
	CustomServer server(60000);
	server.Start();

	while (1)
	{
		server.Update(-1, true);
	}



	return 0;
}