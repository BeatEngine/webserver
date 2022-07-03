#include <stdio.h>

//#include <boost/asio.hpp>
//#include <boost/asio/ssl.hpp>

#define TLS_AMALGAMATION
#include "tlse/tlse.c"

#include "HttpRequest.h"
#include "Server.h"


std::string customRequestEvent(HttpRequest& request, unsigned char* body, FILE* bigBody, size_t bodySize)
{
    return "Custom Body! Your session: " + request.cookies.get("session") + "\n";
}


std::string sum(HttpRequest& request, unsigned char* body, FILE* bigBody, size_t bodySize)
{
    
    float a = (float)atof(request.parameters.get("a").c_str());
    float b = (float)atof(request.parameters.get("b").c_str());
    return std::to_string(a+b);
}

 
std::string downloadUpload(HttpRequest& request, unsigned char* body, FILE* bigBody, size_t bodySize)
{
    std::string encoded = request.path.substr(strlen("/download/"));
    /* Mapping and decode*/
    std::string filepath = "uploads\\" + request.urlDecode(encoded);

    return filepath;
}

std::string showUploads(HttpRequest& request, unsigned char* body, FILE* bigBody, size_t bodySize)
{

    //std::filesystem::
    //filesystem::directory_iterator dir("uploads");
    //filesystem::directory_iterator end;

    std::string jsonUploads = "[";
    /*bool notFirst = false;
    for (dir; dir != end; dir++)
    {
        boost::filesystem::path entry = dir->path();
        if (boost::filesystem::is_regular_file(entry))
        {
            if (notFirst)
            {
                jsonUploads += ", ";
            }
            else
            {
                notFirst = true;
            }
            jsonUploads += "\"" + entry.filename().string() + "\"";
        }
    }
    jsonUploads += "]";*/
    

    return jsonUploads;
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

    server.bindEvent("/add", "POST", sum);

    StringMap valueMappingT1;
    valueMappingT1.put("variable","value");
    valueMappingT1.put("list","[\"Entry 1\", \"Entry 2\", \"Entry 3\", \"Entry 4\", \"Entry 5\"]");
    valueMappingT1.put("testelements_sz","6");
    valueMappingT1.put("testelements", "[\"lol\", \"display\", \"display\", \"nodisplay\", \"display\", \"display\"]");
    server.bindTemplate("/template", "GET", "./TEMPLATES/template.html", valueMappingT1);

    server.bindEvent("/uploads", "POST", showUploads);
    server.bindFileDownload("/download/*", "GET", downloadUpload);

    server.bindMultiPartFileUpload("/upload", "POST", "uploads");
    if(port == 80)
    {
        secure = false;
    }
    
    server.run(port, secure, "certs/newcert.pem", "certs/privkey.pem", consoleOutput, 8);
    
    
    return 0;
}

