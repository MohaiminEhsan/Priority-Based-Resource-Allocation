#include <iostream>
#include <stdlib.h>
#include <string>
#include <map>
using namespace std;
#include "veins/base/utils/Coord.h"

// VID, SimTime, RSU
extern std::vector<std::tuple<int, int, int>> vehicleLocation;

extern std::map<int, std::tuple<int, int>> vehicleResource;
