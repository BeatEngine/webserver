//#include <stdio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include <boost/asio.hpp>
#include <string>
#include <ctime>

#include <vector>

#include <map>

using std::string;

void initOpenSSL()
{
    SSL_load_error_strings();	
    OpenSSL_add_ssl_algorithms();
}

void cleanupOpenSSL()
{
    EVP_cleanup();
}

SSL_CTX *create_context()
{
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    method = SSLv23_server_method();

    ctx = SSL_CTX_new(method);
    if (!ctx) {
	perror("Unable to create SSL context");
	ERR_print_errors_fp(stderr);
	exit(EXIT_FAILURE);
    }

    return ctx;
}

void configure_context(SSL_CTX *ctx, const char* certFile_PEM, const char* keyFile_PEM)
{
    SSL_CTX_set_ecdh_auto(ctx, 1);

    /* Set the key and cert */
    if (SSL_CTX_use_certificate_file(ctx, certFile_PEM, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
	exit(EXIT_FAILURE);
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, keyFile_PEM, SSL_FILETYPE_PEM) <= 0 ) {
        ERR_print_errors_fp(stderr);
	exit(EXIT_FAILURE);
    }
}


string read_(boost::asio::ip::tcp::socket & socket) 
{
        boost::asio::streambuf buf;
        boost::asio::read_until( socket, buf, "\n" );
        string data = boost::asio::buffer_cast<const char*>(buf.data());
        return data;
}

string read_SSL(boost::asio::ip::tcp::socket & socket, SSL* ssl) 
{
        char buff[1024] = {0};
        int r = SSL_read(ssl, buff, 1024);
        return string(buff);
}

void send_(boost::asio::ip::tcp::socket & socket, const string& message)
{
       boost::asio::write( socket, boost::asio::buffer((const void*)(message.c_str()), message.size()));
}

void send_SSL(boost::asio::ip::tcp::socket & socket, const string& message, SSL* ssl)
{
        int c = 0;
        if(message.size() > 1024)
        {
            while (c < message.size())
            {
                if(message.size() - c > 1024)
                {
                    c += SSL_write(ssl, message.c_str(), 1024);
                }
                else
                {
                    c += SSL_write(ssl, message.c_str(), message.size() - c);
                }
            }
        }
        else
        {
            c += SSL_write(ssl, message.c_str(), message.size());
        }
}

void send_file(boost::asio::ip::tcp::socket & socket, const string& path)
{
    //printf("File: %s\n", path.c_str());
    FILE* f = fopen(path.c_str(), "rb");
    if(f == NULL)
    {
        send_(socket, string("HTTP/1.1 404 Not Found\r\n\r\n"));
        return;
    }
    long length = 0;
    fseek(f, 0, SEEK_END);
    length = ftell(f);
    fseek(f, 0, SEEK_SET);
    long snd = 0;
    unsigned char* buffer = (unsigned char*)calloc(sizeof(unsigned char), 1024);
    size_t szChar = sizeof(unsigned char);
    size_t read;
    
    char fsz[10];
    sprintf (fsz, "%d", length);
    send_(socket, string("HTTP/1.1 200 OK\r\ncontent-length: " + string(fsz) + "\r\n\r\n"));

    while (length > 0)
    {
        if(length < 1024)
        {
            read = fread(buffer, szChar, length, f);
        }
        else
        {
            read = fread(buffer, szChar, 1024, f);
        }
        boost::asio::write( socket, boost::asio::buffer((const void*)(buffer), read));
        length -= read;
    }
    free(buffer);
    fclose(f);
}

void send_file_SSL(boost::asio::ip::tcp::socket & socket, const string& path, SSL* ssl)
{
    //printf("File: %s\n", path.c_str());
    FILE* f = fopen(path.c_str(), "rb");
    if(f == NULL)
    {
        send_SSL(socket, string("HTTP/1.1 404 Not Found\r\n\r\n"), ssl);
        return;
    }
    long length = 0;
    fseek(f, 0, SEEK_END);
    length = ftell(f);
    fseek(f, 0, SEEK_SET);
    long snd = 0;
    unsigned char* buffer = (unsigned char*)calloc(sizeof(unsigned char), 1024);
    size_t szChar = sizeof(unsigned char);
    size_t read;
    
    char fsz[10];
    sprintf (fsz, "%d", length);
    send_SSL(socket, string("HTTP/1.1 200 OK\r\ncontent-length: " + string(fsz) + "\r\n\r\n"), ssl);

    while (length > 0)
    {
        if(length < 1024)
        {
            read = fread(buffer, szChar, length, f);
        }
        else
        {
            read = fread(buffer, szChar, 1024, f);
        }
        SSL_write(ssl, buffer, read);
        length -= read;
    }
    free(buffer);
    fclose(f);
}

class HttpRequest
{

    string toLowerCase(string input)
    {
        for(int i = 0; i < input.length(); i++)
        {
            if(input.at(i) < 91 && input.at(i) > 64)
            {
                input.at(i) += 32;
            }
        }
    }

    string toUpperCase(string input)
    {
        for(int i = 0; i < input.length(); i++)
        {
            if(input.at(i) < 123 && input.at(i) > 96)
            {
                input.at(i) -= 32;
            }
        }
    }

public:

