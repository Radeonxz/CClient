
#include <iostream>

#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif
#define ASIO_STANDALONE
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

int main()
{
	asio::error_code ec;

	// Create the asio context
	asio::io_context context;

	// Get the address of the connection
	asio::ip::tcp::endpoint endpoint(asio::ip::make_address("93.184.216.34", ec), 80);

	// Create a socket, associate the context
	asio::ip::tcp::socket socket(context);

	// Conect socket
	socket.connect(endpoint, ec);

	if (!ec)
	{
		std::cout << "Connected" << std::endl;
	}
	else
	{
		std::cout << "Failed to connect to address:\n" << ec.message() << std::endl;
	}

	system("pause");

	return 0;
}