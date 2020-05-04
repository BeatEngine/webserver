//#define _WIN32_WINNT 0x0601

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <map>
#include <string>
#include <vector>

#include "httpRequest.h"

class Webserver
{

	boost::asio::io_context io_context;

	std::string receive(boost::asio::ip::tcp::socket& socket)
	{
		std::size_t s = 0;
		std::size_t total = 0;
		char tmp;
		std::string result;

		unsigned char requestEndReached = 0;

		bool uploadData = false;

		httpRequest request;

		do
		{
		s = boost::asio::read(socket, boost::asio::buffer(&tmp, 1));
			total += s;
			result.append(&tmp, 1);
			if (requestEndReached < 4)
			{
				if (tmp == '\r' || tmp == '\n')
				{
					requestEndReached++;
					if (requestEndReached == 4)
					{
						request.loadOfRaw(result);
						if (socket.available() > 0)
						{
							uploadData = true;
							printf("Upload:\n");
						}
						requestEndReached++;
					}
				}
				else
				{
					requestEndReached = 0;
				}
			}
		} while ( s > 0 && socket.available() > 0);

		if (total == 0)
		{
			return std::string();
		}
		return result;
		//return std::string((const char*)(buf.data().data()), buf.size());
	}

	void send(boost::asio::ip::tcp::socket& socket, std::string& data)
	{
		std::size_t total = 0;
		boost::asio::const_buffer sndbuf(data.c_str(),data.size());

		//do
		//{
			total += boost::asio::write(socket, sndbuf);
		//} while (total < data.size());
		
	}


public:

	Webserver()
	{
		
	}

	void run(int port, bool httpsEnabled, bool commandlineOutput)
	{
		if (httpsEnabled)
		{
			//after successfully http imp.
			//run(port, false, commandlineOutput);
		}
		else
		{
			boost::asio::ip::tcp::acceptor connection(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::address(), port));

			std::string testHTML = "<html><h1 align=\"center\">Test of C++ Webserver!</h1><form method=\"POST\" action=\"testupload\"><input name=\"posttestdata\" value=\"Test Data!\" hidden></input><button>Test upload</button></form></html>";
			std::size_t length = testHTML.length();

			if (commandlineOutput)
			{
				printf("Wait for connection...\n");
			}

			while (true)
			{
				boost::asio::ip::tcp::socket acceptSocket(io_context);
				connection.accept(acceptSocket);
				if (acceptSocket.is_open())
				{
					if (commandlineOutput)
					{
						printf("%s", "Client (");
						printf("%s", (char*)acceptSocket.remote_endpoint().address().to_string().c_str());
						printf("%s", (char*)") Connected!\n");
					}

					std::size_t s = 0;
					std::size_t last = 0;

					std::string recvData = receive(acceptSocket);
					
					if (commandlineOutput)
					{
						printf("Received bytes: %d\n", recvData.size());
					}
					if (recvData.size() > 0)
					{
						if (commandlineOutput)
						{
							printf("Received: %s\n", recvData.data());
						}

						s = 0;
						bool closed = false;

						send(acceptSocket, testHTML);
						
						if (!closed)
						{
							acceptSocket.close();
						}

					}
				}
			}
		}
	}





};








