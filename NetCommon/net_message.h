#pragma once
#include "net_common.h"

namespace olc
{
	namespace net
	{

		template <typename T>
		struct message_header
		{
			// Use emun template for message header to ensure the message are valid  at compile time
			T id{};
			unit32_t size = 0;
		};

		template <typename T>
		struct message
		{
			message_header<T> header{};
			std::vector<uint8_t> body;

			// Return the size of entire message packet in bytes
			size_t size() const
			{
				return sizeof(message_header<T>) + body.size();
			}

			// Override for std::cout compatibility
			friend std::ostream& operator << (std::ostream& os, const message<T>& msg)
			{
				os << "ID:" << int(msg.header.id) << " Size:" << msg.header.size;
				return os;
			}

			// Push data into the message buffer
			template<typename DataType>
			friend message<T>& operator << (message<T>& msg, const DataType& data)
			{
				// Check the type of the data is copyable
				static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be sent to message");

				// Cache current sizze of the vector
				size_t i = msg.body.size();

				// Resize the vector by the size of the data
				msg.body.resize(msg.body.size() + sizeof(DataType));

				// Recalculate the message size
				msg.header.size = msg.size();

				return msg;
			}

			template<typename DataType>
			friend message<T>& operator >> (message<T>& msg, DataType& data)
			{
				// Check pushed data's type is copyable
				static_assert(std::is_standard_layout<DaataType>::value, "Data is too complex to be parsed");

				// Cache the location
				size_t i = msg.body.size() - sizeof(DateType);

				// Copy the data from vector into the user variable
				std::memcpy(&data, msg.body.data() + i, sizeof(DataType));

				// Shrink the vector to remove read bytes and reset end position
				msg.body.resize(i);

				// Recalculate the message size
				msg.header.size() = msg.size();

				return msg;
			}
		};

		// Forward declare the connection
		template <typename T>
		class connection;

		template <typename T>
		struct owned_message
		{
			std::shared_ptr<connection<T>> remote = nullptr;
			message<T> msg;

			friend  std::ostream& operator<<std::ostream& os, const owned_message<T>& msg)
			{
				os << msg.msg;
				return os;
			}
		};
	}
}