 #include <iostream>  
#include <SDL3/SDL.h> 
#include <SDL3_image/SDL_image.h>
#include <vector>
#include <unordered_map>
#include <SDL3/SDL_render.h>
#include <chrono>
#include <vector>
#include <utility>
#include <algorithm>

class Input
{
	
public:
	static std::unordered_map<SDL_Scancode, bool> keysPressed;
	static std::unordered_map<SDL_Scancode, bool> keysDown;
	static bool IsKeyPressed(SDL_Scancode key)
	{
		return keysPressed.find(key) != keysPressed.end() && keysPressed.find(key)->second;
	}
	static bool IsKeyDown(SDL_Scancode key)
	{
		return keysDown.find(key) != keysDown.end() && keysDown.find(key)->second;
	}
};
std::unordered_map<SDL_Scancode, bool> Input::keysPressed;
std::unordered_map<SDL_Scancode, bool> Input::keysDown;

class TextureManager
{
public:
	static SDL_Surface* LoadSurface(std::string path, std::string mode = "r")
	{
		return IMG_LoadPNG_IO(SDL_IOFromFile(path.c_str(), mode.c_str()));
	}

	static SDL_Texture* Load(SDL_Renderer* renderer, std::string path)
	{
		return IMG_LoadTexture(renderer, path.c_str());
	}



	static void Draw(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y, SDL_FRect* srcRect = nullptr)
	{
		float textureWidth, textureHeight;
		SDL_GetTextureSize(texture, &textureWidth, &textureHeight);
		SDL_FRect dst = { (float)floor(x), (float)floor(y), srcRect ? srcRect->w : (float)textureWidth, srcRect ? srcRect->h : (float)textureHeight };
		SDL_RenderTexture(renderer, texture, srcRect, &dst);
	}
};

class Window
// REPLACE WITH OPENGL OR VULKAN WHEN POSSIBLE SO I CAN USE SHADERS
{	
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;


	bool isRunning = true;
public:

	int width = 0;
	int height = 0;


	SDL_Renderer* GetRenderer()
	{
		return renderer;
	}

	SDL_Window* GetWindow()
	{
		return window;
	}


	void Draw()
	{

	}


	int GetWindowWidth()
	{
		return width;
	}
	int GetWindowHeight()
	{
		return height;
	}


	void CreateWindow(std::string title, int width, int height)
	{
		this->width = width;
		this->height = height;
		SDL_CreateWindowAndRenderer(title.c_str(), width, height, SDL_WINDOW_RESIZABLE, &window, &renderer);
		SDL_SetWindowIcon(window, TextureManager::LoadSurface("assets/textures/icon.png"));
	}


	void Quit()
	{
		isRunning = false;
	}
};


std::string printBool(bool value)
{
	if (value)
	{
		return "true";
	}
	else
	{
		return "false";
	}
}


class ECS
{
public:
	
	class Component
	{
	public:
		struct Position
		{
			double x;
			double y;
		};
		struct Velocity
		{
			double x; 
			double y;
		};
		struct Sprite
		{
			SDL_Texture* texture;
			int z = 0;
		};
		struct Collider
		{
			SDL_FRect rect;
			bool colliding = false;
			bool isStatic = false;
		};