    string path;
    string method;
    std::map<string, string> attributes;
    std::map<string, string> parameters;

    HttpRequest()
    {

    }

    

    HttpRequest(string plainRequest)
    {
        int a = 0;
        std::vector<string> lines;
        for(int i = 0; i < plainRequest.length(); i++)
        {
            if(plainRequest.at(i) == '\r' && i + 1 < plainRequest.length())
            {
                if(plainRequest.at(i+1) == '\n')
                {
                    lines.push_back(plainRequest.substr(a, i-a));
                    a = i+2;
                }
            }
        }

        for(int i = 0; i < lines.size(); i++)
        {
            if(i > 0)
            {
                string var;
                string val;
                a = 0;
                for(int o = 0; o < lines.at(i).size(); o++)
                {
                    if(lines.at(i).at(o) == ':')
                    {
                        var = lines.at(i).substr(0, o);
                        a = o + 1;
                    }
                }
                if(a > 1)
                {
                    val = lines.at(i).substr(a, lines.at(i).size());

                    if(val.at(0) == ' ')
                    {
                        val = val.substr(1, val.size());
                    }
                    attributes.insert(std::pair<string,string>(toLowerCase(var), val));
                }

            }
            else
            {
                a = 0;
                int atr = 0;
                int b = 0;
                for(int o = 0; o < lines.at(0).size(); o++)
                {
                    if(lines.at(0).at(o) == ' ')
                    {
                        if(atr == 0)
                        {
                        method = lines.at(0).substr(a, o-a);
                        method = toUpperCase(method);
                        a = o + 1;
                        }
                        else if(atr == 1)
                        {
                            b = a+1;
                            while(b < lines.at(0).length())
                            {
                                if(lines.at(0).at(b) == ' ')
                                {
                                    break;
                                }
                                b++;
                            }

                            path = lines.at(0).substr(a, b-a);
                            break;
                        }
                        atr++;
                    }
                }

                if(atr > 0)
                {
                    std::vector<string> params;
                    a = 0;
                    for(int c = 0; c < path.length(); c++)
                    {
                        if(path.at(c) == '?')
                        {
                            if(c + 1 < path.length())
                            {
                                string paras = path.substr(c+1, path.length());
                                path = path.substr(0, c);
                                a = 0;
                                for(int u = 0; u < paras.length(); u++)
                                {
                                    if(paras.at(u) == '&')
                                    {
                                        params.push_back(paras.substr(a, u-a));
                                        a = u + 1;
                                    }
                                }
                                if(a < paras.length())
                                {
                                    params.push_back(paras.substr(a, paras.length()));
                                }
                            }
                            break;
                        }
                    }

                    string var;
                    string val;
                    for(int c = 0; c < params.size(); c++)
                    {
                        for(int h = 0; h < params.at(c).length(); h++)
                        {
                            if(params.at(c).at(h) == '=')
                            {
                                var = params.at(c).substr(0, h);
                                val = params.at(c).substr(h+1, params.at(c).size());
                                parameters.insert(std::pair<string,string>(var, val));
                                break;
                            }
                        }
                    }
                }

            }
        }

    }


};


int main(int args, char** arg)
{
    int port = 80;
    boost::asio::io_service io_service;
//listen for new connection
    boost::asio::ip::tcp::acceptor acceptor(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port ));
//socket creation 


    SSL_CTX *ctx;
    initOpenSSL();
    ctx = create_context();
    configure_context(ctx, "./ca-certificates/xxxxxx.crt", "./ca-certificates/xxxxx.key");

    boost::asio::ip::tcp::socket socket(io_service);
//waiting for connection
    string dir = ".";
    if(args > 0)
    {
        string dir = string(arg[0]);
    }

    
    printf("Server running! Port: %d\n", port);
    
    while(true)
    {   
        acceptor.accept(socket);

        //SSL* ssl;
        if (false /*SSL_accept(ssl) <= 0*/) {
            ERR_print_errors_fp(stderr);
        }
        else {
        string message = read_(socket);
        string target = dir;
        HttpRequest request(message);


        printf("Method: %s\n", request.method.c_str());
        printf("Path: %s\n", request.path.c_str());
        printf("Attributes: %d\n", request.attributes.size());
        printf("Parameters: %d\n", request.parameters.size());
        
        if(request.path == "/")
        {
            request.path = "/index.html";
        }
        target.append(request.path);

        try
        {
            if(request.method == "GET")
            {
                send_file(socket, target);
            }
            else if(request.method == "POST")
            {

            }
            else
            {
                send_file(socket, dir + "/index.html");
            }
            
            socket.shutdown(socket.shutdown_both);
            socket.close();
        }
        catch( const boost::system::system_error& ex )
        {
            printf("Connection lost (%s)\n", target.c_str());
            ex.what();
            socket.close();
        }


        }
        //SSL_shutdown(ssl);
        //SSL_free(ssl);
        
    }
    SSL_CTX_free(ctx);
    cleanupOpenSSL();
    printf("Exit!\n");
    return 0;
}



