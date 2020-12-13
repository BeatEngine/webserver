#include <stdio.h>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
/*
##define BOOST_ALL_NO_LIB
##define BOOST_THREAD_USE_LIB
##define BOOST_FILESYSTEM_USE_LIB
##define BOOST_ASIO_USE_LIB*/
#include "HttpRequest.h"
#include "Server.h"


std::string customRequestEvent(HttpRequest& request, unsigned char* body, FILE* bigBody, size_t bodySize)
{
    return "Custom Body! Your session: " + request.cookies.get("session") + "\n";
}

std::string receiveAudioData(HttpRequest& request, unsigned char* body, FILE* bigBody, size_t bodySize)
{
    return "OK";
}

std::string certbotVerification(HttpRequest& request, unsigned char* body, FILE* bigBody, size_t bodySize)
{
    return "OK";
}

std::string sum(HttpRequest& request, unsigned char* body, FILE* bigBody, size_t bodySize)
{
    
    float a = (float)atof(request.parameters.get("a").c_str());
    float b = (float)atof(request.parameters.get("b").c_str());
    return std::to_string(a+b);
}


int main(int args, char** argv)
{
    int port = 443;
    bool secure = true;

    bool consoleOutput = true;

    if(args > 1)
    {
        if(strcmp(argv[1], "--help") == 0)
        {
            printf("Usage:\n%s -p PORT(Default=443) (-s)=(disables console output)\n", argv[0]);
            return 0;
        }
        else if(strcmp(argv[1], "-s") == 0)
        {
            consoleOutput = false;
        }
    }

    if(args > 2)
    {
        if(strcmp(argv[1], "-p") == 0)
        {
            port = atoi(argv[2]);
        }
        else if(strcmp(argv[1], "-s") == 0)
        {
            consoleOutput = false;
        }
    }
    
    if(args > 3)
    {
        if(strcmp(argv[1], "-p") == 0)
        {
            port = atoi(argv[2]);
        }
        if(strcmp(argv[3], "-s") == 0)
        {
            consoleOutput = false;
        }

        if(strcmp(argv[2], "-p") == 0)
        {
            port = atoi(argv[3]);
        }
        if(strcmp(argv[1], "-s") == 0)
        {
            consoleOutput = false;
        }

    }

    

    Webserver server;
    server.bindEvent("/event", "GET", customRequestEvent);
    server.bindFile("/source.cpp", "get", "./main.cpp");
    server.bindEvent("/audio/input", "POST", receiveAudioData);
    server.bindEvent("/.well-known/acme-challenge/*", "GET", certbotVerification);

    StringMap valueMappingT1;
    valueMappingT1.put("variable","value");
    valueMappingT1.put("list","[\"Entry 1\", \"Entry 2\", \"Entry 3\", \"Entry 4\", \"Entry 5\"]");
    valueMappingT1.put("testelements_sz","6");
    valueMappingT1.put("testelements", "[\"lol\", \"display\", \"display\", \"nodisplay\", \"display\", \"display\"]");
    server.bindTemplate("/template", "GET", "./TEMPLATES/template.html", valueMappingT1);
    server.bindEvent("/add", "POST", sum);
    if(port == 80)
    {
        secure = false;
    }
    server.run(port, secure, consoleOutput,8);


    return 0;
}

