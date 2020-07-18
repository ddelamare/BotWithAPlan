#pragma once

#include <iostream>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <filesystem>
namespace fs = std::filesystem;
using namespace std;
namespace Util {

	template <class Container>
	void split(const std::string& str, Container& cont,
		const std::string& delims = " ")
	{
		std::size_t current, previous = 0;
		current = str.find_first_of(delims);
		while (current != std::string::npos) {
			cont.push_back(str.substr(previous, current - previous));
			previous = current + 1;
			current = str.find_first_of(delims, previous);
		}
		cont.push_back(str.substr(previous, current - previous));
	}

	static map<string, string> GetConfigValues(string fileName)
	{
		map<string, string> configValues;
		fstream configFile;
		configFile.open(fileName, ios::in); //open a file to perform read operation using file object
		if (configFile.is_open())
		{   //checking whether the file is open
			string tp;
			while (getline(configFile, tp))  //read data from file object and put it into string.
			{
				std::vector<string> parts;
				split(tp, parts, "=");
				if (parts.size() == 2)
				{
					configValues[parts.front()] = parts.back();
				}
			}
			configFile.close(); //close the file object.
		}

		return configValues;
	}

	static string GetValueOrDefault(map<string, string> config, string key, string defaultVal)
	{
		string val = config[key];
		if (!val.empty()) return val;
		return defaultVal;
	}
}