		// never use this (for now)
		struct ColliderCircle
		{
			float radius;
			bool colliding = false;
		};
		struct Animation
		{
			int frame;
			float elapsed;
			float frameTime = 100.0f; // 100 ms
			bool looped = false;
			std::string state = "";
			float speed = 1.0f;
			int row = 0;
			int maxFrames = 1;
			
		};
	};
	class System
	{
	public:
		static void Move(std::unordered_map<int, Component::Velocity>& velocities, std::unordered_map<int, Component::Position>& positions, double elapsed)
		{
			for (auto& i : positions)
			{
				int id = i.first;
				if (velocities.find(id) != velocities.end())
				{
					auto& pos = positions[id];
					auto& vel = velocities[id];
					pos.x += vel.x * elapsed;
					pos.y += vel.y * elapsed;
				}
			}
		}
		static void Draw(SDL_Renderer* renderer, std::unordered_map<int, Component::Sprite>& sprites, std::unordered_map<int, Component::Animation>& animations, std::unordered_map<int, Component::Position>& positions, int camX = 0, int camY = 0, int camW = 0, int camH = 0)
		{
			std::vector<std::pair<int, Component::Sprite>> sortedSprites;
			for (auto& i : sprites)
			{
				std::pair<int, Component::Sprite> sprite = i;
				sortedSprites.push_back(sprite);
			}

			std::sort(sortedSprites.begin(), sortedSprites.end(), [](const std::pair<int, Component::Sprite>& a, const std::pair<int, Component::Sprite>& b) {return a.second.z < b.second.z;});
			
			
			for (auto& sprite : sortedSprites)
			{
				int x = positions[sprite.first].x;
				int y = positions[sprite.first].y;
				float w;
				float h;
				SDL_GetTextureSize(sprite.second.texture, &w, &h);
				if (
					(x + w < camX) ||
					(y + h < camY) ||
					(x > camX + camW) ||
					(y > camY + camH)
					)
				{
					continue;
				}

				auto animation = animations.find(sprite.first);
				SDL_FRect srcRect;
				if (animation != animations.end())
				{
					auto& anim = animation->second;

					float frameWidth = w / anim.maxFrames;
					float frameHeight = h / 8;

					srcRect = { frameWidth * anim.frame,frameHeight * anim.row, frameWidth, frameHeight };
				
				}

				else
				{
					srcRect = { 0, 0, w, h };
				}
				TextureManager::Draw(renderer, sprite.second.texture, positions[sprite.first].x - camX, positions[sprite.first].y - camY, &srcRect);
				
			}
		}


		static void Fall(std::unordered_map<int, Component::Velocity> )
		{

		}

		static void UpdateAnimationFrames(std::unordered_map<int, Component::Animation>& animations, double elapsed)
		{
			for (auto& i : animations)
			{
				i.second.elapsed += elapsed * 1000.0 * i.second.speed;
				if (i.second.elapsed >= i.second.frameTime)
				{
					i.second.elapsed -= i.second.frameTime;
					i.second.frame++;
					if (i.second.looped)
					{
						i.second.frame %= i.second.maxFrames;
					}
					else
					{
						i.second.frame = std::min(i.second.frame, i.second.maxFrames - 1);
					}
				}
			}
		}

		static void PlayAnimation(std::string state)
		{

		}

		static void Collide(std::unordered_map<int, Component::Collider>& colliders)
		{
			for (auto i = colliders.begin(); i != colliders.end(); ++i)
			{
				if (!i->second.isStatic)
				for (auto j = std::next(i); j != colliders.end(); ++j)
				{
					float dlx = (i->second.rect.x + i->second.rect.w) - j->second.rect.x;
					float drx = (j->second.rect.x + j->second.rect.w) - i->second.rect.x;
					float overlapX = std::min(dlx, drx);

					float duy = (i->second.rect.y + i->second.rect.h) - j->second.rect.y;
					float ddy = (j->second.rect.y + j->second.rect.h) - i->second.rect.y;
					float overlapY = std::min(duy, ddy);



					if (overlapX < overlapY)
					{
						if (!i->second.isStatic && j->second.isStatic)
						{
							i->second.rect.x += (i->second.rect.x < j->second.rect.x ? -overlapX : overlapX);
						}
						else if (i->second.isStatic && !j->second.isStatic)
						{
							j->second.rect.x += (j->second.rect.x < i->second.rect.x ? -overlapX : overlapX);
						}
						else if (!i->second.isStatic && !j->second.isStatic)
						{
							if (i->second.rect.x < j->second.rect.x)
							{
								i->second.rect.x -= overlapX / 2;
								j->second.rect.x += overlapX / 2;
							}
							else
							{
								i->second.rect.x += overlapX / 2;
								j->second.rect.x -= overlapX / 2;
							}
						}
					}
					else
					{
						if (!i->second.isStatic && j->second.isStatic)
						{
							i->second.rect.y += (i->second.rect.y < j->second.rect.y ? -overlapY : overlapY);
						}
						else if (i->second.isStatic && !j->second.isStatic)
						{
							j->second.rect.y += (j->second.rect.y < i->second.rect.y ? -overlapY : overlapY);
						}
						else if (!i->second.isStatic && !j->second.isStatic)
						{
							if (i->second.rect.y < j->second.rect.y)
							{
								i->second.rect.y -= overlapY / 2;
								j->second.rect.y += overlapY / 2;
							}
							else
							{
								i->second.rect.y += overlapY / 2;
								j->second.rect.y -= overlapY / 2;
							}
						}
					}

					if (SDL_HasRectIntersectionFloat(&i->second.rect, &j->second.rect))
					{
						i->second.colliding = true;
						j->second.colliding = true;
					}
					else
					{
						i->second.colliding = false;
						j->second.colliding = false;
					}
				}
				
			}
			
		}
	};
	class Entity
	{
		
