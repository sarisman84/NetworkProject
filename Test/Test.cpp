#include "pch.h"
#include "CppUnitTest.h"
#include "NetworkEntity.h"

#include <array>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Networking
{
	TEST_CLASS(BufferTest)
	{
	public:

		TEST_METHOD(SendAndWrite)
		{

			//Init variables
			char message[512];
			int messageSize;

			//Cache data to init variables
			const char* msg = "This is a test";
			messageSize = strlen(msg) + 1;
			memcpy(message, msg, messageSize);

			Buffer buffer;
			buffer.WriteData(DataType::Message);
			buffer.WriteData(messageSize);
			buffer.WriteData(message, messageSize);
			//Simulating scenario where the data is sent by resetting the index of this buffer.

			buffer.Reset();
			//Recieved data;
			char incomingMessage[512];
			int incomingMessageSize;
			DataType incomingType;

			buffer.ReadData(incomingType);
			buffer.ReadData(incomingMessageSize);
			buffer.ReadData(incomingMessage, incomingMessageSize);

			Assert::IsTrue(incomingType == DataType::Message);
			Assert::AreEqual(message, incomingMessage);


		}



		TEST_METHOD(SendAndWriteMultiple)
		{
			constexpr unsigned int messageCount = 4;
			Buffer buffer;

			std::array<char[512], messageCount> cachedMessages;
			std::array<int, messageCount> cachedMessageSizes;

			for (size_t i = 0; i < messageCount; i++)
			{
				char message[512];
				int messageSize;

				//Cache data to init variables
				const char* msg = "This is a test";
				messageSize = strlen(msg) + 1;
				memcpy(message, msg, messageSize);

				buffer.WriteData(DataType::Message);
				buffer.WriteData(messageSize);
				buffer.WriteData(message, messageSize);

				//Cache result
				memcpy(cachedMessages[i], message, messageSize);
				cachedMessageSizes[i] = messageSize;

			}
			//Simulating scenario where the data is sent by resetting the index of this buffer.
			buffer.Reset();


			//Recieved data;
			for (size_t i = 0; i < messageCount; i++)
			{

				char incomingMessage[512];
				int incomingMessageSize;
				DataType incomingType;

				buffer.ReadData(incomingType);
				buffer.ReadData(incomingMessageSize);
				buffer.ReadData(incomingMessage, incomingMessageSize);

				Assert::IsTrue(incomingType == DataType::Message);
				Assert::AreEqual(cachedMessages[i], incomingMessage);
			}
		}
	};



}
