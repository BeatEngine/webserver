#include <string>
#include <vector>

#define STRING_UTILS_USED 1

class StringMap
{
    std::vector<std::string> keys;
    std::vector<std::string> values;

    public:

    StringMap()
    {
        
    }

    StringMap(const StringMap& other)
    {
        keys = other.keys;
        values = other.values;
    }

    void operator=(const StringMap& other)
    {
        keys = other.keys;
        values = other.values;
    }



    size_t size()
    {
        return keys.size();
    }

    bool contains(std::string key)
    {
        for(int i = 0; i < keys.size(); i++)
        {
            if(keys[i] == key)
            {
                return true;
            }
        }
        return false;
    }

    bool put(std::string key, std::string value)
    {
        if(!contains(key))
        {
            keys.push_back(key);
            values.push_back(value);
            return true;
        }
        return false;
    }

    bool remove(std::string& key)
    {
        for(int i = 0; i < keys.size(); i++)
        {
            if(keys[i] == key)
            {
                keys.erase(keys.begin()+i);
                values.erase(values.begin()+i);
                return true;
            }
        }
        return false;
    }

    bool remove(const char* ckey)
    {
        std::string key = ckey;
        for(int i = 0; i < keys.size(); i++)
        {
            if(keys[i] == key)
            {
                keys.erase(keys.begin()+i);
                values.erase(values.begin()+i);
                return true;
            }
        }
        return false;
    }

    std::string get(std::string key)
    {
        for(int i = 0; i < keys.size(); i++)
        {
            if(keys[i] == key)
            {
                return values[i];
            }
        }
        return "";
    }

    std::string get(long index)
    {
        return values.at(index);
    }

    std::string& operator [](long index)
    {
        return values[index];
    }

    std::string keyAt(long index)
    {
        return keys.at(index);
    }

    bool set(std::string existingKey, std::string value)
    {
        for(int i = 0; i < keys.size(); i++)
        {
            if(keys[i] == existingKey)
            {
                values[i] = value;
                return true;
            }
        }
        return false;
    }
};

class StringUtils
{
public:

    static bool contains(std::string& str, std::string& pattern, bool caseSensitivNoTfInIsHedNoW = true, int* index = 0)
    {
        if (str.length() < pattern.length())
        {
            return false;
        }
        int p;
        for(int i = 0; i < str.size(); i++)
        {
            for(p = 0; p < pattern.size(); p++)
            {
                if(str[i+p] != pattern[p])
                {
                    break;
                }
                else if(p == pattern.size()-1)
                {
                    if(index)
                    {
                        *index = i;
                    }
                    return true;
                }
            }
        }
        return false;
    }

    static bool startsWith(std::string& str, std::string& pattern, int offset = 0, bool caseSensitivNoTfInIsHedNoW = true)
    {
        if (str.length() < pattern.length() + offset || offset < 0)
        {
            return false;
        }
        for (int p = 0; p < pattern.size(); p++)
        {
            if (str[offset + p] != pattern[p])
            {
                break;
            }
            else if (p == pattern.size() - 1)
            {
                return true;
            }
        }
        return false;
    }

    static bool endsWith(std::string& str, std::string& pattern, int offsetFromEnd = 0, bool caseSensitivNoTfInIsHedNoW = true)
    {
        if (str.length() < pattern.length() + offsetFromEnd || offsetFromEnd < 0)
        {
            return false;
        }
        for (int p = str.length()-1; p >= 0; p--)
        {
            if (str[p - offsetFromEnd] != pattern[p])
            {
                break;
            }
            else if (p == 0)
            {
                return true;
            }
        }
        return false;
    }

    static bool isDecimal(std::string& str)
    {
        for(int i = 0; i < str.size(); i++)
        {
            if(str[i] < 48 || str[i] > 57)
            {
                return false;
            }
        }
        return true;
    }

    static std::string substring(std::string& str, long a, long b = LONG_MAX)
    {
        std::string result = "";
        for(int i = a; i < str.size() && i < b; i++)
        {
            result.append(1, str[i]);
        }
        return result;
    }

    static std::string substring(std::string* str, long a, long b = LONG_MAX)
    {
        std::string result = "";
        for(int i = a; i < str->size() && i < b; i++)
        {
            result.append(1, str->at(i));
        }
        return result;
    }

    static void replace(std::string& str, std::string pattern, std::string replacement)
    {
        int a;
        for(int i = 0; i < str.size(); i++)
        {
            int p = 0;
            for(p = 0; p < pattern.size(); p++)
            {
                if(str[i+p] != pattern[p])
                {
                    p = 0;
                    break;
                }
            }
            if(p != 0)
            {
                std::string tmps = "";
                tmps = substring(str, i + pattern.size());
                str = substring(str, 0, i);
                str.append(replacement);
                str.append(tmps);
            }
        }
    }

	static std::vector<std::string> split(std::string& str, std::string sepperator, bool keepInternalStrings = false)
	{
		std::vector<std::string> parts;
		unsigned int a = 0;
		unsigned int s;
		for (unsigned int i = 0; i < str.length(); i++)
		{
            if(keepInternalStrings && str[i] == '"')
            {
                i++;
                while(i < str.length() && str[i] != '"')
                {
                    i++;
                }
            }
			for (s = 0; s < sepperator.length() && (i + s) < str.length(); s++)
			{
				if (str.at(i + s) != sepperator.at(s))
				{
					break;
				}
				else if (s == sepperator.length() - 1)
				{
					if (a < i)
					{
						parts.push_back(std::string(str.data()+a,i-a));
					}
					a = i + s + 1;
				}
			}
		}
		if (a < str.length())
		{
			parts.push_back(std::string(str.substr(a, str.length())));
		}
		return parts;
	}





};