		static int nextEntity;
	public:
		static std::vector<int> entities;
		static std::unordered_map<int, Component::Velocity> velocities;
		static std::unordered_map<int, Component::Position> positions;
		static std::unordered_map<int, Component::Sprite> sprites;
		static std::unordered_map<int, Component::Collider> colliders;
		static std::unordered_map<int, Component::Animation> animations;
		
	
		template<typename T>
		static void AddComponent(int entity, T component, std::unordered_map<int, T>& componentMap)
		{
			componentMap[entity] = component;
		};
		template<typename T>
		static T* GetComponent(int entity, std::unordered_map<int, T>& componentMap)
		{

			if (componentMap.find(entity) != componentMap.end())
			{
				return &componentMap.find(entity)->second;
			}


			return nullptr;
		}
		static void Destroy(int entity)
		{
			positions.erase(entity);
			sprites.erase(entity);
			velocities.erase(entity);
		}
		static int CreateEntity()
		{
			int id = nextEntity++;
			entities.push_back(id);
			return id;
		}
		
	};
	
	class Game
	{
	public:
		int camera = Entity::CreateEntity();
		int player = Entity::CreateEntity();
		int world = Entity::CreateEntity();

		

		std::vector<int> trees;
		Window* window;

		ECS::Component::Position* camera_position;
		ECS::Component::Position* player_position;
		ECS::Component::Velocity* player_velocity;
		ECS::Component::Sprite* player_sprite;
		ECS::Component::Animation* player_animation;
		void Init(SDL_Renderer* renderer)
		{
			
			for (int i = 0; i < 10000; i++)
			{
				int tree = Entity::CreateEntity();
				Entity::AddComponent(tree, Component::Sprite{ TextureManager::Load(renderer, "assets/textures/tree.png"), 1 }, Entity::sprites);
				Entity::AddComponent(tree, Component::Position{ (double)(rand() % 5420), (double)(rand() % 5420) }, Entity::positions);
				trees.push_back(tree);
			}
			
			Entity::AddComponent(camera, Component::Position{0, 0}, Entity::positions);
			
			Entity::AddComponent(player, Component::Position{ 0, 0 }, Entity::positions);
			Entity::AddComponent(player, Component::Velocity{ 0, 0 }, Entity::velocities);
			Entity::AddComponent(player, Component::Sprite{ TextureManager::Load(renderer, "assets/textures/player_spritesheet.png"), 420 }, Entity::sprites);
			Entity::AddComponent(player, Component::Animation{ 0, 0.0f, 100.0f, true, {}, 1.0f, 0, 0 }, Entity::animations);


			Entity::AddComponent(world, Component::Position{ 200, 200 }, Entity::positions);
			Entity::AddComponent(world, Component::Sprite{ TextureManager::Load(renderer, "assets/textures/world.png"), 1}, Entity::sprites);
			camera_position = Entity::GetComponent(camera, Entity::positions);
			player_position = Entity::GetComponent(player, Entity::positions);
			player_velocity = Entity::GetComponent(player, Entity::velocities);
			player_sprite = Entity::GetComponent(player, Entity::sprites);
			player_animation = Entity::GetComponent(player, Entity::animations);


			if (!player_sprite || !player_sprite->texture)
			{
				std::cerr << "ERROR NO SPRITE!!!!!!!!";
			}


		}
		void Loop(SDL_Renderer* renderer, double elapsed)
		{
			// process inputs -> process behaviors -> calculate velocities -> move entities -> resolve collisions -> render frame


			double player_speed = 200.0;

			player_velocity->x = 0.0f;
			player_velocity->y = 0.0f;
			player_animation->state = "Idle";
			if (Input::IsKeyDown(SDL_SCANCODE_W))
			{
				player_velocity->y = -player_speed;
			}
			if (Input::IsKeyDown(SDL_SCANCODE_A))
			{
				player_velocity->x = -player_speed;
			}
			if (Input::IsKeyDown(SDL_SCANCODE_S))
			{
				player_velocity->y = player_speed;
			}
			if (Input::IsKeyDown(SDL_SCANCODE_D))
			{
				player_velocity->x = player_speed;
			}


			if (Input::IsKeyDown(SDL_SCANCODE_W) || Input::IsKeyDown(SDL_SCANCODE_A) || Input::IsKeyDown(SDL_SCANCODE_S) || Input::IsKeyDown(SDL_SCANCODE_D))
			{
				player_animation->state = "Walking";
			}




			if (player_animation->state == "Idle")
			{
				//player_animation->x = player_animation->frame * player_sprite->texture->w/player_animation->maxFrames;
				player_animation->row = 0;
				player_animation->maxFrames = 8;
			}
			else if (player_animation->state == "Walking")
			{
				//player_animation->x = player_animation->frame * player_sprite->texture->w / player_animation->maxFrames;
				player_animation->row = 1;
				player_animation->maxFrames = 8;
			}
			
			float textureWidth, textureHeight;
			SDL_GetTextureSize(player_sprite->texture,  & textureWidth, & textureHeight);

			camera_position->x = (player_position->x + (textureWidth/player_animation->maxFrames) /2) - window->width/2;
			camera_position->y = (player_position->y + (textureHeight / player_animation->maxFrames) /2 ) - window->height/2;

			


		}
	};
};
int ECS::Entity::nextEntity = 0;
std::vector<int> ECS::Entity::entities = {};
std::unordered_map<int, ECS::Component::Velocity> ECS::Entity::velocities = {};
std::unordered_map<int, ECS::Component::Position> ECS::Entity::positions = {};
std::unordered_map<int, ECS::Component::Sprite> ECS::Entity::sprites = {};
std::unordered_map<int, ECS::Component::Collider> ECS::Entity::colliders = {};
std::unordered_map<int, ECS::Component::Animation> ECS::Entity::animations = {};


