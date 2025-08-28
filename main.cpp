#ifdef MYGAMEORGAMEENGINE
#define MYGAMEORGAMEENGINE
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <unordered_map>
class Component;
std::unordered_map<int, std::unordered_map<int, Component>> components;
static int componentIndex;
static int entity;
static int CreateComponent()
{
	return componentIndex++;
}
static int CreateEntity()
{
	return entity++;
}


class Entity
{
public:
	int id;

	template <typename T>
	static T* GetComponent(int entity)
	{
		for (auto& component : components)
		{
			if (component.second.find(entity) != component.second.end())
			{
				return component.second.find(entity)->second;
			}
		}
		return nullptr;
	}

	template <typename Component>
	static void AddComponent(Component component)
	{
		for (auto& it : components)
		{
			auto compId = it.first;
			auto comps = it.second;
			if (component.find(compId) == component.second.end())
			{
				components[CreateComponent()] = std::make_pair("Hello", 'A');;
			}

		}
	}
};
class Component
{
public:
	struct Position
	{
		double x, y;
	};
};



int main()
{
	Entity::AddComponent<Component::Position>({ 100, 100 });
	return 0;
}










#endif