#ifndef ssl_socket
    #define ssl_socket boost::asio::ssl::stream<boost::asio::ip::tcp::socket>
#endif

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
    std::string suffix = request.getSuffix();
    if(!request.attributes.contains("content-type"))
    {
        if(suffix == ".txt")
        {
            type = "text/plain";
        }
        else if(suffix == ".csv")
        {
            type = "text/"+suffix.substr(1);
        }
        else if(suffix == ".css")
        {
            type = "text/"+suffix.substr(1);
        }
        else if(suffix == ".js")
        {
            type = "text/javascript";
        }
        else if(suffix == ".jpg")
        {
            type = "image/"+suffix.substr(1);
        }
        else if(suffix == ".jpge")
        {
            type = "image/"+suffix.substr(1);
        }
        else if(suffix == ".png")
        {
            type = "image/"+suffix.substr(1);
        }
        else if(suffix == ".ico")
        {
            type = "image/"+suffix.substr(1);
        }
        else if(suffix == ".gif")
        {
            type = "image/"+suffix.substr(1);
        }
        else if(suffix == ".svg")
        {
            type = "image/"+suffix.substr(1);
        }
        else if(suffix == ".tiff")
        {
            type = "image/"+suffix.substr(1);
        }
        else if(suffix == ".webp")
        {
            type = "image/"+suffix.substr(1);
        }
        else if(suffix == ".wav")
        {
            type = "audio/"+suffix.substr(1);
        }
        else if(suffix == ".mp3")
        {
            type = "audio/"+suffix.substr(1);
        }
        else if(suffix == ".mp4")
        {
            type = "video/"+suffix.substr(1);
        }
        else if(suffix == ".ogg")
        {
            type = "video/"+suffix.substr(1);
        }
        else if(suffix == ".webm")
        {
            type = "video/"+suffix.substr(1);
        }
        else if(suffix != ".html" && suffix != ".htm" && suffix != ".php"  && suffix.length() > 1)
        {
            type = "application/"+suffix.substr(1);
        }
    }
    std::string response = "HTTP/1.1 200 OK\r\n";
    response += "content-type: " + type + "\r\n" + "content-length: " + std::to_string(sizeBytes) + "\r\n" + "Server: BeatEngine-Webserver\r\n" + customAttribute + "\r\n";
    return response;
}

std::size_t socketAvailable(ssl_socket& socket)
{
    return socket.next_layer().available();
}

std::size_t socketAvailable(boost::asio::ip::tcp::socket& socket)
{
    return socket.available();
}

class RequestHandler
{
    std::vector<std::string> paths;
    std::vector<void*> events;
    std::vector<std::string> methods;
    std::vector<std::string> filePath;
    public:

    RequestHandler(){};

    void setPathEvent(std::string& path, std::string& method, std::string(*event)(HttpRequest& request, unsigned char* requestBodyBuffer, FILE* requestBodyFile, size_t bufferSize))
    {
        paths.push_back(path);
        methods.push_back(method);
        events.push_back((void*)event);
        filePath.push_back("");
    }

    void setPathFileMapping(std::string& path, std::string& method, std::string& pathToFile)
    {
        paths.push_back(path);
        methods.push_back(method);
        events.push_back(0);
        filePath.push_back(pathToFile);
    }

    bool containsPath(std::string& path, std::string& method, std::string* returnMappedFilePath)
    {
        for(int i = 0; i < paths.size(); i++)
        {
            if(paths[i] == path && methods[i] == method)
            {
                if(returnMappedFilePath)
                {
                    *returnMappedFilePath = filePath[i];
                }
                return true;
            }
            else if(paths[i].substr(paths[i].length()-1) == "*" && paths[i].length()-1 <= path.length())
            {
                if(paths[i].substr(0, paths[i].length()-1) == path.substr(0, paths[i].length()-1) && methods[i] == method)
                {
                    if(returnMappedFilePath)
                    {
                        *returnMappedFilePath = filePath[i];
                    }
                    return true;
                }
            }
        }
        return false;
    }

    std::string getEventResult(HttpRequest& request, unsigned char* buffer, FILE* fbuffer, size_t bufferSize)
    {
        for(int i = 0; i < paths.size(); i++)
        {
            if(paths[i] == request.path && methods[i] == request.method && events[i] != 0)
            {
                return ((std::string(*)(HttpRequest& request, unsigned char* requestBodyBuffer, FILE* requestBodyFile, size_t bufferSize))(events[i]))(request, buffer, fbuffer, bufferSize);
            }
            else if(paths[i].substr(paths[i].length()-1) == "*" && paths[i].length()-1 <= request.path.length())
            {
                if(paths[i].substr(0, paths[i].length()-1) == request.path.substr(0, paths[i].length()-1) && methods[i] == request.method)
                {
                    return ((std::string(*)(HttpRequest& request, unsigned char* requestBodyBuffer, FILE* requestBodyFile, size_t bufferSize))(events[i]))(request, buffer, fbuffer, bufferSize);
                }
            }
        }
        return "";
    }

