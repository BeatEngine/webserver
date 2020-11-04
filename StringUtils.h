#include <string>
#include <vector>

#define STRING_UTILS_USED 1

class StringMap
{
    std::vector<std::string> keys;
    std::vector<std::string> values;

    public:

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
                values.erase(keys.begin()+i);
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

    std::string operator [](long index)
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

