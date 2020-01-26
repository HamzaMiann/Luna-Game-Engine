
#include "cLayoutBuilder.h"
#include "cGameObject.h"
#include <Lua/cLuaBrain.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <Components/ComponentFactory.h>
using namespace rapidxml;

std::string trim(std::string& str)
{
	size_t first = str.find_first_not_of(' ');
	size_t last = str.find_last_not_of(' ');
	return str.substr(first, (last - first + 1));
}

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
	printf("Building Game Objects...\n");

	for (xml_node<>* object_node = node->first_node(); object_node; object_node = object_node->next_sibling())
	{
		cGameObject* ptr = nullptr;
		std::string objectName = object_node->name();
		xml_attribute<>* meshAttr = object_node->first_attribute("mesh");
		xml_attribute<>* tagAttr = object_node->first_attribute("tag");
		std::string shader = object_node->first_attribute("shader")->value();
		if (objectName == "GameObject")
		{
			ptr = new cGameObject();
			//cRigidBody* body = ptr->AddComponent<cRigidBody>();

			if (meshAttr) ptr->meshName = meshAttr->value();
			if (tagAttr) ptr->tag = tagAttr->value();
			ptr->shaderName = shader;
			int texture_id = 0;
			for (xml_node<>* property_node = object_node->first_node();
				 property_node;
				 property_node = property_node->next_sibling())
			{
				std::string propName = property_node->name();
				if (propName == "Position")
				{
					setXYZ(ptr->transform.pos, property_node);
				}
				else if (propName == "Rotation")
				{
					glm::vec3 orientation(0.f);
					setXYZ(orientation, property_node);
					ptr->transform.SetEulerRotation(orientation);
				}
				else if (propName == "Colour")
				{
					setXYZW(ptr->colour, property_node);
				}
				/*else if (propName == "Velocity")
				{
					glm::vec3 vel = body->GetVelocity();
					setXYZ(vel, property_node);
					body->SetVelocity(vel);
				}
				else if (propName == "Acceleration")
				{
					glm::vec3 accel = body->GetAcceleration();
					setXYZ(accel, property_node);
					body->SetAcceleration(accel);
				}*/
				else if (propName == "cRigidBody")
				{
					iComponent* comp = ComponentFactory::GetComponent(propName, ptr);
					comp->deserialize(property_node);
				}
				else if (propName == "Scale")
				{
					ptr->transform.scale = glm::vec3(strtof(property_node->value(),0));
				}
				/*else if (propName == "IMass")
				{
					body->inverseMass = strtof(property_node->value(), 0);
				}*/
				else if (propName == "SpecIntensity")
				{
					ptr->specIntensity = strtof(property_node->value(), 0);
				}
				else if (propName == "SpecColour")
				{
					setXYZ(ptr->specColour, property_node);
				}
				else if (propName == "LuaUpdate")
				{
					std::string script = std::string(property_node->value());
					script = trim(script);
					ptr->brain->LoadScript(std::string(property_node->first_attribute("name")->value()),
										   script);
				}
				else if (propName == "LuaInit")
				{
					ptr->lua_script = std::string(property_node->value());
					ptr->lua_script = trim(ptr->lua_script);
				}
				else if (propName == "CollidePoint")
				{
					glm::vec3 point(0.f);
					setXYZ(point, property_node);
					ptr->CollidePoints.push_back(point);
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
					else if (collider == "AABB")
					{
						ptr->Collider = AABB;
					}
					else if (collider == "POINT")
					{
						ptr->Collider = POINT;
					}
					else
					{
						ptr->Collider = NONE;
					}
				}
				else if (propName == "Texture")
				{
					ptr->textureRatio[texture_id] = strtof(property_node->first_attribute("ratio")->value(), 0);
					ptr->texture[texture_id++] = property_node->value();
				}
			}
		}

		if (ptr)
			scene.vecGameObjects.push_back(ptr);
	}
}