    size_t size()
    {
        return paths.size();
    }

};

std::string randomFilename(int length = 10)
{
    srand(clock());
    std::string fn = "";
    for(int i = 0; i < length; i++)
    {
        if(rand()%2 == 0)
        {
            fn += (char)(97+rand() % 26);
        }
        else if(rand()%3 == 0)
        {
            fn += (char)(48+rand() % 9);
        }
        else
        {
            fn += (char)(65+rand() % 26);
        }
    }
    return fn;
}

template<class SocketType>
void handleHTTPSRequest(SocketType& server, RequestHandler* requestHandle = 0, bool consoleOutput = true)
{
    unsigned char buffer[2048];
    int recv = 0;
    int snd = 0;
    long transferSize = 0;
    long transfered = 0;
    int w = 0;
    long av;
    HttpRequest request;
    bool secure = typeid(SocketType) == typeid(ssl_socket);
    std::string randomFN = randomFilename();
    FILE* tmpStorage;
    bool useFileBuffer = false;
    while (true)
    {
        av = socketAvailable(server);
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
                av = socketAvailable(server);
                stp ++;
            }
            if(av > 2048)
            {
                av = 2048;
            }
        }
        try
        {
            if(recv > 0)
            {
                if(useFileBuffer == false)
                {
                   tmpStorage = fopen(randomFN.c_str(), "wb");
                   useFileBuffer = true;
                }
                fwrite(buffer, 1, recv, tmpStorage);
            }
            recv = server.read_some(boost::asio::buffer(buffer, av));
        }
        catch(std::exception e)
        {
            return;
        }
        if(transferSize == 0)
        {
            request = HttpRequest(std::string((char*)buffer));
            if(recv > 0 && request.method == "GET" && consoleOutput)
            {
                printf("%s %s %s   %s\n",request.method.c_str(), request.path.c_str(), request.getQuery().c_str(), request.attributes.get("content-type").c_str());
            }
            else if(recv > 0 && request.method == "POST" && consoleOutput)
            {
                printf("%s %s      %s\n",request.method.c_str(), request.path.c_str(), request.attributes.get("content-type").c_str());
            }
        }
        if(recv > 0)
        {
            transferSize += recv;
        }
        if(recv <= 0)
        {
            break;
        }
    }
    
    if(transferSize > 0)
    {
        if(transferSize > 2048)
        {
            if(recv > 0)
            {
                fwrite(buffer, 1, recv, tmpStorage);
            }
        }
        transfered = 0;
        snd = 0;
        std::string generatedBody = "";
        std::string mappedFile = "";
        if(requestHandle)
        {
            if(requestHandle->containsPath(request.path, request.method, &mappedFile))
            {
                if(mappedFile.empty())
                {
                    if(transferSize > 2048)
                    {
                        generatedBody = requestHandle->getEventResult(request, 0, tmpStorage, transferSize);
                    }
                    else
                    {
                        generatedBody = requestHandle->getEventResult(request, buffer, 0, transferSize);
                    }
                }
            }
            if(generatedBody.length() > 0)
            {
                if(consoleOutput)
                {
                    printf("Custom-Response size:%ld\n", generatedBody.size());
                }
                std::string responseHead = generateResponsehead(generatedBody.size(), request);
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
                    if(useFileBuffer)
                    {
                        fclose(tmpStorage);
                        remove(randomFN.c_str());
                    }
                    return;
                }
                snd = 0;
                try
                {
                    unsigned char* tmpData = (unsigned char*)generatedBody.c_str();
                    while (snd < generatedBody.size())
                    {
                        snd += server.write_some(boost::asio::buffer(tmpData+snd, generatedBody.size()-snd));
                        if(snd == -1)
                        {
                            break;
                        }
                        transfered += snd;
                    }
                }
                catch(std::exception e)
                {

                }
                if(useFileBuffer)
                {
                    fclose(tmpStorage);
                    remove(randomFN.c_str());
                }
                return;
            }
        }
        if(request.method == "GET")
        {
            std::string path = request.path;
            if(path == "/")
            {
                path = "WEB/index.html";
            }
            if(path[0] == '/')
            {
                //path = path.substr(1);
                path = "WEB" + path;
            }
            if(!mappedFile.empty())
            {
                path = mappedFile;
            }
            if(access(path.c_str(), F_OK ) == -1)
            {
                snd = 0;
                std::string notFound = "HTTP/1.1 404 Not Found\r\n\r\n<html><head><title>Not Found 404</title></head><body><h1>Not Found!</h1></body></html>";
                if(consoleOutput)
                {
                    printf("Not found!\n");
                }
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
                    if(useFileBuffer)
                    {
                        fclose(tmpStorage);
                        remove(randomFN.c_str());
                    }
                    return;
                }
                if(useFileBuffer)
                {
                    fclose(tmpStorage);
                    remove(randomFN.c_str());
                }
                return;
            }
            FILE* f = fopen(path.c_str(), "rb");
            if(f == 0)
            {
                snd = 0;
                std::string notFound = "HTTP/1.1 404 Not Found\r\n\r\n<html><head><title>Not Found 404</title></head><body><h1>Not Found!</h1></body></html>";
                if(consoleOutput)
                {
                    printf("Not found!\n");
                }
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
                    if(useFileBuffer)
                    {
                        fclose(tmpStorage);
                        remove(randomFN.c_str());
                    }
                    return;
                }
                if(useFileBuffer)
                {
                    fclose(tmpStorage);
                    remove(randomFN.c_str());
                }
                return;
            }
            fseek(f, 0L, SEEK_END);
            long size = ftell(f);
            if(consoleOutput)
            {
                printf("Size:%ld\n", size);
            }
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
                if(useFileBuffer)
                {
                    fclose(tmpStorage);
                    remove(randomFN.c_str());
                }
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
                if(useFileBuffer)
                {
                    fclose(tmpStorage);
                    remove(randomFN.c_str());
                }
                return;
            }
            fclose(f);
        }
        else
        {
            snd = 0;
            std::string notFound = "HTTP/1.1 404 Not Found\r\n\r\n<html><head><title>Not Found 404</title></head><body><h1>Not Found!</h1></body></html>";
            if(consoleOutput)
            {
                printf("Not found!\n");
            }
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
                if(useFileBuffer)
                {
                    fclose(tmpStorage);
                    remove(randomFN.c_str());
                }
                return;
            }
            if(useFileBuffer)
            {
                fclose(tmpStorage);
                remove(randomFN.c_str());
            }
            return;
        }
    }
    if(useFileBuffer)
    {
        fclose(tmpStorage);
        remove(randomFN.c_str());
    }
}

