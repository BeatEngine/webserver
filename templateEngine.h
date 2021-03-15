
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

        void loadChildren(std::string& sourceCode, int start, int end, bool special)
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
                            if(tstr == current && tgc != 0)
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
                    children.push_back(Tag(sourceCode, inners[i], inners[i+1], this));
                    A = inners[i+1]+1;
                }
            }   
            if(inners.size() == 0 || special)
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
                else if(pair.size() > 0)
                {
                    std::string tpa = pair[0];
                    std::string tpb = "";
                    for(int t = 1; t < pair.size(); t++)
                    {
                        tpb += pair[t];
                        if(t+1 < pair.size())
                        {
                            tpb += "=";
                        }
                    }
                    if(tpb[0] == '"')
                    {
                        tpb = tpb.substr(1);
                    }
                    if(tpb[tpb.size()-1] == '"')
                    {
                        tpb = tpb.substr(0, tpb.size()-1);
                    }
                    attributes.put(tpa, tpb);
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
            children = std::vector<Tag>();
        }
        Tag(std::string& sourceCode, Tag* parent = 0)
        {
            load(sourceCode);
        }

        Tag(std::string& sourceCode, int start, int end, Tag* parent = 0)
        {
            load(sourceCode, start, end);
        }

        void loadTemplate(StringMap& variables)
        {
            int start = 0;
            int end = 0;
            std::string pattA = "{{";
            std::string pattB = "}}";

            for(int i = 0; i < children.size(); i++)
            {
                if(children[i].name == "for")
                {
                    if(children[i].attributes.contains("condition"))
                    {
                        std::string condition = children[i].attributes.get("condition");
                        StringUtils::replace(condition, " ", "");
                        std::vector<std::string> parts = StringUtils::split(condition, ";");
                        if(parts.size() == 3)
                        {
                            std::vector<std::string> subs = StringUtils::split(parts[0], "=");
                            if(subs.size() == 2)
                            {
                                std::string var = subs[0];
                                std::string start = subs[1];
                                std::string eq = "==";
                                std::string se = "less=";
                                std::string ge = "greater=";
                                std::string sm = "less";
                                std::string gr = "greater";
                                std::string ne = "!=";
                                std::string loopConditionValue = "";
                                bool inc = true;
                                std::string conditionOperator = "";

                                if(StringUtils::contains(parts[1], eq))
                                {
                                    std::vector<std::string> prts = StringUtils::split(parts[1], eq);
                                    if(prts.size() == 2)
                                    {
                                        loopConditionValue = prts[1];
                                    }
                                    else
                                    {
                                        children.erase(children.begin()+i);
                                        i--;
                                    }
                                    conditionOperator = eq;
                                }
                                else if(StringUtils::contains(parts[1], se))
                                {
                                    std::vector<std::string> prts = StringUtils::split(parts[1], se);
                                    if(prts.size() == 2)
                                    {
                                        loopConditionValue = prts[1];
                                    }
                                    else
                                    {
                                        children.erase(children.begin()+i);
                                        i--;
                                    }
                                    conditionOperator = se;
                                }
                                else if(StringUtils::contains(parts[1], sm))
                                {
                                    std::vector<std::string> prts = StringUtils::split(parts[1], sm);
                                    if(prts.size() == 2)
                                    {
                                        loopConditionValue = prts[1];
                                    }
                                    else
                                    {
                                        children.erase(children.begin()+i);
                                        i--;
                                    }
                                    conditionOperator = sm;
                                }
                                else if(StringUtils::contains(parts[1], ge))
                                {
                                    std::vector<std::string> prts = StringUtils::split(parts[1], ge);
                                    if(prts.size() == 2)
                                    {
                                        loopConditionValue = prts[1];
                                    }
                                    else
                                    {
                                        children.erase(children.begin()+i);
                                        i--;
                                    }
                                    conditionOperator = ge;
                                }
                                else if(StringUtils::contains(parts[1], gr))
                                {
                                    std::vector<std::string> prts = StringUtils::split(parts[1], gr);
                                    if(prts.size() == 2)
                                    {
                                        loopConditionValue = prts[1];
                                    }
                                    else
                                    {
                                        children.erase(children.begin()+i);
                                        i--;
                                    }
                                    conditionOperator = gr;
                                }
                                else if(StringUtils::contains(parts[1], ne))
                                {
                                    std::vector<std::string> prts = StringUtils::split(parts[1], ne);
                                    if(prts.size() == 2)
                                    {
                                        loopConditionValue = prts[1];
                                    }
                                    else
                                    {
                                        children.erase(children.begin()+i);
                                        i--;
                                    }
                                    conditionOperator = ne;
                                }
                                else
                                {
                                    children.erase(children.begin()+i);
                                    i--;
                                }
                                if(loopConditionValue.size()>0)
                                {
                                    if(StringUtils::contains(parts[2], var))
                                    {
                                        std::vector<std::string> mode = StringUtils::split(parts[2], var);
                                        if(mode[0] == "++")
                                        {
                                            inc = true;
                                        }
                                        else if(mode[0] == "--")
                                        {
                                            inc = false;
                                        }
                                        else
                                        {
                                            children.erase(children.begin()+i);
                                            i--;
                                            continue;
                                        }
                                        if(conditionOperator.length()>0)
                                        {

                                            int it;
                                            int x;

                                            if(StringUtils::isDecimal(start))
                                            {
                                                it = atoi(start.c_str());
                                            }
                                            else if(variables.contains(start))
                                            {
                                                it = atoi(variables.get(start).c_str());
                                            }
                                            else
                                            {
                                                children.erase(children.begin()+i);
                                                i--;
                                                continue;
                                            }

                                            if(StringUtils::isDecimal(loopConditionValue))
                                            {
                                                x = atoi(loopConditionValue.c_str());
                                            }
                                            else if(variables.contains(loopConditionValue))
                                            {
                                                x = atoi(variables.get(loopConditionValue).c_str());
                                            }
                                            else
                                            {
                                                children.erase(children.begin()+i);
                                                i--;
                                                continue;
                                            }
                                            
                                            Tag generated;
                                            std::string IFstr = "if";

                                            while(true)
                                            {
                                                if(conditionOperator == eq)
                                                {
                                                    if(x != it)
                                                    {
                                                        break;
                                                    }
                                                }
                                                else if(conditionOperator == sm)
                                                {
                                                    if(it >= x)
                                                    {
                                                        break;
                                                    }
                                                }
                                                else if(conditionOperator == se)
                                                {
                                                    if(it > x)
                                                    {
                                                        break;
                                                    }
                                                }
                                                else if(conditionOperator == gr)
                                                {
                                                    if(it <= x)
                                                    {
                                                        break;
                                                    }
                                                }
                                                else if(conditionOperator == ge)
                                                {
                                                    if(it < x)
                                                    {
                                                        break;
                                                    }
                                                }
                                                else if(conditionOperator == ne)
                                                {
                                                    if(it != x)
                                                    {
                                                        break;
                                                    }
                                                }
                                                else
                                                {
                                                    break;
                                                }
                                                    //Action in loop

                                                    for(int cc = 0; cc < children[i].children.size(); cc++)
                                                    {
                                                        Tag child = children[i].children[cc];
                                                        if(child.attributes.contains(IFstr))
                                                        {
                                                            std::string ifCondition = child.attributes.get(IFstr);

                                                            bool conditionResult = true;
                                                            if(conditionResult)
                                                            {
                                                                StringMap varis = variables;
                                                                varis.put(var, std::to_string(it));
                                                                child.loadTemplate(varis);
                                                                child.attributes.remove("if");
                                                                generated.children.push_back(child);
                                                            }
                                                        }
                                                        else
                                                        {
                                                            StringMap varis = variables;
                                                            varis.put(var, std::to_string(it));
                                                            child.loadTemplate(varis);
                                                            generated.children.push_back(child);
                                                        }
                                                    }


                                                if(inc)
                                                {
                                                    it++;
                                                }
                                                else
                                                {
                                                    it--;
                                                }
                                            }
                                            children.erase(children.begin()+i);
                                            for(int gc = generated.children.size()-1; gc >= 0; gc--)
                                            {
                                                children.insert(children.begin()+i, generated.children[gc]);
                                            }
                                        }
                                    }
                                    else
                                    {
                                        children.erase(children.begin()+i);
                                        i--;
                                    }
                                }
                            }
                            else
                            {
                                children.erase(children.begin()+i);
                                i--;
                            }

                        }
                        else
                        {
                            children.erase(children.begin()+i);
                            i--;
                        }
                    }
                    else
                    {
                        children.erase(children.begin()+i);
                        i--;
                    }
                }
            }

            while(StringUtils::contains(innerText, pattA, true, &start) && StringUtils::contains(innerText, pattB, true, &end))
            {
                if(start < end)
                {
                    std::string varName = innerText.substr(start+2, end-start-2);
                    std::string brackA = "[";
                    std::string brackB = "]";
                    std::string accessVar = "";
                    if(StringUtils::contains(varName, brackA) && StringUtils::contains(varName, brackB))
                    {
                        int ba = varName.find(brackA);
                        int bb = varName.find(brackB);
                        if(bb > ba)
                        {
                            accessVar = varName.substr(ba+1, bb-ba-1);
                            varName = varName.substr(0, ba);
                        }
                    }
                    if(variables.contains(varName))
                    {
                        std::string var = variables.get(varName);
                        if(var.size()>0 && var[0] == '[' && var[var.size()-1] == ']')
                        {
                            var = var.substr(1,var.size()-2);
                            std::vector<std::string> items = StringUtils::split(var, ",", true);
                            for(int i = 0; i < items.size(); i++)
                            {
                                while(items[i].size() > 0 && items[i][0] == ' ')
                                {
                                    items[i] = items[i].substr(1);
                                }
                                if(items[i].size() > 0 && items[i][0] == '"' && items[i][items[i].size()-1] == '"')
                                {
                                    items[i] = items[i].substr(1,items[i].size()-2);
                                }
                            }
                            innerText = "";
                            if(accessVar.size() == 0)
                            {
                                for(int i = 0; i < items.size(); i++)
                                {
                                    std::string src = "<"+name+">"+items[i]+"</"+name+">";
                                    Tag item(src, this);
                                    children.push_back(item);
                                }
                                name = "span";
                            }
                            else
                            {
                                int idx;
                                if(StringUtils::isDecimal(accessVar))
                                {
                                    idx = atoi(accessVar.c_str());
                                    innerText = items[idx];
                                }
                                else if(variables.contains(accessVar))
                                {
                                    std::string tv = variables.get(accessVar);
                                    if(StringUtils::isDecimal(tv))
                                    {
                                        idx = atoi(tv.c_str());
                                        if(idx < items.size() && idx >= 0)
                                        {
                                            innerText = items[idx];
                                        }
                                        else
                                        {
                                            innerText = "{null}";
                                        }
                                    }
                                }
                                
                            }
                        }
                        else
                        {
                            innerText = innerText.substr(0, start) + variables.get(varName) + innerText.substr(end+2);
                        }
                    }
                    else
                    {
                        break;
                    }
                }
            }
            end = 0;
            start = 0;
            for(int i = 0; i < attributes.size(); i++)
            {
                std::string atv = attributes[i];
                if(StringUtils::contains(atv, pattA, true, &start) && StringUtils::contains(atv, pattB, true, &end))
                {
                    if(start < end)
                    {
                        std::string varName = attributes[i].substr(start+2, end-start-2);
                        if(variables.contains(varName))
                        {
                            if(attributes.keyAt(i) == "text")
                            {
                                innerText = variables.get(varName);
                                attributes.remove("text");
                            }
                            else
                            {
                                std::string res = attributes[i].substr(0, start) + variables.get(varName) + attributes[i].substr(end + 2);
                                attributes[i] = res;
                            }
                        }
                    }
                }
            }
            for(int i = 0; i < children.size(); i++)
            {
                children[i].loadTemplate(variables);
            }
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
                                    while(innerStart < end && sourceCode[innerStart-1] != '>')
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
                bool special = false;
                if (name == "script" || name == "style")
                {
                    special = true;
                }
                loadChildren(sourceCode, innerStart, innerEnd, special);
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

    void createTemplate(StringMap& variables)
    {
        root.loadTemplate(variables);
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





