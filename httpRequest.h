#include "StringUtils.h"

class httpRequestAttribute
{
public:

	httpRequestAttribute(std::string& key, std::string& value)
	{
		this->key = std::string(key);
		this->value = std::string(value);
	}

	std::string key;
	std::string value;
};

class httpRequestParameter
{
public:

	httpRequestParameter(std::string& key, std::string& value)
	{
		this->key = std::string(key);
		this->value = std::string(value);
	}

	std::string key;
	std::string value;
};

class httpRequest
{

	std::vector<httpRequestAttribute> attributes;
	std::vector<httpRequestParameter> parameters;

public:

	std::string method;
	std::string path;
	std::string version;

	std::string getAttributeValueOfKey(std::string& key)
	{
		for (int i = 0; i < attributes.size(); i++)
		{
			if (attributes.at(i).key == key)
			{
				return attributes.at(i).value;
			}
		}
		return NULL;
	}

	httpRequest()
	{

	}

	httpRequest(std::string& rawRequest)
	{
		loadOfRaw(rawRequest);
	}

	void loadParametersOfRaw(std::string& rawParameters)
	{
		if (rawParameters.find('&'))
		{
			std::vector<std::string> params = StringUtils::split(rawParameters, std::string("&"));
			for (int i = 0; i < params.size(); i++)
			{
				std::vector<std::string> pair = StringUtils::split(params.at(i), std::string("="));
				if (pair.size() == 2)
				{
					parameters.push_back(httpRequestParameter(pair.at(0), pair.at(1)));
				}
			}
		}
		else
		{
			std::vector<std::string> pair = StringUtils::split(rawParameters, std::string("="));
			if (pair.size() == 2)
			{
				parameters.push_back(httpRequestParameter(pair.at(0), pair.at(1)));
			}
		}
	}

	void loadOfRaw(std::string& rawRequest)
	{
		std::vector<std::string> lines = StringUtils::split(rawRequest, std::string("\r\n"));
		for (int i = 0; i < lines.size(); i++)
		{
			std::vector<std::string> pairs = StringUtils::split(lines.at(i), std::string(": "));
			if (pairs.size() == 2)
			{
				attributes.push_back(httpRequestAttribute(pairs.at(0), pairs.at(1)));
			}
			else if(pairs.size() == 1) //method path version
			{
				std::vector<std::string> parts = StringUtils::split(lines.at(i), std::string(" "));
				if (parts.size() == 3)
				{
					method = parts.at(0);
					std::vector<std::string> pathAndParameters = StringUtils::split(parts.at(1), std::string("?"));

					if (pathAndParameters.size() == 2)
					{
						path = pathAndParameters.at(0);
						loadParametersOfRaw(pathAndParameters.at(1));
					}
					else
					{
						if (parts.at(1).find('&') == std::string::npos)
						{
							path = parts.at(1);
						}
						else
						{
							path = StringUtils::split(parts.at(1), std::string("&")).at(0);
							std::string tmp = parts.at(1).substr(path.size(), parts.at(1).size());
							loadParametersOfRaw(tmp);
						}
					}
					version = parts.at(2);
				}
			}
		}

		int dbg = 0;

	}



};

