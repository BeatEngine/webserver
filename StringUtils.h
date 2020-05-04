#include <string>
#include <vector>

class StringUtils
{
public:


	static std::vector<std::string> split(std::string& str, std::string sepperator)
	{
		std::vector<std::string> parts;
		unsigned int a = 0;
		unsigned int s;
		for (unsigned int i = 0; i < str.length(); i++)
		{
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

