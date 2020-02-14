
#include "cLayoutBuilder.h"
#include "cGameObject.h"
#include <Lua/cLuaBrain.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <Components/ComponentFactory.h>
#include <Physics/global_physics.h>
#include <xml_helper.h>
#include <EntityManager/cEntityManager.h>
using namespace rapidxml;

std::string trim(std::string& str)
{
	size_t first = str.find_first_not_of(' ');
	size_t last = str.find_last_not_of(' ');
	return str.substr(first, (last - first + 1));
}

void MakeGO(xml_node<>* object_node, cGameObject* ptr);


void cLayoutBuilder::Build(Scene& scene, rapidxml::xml_node<>* node)
{
	printf("Building Game Objects...\n");

	for (xml_node<>* object_node = node->first_node(); object_node; object_node = object_node->next_sibling())
	{
		std::string objectName = object_node->name();

		if (objectName == "GameObject")
		{
			cGameObject* ptr = new cGameObject();
			MakeGO(object_node, ptr);

			if (ptr)
				cEntityManager::Instance()->AddEntity(ptr);

			continue;
		}

	}
}

void MakeGO(xml_node<>* object_node, cGameObject* ptr)
{
	xml_attribute<>* meshAttr = object_node->first_attribute("mesh");
	xml_attribute<>* tagAttr = object_node->first_attribute("tag");
	std::string shader = object_node->first_attribute("shader")->value();

	if (meshAttr) ptr->meshName = meshAttr->value();
	if (tagAttr) ptr->tag = tagAttr->value();
	ptr->shader = Shader::FromName(shader);
	int texture_id = 0;
	for (xml_node<>* property_node = object_node->first_node();
		property_node;
		property_node = property_node->next_sibling())
	{
		std::string propName = property_node->name();
		if (propName == "Position")
		{
			ptr->transform.pos = XML_Helper::AsVec3(property_node);
		}
		else if (propName == "Rotation")
		{
			glm::vec3 orientation = XML_Helper::AsVec3(property_node);
			ptr->transform.SetEulerRotation(orientation);
		}
		else if (propName == "Colour")
		{
			ptr->colour = XML_Helper::AsVec4(property_node);
		}
		else if (propName == "Components")
		{
			for (xml_node<>* component_node = property_node->first_node(); component_node; component_node = component_node->next_sibling())
			{
				std::string n = component_node->name();
				if (n == "SphereBody")
				{
					nPhysics::sSphereDef def;
					def.velocity = XML_Helper::AsVec3(component_node->first_node("Velocity"));
					def.mass = XML_Helper::AsFloat(component_node->first_node("Mass"));
					def.gravity_factor = XML_Helper::AsFloat(component_node->first_node("GFactor"));
					def.Offset = XML_Helper::AsVec3(component_node->first_node("Offset"));
					def.Radius = XML_Helper::AsFloat(component_node->first_node("Radius"));
					
					nPhysics::iPhysicsComponent* component = g_PhysicsFactory->CreateSphere(ptr, def);
					component->setIsRotateable(false);
					if (component_node->first_node("Rotateable"))
					{
						component->setIsRotateable(true);
					}

					ptr->AddComponent(component);
				}
				else if (n == "PlaneBody")
				{
					nPhysics::sPlaneDef def;
					def.velocity = XML_Helper::AsVec3(component_node->first_node("Velocity"));
					def.mass = XML_Helper::AsFloat(component_node->first_node("Mass"));
					def.gravity_factor = XML_Helper::AsFloat(component_node->first_node("GFactor"));
					def.Normal = XML_Helper::AsVec3(component_node->first_node("Normal"));
					def.Normal = glm::normalize(def.Normal);
					def.Constant = XML_Helper::AsFloat(component_node->first_node("Constant"));
					ptr->AddComponent(g_PhysicsFactory->CreatePlane(ptr, def));
				}
				else if (n == "CubeBody")
				{
					nPhysics::sCubeDef def;
					def.velocity = XML_Helper::AsVec3(component_node->first_node("Velocity"));
					def.mass = XML_Helper::AsFloat(component_node->first_node("Mass"));
					def.gravity_factor = XML_Helper::AsFloat(component_node->first_node("GFactor"));
					def.Scale = XML_Helper::AsVec3(component_node->first_node("Scale"));
					def.Origin = XML_Helper::AsVec3(component_node->first_node("Origin"));

					nPhysics::iPhysicsComponent* component = g_PhysicsFactory->CreateCube(ptr, def);
					component->setIsRotateable(false);
					if (component_node->first_node("Rotateable"))
					{
						component->setIsRotateable(true);
					}

					ptr->AddComponent(component);
				}
				else
				{
					iComponent* comp = ComponentFactory::GetComponent(n, ptr);
					if (comp) comp->deserialize(component_node);
				}
			}
		}
		else if (propName == "Scale")
		{
			ptr->transform.scale = glm::vec3(XML_Helper::AsFloat(property_node));
		}
		else if (propName == "SpecIntensity")
		{
			ptr->specIntensity = XML_Helper::AsFloat(property_node);
		}
		else if (propName == "SpecColour")
		{
			ptr->specColour = XML_Helper::AsVec3(property_node);
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
			glm::vec3 point = XML_Helper::AsVec3(property_node);
			ptr->CollidePoints.push_back(point);
		}
		else if (propName == "Texture")
		{
			ptr->texture[texture_id].SetBlend(strtof(property_node->first_attribute("ratio")->value(), 0));
			ptr->texture[texture_id++].SetTexture(property_node->value());
		}
		else if (propName == "GameObject")
		{
			cGameObject* newPtr = new cGameObject;
			MakeGO(property_node, newPtr);
			newPtr->parent = ptr;
			ptr->children.push_back(newPtr);
		}
	}
}