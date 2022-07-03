#ifndef ssl_socket
    #define ssl_socket int
#endif



#ifdef __linux__ 
#define USLEEPDEFINED 1
#include <sys/ioctl.h>
#else
#define _WIN32_WINNT 0xFFFF
#include <Windows.h>
#include <ws2tcpip.h>
void* winWSADATA = 0;
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

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

#ifndef pthread_t
    #include <thread>
    #ifdef _THREAD_
    #define pthread_t std::thread
    void pthread_create(pthread_t* thread, int opt, void* (*function)(void* v), void* args)
    {
        *thread = std::thread((void(*)(void* v))(*function), args);
    }

    void pthread_join(pthread_t& thread, void* resultReturn)
    {
        thread.join();
    }
    #else
    #ifndef __linux__ 
    #include <Windows.h>
    #define pthread_t HANDLE
    void pthread_create(pthread_t* thread, int opt, void* (*function)(void* v), void* args)
    {
        DWORD ret;
        CreateThread((LPSECURITY_ATTRIBUTES)0, 0, (LPTHREAD_START_ROUTINE)function, args, 0, &ret);
    }

    void pthread_join(pthread_t& thread, void* resultReturn)
    {
        WaitForSingleObject(thread, INFINITE);
        CloseHandle(thread);
    }
    #endif
    #endif
#endif // !pthread_t

#include "uuid/uuid.h"

#ifdef __linux__
#include <sys/socket.h>
#include <arpa/inet.h>
#else
#include <winsock2.h>
#define socklen_t int
#define sleep(x)    Sleep(x*1000)
#endif

namespace filesystem
{

#include <sys/stat.h>
#ifdef __linux__ 
    inline int create_directory(const std::string path)
    {
        mkdir(path.c_str(), 0777);
    }
    inline int create_directory(const char* path)
    {
        mkdir(path, 0777);
    }
#else
#include <direct.h>
    inline int create_directory(const std::string path)
    {
        mkdir(path.c_str());
    }
    inline int create_directory(const char* path)
    {
        mkdir(path);
    }
#endif
    inline bool exists(const std::string name) {
        struct stat buffer;
        return (stat(name.c_str(), &buffer) == 0);
    }

    inline bool exists(const char* name) {
        struct stat buffer;
        return (stat(name, &buffer) == 0);
    }
}

//#include <boost/uuid/uuid.hpp>
//#include <boost/uuid/uuid_io.hpp>
//#include <boost/uuid/uuid_generators.hpp>
//#include <boost/filesystem.hpp>
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
    unsigned long av = 0;
#ifdef __linux__
    ioctl(socket, FIONREAD, &av);
    //WSAIoctl(socket, FIONREAD, 0, 0, &av, 0, &sz, &ov, 0);
#else
    unsigned long sz = 0;
    WSAOVERLAPPED ov = { 0 };
    WSAIoctl(socket, FIONREAD, 0,0, &av, 0, &sz, &ov, 0);
#endif
    return av;
}

inline std::size_t socketAvailable(TLSContext* p)
{
    return socketAvailable(*((ssl_socket*)(p)));
}

inline int unsafe_read(TLSContext* server, unsigned char* buffer, int size)
{
    return recv((intptr_t)server, (char*)buffer, size, 0);
}

