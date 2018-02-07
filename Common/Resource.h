#pragma once
#include <iostream>
#include <string>
#include <map>
using namespace std;
#define LOGLEVEL 1 
#define LOG(level) if (level>LOGLEVEL){} else cout 

// Due to bugs in the sc2 ai library, value overrides are here
#define CHRONO_OVERRIDE 3755

enum class RESOURCES {
	MINERALS = 1,
	PROBE,
	GATEWAY,
	ZEALOT,
	STALKER,
	CYBERNETICS,
	PYLON,
	ASSIMILATOR,
	GAS,
	NEXUS,
	STARGATE,
	TWILIGHT,
	FLEETBEACON,
	CARRIER,
	DARKTEMPLAR
};

struct Resource
{
	Resource() {}
	Resource(RESOURCES t, string n, float c)
	{
		type = t;
		name = n;
		costModifier = c;
	}
	RESOURCES type;
	string name;
	float costModifier;
};


typedef map<RESOURCES, int>	ResourceMap;
extern map<RESOURCES, Resource> RESOURCE_MAP;


void InitResources();

