
#include <string>
#include <vector>

#ifndef STRING_UTILS_USED
    #include "StringUtils.h"
#endif

namespace htmlElement
{
    class Tag
    {
        public:
        StringMap attributes;
        std::string name;
        std::string innerText;
        std::vector<Tag> children;

        void loadChildren(std::string& sourceCode, int start, int end)
        {
            std::vector<int> inners;
            std::string current = "";
            std::string tstr;
            int tgc = 0;
            for(int i = start; i < end; i++)
            {
                if(sourceCode[i] == '<' && i+1 < end)
                {
                    if(sourceCode[i+1] == '/')
                    {
                        int a = i;
                        while (i < end && sourceCode[i] != ' ' && sourceCode[i] != '>')
                        {
                            i++;
                        }
                        if(i < end && (sourceCode[i] || ' ' && sourceCode[i] == '>'))
                        {
                            tstr = sourceCode.substr(a+2,i-a-2);
                            if(tstr == current)
                            {
                                tgc--;
                            }
                        }
                        if(tgc == 0)
                        {
                            int tmpis;
                            if(sourceCode[i] == '>')
                            {
                                tmpis = i + 1;
                            }
                            else
                            {
                                tmpis = i + 1;
                                while(tmpis < end && sourceCode[tmpis] != '>')
                                {
                                    tmpis++;
                                }
                            }
                            inners.push_back(tmpis);
                        }
                    }
                    else
                    {
                        int a = i;
                        while (i < end && sourceCode[i] != ' ' && sourceCode[i] != '>')
                        {
                            i++;
                        }
                        if(i < end && (sourceCode[i] == ' ' || sourceCode[i] == '>'))
                        {
                            tstr = sourceCode.substr(a+1,i-a-1);
                            if(tstr == current)
                            {
                                tgc++;
                            }
                            if(tgc == 0)
                            {
                                current = tstr;
                                inners.push_back(a);
                                tgc++;
                            }
                        }
                    }
                }
            }
            int A = start;
            for(int i = 0; i < inners.size(); i+=2)
            {
                innerText = "";
                if(i+1 < inners.size())
                {
                    innerText += sourceCode.substr(A, (size_t)(inners[i])-A);
                    children.push_back(Tag(sourceCode, inners[i], inners[i+1]));
                    A = inners[i+1]+1;
                }
            }   
            if(inners.size() == 0)
            {
                innerText = sourceCode.substr(start, end-start);
            } 
        }

        void loadAttributes(std::string attr)
        {
            std::vector<std::string> pairs = StringUtils::split(attr, " ", true);
            for(int i = 0; i < pairs.size(); i++)
            {
                std::vector<std::string> pair = StringUtils::split(pairs[i], "=");
                if(pair.size() == 2)
                {
                    if(pair[1][0] == '"')
                    {
                        pair[1] = pair[1].substr(1);
                    }
                    if(pair[1][pair[1].size()-1] == '"')
                    {
                        pair[1] = pair[1].substr(0, pair[1].size()-1);
                    }
                    attributes.put(pair[0], pair[1]);
                }
            }
        }

        bool innerTextIsNotTrash()
        {
            for(int i = 0; i < innerText.size(); i++)
            {
                if(innerText[i] != ' ' && innerText[i] != '\t' && innerText[i] != '\n')
                {
                    return true;
                }
            }
            return true;
        }

        public:
        Tag()
        {

        }
        Tag(std::string& sourceCode)
        {
            load(sourceCode);
        }

        Tag(std::string& sourceCode, int start, int end)
        {
            load(sourceCode, start, end);
        }

        std::string toString()
        {
            std::string result = "";
            result += "<" + name;
            for(int i = 0; i < attributes.size(); i++)
            {
                result += " ";
                result += attributes.keyAt(i);
                result += "=\"";
                result += attributes[i] + "\"";
            }
            result += ">";
            if(innerTextIsNotTrash())
            {
                result += innerText;
            }
            for(int i = 0; i < children.size(); i++)
            {
                result += children[i].toString();
            }
            result += "</" + name + ">";
            return result;
        }

        void load(std::string& sourceCode, int start = 0, int end = -1)
        {
            std::string current = "";
            std::string tstr;
            int tgc = 0;
            int innerStart = 0;
            int innerEnd = 0;
            if(end == -1)
            {
                end = sourceCode.size();
            }
            for(int i = start; i < end; i++)
            {
                if(sourceCode[i] == '<' && i+1 < end)
                {
                    if(sourceCode[i+1] == '/')
                    {
                        int a = i;
                        while (i < end && sourceCode[i] != ' ' && sourceCode[i] != '>')
                        {
                            i++;
                        }
                        if(i < end && (sourceCode[i] || ' ' && sourceCode[i] == '>'))
                        {
                            tstr = sourceCode.substr(a+2,i-a-2);
                            if(tstr == current)
                            {
                                tgc--;
                            }
                        }
                        if(tgc == 0)
                        {
                            innerEnd = a;
                        }
                    }
                    else
                    {
                        int a = i;
                        while (i < end && sourceCode[i] != ' ' && sourceCode[i] != '>')
                        {
                            i++;
                        }
                        if(tgc == 0)
                        {
                            int headStart = i+1;
                            int headEnd = i;
                            while (headEnd < end && sourceCode[headEnd] != '>')
                            {
                                headEnd++;
                            }
                            headEnd--;
                            if(headStart < headEnd)
                            {
                                loadAttributes(sourceCode.substr(headStart, headEnd-headStart));
                            }
                        }
                        if(i < end && (sourceCode[i] == ' ' || sourceCode[i] == '>'))
                        {
                            tstr = sourceCode.substr(a+1,i-a-1);
                            if(tstr == current)
                            {
                                tgc++;
                            }
                            if(tgc == 0)
                            {
                                current = tstr;
                                if(sourceCode[i] == '>')
                                {
                                    innerStart = i + 1;
                                }
                                else
                                {
                                    innerStart = i + 1;
                                    while(innerStart < end && sourceCode[innerStart] != '>')
                                    {
                                        innerStart++;
                                    }
                                }
                                tgc++;
                            }
                        }
                    }
                }
            }
            name = current;
            if(innerEnd > innerStart)
            {
                loadChildren(sourceCode, innerStart, innerEnd);
            }
        }
    };
}

class htmlDocument
{


    htmlElement::Tag root;

    public:
    htmlDocument()
    {

    }

    htmlDocument(std::string html)
    {
        load(html);
    }

    void load(std::string& html)
    {
        root = htmlElement::Tag(html);
    }

    std::string toString()
    {
        return root.toString();
    }

    htmlElement::Tag& node()
    {
        return root;
    }

};