class Webserver
{

    std::string toUppercase(std::string input)
    {
        for(int i = 0; i < input.length(); i++)
        {
            if(input[i] > 96 && input[i] < 123)
            {
                input[i] -= 32;
            }
        }
        return input;
    }

    inline bool fileExist(std::string& path)
    {
        return access( path.c_str(), F_OK ) != -1;
    }

    public:
    Webserver()
    {

    }

    RequestHandler customRequests;

    void bindEvent(std::string requestPath, std::string requestMethod, std::string(*event)(HttpRequest& request, unsigned char* requestBodyBuffer, FILE* requestBodyFile, size_t bufferSize))
    {
        std::string meth = toUppercase(requestMethod);
        customRequests.setPathEvent(requestPath, meth, event);
    }

    void bindFile(std::string requestPath, std::string requestMethod, std::string filePath)
    {
        std::string meth = toUppercase(requestMethod);
        if(fileExist(filePath))
        {
            customRequests.setPathFileMapping(requestPath, meth, filePath);
        }
    }

    void run(int port, bool https = true, bool consoleOutput = true)
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
                    if(customRequests.size() > 0)
                    {
                        handleHTTPSRequest(socket, &customRequests, consoleOutput);
                    }
                    else
                    {
                        handleHTTPSRequest(socket, 0, consoleOutput);
                    }
                    socket.next_layer().close();
                }
                catch(const std::exception& e)
                {
                    if(consoleOutput)
                    {
                        printf("Connection Error!\n");
                    }
                }
            }

        }
        else
        {
            boost::asio::ip::tcp::endpoint server(boost::asio::ip::tcp::v4(), port);
            boost::asio::io_service io_service;
            boost::asio::ip::tcp::acceptor acceptor(io_service, server);
            while(true)
            {
                try
                {
                    boost::asio::ip::tcp::socket socket(io_service);
                    acceptor.accept(socket);
                    if(customRequests.size() > 0)
                    {
                        handleHTTPSRequest(socket, &customRequests, consoleOutput);
                    }
                    else
                    {
                        handleHTTPSRequest(socket, 0, consoleOutput);
                    }
                    socket.close();
                }
                catch(const std::exception& e)
                {
                    if(consoleOutput)
                    {
                        printf("Connection Error!\n");
                    }
                }
            }
        }
    }
};