inline int unsafe_write(TLSContext* server, const char* buffer, int size)
{
    return send((intptr_t)server, buffer, size, 0);
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


void handleHTTPSRequest(TLSContext* server, RequestHandler* requestHandle = 0, bool consoleOutput = true, std::string* sessionCookieKeyValuePair = 0, bool secure = true)
{
    unsigned char buffer[2048];
    int recv = 0;
    int snd = 0;
    long transferSize = 0;
    long transfered = 0;
    int w = 0;
    long av;
    HttpRequest request;
    std::string randomFN = "tmp/" + randomFilename();
    FILE* tmpStorage = 0;
    bool useFileBuffer = false;
    int timeoutcounter = 40;//333;
    int timeoutdelay = 30;
    long knownreceivesize = 0;
    clock_t timeBegin = clock();
    while (true)
    {
        av = 2048;
        
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
            
            //recv = server.read_some(boost::asio::buffer(buffer, av));
            if (secure)
            {
                recv = SSL_read(server, buffer, av);
            }
            else
            {
                recv = unsafe_read(server, buffer, av);
            }
            //printf("%*.s\n", recv, buffer);
            if (recv < 0)
            {
                break;
            }

            
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
        if(recv < 0)
        {
            //if (transferSize >= knownreceivesize || (clock()-timeBegin)/CLOCKS_PER_SEC > 5)
            //{
                break;
            //}
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
                if(!request.cookies.set("session", uuid::UUID::New().String()))
                {
                    request.cookies.put("session", uuid::UUID::New().String());
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
                        if (secure)
                        {
                            snd += SSL_write(server, responseHead.c_str() + snd, responseHead.size() - snd);
                        }
                        else
                        {
                            snd += unsafe_write(server, responseHead.c_str() + snd, responseHead.size() - snd);
                        }
                        
                        //snd += server.write_some(boost::asio::buffer(responseHead.c_str()+snd, responseHead.size()-snd));
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
                        
                        if (secure)
                        {
                            snd += SSL_write(server, tmpData + snd, generatedBody.size() - snd);
                        }
                        else
                        {
                            snd += unsafe_write(server, (const char*)(tmpData + snd), generatedBody.size() - snd);
                        }
                        //snd += server.write_some(boost::asio::buffer(tmpData+snd, generatedBody.size()-snd));
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
                        
                        if (secure)
                        {
                            snd = SSL_write(server, notFound.c_str() + snd, notFound.size() - snd);
                        }
                        else
                        {
                            snd = unsafe_write(server, notFound.c_str() + snd, notFound.size() - snd);
                        }
                        //snd = server.write_some(boost::asio::buffer(notFound.c_str()+snd, notFound.size()-snd));
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
                        if (secure)
                        {
                            snd = SSL_write(server, notFound.c_str() + snd, notFound.size() - snd);
                        }
                        else
                        {
                            snd = unsafe_write(server, notFound.c_str() + snd, notFound.size() - snd);
                        }
                        
                        //snd = server.write_some(boost::asio::buffer(notFound.c_str()+snd, notFound.size()-snd));
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
                if (!request.cookies.set("session", uuid::UUID::New().String()))
                {
                    request.cookies.put("session", uuid::UUID::New().String());
                }
            }
            std::string responseHead = generateResponsehead(size, request);
            snd = 0;
            try
            {
                while (snd < responseHead.length())
                {
                    int s = 0;
                    if (secure)
                    {
                        s = SSL_write(server, responseHead.c_str() + snd, responseHead.size() - snd);
                    }
                    else
                    {
                        s = unsafe_write(server, responseHead.c_str() + snd, responseHead.size() - snd);
                    }
                    //snd += server.write_some(boost::asio::buffer(responseHead.c_str()+snd, responseHead.size()-snd));
                    if(s <= 0)
                    {
                        break;
                    }
                    snd += s;
                    transfered += snd;
                    if (snd >= responseHead.size())
                    {
                        break;
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
                        int s = 0;
                        if (secure)
                        {
                            s = SSL_write(server, buffer + snd, r - snd);
                        }
                        else
                        {
                            s = unsafe_write(server,(const char*)(buffer + snd), r - snd);
                        }
                        //snd += server.write_some(boost::asio::buffer(buffer+snd, r-snd));
                        if(s == -1)
                        {
                            break;
                        }
                        snd += s;
                        transfered += snd;
                        if (snd >= responseHead.size())
                        {
                            break;
                        }
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
                    
                    if (secure)
                    {
                        snd = SSL_write(server, notFound.c_str() + snd, notFound.size() - snd);
                    }
                    else
                    {
                        snd = unsafe_write(server, notFound.c_str() + snd, notFound.size() - snd);
                    }
                    //snd = server.write_some(boost::asio::buffer(notFound.c_str()+snd, notFound.size()-snd));
                    if(snd == -1)
                    {
                        break;
                    }
                    transfered += snd;
                    if (snd >= notFound.size())
                    {
                        break;
                    }
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
        if (!filesystem::exists(storageDirectory))
        {
            filesystem::create_directory(storageDirectory);
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

        std::string pathToPublicKey = "certs/newcert.pem";
        std::string pathToPrivatKey = "certs/privkey.pem";

        

        SSL* server_ctx = SSL_CTX_new(SSLv3_server_method());
        if (!server_ctx) {
            fprintf(stderr, "Error creating server context\n");
            return;
        }
        SSL_CTX_use_certificate_file(server_ctx, pathToPublicKey.c_str(), SSL_SERVER_RSA_CERT);
        SSL_CTX_use_PrivateKey_file(server_ctx, pathToPrivatKey.c_str(), SSL_SERVER_RSA_KEY);

        if (!SSL_CTX_check_private_key(server_ctx)) {
            fprintf(stderr, "Private key not loaded\n");
            return;
        }

        while(true)
        {
            sockaddr_in server;
            sockaddr_in client;
           
            try
            {
                int client_sock, read_size;
                int c;
                ssl_socket sock;


                client_sock = -1;
                
                int enable = 1;
#ifdef __linux__ 
              
                sock = socket(AF_INET, SOCK_STREAM, 0);
                server.sin_port = htons(port);
                server.sin_family = AF_INET;
                
                setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&enable, sizeof(int));
                struct timeval timeout;
                timeout.tv_sec = 1;
                timeout.tv_usec = 0;
                if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) < 0)
                {
                    std::string err = "set socket option timeout failed!\n";
                    perror(err.c_str());
                    return;
                }
                if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout)) < 0)
                {
                    std::string err = "set socket option timeout failed!\n";
                    perror(err.c_str());
                    return;
                }
                int err = bind(sock, (struct sockaddr*)&server, sizeof(server));
#else
                struct addrinfo hints;
                struct addrinfo* result = 0;
                memset(&hints, 0, sizeof(hints));
                hints.ai_family = AF_INET;
                hints.ai_socktype = SOCK_STREAM;
                hints.ai_protocol = IPPROTO_TCP;
                hints.ai_flags = AI_PASSIVE;

                // Resolve the server address and port
                int iResult = getaddrinfo(NULL, std::to_string(port).c_str(), &hints, &result);
                if (iResult != 0) {
                    printf("getaddrinfo failed with error: %d\n", iResult);
                    return;
                }

                hostent* localHost;
                char* localIP;

         
                sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

             
                setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&enable, sizeof(int));
                int timeout = 1000;
                if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) < 0)
                {
                    std::string err = "set socket option: ";
                    err += std::to_string(WSAGetLastError());
                    perror(err.c_str());
                    return;
                }
              
                int err = bind(sock, result->ai_addr, (int)result->ai_addrlen);

                
#endif


                
                
                if (err < 0) {
                    perror(std::string("bind failed. Error: " + std::to_string(err)).c_str());
                    return;
                }


                listen(sock, 3);

                c = sizeof(struct sockaddr_in);

                unsigned int size;

                

                //acceptorV4->accept(socket.next_layer());
                //socket.handshake(boost::asio::ssl::stream_base::handshake_type::server);
                
                while (client_sock < 0)
                {
                    client_sock = accept(sock, (struct sockaddr*)&client, &c);
                    usleep(10);
                }

                SSL* client = SSL_new(server_ctx);
                if (!client) {
                    fprintf(stderr, "Error creating SSL client\n");
                    break;
                }

                SSL_set_fd(client, client_sock);
                if (!SSL_accept(client))
                {
                    fprintf(stderr, "Error in handshake\n");
                    continue;
                }
                bool isNewClient = wserver->addSession(((struct sockaddr*)&client_sock)->sa_data);
                if (wserver->customRequests.size() > 0)
                {
                    if (isNewClient)
                    {
                        std::string sessionCookie[2];
                        sessionCookie[0] = "session";
                        sessionCookie[1] = uuid::UUID::New().String();
                        //printf("A\n");
                        handleHTTPSRequest((TLSContext*)client, &(wserver->customRequests), consoleOutput, sessionCookie);
                        //printf("B\n");
                    }
                    else
                    {
                        handleHTTPSRequest((TLSContext*)client, &(wserver->customRequests), consoleOutput);
                    }
                }
                else
                {
                    if (isNewClient)
                    {
                        std::string sessionCookie[2];
                        sessionCookie[0] = "session";
                        sessionCookie[1] = uuid::UUID::New().String();
                        handleHTTPSRequest((TLSContext*)client, 0, consoleOutput, sessionCookie);
                    }
                    else
                    {
                        handleHTTPSRequest((TLSContext*)client, 0, consoleOutput);
                    }
                }

                //printf("Shutting down...\n");
                //socket.next_layer().close();
                SSL_shutdown(client);
#ifdef __linux__
                //printf("Shutting down 2...\n");
                shutdown(client_sock, SHUT_RDWR);
                //printf("Shutting down close...\n");
                shutdown(sock, SHUT_RDWR);
                close(client_sock);
                close(sock);
#else
                shutdown(client_sock, SD_BOTH);
                shutdown(sock, SD_BOTH);
                closesocket(client_sock);
                closesocket(sock);
#endif
                //printf("End...\n");
                SSL_free(client);

                
                
            }
            catch(std::exception e)
            {
                if(consoleOutput)
                {
                    printf("Connection Error!\n");
                }
            }
            
        }
        SSL_CTX_free(server_ctx);
    }

    static void sThreadUnsafe(void* params)
    {
        uintptr_t* parm = (uintptr_t*)params;
        int port = (int)(parm[0]);
        bool https = (bool)(parm[1]);
        bool consoleOutput = (bool)(parm[2]);
        Webserver* wserver = (Webserver*)parm[3];
        //boost::asio::ip::tcp::endpoint* serverV4 = (boost::asio::ip::tcp::endpoint*)parm[4];
        //boost::asio::ip::tcp::endpoint* serverV6 = (boost::asio::ip::tcp::endpoint*)parm[5];
        //boost::asio::io_service* io_service = (boost::asio::io_service*)parm[6];
        //boost::asio::ip::tcp::acceptor* acceptorV4 = (boost::asio::ip::tcp::acceptor*)parm[7];
        //boost::asio::ip::tcp::acceptor acceptorV6(io_service, serverV6);
        //boost::asio::ssl::context* ssl_context = (boost::asio::ssl::context*)parm[8];

        std::string pathToPublicKey = "certs/newcert.pem";
        std::string pathToPrivatKey = "certs/privkey.pem";



        SSL* server_ctx = SSL_CTX_new(SSLv3_server_method());
        if (!server_ctx) {
            fprintf(stderr, "Error creating server context\n");
            return;
        }
        SSL_CTX_use_certificate_file(server_ctx, pathToPublicKey.c_str(), SSL_SERVER_RSA_CERT);
        SSL_CTX_use_PrivateKey_file(server_ctx, pathToPrivatKey.c_str(), SSL_SERVER_RSA_KEY);

        if (!SSL_CTX_check_private_key(server_ctx)) {
            fprintf(stderr, "Private key not loaded\n");
            return;
        }

        while (true)
        {
            sockaddr_in server;
            sockaddr_in client;

            try
            {
                int client_sock, read_size;
                int c;
                ssl_socket sock;


                client_sock = -1;

                int enable = 1;
#ifdef __linux__ 

                sock = socket(AF_INET, SOCK_STREAM, 0);
                server.sin_port = htons(port);
                server.sin_family = AF_INET;

                setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&enable, sizeof(int));
                struct timeval timeout;
                timeout.tv_sec = 1;
                timeout.tv_usec = 0;
                if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) < 0)
                {
                    std::string err = "set socket option timeout failed!\n";
                    perror(err.c_str());
                    return;
                }
                if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout)) < 0)
                {
                    std::string err = "set socket option timeout failed!\n";
                    perror(err.c_str());
                    return;
                }
                int err = bind(sock, (struct sockaddr*)&server, sizeof(server));
