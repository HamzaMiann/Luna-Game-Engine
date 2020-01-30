
#include "cLayoutBuilder.h"
#include "cGameObject.h"
#include <Lua/cLuaBrain.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <Components/ComponentFactory.h>
#include <Physics/global_physics.h>
#include <xml_helper.h>
using namespace rapidxml;

std::string trim(std::string& str)
{
	size_t first = str.find_first_not_of(' ');
	size_t last = str.find_last_not_of(' ');
	return str.substr(first, (last - first + 1));
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
					ptr->transform.pos = XML_Helper::AsVec3(property_node);
					//setXYZ(ptr->transform.pos, property_node);
				}
				else if (propName == "Rotation")
				{
					glm::vec3 orientation = XML_Helper::AsVec3(property_node);
					//setXYZ(orientation, property_node);
					ptr->transform.SetEulerRotation(orientation);
				}
				else if (propName == "Colour")
				{
					ptr->colour = XML_Helper::AsVec4(property_node);
					//setXYZW(ptr->colour, property_node);
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
							ptr->physics_body = g_PhysicsFactory->CreateSphere(ptr, def);
							ptr->AddComponent(ptr->physics_body);
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
							ptr->physics_body = g_PhysicsFactory->CreatePlane(ptr, def);
							ptr->AddComponent(ptr->physics_body);
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
					//ptr->transform.scale = glm::vec3(strtof(property_node->value(),0));
				}
				/*else if (propName == "IMass")
				{
					body->inverseMass = strtof(property_node->value(), 0);
				}*/
				else if (propName == "SpecIntensity")
				{
					ptr->specIntensity = XML_Helper::AsFloat(property_node);
					//ptr->specIntensity = strtof(property_node->value(), 0);
				}
				else if (propName == "SpecColour")
				{
					ptr->specColour = XML_Helper::AsVec3(property_node);
					//setXYZ(ptr->specColour, property_node);
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
					//setXYZ(point, property_node);
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
