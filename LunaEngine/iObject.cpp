#include <iObject.h>
#include <Components/ComponentFactory.h>

iComponent* iObject::AddComponent(iComponent* component)
{
	for (iComponent* c : _components)
	{
		if (component == c) return c;
	}
	_components.push_back(component);
	return component;
}

bool iObject::serialize(rapidxml::xml_node<>* root_node)
{
	// Get object name
	const char* node_name = ObjectName().c_str();
	// node for the object
	rapidxml::xml_node<>* node = new rapidxml::xml_node<>(rapidxml::node_type::node_element);
	node->name(node_name);
	// node that hosts all the components
	rapidxml::xml_node<>* component_node = new rapidxml::xml_node<>(rapidxml::node_type::node_element);
	component_node->name("Components");

	// serialize each component
	for (iComponent* c : _components)
	{
		c->serialize(component_node);
	}

	// add the nodes to the root
	node->append_node(component_node);
	root_node->append_node(node);
	return true;
}

bool iObject::deserialize(rapidxml::xml_node<>* root_node)
{
	auto component_node = root_node->first_node("Components");
	for (rapidxml::xml_node<>* node = component_node->first_node();
		 node; node = node->next_sibling())
	{
		std::string name = node->name();
		iComponent* comp = ComponentFactory::GetComponent(name, this);
		if (comp)
			comp->deserialize(node);
	}
	return true;
}