#else
                struct addrinfo hints;
                struct addrinfo* result = 0;
                memset(&hints, 0, sizeof(hints));
                hints.ai_family = AF_INET;
                hints.ai_socktype = SOCK_STREAM;
                hints.ai_protocol = IPPROTO_TCP;
                hints.ai_flags = AI_PASSIVE;

                // Resolve the server address and port
                int iResult = getaddrinfo(NULL, std::to_string(port).c_str(), &hints, &result);
                if (iResult != 0) {
                    printf("getaddrinfo failed with error: %d\n", iResult);
                    return;
                }

                hostent* localHost;
                char* localIP;


                sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);


                //localHost = gethostbyname("");
                //localIP = inet_ntoa(*(struct in_addr*)*localHost->h_addr_list);


                //server.sin_family = AF_INET;
                //server.sin_addr.s_addr = inet_addr(localIP);
                //server.sin_port = htons(5150);

                setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&enable, sizeof(int));
                int timeout = 1000;
                if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) < 0)
                {
                    std::string err = "set socket option: ";
                    err += std::to_string(WSAGetLastError());
                    perror(err.c_str());
                    return;
                }

                int err = bind(sock, result->ai_addr, (int)result->ai_addrlen);


#endif




                if (err < 0) {
                    perror(std::string("bind failed. Error: " + std::to_string(err)).c_str());
                    return;
                }


                listen(sock, 3);

                c = sizeof(struct sockaddr_in);

                unsigned int size;



                //acceptorV4->accept(socket.next_layer());
                //socket.handshake(boost::asio::ssl::stream_base::handshake_type::server);

                while (client_sock < 0)
                {
                    client_sock = accept(sock, (struct sockaddr*)&client, &c);
                    usleep(10);
                }

                bool isNewClient = wserver->addSession(((struct sockaddr*)&client_sock)->sa_data);
                if (wserver->customRequests.size() > 0)
                {
                    if (isNewClient)
                    {
                        std::string sessionCookie[2];
                        sessionCookie[0] = "session";
                        sessionCookie[1] = uuid::UUID::New().String();
                        printf("A\n");
                        handleHTTPSRequest((TLSContext*)client_sock, &(wserver->customRequests), consoleOutput, sessionCookie, false);
                        printf("B\n");
                    }
                    else
                    {
                        handleHTTPSRequest((TLSContext*)client_sock, &(wserver->customRequests), consoleOutput,0, false);
                    }
                }
                else
                {
                    if (isNewClient)
                    {
                        std::string sessionCookie[2];
                        sessionCookie[0] = "session";
                        sessionCookie[1] = uuid::UUID::New().String();
                        handleHTTPSRequest((TLSContext*)client_sock, 0, consoleOutput, sessionCookie, false);
                    }
                    else
                    {
                        handleHTTPSRequest((TLSContext*)client_sock, 0, consoleOutput,0, false);
                    }
                }

