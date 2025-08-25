 #include <iostream>  
#include <SDL3/SDL.h> 
#include <SDL3_image/SDL_image.h>
#include <vector>
#include <map>
#include <SDL3/SDL_render.h>
#include <chrono>
class Object
{
public:
	void Update()
	{
		
	}
	void Draw()
	{
		
	}
};
class Collider
{
	int x;
	int y;
	int w;
	int h;
};
class Keys
{
	enum KeyboardKeys
	{
		Left = SDLK_LEFT,
	};
};
class Input
{
	
public:
	static std::map<SDL_Scancode, bool> keysPressed;
	static std::map<SDL_Scancode, bool> keysDown;
	static bool IsKeyPressed(SDL_Scancode key)
	{
		return keysPressed.find(key) != keysPressed.end() && keysPressed.find(key)->second;
	}
	static bool IsKeyDown(SDL_Scancode key)
	{
		return keysDown.find(key) != keysDown.end() && keysDown.find(key)->second;
	}
};
std::map<SDL_Scancode, bool> Input::keysPressed;
std::map<SDL_Scancode, bool> Input::keysDown;

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
		SDL_FRect* dst = new SDL_FRect{ (float)x, (float)y, (float)texture->w, (float)texture->h };
		SDL_RenderTexture(renderer, texture, srcRect, dst);
	}
};

class Window
// REPLACE WITH OPENGL OR VULKAN WHEN POSSIBLE SO I CAN USE SHADERS
{	
	SDL_Window* window;
	SDL_Renderer* renderer;


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




int main()  
{ 
	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		std::cerr << SDL_GetError();
		std::cout << "SDL Video NOT!!!!!!!!!!!!!!!!!!!!!!!!  initialized!\n";
	}
	else
	{
		std::cout << "SDL Video initialized!\n";
	}

	Window window = Window();
	std::cout << "!!!!!!!hi\n";  

	window.CreateWindow("Hi!!!", 1280, 720);

	auto camera = new SDL_FRect{ 0, 0, (float)window.width, (float)window.height};
	auto player = new SDL_FRect{ 0, 0, 16, 16 };
	auto icon = TextureManager::Load(window.GetRenderer(), "assets/textures/icon.png");
	auto player_texture = TextureManager::Load(window.GetRenderer(), "assets/textures/player.png");
	camera->x = player->x / camera->w;
	camera->y = player->y / camera->h;
	SDL_Event event;
	bool isRunning = true;

	double elapsed = 0.016;
	while (isRunning)
	{
		//std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
		
		for (std::map<SDL_Scancode, bool>::iterator i = Input::keysPressed.begin(); i != Input::keysPressed.end(); i++)
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

		if (Input::IsKeyDown(SDL_SCANCODE_W)) 
		{
			player->y -= 1 * elapsed;
		}
		if (Input::IsKeyDown(SDL_SCANCODE_A))
		{
			player->x -= 1 * elapsed;
		}
		if (Input::IsKeyDown(SDL_SCANCODE_S))
		{
			player->y += 1 * elapsed;
		}
		if (Input::IsKeyDown(SDL_SCANCODE_D))
		{
			player->x += 1 * elapsed	;
		}


		camera->x = player->x - camera->w/2;
		camera->y = player->y - camera->h/2;
		SDL_RenderClear(window.GetRenderer());




		TextureManager::Draw(window.GetRenderer(), icon, 250 - camera->x, 200 - camera->y);
		TextureManager::Draw(window.GetRenderer(), player_texture, player->x - camera->x, player->y - camera->y);



		SDL_RenderPresent(window.GetRenderer());
		//std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
		//elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
	}

	SDL_Quit();
	return 0;  
}