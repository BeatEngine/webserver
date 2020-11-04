#ifndef ssl_socket
    #define ssl_socket boost::asio::ssl::stream<boost::asio::ip::tcp::socket>
#endif
#include "StringUtils.h"

class HttpRequest
{

    std::string toLowerCase(std::string input)
    {
        for(int i = 0; i < input.length(); i++)
        {
            if(input.at(i) < 91 && input.at(i) > 64)
            {
                input.at(i) += 32;
            }
        }
        return input;
    }

    std::string toUpperCase(std::string input)
    {
        for(int i = 0; i < input.length(); i++)
        {
            if(input.at(i) < 123 && input.at(i) > 96)
            {
                input.at(i) -= 32;
            }
        }
        return input;
    }

public:

    std::string path;
    std::string method;
    StringMap attributes;
    StringMap parameters;
    StringMap cookies;


    HttpRequest()
    {

    }

    std::string getQuery()
    {
        std::string result = "";
        for(int i = 0; i < parameters.size(); i++)
        {
            if(i == 0)
            {
                result += '?';
            }
            result += parameters.keyAt(i);
            result += parameters[i];
            if(i + 1 < parameters.size())
            {
                result += "&";
            }
        }
        return result;
    }

    std::string getSuffix()
    {
        for(int i = path.length()-1; i > 0; i--)
        {
            if(path[i] == '.')
            {
                return path.substr(i);
            }
        }
        return "";
    }

    std::string toString()
    {
        std::string generatedHeader = method + " " + path;
        if(parameters.size() > 0 && method == "GET")
        {
            generatedHeader += "?";
            for(int i = 0; i < parameters.size(); i++)
            {
                generatedHeader += parameters.keyAt(i) + "=" + parameters[i];
                if(i < parameters.size()-1)
                {
                    generatedHeader += "&";
                }
            }
        }
        if(parameters.size() > 0 && method == "POST")
        {
            for(int i = 0; i < parameters.size(); i++)
            {
                generatedHeader += parameters.keyAt(i) + "=" + parameters[i];
                if(i < parameters.size()-1)
                {
                    generatedHeader += "&";
                }
            }
        }
        generatedHeader += "\r\n";
        if(attributes.size() > 0 && method == "GET")
        {
            for(int i = 0; i < attributes.size(); i++)
            {
                generatedHeader += attributes.keyAt(i) + ": " + attributes[i];
                generatedHeader += "\r\n";
            }
        }
        for(int i = 0; i < cookies.size(); i++)
        {
            if(i == 0)
            {
                generatedHeader += "cookie: ";
            }
            generatedHeader += cookies.keyAt(i) + "=" + cookies[i];
            if(i+1 < cookies.size())
            {
                generatedHeader += "; ";
            }
        }
        generatedHeader += "\r\n";
        return generatedHeader;
    }

    

    HttpRequest(std::string plainRequest)
    {
        int a = 0;
        std::vector<std::string> lines;
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
                std::string var;
                std::string val;
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
                    attributes.put(toLowerCase(var), val);
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
                    std::vector<std::string> params;
                    a = 0;
                    for(int c = 0; c < path.length(); c++)
                    {
                        if(path.at(c) == '?')
                        {
                            if(c + 1 < path.length())
                            {
                                std::string paras = path.substr(c+1, path.length());
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

                    std::string var;
                    std::string val;
                    for(int c = 0; c < params.size(); c++)
                    {
                        for(int h = 0; h < params.at(c).length(); h++)
                        {
                            if(params.at(c).at(h) == '=')
                            {
                                var = params.at(c).substr(0, h);
                                val = params.at(c).substr(h+1, params.at(c).size());
                                parameters.put(var, val);
                                break;
                            }
                        }
                    }
                }

            }
        }

        if(attributes.contains("Cookie"))
        {
            std::string tmpc = attributes.get("Cookie");
            std::vector<std::string> pairs = StringUtils::split(tmpc, ";");
            for(int i = 0; i < pairs.size(); i++)
            {
                std::vector<std::string> pair = StringUtils::split(pairs[i], "=");
                if(pair.size() == 2)
                {
                    if(pair[0][0] == ' ')
                    {
                        pair[0] = pair[0].substr(1);
                    }
                    if(pair[1][0] == ' ')
                    {
                        pair[1] = pair[1].substr(1);
                    }
                    if(pair[0].size() > 0)
                    {
                        cookies.put(pair[0], pair[1]);
                    }
                }
            }
        } else if(attributes.contains("cookie"))
        {
            std::string tmpc = attributes.get("cookie");
            std::vector<std::string> pairs = StringUtils::split(tmpc, ";");
            for(int i = 0; i < pairs.size(); i++)
            {
                std::vector<std::string> pair = StringUtils::split(pairs[i], "=");
                if(pair.size() == 2)
                {
                    if(pair[0][0] == ' ')
                    {
                        pair[0] = pair[0].substr(1);
                    }
                    if(pair[1][0] == ' ')
                    {
                        pair[1] = pair[1].substr(1);
                    }
                    if(pair[0].size() > 0)
                    {
                        cookies.put(pair[0], pair[1]);
                    }
                }
            }
        }

    }


};