#ifdef __linux__
                //printf("Shutting down ...\n");
                shutdown(client_sock, SHUT_RDWR);
                //printf("Shutting down close...\n");
                shutdown(sock, SHUT_RDWR);
                close(client_sock);
                close(sock);
#else
                shutdown(client_sock, SD_BOTH);
                shutdown(sock, SD_BOTH);
                closesocket(client_sock);
                closesocket(sock);
#endif
                //printf("End...\n");



            }
            catch (std::exception e)
            {
                if (consoleOutput)
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
            
            uintptr_t args[9];
            args[0] = params[0];
            args[1] = params[1];
            args[2] = params[2];
            args[3] = params[3];
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
            uintptr_t args[9];
            args[0] = params[0];
            args[1] = params[1];
            args[2] = params[2];
            args[3] = params[3];
            uintptr_t* f = (uintptr_t*)(sThreadUnsafe);
            for (int i = 0; i < threads; i++)
            {
                pthread_create(&thread[i], 0, ((void* (*)(void* v))(f)), args);
            }
            for (int i = 0; i < threads; i++)
            {
                pthread_join(thread[i], 0);
            }
        }
    }

    void run(int port, bool https = true, std::string pathToPublicKey = "certs/newcert.pem", std::string pathToPrivatKey = "certs/privkey.pem", bool consoleOutput = true, int threads = 8)
    {

#ifndef __linux__ 
        WSADATA* wsaData = (WSADATA*)calloc(1, sizeof(WSADATA));
        winWSADATA = wsaData;
        int currentStateResult = WSAStartup(MAKEWORD(2, 2), wsaData);
        if (currentStateResult != 0) {
            std::string err = "WSAStartup failed with result: ";
            err += std::to_string(currentStateResult);
            perror(err.c_str());
            return;
        }
#endif


        bool noCert = false;
        if (!filesystem::exists(pathToPublicKey))
        {
            printf("Error: Public SSL cert not found (%s)\n", pathToPublicKey.c_str());
            noCert = true;
        }
        if (!filesystem::exists(pathToPrivatKey))
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

        if(!filesystem::exists("./tmp"))
        {
            filesystem::create_directory("./tmp");
        }
        this->privatKeyPath = pathToPrivatKey;
        this->publicKeyPath = pathToPublicKey;

        uintptr_t args[4];
        args[0] = (uintptr_t)(port);
        args[1] = (uintptr_t)(https);
        args[2] = (uintptr_t)(consoleOutput);
        args[3] = (uintptr_t)(this);
        sthread(args, threads);
#ifndef __linux__ 
        if (winWSADATA)
        {
            WSADATA* wsaData = (WSADATA*)winWSADATA;
            if (wsaData->lpVendorInfo)
            {
                free(wsaData->lpVendorInfo);
            }
            free(winWSADATA);
            WSACleanup();
        }
#endif
    }
};

