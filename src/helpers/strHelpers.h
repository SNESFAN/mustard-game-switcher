#pragma once

#include <cstdlib>
#include <string>

inline bool strEndsWith(std::string str, std::string suffix)
{
    if (str.length() < suffix.length())
    {
        return false;
    }
    return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}

inline bool strStartsWith(std::string str, std::string prefix)
{
    if (str.length() < prefix.length())
    {
        return false;
    }
    return str.compare(0, prefix.length(), prefix) == 0;
}

inline std::string strToUpper(const std::string &str)
{
    std::string result(str); // Create a copy of str
    for (char &c : result)
    {
        c = std::toupper(c);
    }
    return result;
}

inline std::string strToLower(const std::string &str)
{
    std::string result(str); // Create a copy of str
    for (char &c : result)
    {
        c = std::tolower(c);
    }
    return result;
}
inline std::string strReplaceAll(const std::string &str, const std::string &from, const std::string &to)
{
    std::string result = str;
    size_t startPos = 0;
    while ((startPos = result.find(from, startPos)) != std::string::npos)
    {
        result.replace(startPos, from.length(), to);
        startPos += to.length();
    }
    return result;
}

inline std::vector<std::string> strSplit(const std::string &str, char delimiter)
{
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;

    while (std::getline(ss, token, delimiter))
    {
        tokens.push_back(token);
    }

    return tokens;
}

inline std::string strJoin(const std::vector<std::string> &strings, std::string delimiter)
{
    std::string result;
    for (size_t i = 0; i < strings.size(); ++i)
    {
        if (i > 0)
        {
            result += delimiter;
        }
        result += strings[i];
    }
    return result;
}

inline std::string strLeftPadInt(int num, size_t width, char padChar = '0')
{
    std::stringstream ss;
    ss << num;
    std::string str = ss.str();

    if (str.length() < width)
    {
        str = std::string(width - str.length(), padChar) + str;
    }

    return str;
}
