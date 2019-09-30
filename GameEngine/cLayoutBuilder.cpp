
#include "cLayoutBuilder.h"
using namespace rapidxml;

void setXYZ(glm::vec3& vec3, xml_node<>* node)
{
	vec3.x = strtof(node->first_attribute("x")->value(), 0);
	vec3.y = strtof(node->first_attribute("y")->value(), 0);
	vec3.z = strtof(node->first_attribute("z")->value(), 0);
}

void setXYZW(glm::vec4& vec4, xml_node<>* node)
{
	vec4.x = strtof(node->first_attribute("x")->value(), 0);
	vec4.y = strtof(node->first_attribute("y")->value(), 0);
	vec4.z = strtof(node->first_attribute("z")->value(), 0);
	vec4.w = strtof(node->first_attribute("w")->value(), 0);
}

void cLayoutBuilder::Build(Scene& scene, rapidxml::xml_node<>* node)
{
	for (xml_node<>* object_node = node->first_node(); object_node; object_node = object_node->next_sibling())
	{
		cGameObject* ptr = nullptr;
		std::string objectName = object_node->name();
		xml_attribute<>* meshAttr = object_node->first_attribute("mesh");
		if (objectName == "GameObject")
		{
			ptr = ptr = new cGameObject();
			if (meshAttr) ptr->meshName = meshAttr->value();
			for (xml_node<>* property_node = object_node->first_node();
				 property_node;
				 property_node = property_node->next_sibling())
			{
				std::string propName = property_node->name();
				if (propName == "Position")
				{
					setXYZ(ptr->positionXYZ, property_node);
				}
				else if (propName == "Rotation")
				{
					setXYZ(ptr->rotationXYZ, property_node);
				}
				else if (propName == "Colour")
				{
					setXYZW(ptr->objectColourRGBA, property_node);
				}
				else if (propName == "Velocity")
				{
					setXYZ(ptr->velocity, property_node);
				}
				else if (propName == "Acceleration")
				{
					setXYZ(ptr->acceleration, property_node);
				}
				else if (propName == "Scale")
				{
					ptr->scale = strtof(property_node->value(),0);
				}
				else if (propName == "IMass")
				{
					ptr->inverseMass = strtof(property_node->value(), 0);
				}
				else if (propName == "Collider")
				{
					std::string collider = property_node->value();
					if (collider == "SPHERE")
					{
						ptr->Collider = SPHERE;
					}
					else if (collider == "MESH")
					{
						ptr->Collider = MESH;
					}
					else
					{
						ptr->Collider = NONE;
					}
				}
			}
		}

		if (ptr)
			scene.vecGameObjects.push_back(ptr);
	}
}