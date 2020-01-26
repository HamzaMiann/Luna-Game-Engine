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
	std::string node_name = ObjectName();
	rapidxml::xml_node<>* node = new rapidxml::xml_node<>(rapidxml::node_type::node_element);
	for (iComponent* c : _components)
	{
		c->serialize(node);
	}
	root_node->append_node(node);
	return true;
}

bool iObject::deserialize(rapidxml::xml_node<>* root_node)
{
	for (rapidxml::xml_node<>* node = root_node->first_node();
		 node; node = node->next_sibling())
	{
		std::string name = node->name();
		iComponent* comp = ComponentFactory::GetComponent(name, this);
		if (comp)
			comp->deserialize(node);
	}
	return true;
}