int main()  
{ 
	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		std::cerr << SDL_GetError();
	}


	Window window = Window();
	std::cout << "!!!!!!!hi\n";  

	window.CreateWindow("Hi!!!", 1280, 720);

	SDL_Event event;
	bool isRunning = true;

	ECS::Game game = ECS::Game();
	game.window = &window;
	game.Init(window.GetRenderer());



	double elapsed = 0.016;
	while (isRunning)
	{
		// process inputs -> process behaviors -> calculate velocities -> move entities -> resolve collisions -> render frame
		std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
		
		for (std::unordered_map<SDL_Scancode, bool>::iterator i = Input::keysPressed.begin(); i != Input::keysPressed.end(); i++)
		{
			i->second = false;
		}

		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_EVENT_QUIT:
				isRunning = false;
				break;
			case SDL_EVENT_KEY_DOWN:
				Input::keysPressed[event.key.scancode] = true;
				Input::keysDown[event.key.scancode] = true;
				break;
			case SDL_EVENT_KEY_UP:
				Input::keysDown[event.key.scancode] = false;
				break;
			}

			
		}
		

		game.Loop(window.GetRenderer(), elapsed);
		ECS::System::Move(ECS::Entity::velocities, ECS::Entity::positions, elapsed);
		ECS::System::UpdateAnimationFrames(ECS::Entity::animations, elapsed);
		ECS::System::Collide(ECS::Entity::colliders);
		SDL_RenderClear(window.GetRenderer());
		SDL_SetRenderDrawColor(window.GetRenderer(), 135, 197, 255, SDL_ALPHA_OPAQUE);
		ECS::System::Draw(window.GetRenderer(), ECS::Entity::sprites, ECS::Entity::animations, ECS::Entity::positions, game.camera_position->x, game.camera_position->y,	game.window->width, game.window->height);
		SDL_RenderPresent(window.GetRenderer());

		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
		std::chrono::duration<double> deltaTime = end - begin;
		std::cout << "fps: " << 1.0 / deltaTime.count() << "\n";
		elapsed = deltaTime.count();

		
	}

	SDL_Quit();
	return 0;  
}