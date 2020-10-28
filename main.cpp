//#include <stdio.h>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#define ssl_socket boost::asio::ssl::stream<boost::asio::ip::tcp::socket>
#include "HttpRequest.h"

std::string generateResponsehead(long sizeBytes, HttpRequest& request, std::string customAttribute = "")
{
    std::string type = "text/html; charset=utf-8";
    if(request.attributes.contains("content-type"))
    {
        type = request.attributes.get("content-type");
    }
    if(customAttribute.length() > 0)
    {
        customAttribute += "\r\n";
    }
    std::string response = "HTTP/1.1 200 OK\r\n";
    response += "content-type: " + type + "\r\n" + "content-length: " + std::to_string(sizeBytes) + "\r\n" + "Server: BeatEngine-Webserver\r\n" + customAttribute + "\r\n";
    return response;
}

void handleHTTPSRequest(ssl_socket& server)
{
    unsigned char buffer[2048];
    int recv = 0;
    int snd = 0;
    long transferSize = 0;
    long transfered = 0;
    int w = 0;
    long av;
    HttpRequest request;
    while (true)
    {
        av = server.next_layer().available();
        if(av > 2048)
        {
            av = 2048;
        }
        if(av == 0)
        {
            int stp = 0;
            while (av == 0)
            {
                if(stp > 1000)
                {
                    if(transfered == 0)
                    {
                        break;
                    }
                    return;
                }
                usleep(100);
                av = server.next_layer().available();
                stp ++;
            }
            if(av > 2048)
            {
                av = 2048;
            }
        }
        try
        {
            recv = server.read_some(boost::asio::buffer(buffer, av));
        }
        catch(std::exception e)
        {
            return;
        }
        if(transfered == 0)
        {
            /*std::string buf((const char*)buffer);
            int h = buf.find("Host:") + 4;
            while(buffer[h] != '\r' && buffer[h] != '\n' && h > 4)
            {
                if(buffer[h] == ':')
                {
                    std::string np = destination.next_layer().remote_endpoint().address().to_string()+":"+std::to_string(targetport);
                    int p = h;
                    for(int i = 0; i < np.length(); i++)
                    {
                        buffer[h+1+i] = np[i];
                        p = h+1+i;
                    }
                    p++;
                    while (buffer[p] != '\r' && buffer[p] != '\n')
                    {
                        buffer[p] = ' ';
                        p++;
                    }
                    break;
                }
                h++;
            }*/
            request = HttpRequest(std::string((char*)buffer));
            if(recv > 0 && request.method == "GET")
            {
                printf("%s %s %s   %s\n",request.method.c_str(), request.path.c_str(), request.getQuery().c_str(), request.attributes.get("content-type").c_str());
            }
        }
        if(recv <= 0)
        {
            break;
        }
        transferSize += recv;
    }
    if(transferSize > 0)
    {
        transfered = 0;
        snd = 0;
        if(request.method == "GET")
        {
            std::string path = request.path;
            if(path == "/")
            {
                path = "index.html";
            }
            if(path[0] == '/')
            {
                path = path.substr(1);
            }
            FILE* f = fopen(path.c_str(), "rb");
            if(f == 0)
            {
                snd = 0;
                std::string notFound = "HTTP/1.1 404 Not Found\r\n\r\n<html><head><title>Not Found 404</title></head><body><h1>Not Found!</h1></body></html>";
                printf("Not found!\n");
                try
                {
                    while (snd < notFound.length())
                    {
                        snd = server.write_some(boost::asio::buffer(notFound.c_str()+snd, notFound.size()-snd));
                        if(snd == -1)
                        {
                            break;
                        }
                        transfered += snd;
                    }
                }
                catch(std::exception e)
                {
                    return;
                }
                return;
            }
            fseek(f, 0L, SEEK_END);
            long size = ftell(f);
            printf("Size:%d\n", size);
            fseek(f, 0L, SEEK_SET);
            std::string responseHead = generateResponsehead(size, request);
            snd = 0;
            try
            {
                while (snd < responseHead.length())
                {
                    snd += server.write_some(boost::asio::buffer(responseHead.c_str()+snd, responseHead.size()-snd));
                    if(snd == -1)
                    {
                        break;
                    }
                    transfered += snd;
                }
            }
            catch(std::exception e)
            {
                fclose(f);
                return;
            }
            long r = 0;
            long tr = 0;
            try
            {
                while (tr < size)
                {
                    r = 0;
                    if(size - r > 2048)
                    {
                        r = fread(buffer, 1, 2048, f);
                    }
                    else
                    {
                        r = fread(buffer, 1, size-r, f);
                    }
                    tr += r;
                    snd = 0;
                    while (snd < r)
                    {
                        snd += server.write_some(boost::asio::buffer(buffer+snd, r-snd));
                        if(snd == -1)
                        {
                            break;
                        }
                        transfered += snd;
                    }
                }
            }
            catch(std::exception e)
            {
                fclose(f);
                return;
            }
            fclose(f);
        }
        else
        {
            /* code */
        }
    }

}

void server(int port, bool https = true)
{
    if(https)
    {
        boost::asio::ip::tcp::endpoint serverV4(boost::asio::ip::tcp::v4(), port);
        boost::asio::ip::tcp::endpoint serverV6(boost::asio::ip::tcp::v6(), port);
        boost::asio::io_service io_service;
        boost::asio::ip::tcp::acceptor acceptorV4(io_service, serverV4);
        //boost::asio::ip::tcp::acceptor acceptorV6(io_service, serverV6);
        boost::asio::ssl::context ssl_context(boost::asio::ssl::context::tlsv13_server);
        ssl_context.use_certificate_file("certs/newcert.pem", boost::asio::ssl::context_base::pem);
        ssl_context.use_private_key_file("certs/privkey.pem", boost::asio::ssl::context_base::pem);
        ssl_context.use_tmp_dh_file("certs/dh2048.pem");

        while(true)
        {
            try
            {
                ssl_socket socket(io_service, ssl_context);
                acceptorV4.accept(socket.next_layer());
                socket.handshake(boost::asio::ssl::stream_base::handshake_type::server);
                handleHTTPSRequest(socket);
                socket.next_layer().close();
            }
            catch(const std::exception& e)
            {
                printf("Connection Error!\n");
            }
        }

    }
}

int main(int args, char** arg)
{
    int port = 8080;

    server(port);


    return 0;
}

