#ifndef ssl_socket
    #define ssl_socket boost::asio::ssl::stream<boost::asio::ip::tcp::socket>
#endif



#ifdef __linux__ 
#define USLEEPDEFINED 1

#else
#define F_OK 1
#define WINVCUSED 1
int access(const char* path, int stat = 1)
{
	if (INVALID_FILE_ATTRIBUTES == GetFileAttributes(path))
	{
		return -1;
	}
	return 0;
}
#include <thread>
#define pthread_t std::thread
void pthread_create(pthread_t* thread, int opt, void*(*function)(void* v), void* args)
{
	*thread = std::thread((void(*)(void* v))(*function), args);
}

void pthread_join(pthread_t& thread, void* resultReturn)
{
	thread.join();
}
#endif

#ifndef USLEEPDEFINED
#include <time.h>
void usleep(long mics)
{
	clock_t now = clock();
	double tm = (double)mics;
	double f = CLOCKS_PER_SEC / 1000000.0;
	while ((double)(clock() - now) / f < tm)
	{

	}
}
#endif // !usleep

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/filesystem.hpp>
#include "templateEngine.h"

#include <iostream>

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
    std::string cookies = "";
    for(int i = 0; i < request.cookies.size(); i++)
    {
        cookies += "Set-Cookie: " + request.cookies.keyAt(i) + "=" + request.cookies[i] + "\r\n";
    }
    std::string response = "HTTP/1.1 200 OK\r\n";
    response += "content-type: " + type + "\r\n" + "content-length: " + std::to_string(sizeBytes) + "\r\n" + "Server: BeatEngine-Webserver\r\n" + customAttribute + cookies  + "\r\n";
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
    std::vector<StringMap> templateVariables;
    std::vector<bool> istemplate;
    std::vector<bool> isupload;
    std::vector<bool> isdownload;

    std::string processMultiPartUpload(HttpRequest& request, unsigned char* buffer, FILE* fbuffer, size_t bufferSize, std::string uploadDirectory, int index = 0)
    {
        std::string type = request.attributes.get("content-type");
        if (uploadDirectory[uploadDirectory.length() - 1] != '/' && uploadDirectory[uploadDirectory.length() - 1] != '\\')
        {
#ifdef __linux__ 
            uploadDirectory += "/";
#else
            uploadDirectory += "\\";
#endif
        }
        if (!type.empty())
        {
            if (type.find("multipart/form-data; boundary=") >= 0)
            {
                std::vector<std::string> parts = StringUtils::split(type, "=");
                if (parts.size() == 2)
                {
                    std::string boundary = parts[1];
                    if (buffer)
                    {
                        int f = 0;
                        int start = 0;
                        std::string info = "";

                        for (int i = 4; i < bufferSize; i++)
                        {
                            if (buffer[i - 3] == '\r' && buffer[i - 2] == '\n' && buffer[i - 1] == '\r' && buffer[i] == '\n')
                            {
                                start = i;
                                break;
                            }
                        }

                        for (int i = start; i < bufferSize; i++)
                        {
                            for (f = 0; f < boundary.size() && i+f < bufferSize; f++)
                            {
                                if (buffer[i + f] != boundary[f])
                                {
                                    break;
                                }
                            }
                            if (f == boundary.size())
                            {
                                if (info.empty())
                                {
                                    int d = i + f;
                                    while (d < bufferSize)
                                    {
                                        if (buffer[d] == '\r' && buffer[d + 1] == '\n' && buffer[d + 2] == '\r' && buffer[d+3] == '\n')
                                        {
                                            int pos = info.find("filename");
                                            if (pos >= 0)
                                            {
                                                info = info.substr(pos);
                                                for (int c = 0; c < info.length(); c++)
                                                {
                                                    if (info[c] == '\r')
                                                    {
                                                        info = info.substr(0, c);
                                                        break;
                                                    }
                                                }
                                            }
                                            d += 4;
                                            break;
                                        }
                                        info += buffer[d];
                                        d++;
                                    }
                                    start = d;
                                }
                                else
                                {
                                    std::string filename = "";
                                    std::vector<std::string> prts = StringUtils::split(info, "=", false);
                                    if (prts.size() == 2)
                                    {
                                        filename = prts[1];
                                        if (filename[0] == '"' && filename[filename.length()-1] == '"')
                                        {
                                            filename = filename.substr(1, filename.length() - 2);
                                        }
                                        FILE* file = fopen((uploadDirectory + filename).c_str(), "wb");
                                        fwrite(buffer + start, sizeof(char), i - start -4, file);
                                        fclose(file);
                                        /*for (int index = start; index < i + f; index++)
                                        {

                                        }*/

                                    }
                                    info = "";
                                    start = i + f;
                                }
                                
                            }
                        }
                    }
                    else /* File Buffer */
                    {
                        long pageSize = 1024;
                        unsigned char buff[1025] = { 0 };
                        long page = 0;



                        fseek(fbuffer, 0, SEEK_END);
                        bufferSize = ftell(fbuffer);
                        fseek(fbuffer, 0, SEEK_SET);

                        int sread = fread(buff, sizeof(char), pageSize, fbuffer);
                        while (sread < pageSize)
                        {
                            sread += fread(buff + sread, sizeof(char), pageSize - sread, fbuffer);
                        }

                        std::string info = "";

                        std::string infoEnd = "\r\n\r\n";

                        long start = 0;
                        long i = std::string((char*)buff).find("\r\n\r\n")+2;

                        long fbndry = std::string((char*)buff).find(boundary, i);

                        if (fbndry != -1)
                        {
                            fbndry--;
                            while (buff[fbndry] != '\n')
                            {
                                boundary.insert(boundary.begin(), buff[fbndry]);
                                fbndry--;
                            }
                            boundary = "\r\n" + boundary;
                        }

                        long f = 0;
                        long headEnd = i;

                        unsigned char search[500] = { 0 };
                        long sp = 0;

                        unsigned char byte;

                        FILE* currentUploadFile = 0;

                        fseek(fbuffer, i, SEEK_SET);
                        for (i = i; i < bufferSize; i++)
                        {
                            fread(&byte, 1, 1, fbuffer);
                            if (byte == boundary[sp])
                            {
                                search[sp] = byte;
                                sp++;
                            }
                            else
                            {
                                if (sp == 0)
                                {
                                    if (currentUploadFile)
                                    {
                                        fwrite(&byte, 1, 1, currentUploadFile);
                                    }
                                }
                                else
                                {
                                    if (currentUploadFile)
                                    {
                                        fwrite(search, 1, 1, currentUploadFile);
                                    }
                                    /*if (currentUploadFile)
                                    {
                                        fwrite(search, 1, sp, currentUploadFile);
                                    }*/
                                    fseek(fbuffer, -sp + 0, SEEK_CUR);
                                    i -= sp;
                                    sp = 0;
                                    continue;
                                }

                                sp = 0;

                            }
                            if (sp == boundary.size())
                            {
                                sp = 0;
                                info = "";
                                
                                for (i = i; i < bufferSize; i++)
                                {
                                    fread(&byte, 1, 1, fbuffer);
                                    if (byte == infoEnd[sp])
                                    {
                                        search[sp] = byte;
                                        sp++;
                                    }
                                    else
                                    {
                                        if (sp == 0)
                                        {
                                            info += byte;
                                        }
                                        else
                                        {
                                            for (int a = 0; a < sp; a++)
                                            {
                                                info += search[a];
                                            }
                                        }
                                        if (sp != 0)
                                        {
                                            fseek(fbuffer, -sp + 1, SEEK_CUR);
                                            i -= sp;
                                        }
                                        
                                        sp = 0;
                                    }
                                    if (sp == infoEnd.size())
                                    {

                                        break;
                                    }
                                }
                                sp = 0;
                                int fnp = info.find("filename=");
                                if (currentUploadFile)
                                {
                                    fclose(currentUploadFile);
                                    currentUploadFile = 0;
                                }
                                if (fnp == -1)
                                {
                                    /*time_t ti = time(NULL);
                                    tm tim = *localtime(&ti);

                                    std::string noFileName = "UNKNOWN_";
                                    noFileName += std::to_string(tim.tm_mday) + "." + std::to_string(tim.tm_mon + 1) + "." + std::to_string(tim.tm_year + 1900) + " " + std::to_string(tim.tm_hour) + "-" + std::to_string(tim.tm_min) + "-" + std::to_string(tim.tm_sec) + ".file";

                                    currentUploadFile = fopen((uploadDirectory + noFileName).c_str(), "wb");*/
                                }
                                else
                                {
                                    fnp += 9;
                                    std::string filename = "";
                                    for (int b = fnp; b < info.size(); b++)
                                    {
                                        if (info[b] == '\r' || info[b] == '\n' || b == info.size()-1)
                                        {
                                            filename = info.substr(fnp, b-fnp);
                                            if (filename[0] == '"')
                                            {
                                                filename = filename.substr(1);
                                            }
                                            if (filename.size() > 0)
                                            {
                                                if (filename[filename.size() - 1] == '"')
                                                {
                                                    filename = filename.substr(0, filename.size() - 1);
                                                }
                                            }
                                            break;
                                        }
                                    }
                                    printf("%s\n", filename.c_str());
                                    currentUploadFile = fopen((uploadDirectory + filename).c_str(), "wb");
                                }
                                sp = 0;
                            }
                        }
                        if (currentUploadFile)
                        {
                            fclose(currentUploadFile);
                        }

                    }
                }
            }
        }
        request.attributes.set("content-type", "text/plain");
        return "400 Bad Request wrong content-type.";
    }

    /* Syntax like thymeleaf */
    std::string processTemplate(HttpRequest& request, unsigned char* buffer, FILE* fbuffer, size_t bufferSize, std::string templateFilePath, int index = 0)
    {
        std::string html;
        char tmp[2048];
        html = "";
        long sz = 0;
        long s;
        FILE* tmpf = fopen(templateFilePath.c_str(), "rb");
        fseek(tmpf, 0L, SEEK_END);
        long fsz = ftell(tmpf);
        fseek(tmpf, 0L, SEEK_SET);
        do
        {
            if(fsz > 2048)
            {
                s = fread(tmp, 1, 2048, tmpf);
            }
            else
            {
                s = fread(tmp, 1, fsz, tmpf);
            }
            sz += s;
            html.append(tmp, s);
        }while(sz < fsz);
        if(html.size() > 0)
        {
            htmlDocument document(html);

            document.createTemplate(templateVariables[index]);

            std::string result = document.toString();

            for(int isz = 0; isz < templateVariables[index].size(); isz++)
            {
                StringUtils::replace(result, "[[" + templateVariables[index].keyAt(isz) + "]]", templateVariables[index][(long)isz]);
            }

            return result + "\r\n";
        }
        return "Not Valid Template";
    }

    public:

    RequestHandler(){};

    void setPathEvent(std::string& path, std::string& method, std::string(*event)(HttpRequest& request, unsigned char* requestBodyBuffer, FILE* requestBodyFile, size_t bufferSize))
    {
        paths.push_back(path);
        methods.push_back(method);
        events.push_back((void*)event);
        filePath.push_back("");
        istemplate.push_back(false);
        isupload.push_back(false);
        isdownload.push_back(false);
        templateVariables.push_back(StringMap());
    }

    void setPathFileDownload(std::string& path, std::string& method, std::string(*event)(HttpRequest& request, unsigned char* requestBodyBuffer, FILE* requestBodyFile, size_t bufferSize))
    {
        paths.push_back(path);
        methods.push_back(method);
        events.push_back((void*)event);
        filePath.push_back("");
        istemplate.push_back(false);
        isupload.push_back(false);
        isdownload.push_back(true);
        templateVariables.push_back(StringMap());
    }

    void setPathFileMapping(std::string& path, std::string& method, std::string& pathToFile)
    {
        paths.push_back(path);
        methods.push_back(method);
        events.push_back(0);
        filePath.push_back(pathToFile);
        istemplate.push_back(false);
        isupload.push_back(false);
        isdownload.push_back(false);
        templateVariables.push_back(StringMap());
    }

    void setTemplateMapping(std::string& path, std::string& method, std::string& pathToFile, StringMap& values)
    {
        paths.push_back(path);
        methods.push_back(method);
        events.push_back(0);
        filePath.push_back(pathToFile);
        istemplate.push_back(true);
        isupload.push_back(false);
        isdownload.push_back(false);
        templateVariables.push_back(values);
    }

    void setPathMultiPartFileUpload(std::string& path, std::string& method, std::string& directory)
    {
        paths.push_back(path);
        methods.push_back(method);
        events.push_back(0);
        filePath.push_back(directory);
        istemplate.push_back(false);
        isupload.push_back(true);
        isdownload.push_back(false);
        templateVariables.push_back(StringMap());
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

    bool isTemplate(std::string& path, std::string& method)
    {
        for(int i = 0; i < paths.size(); i++)
        {
            if(paths[i] == path && methods[i] == method)
            {
                return istemplate[i];
            }
        }
        return false;
    }

    bool isUpload(std::string& path, std::string& method)
    {
        for (int i = 0; i < paths.size(); i++)
        {
            if (paths[i] == path && methods[i] == method)
            {
                return isupload[i];
            }
        }
        return false;
    }

    bool isDownload(std::string& path, std::string& method)
    {
        for (int i = 0; i < paths.size(); i++)
        {
            if (paths[i] == path && methods[i] == method)
            {
                return isdownload[i];
            }
            else if (paths[i].length() - 1 <= path.length() && paths[i].substr(paths[i].length() - 1) == "*")
            {
                if (paths[i].substr(0, paths[i].length() - 1) == path.substr(0, paths[i].length() - 1) && methods[i] == method)
                {
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
            if(paths[i] == request.path && methods[i] == request.method && (events[i] != 0 || istemplate[i] || isupload[i] || isdownload[i]))
            {
                if(istemplate[i])
                {
                    return processTemplate(request, buffer, fbuffer, bufferSize, filePath[i], i);
                }
                else if (isupload[i])
                {
                    return processMultiPartUpload(request, buffer, fbuffer, bufferSize, filePath[i], i);
                }
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
void handleHTTPSRequest(SocketType& server, RequestHandler* requestHandle = 0, bool consoleOutput = true, std::string* sessionCookieKeyValuePair = 0)
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
    std::string randomFN = "tmp/" + randomFilename();
    FILE* tmpStorage = 0;
    bool useFileBuffer = false;
    int timeoutcounter = 4;//333;
    int timeoutdelay = 30;
    long knownreceivesize = 0;
    clock_t timeBegin = clock();
    while (true)
    {
        av = socketAvailable(server);
        if(av > 2048)
        {
            av = 2048;
        }
        if (knownreceivesize > 0 && av < knownreceivesize - transferSize)
        {
            av = knownreceivesize-transferSize;
            if (av > 2048)
            {
                av = 2048;
            }
        }
        if(av == 0)
        {
            int stp = 0;
            while (av == 0)
            {
                if(stp > timeoutcounter)
                {
                    if(transfered == 0)
                    {
                        break;
                    }
                    return;
                }
                usleep(timeoutdelay);
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
                int sucwrt = fwrite(buffer, 1, recv, tmpStorage);
                while (sucwrt < recv)
                {
                    sucwrt += fwrite(buffer, 1, recv-sucwrt, tmpStorage);
                }
            }
            
            recv = server.read_some(boost::asio::buffer(buffer, av));
            //printf("%.*s", recv, buffer);
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
            if (request.method == "POST" && request.attributes.get("content-type").find("multipart") >= 0)
            {
                timeoutcounter = 16;
                timeoutdelay = 30;
                knownreceivesize = atol(request.attributes.get("content-length").c_str());
                int headLength = std::string((char*)buffer).find("\r\n\r\n");
                if (headLength > 0)
                {
                    knownreceivesize += headLength + 4;
                }
            }
        }
        if(recv > 0)
        {
            transferSize += recv;
        }
        if(recv <= 0)
        {
            if (transferSize >= knownreceivesize || (clock()-timeBegin)/CLOCKS_PER_SEC > 5)
            {
                break;
            }
        }
    }
    
    if(transferSize > 0)
    {
        if(transferSize > 2048)
        {
            if(recv > 0)
            {
                int sucwrt = fwrite(buffer, 1, recv, tmpStorage);
                while (sucwrt < recv)
                {
                    sucwrt += fwrite(buffer, 1, recv - sucwrt, tmpStorage);
                }
            }
        }
        if (useFileBuffer)
        {
            fflush(tmpStorage);
        }
        transfered = 0;
        snd = 0;
        std::string generatedBody = "";
        std::string mappedFile = "";
        if(requestHandle)
        {
            if(request.cookies.size() > 0)
            {
                if(sessionCookieKeyValuePair)
                {
                    bool setCookie = false;
                    for(int i = 0; i < request.cookies.size(); i++)
                    {
                        if(request.cookies.keyAt(i) == sessionCookieKeyValuePair[0])
                        {
                            request.cookies.set(sessionCookieKeyValuePair[0], sessionCookieKeyValuePair[1]);
                            setCookie = true;
                            break;
                        }
                    }
                    if(!setCookie)
                    {
                        request.cookies.put(sessionCookieKeyValuePair[0], sessionCookieKeyValuePair[1]);
                    }
                }
            }
            else if(sessionCookieKeyValuePair)
            {
                if(!request.cookies.set(sessionCookieKeyValuePair[0], sessionCookieKeyValuePair[1]))
                {
                    request.cookies.put(sessionCookieKeyValuePair[0], sessionCookieKeyValuePair[1]);
                }
            }
            else
            {
                boost::uuids::random_generator_pure gen;
                boost::uuids::uuid gid = gen();
                if(!request.cookies.set("session", boost::uuids::to_string(gid)))
                {
                    request.cookies.put("session", boost::uuids::to_string(gid));
                }
            }
            if(requestHandle->containsPath(request.path, request.method, &mappedFile))
            {
                if(mappedFile.empty() || requestHandle->isTemplate(request.path, request.method) || requestHandle->isUpload(request.path, request.method) || requestHandle->isDownload(request.path, request.method))
                {
                    fclose(tmpStorage);
                    tmpStorage = fopen(randomFN.c_str(), "rb");

                    if(transferSize > 2048)
                    {
                        generatedBody = requestHandle->getEventResult(request, 0, tmpStorage, transferSize);
                    }
                    else
                    {
                        generatedBody = requestHandle->getEventResult(request, buffer, 0, transferSize);
                    }
                    if (requestHandle->isDownload(request.path, request.method))
                    {
                        mappedFile = generatedBody;
                        generatedBody = "";
                    }
                    else if (requestHandle->isUpload(request.path, request.method))
                    {
                        std::string newMeth = "GET";
                        request.attributes.set("content-type", "text/html");
                        request.method = newMeth;
                        if (requestHandle->containsPath(request.path, newMeth, &mappedFile))
                        {
                            generatedBody = requestHandle->getEventResult(request, buffer, 0, transferSize);
                        }
                        else
                        {
                            //std::string origin = request.attributes.get("origin");
                            std::string origin = "";
                            origin = request.attributes.get("origin");
                            if (!origin.empty() && origin != "/")
                            {
                                mappedFile = "";
                                request.path = origin;
                                int o = 0;
                                int idx = 0;
                                while (o < 3 && idx<origin.size())
                                {
                                    if (request.path[idx] == '/')
                                    {
                                        o++;
                                    }
                                    idx++;
                                }
                                request.path = "/" + request.path.substr(idx);
                                generatedBody = "";
                                mappedFile = "";
                            }
                            else
                            {
                                mappedFile = "";
                                request.path = "/";
                                generatedBody = "";
                            }
                        }
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
            if(request.cookies.size() > 0)
            {
                if(sessionCookieKeyValuePair)
                {
                    bool setCookie = false;
                    for(int i = 0; i < request.cookies.size(); i++)
                    {
                        if(request.cookies.keyAt(i) == sessionCookieKeyValuePair[0])
                        {
                            request.cookies.set(sessionCookieKeyValuePair[0], sessionCookieKeyValuePair[1]);
                            setCookie = true;
                            break;
                        }
                    }
                    if(!setCookie)
                    {
                        request.cookies.put(sessionCookieKeyValuePair[0], sessionCookieKeyValuePair[1]);
                    }
                }
            }
            else if(sessionCookieKeyValuePair)
            {
                if(!request.cookies.set(sessionCookieKeyValuePair[0], sessionCookieKeyValuePair[1]))
                {
                    request.cookies.put(sessionCookieKeyValuePair[0], sessionCookieKeyValuePair[1]);
                }
            }
            else
            {
                boost::uuids::random_generator_pure gen;
                boost::uuids::uuid gid = gen();
                if(!request.cookies.set("session", boost::uuids::to_string(gid)))
                {
                    request.cookies.put("session", boost::uuids::to_string(gid));
                }
            }
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

    std::vector<std::string> sessions;

    public:
    std::string privatKeyPath = "";
    std::string publicKeyPath = "";
    bool addSession(std::string ip)
    {
        for(int i = 0; i < sessions.size(); i++)
        {
            if(sessions[i] == ip)
            {
                return false;
            }
        }
        sessions.push_back(ip);
        return true;
    }

    Webserver()
    {

    }

    RequestHandler customRequests;

    void bindEvent(std::string requestPath, std::string requestMethod, std::string(*event)(HttpRequest& request, unsigned char* requestBodyBuffer, FILE* requestBodyFile, size_t bufferSize))
    {
        std::string meth = toUppercase(requestMethod);
        customRequests.setPathEvent(requestPath, meth, event);
    }

    /**
    * The Event returns a path to the downloadfile
    *
    */
    void bindFileDownload(std::string requestPath, std::string requestMethod, std::string(*event)(HttpRequest& request, unsigned char* requestBodyBuffer, FILE* requestBodyFile, size_t bufferSize))
    {
        std::string meth = toUppercase(requestMethod);
        customRequests.setPathFileDownload(requestPath, meth, event);
    }

    void bindFile(std::string requestPath, std::string requestMethod, std::string filePath)
    {
        std::string meth = toUppercase(requestMethod);
        if(fileExist(filePath))
        {
            customRequests.setPathFileMapping(requestPath, meth, filePath);
        }
        else
        {
            printf("File Mapping: %s --> %s failed! File doesn't exist.", requestPath.c_str(), filePath.c_str());
        }
    }

    void bindTemplate(std::string requestPath, std::string requestMethod, std::string filePath, StringMap templateValueMapping = StringMap())
    {
        std::string meth = toUppercase(requestMethod);
        if(fileExist(filePath))
        {
            customRequests.setTemplateMapping(requestPath, meth, filePath, templateValueMapping);
        }
        else
        {
            printf("Template Mapping: %s --> %s failed! File doesn't exist.", requestPath.c_str(), filePath.c_str());
        }
    }

    void bindMultiPartFileUpload(std::string requestPath, std::string requestMethod, std::string storageDirectory)
    {
        std::string meth = toUppercase(requestMethod);
        if (!boost::filesystem::exists(boost::filesystem::path(storageDirectory)))
        {
            boost::filesystem::create_directory(boost::filesystem::path(storageDirectory));
        }
        customRequests.setPathMultiPartFileUpload(requestPath, meth, storageDirectory);
    }

    static void sThreadSafe(void* params)
    {
        uintptr_t *parm = (uintptr_t*)params;
        int port = (int)(parm[0]);
        bool https = (bool)(parm[1]);
        bool consoleOutput = (bool)(parm[2]);
        Webserver* wserver = (Webserver*)parm[3];
        boost::asio::ip::tcp::endpoint* serverV4 = (boost::asio::ip::tcp::endpoint*)parm[4];
        boost::asio::ip::tcp::endpoint* serverV6 = (boost::asio::ip::tcp::endpoint*)parm[5];
        boost::asio::io_service* io_service = (boost::asio::io_service*)parm[6];
        boost::asio::ip::tcp::acceptor* acceptorV4 = (boost::asio::ip::tcp::acceptor*)parm[7];
        //boost::asio::ip::tcp::acceptor acceptorV6(io_service, serverV6);
        boost::asio::ssl::context* ssl_context = (boost::asio::ssl::context*)parm[8];

        while(true)
        {
            try
            {
                ssl_socket socket(*io_service, *ssl_context);
                acceptorV4->accept(socket.next_layer());
                socket.handshake(boost::asio::ssl::stream_base::handshake_type::server);
                bool isNewClient = wserver->addSession(socket.next_layer().remote_endpoint().address().to_string());
                if(wserver->customRequests.size() > 0)
                {
                    if(isNewClient)
                    {
                        boost::uuids::random_generator_pure gen;
                        boost::uuids::uuid gid = gen();
                        std::string sessionCookie[2];
                        sessionCookie[0] = "session";
                        sessionCookie[1] = boost::uuids::to_string(gid);
                        handleHTTPSRequest(socket, &(wserver->customRequests), consoleOutput, sessionCookie);
                    }
                    else
                    {
                        handleHTTPSRequest(socket, &(wserver->customRequests), consoleOutput);
                    }
                }
                else
                {
                    if(isNewClient)
                    {
                        boost::uuids::random_generator_pure gen;
                        boost::uuids::uuid gid = gen();
                        std::string sessionCookie[2];
                        sessionCookie[0] = "session";
                        sessionCookie[1] = boost::uuids::to_string(gid);
                        handleHTTPSRequest(socket, 0, consoleOutput, sessionCookie);
                    }
                    else
                    {
                        handleHTTPSRequest(socket, 0, consoleOutput);
                    }
                }
                socket.next_layer().close();

            }
            catch(std::exception e)
            {
                if(consoleOutput)
                {
                    printf("Connection Error!\n");
                }
            }
        }
    }

    static void sThreadUnsafe(void* params)
    {
        uintptr_t *parm = (uintptr_t*)params;
        int port = (int)(parm[0]);
        bool https = (bool)(parm[1]);
        bool consoleOutput = (bool)(parm[2]);
        Webserver* wserver = (Webserver*)parm[3];
        boost::asio::ip::tcp::endpoint* server = (boost::asio::ip::tcp::endpoint*)parm[4];
        boost::asio::io_service* io_service = (boost::asio::io_service*)parm[5];
        boost::asio::ip::tcp::acceptor* acceptor = (boost::asio::ip::tcp::acceptor*)parm[6];
        
        while(true)
        {
            try
            {
                boost::asio::ip::tcp::socket socket(*io_service);    
                acceptor->accept(socket);
                bool isNewClient = wserver->addSession(socket.remote_endpoint().address().to_string());
                if(wserver->customRequests.size() > 0)
                {
                    if(isNewClient)
                    {
                        boost::uuids::random_generator_pure gen;
                        boost::uuids::uuid gid = gen();
                        std::string sessionCookie[2];
                        sessionCookie[0] = "session";
                        sessionCookie[1] = boost::uuids::to_string(gid);
                        handleHTTPSRequest(socket, &(wserver->customRequests), consoleOutput, sessionCookie);
                    }
                    else
                    {
                        handleHTTPSRequest(socket, &(wserver->customRequests), consoleOutput);
                    }
                }
                else
                {
                    if(isNewClient)
                    {
                        boost::uuids::random_generator_pure gen;
                        boost::uuids::uuid gid = gen();
                        std::string sessionCookie[2];
                        sessionCookie[0] = "session";
                        sessionCookie[1] = boost::uuids::to_string(gid);
                        handleHTTPSRequest(socket, 0, consoleOutput, sessionCookie);
                    }
                    else
                    {
                        handleHTTPSRequest(socket, 0, consoleOutput);
                    }
                }
                socket.close();
            }
            catch(std::exception e)
            {
                if(consoleOutput)
                {
                    printf("Connection Error!\n");
                }
            }
        }
    }

    void sthread(uintptr_t* params, int threads)
    {
        int port = (int)(params[0]);
        bool https = (bool)(params[1]);
        bool consoleOutput = (bool)(params[2]);
        Webserver* wserver = (Webserver*)(params[3]);
		pthread_t thread[64];
        if(https)
        {
            boost::asio::ip::tcp::endpoint serverV4(boost::asio::ip::tcp::v4(), port);
            boost::asio::ip::tcp::endpoint serverV6(boost::asio::ip::tcp::v6(), port);
            boost::asio::io_service io_service;
            boost::asio::ip::tcp::acceptor acceptorV4(io_service, serverV4);
            //boost::asio::ip::tcp::acceptor acceptorV6(io_service, serverV6);
            boost::asio::ssl::context ssl_context(boost::asio::ssl::context::tlsv13_server);
            ssl_context.use_certificate_file(wserver->publicKeyPath, boost::asio::ssl::context_base::pem);
            ssl_context.use_private_key_file(wserver->privatKeyPath, boost::asio::ssl::context_base::pem);
            //ssl_context.use_tmp_dh_file("certs/dh2048.pem");
            
            uintptr_t args[9];
            args[0] = params[0];
            args[1] = params[1];
            args[2] = params[2];
            args[3] = params[3];
            args[4] = (uintptr_t)(&serverV4);
            args[5] = (uintptr_t)(&serverV6);
            args[6] = (uintptr_t)(&io_service);
            args[7] = (uintptr_t)(&acceptorV4);
            args[8] = (uintptr_t)(&ssl_context);
            uintptr_t* f = (uintptr_t*)(sThreadSafe);
            for(int i = 0; i < threads; i++)
            {
                pthread_create(&thread[i], 0, ((void*(*)(void* v))(f)), args);
            }
            for(int i = 0; i < threads; i++)
            {
                pthread_join(thread[i], 0);
            }
        }
        else
        {
            boost::asio::ip::tcp::endpoint server(boost::asio::ip::tcp::v4(), port);
            boost::asio::io_service io_service;
            boost::asio::ip::tcp::acceptor acceptor(io_service, server);
            
            uintptr_t args[9];
            args[0] = params[0];
            args[1] = params[1];
            args[2] = params[2];
            args[3] = params[3];
            args[4] = (uintptr_t)(&server);
            args[5] = (uintptr_t)(&io_service);
            args[6] = (uintptr_t)(&acceptor);
            uintptr_t* f = (uintptr_t*)(sThreadUnsafe);
            for(int i = 0; i < threads; i++)
            {
                pthread_create(&thread[i], 0, ((void*(*)(void* v))(f)), args);
            }
            for(int i = 0; i < threads; i++)
            {
                pthread_join(thread[i], 0);
            }
        }
    }

    void run(int port, bool https = true, std::string pathToPublicKey = "certs/newcert.pem", std::string pathToPrivatKey = "certs/privkey.pem", bool consoleOutput = true, int threads = 8)
    {
        bool noCert = false;
        if (!boost::filesystem::exists(boost::filesystem::path(pathToPublicKey)))
        {
            printf("Error: Public SSL cert not found (%s)\n", pathToPublicKey.c_str());
            noCert = true;
        }
        if (!boost::filesystem::exists(boost::filesystem::path(pathToPrivatKey)))
        {
            printf("Error: Private SSL cert not found (%s)\n", pathToPrivatKey.c_str());
            noCert = true;
        }
        if (noCert)
        {
            //system("pause");
            std::cin.get();
            return;
        }

        if(!boost::filesystem::exists(boost::filesystem::path("./tmp")))
        {
            boost::filesystem::create_directory(boost::filesystem::path("./tmp"));
        }
        this->privatKeyPath = pathToPrivatKey;
        this->publicKeyPath = pathToPublicKey;

        uintptr_t args[4];
        args[0] = (uintptr_t)(port);
        args[1] = (uintptr_t)(https);
        args[2] = (uintptr_t)(consoleOutput);
        args[3] = (uintptr_t)(this);
        sthread(args, threads);
    }
};

