#ifdef MYGAMEORGAMEENGINE
#define MYGAMEORGAMEENGINE

#include <iostream>

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
struct Component {};

struct Position : public Component
{
	double x, y;
};

template <typename T>
T* GetComponent(int entity)
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

template <typename T>
void AddComponent(int entity, T component)
{

	for (auto& it : components)
	{
		auto compId = it.first;
		auto comps = it.second;
	
		components[CreateComponent()][entity] = component;

	}
}

int main()
{
	Position pos;
	pos.x = 100;
	pos.y = 69;
	int player = CreateEntity();
	AddComponent<Position>(player, pos);
	std::cout << GetComponent<Position>(player) << "\n";
	return 0;
}










#endif