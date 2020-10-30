#include <stdio.h>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include "HttpRequest.h"
#include "Server.h"



std::string customRequestEvent(HttpRequest& request)
{
    return "Custom Body!";
}


int main(int args, char** argv)
{
    int port = 443;
    bool secure = true;

    if(args > 1)
    {
        if(strcmp(argv[1], "--help") == 0)
        {
            printf("Usage:\n%s -p PORT(Default=443)\n", argv[0]);
            return 0;
        }
    }

    if(args > 2)
    {
        if(strcmp(argv[1], "-p") == 0)
        {
            port = atoi(argv[2]);
        }
    }
    Webserver server;
    server.bindEvent("/event", "GET", customRequestEvent);
    server.bindFile("/source.cpp", "get", "./main.cpp");
    if(port == 80)
    {
        secure = false;
    }
    server.run(port, secure);


    return 0;
}

