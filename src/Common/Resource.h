#pragma once
#include <iostream>
#include <string>
#include <map>
#include "sc2api\sc2_api.h"
using namespace std;
#define LOGLEVEL 0 
#define LOG(level) if (level>LOGLEVEL){} else cout 

// Due to bugs in the sc2 ai library, value overrides are here
#define CHRONO_OVERRIDE 3755

struct Resource
{
	Resource() {}
	Resource(sc2::UNIT_TYPEID t, string n, float c)
	{
		type = t;
		name = n;
		costModifier = c;
	}
	sc2::UNIT_TYPEID type;
	string name;
	float costModifier;
};


typedef map<sc2::UNIT_TYPEID, int>	ResourceMap;
extern map<sc2::UNIT_TYPEID, Resource> RESOURCE_MAP;


void InitResources();

