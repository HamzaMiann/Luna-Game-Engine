#pragma once

#include "cSpaceObject.h"

class cSpaceFactory
{
public:

	cSpaceObject* make_object(std::string type);

	bool build_details(cSpaceObject* object, std::string type);

};