#include <iostream>  
#include <stdio.h>

#include <SDL3/SDL.h> 
#include <SDL3/SDL_render.h>
#include <SDL3_image/SDL_image.h>
//#include <SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <vector>
#include <unordered_map>
#include <typeindex>

#include <chrono>
#include <utility>
#include <algorithm>

#ifndef MYGAMEORGAMEENGINE
#define MYGAMEORGAMEENGINE
static TTF_Font* font;
class Input
{
	
public:
	static std::unordered_map<SDL_Scancode, bool> keysPressed;
	static std::unordered_map<SDL_Scancode, bool> keysDown;
	static std::unordered_map<SDL_MouseButtonFlags, bool> mouseDown;
	static std::unordered_map<SDL_MouseButtonFlags, bool> mousePressed;
	static int mouseX;
	static int mouseY;
	static bool IsKeyPressed(SDL_Scancode key)
	{
		return keysPressed.find(key) != keysPressed.end() && keysPressed.find(key)->second;
	}
	static bool IsKeyDown(SDL_Scancode key)
	{
		return keysDown.find(key) != keysDown.end() && keysDown.find(key)->second;
	}


	static int GetMousePosX()
	{
		return 0;
	}
	static int GetMousePosY()
	{
		return 0;
	}
	static bool IsLeftKeyPressed()
	{
		return false;
	}

	static bool IsMousePressed(SDL_MouseButtonFlags flags)
	{
		return mousePressed.find(flags) != mousePressed.end() && mousePressed.find(flags)->second;
	}
	static bool IsMouseDown(SDL_MouseButtonFlags flags)
	{
		return mouseDown.find(flags) != mouseDown.end() && mouseDown.find(flags)->second;
	}
};
std::unordered_map<SDL_Scancode, bool> Input::keysPressed;
std::unordered_map<SDL_Scancode, bool> Input::keysDown;
std::unordered_map<SDL_MouseButtonFlags, bool> Input::mousePressed;
std::unordered_map<SDL_MouseButtonFlags, bool> Input::mouseDown;


class Window
	// REPLACE WITH OPENGL OR VULKAN WHEN POSSIBLE SO I CAN USE SHADERS
{
	static SDL_Window* window;
	static SDL_Renderer* renderer;


	static bool isRunning;
public:

	static int width;
	static int height;

	static void ForceQuit()
	{
		isRunning = false;
	}
	static SDL_Renderer* GetRenderer()
	{
		return renderer;
	}

	static SDL_Window* GetWindow()
	{
		return window;
	}

	static int GetWindowWidth()
	{
		return width;
	}
	static int GetWindowHeight()
	{
		return height;
	}


	void CreateWindow(std::string title, int width, int height)
	{
		this->width = width;
		this->height = height;
		SDL_CreateWindowAndRenderer(title.c_str(), width, height, SDL_WINDOW_RESIZABLE, &window, &renderer);
		SDL_GetWindowSize(window, &this->width, &this->height);
	}
	void SetWindowIcon(SDL_Surface* icon)
	{
		SDL_SetWindowIcon(window, icon);
	}


	void Quit()
	{
		isRunning = false;
	}
};
SDL_Window* Window::window;
SDL_Renderer* Window::renderer;
int Window::width;
int Window::height;
bool Window::isRunning = true;

class Camera
{
	static Camera* camera;
	SDL_Rect* rect;

public:
	static Camera* GetCurrentCamera()
	{
		return camera;
	}
	
};


class TextureManager
{
public:
	static SDL_Surface* LoadSurface(std::string path, std::string mode = "r")
	{
		return IMG_LoadPNG_IO(SDL_IOFromFile(path.c_str(), mode.c_str()));
	}
	static SDL_Texture* CreateTextureFromSurface(SDL_Surface* surface)
	{			
		SDL_Texture* texture = SDL_CreateTextureFromSurface(Window::GetRenderer(), surface);
		return texture;
	}

	static SDL_Texture* Load(std::string path)
	{
		SDL_Texture* texture = IMG_LoadTexture(Window::GetRenderer(), path.c_str());
		return texture;
	}
	static SDL_Texture* MakeRectangleTexture(int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255)
	{
		SDL_Texture* texture = SDL_CreateTexture(Window::GetRenderer(), SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, w, h);
		SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
		SDL_SetRenderTarget(Window::GetRenderer(), texture);
		SDL_SetRenderDrawColor(Window::GetRenderer(), r, g, b, a);
		SDL_RenderClear(Window::GetRenderer());
		SDL_SetRenderTarget(Window::GetRenderer(), nullptr);
		return texture;
	}

	static SDL_Texture* CreateCircleTexture(float radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255)
	{
		int diameter = radius * 2;
		SDL_Surface* surface = SDL_CreateSurface(diameter, diameter, SDL_PIXELFORMAT_RGBA8888);
		SDL_FillSurfaceRect(surface, NULL, SDL_MapRGBA(SDL_GetPixelFormatDetails(surface->format), NULL, 0, 0, 0, 0));
		int cx = (int)radius;
		int cy = (int)radius;
		SDL_SetRenderDrawColor(Window::GetRenderer(), r, g, b, a);
		const float pi = 3.1415926535f;
		for (float angle = 0.0f; angle < 2 * pi; angle += 0.01f) {
			int _x = cx + (int)radius * cos(angle);
			int _y = cy + (int)radius * sin(angle);
			SDL_WriteSurfacePixel(surface, _x, _y, r, g, b, a);
		}
		SDL_Texture* texture = TextureManager::CreateTextureFromSurface(surface);
		SDL_DestroySurface(surface);
		return texture;
	};
	static SDL_Texture* CreateCircleTextureFill(float radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255)
	{

		int diameter = radius * 2;
		SDL_Surface* surface = SDL_CreateSurface(diameter, diameter, SDL_PIXELFORMAT_RGBA8888);
		auto formatDetails = SDL_GetPixelFormatDetails(surface->format);
		Uint32 color = SDL_MapRGBA(formatDetails, NULL, r, g, b, a);
		SDL_FillSurfaceRect(surface, NULL, SDL_MapRGBA(formatDetails, NULL, 0, 0, 0, 0));

		int cx = (int)radius;
		int cy = (int)radius;
		SDL_SetRenderDrawColor(Window::GetRenderer(), r, g, b, a);
		const float pi = 3.1415926535f;
		for (int y = -radius; y <= radius; y++)
		{
			for (int x = -radius; x <= radius; x++)
			{
				if (x * x + y * y <= radius * radius)
				{
					int px = cx + x;
					int py = cy + y;
					Uint32* pixels = (Uint32*)surface->pixels;
					pixels[py * surface->w + px] = color;
				}
			}
		}
		SDL_Texture* texture = SDL_CreateTextureFromSurface(Window::GetRenderer(), surface);
		SDL_DestroySurface(surface);
		return texture;
	}



	static void Draw(SDL_Texture* texture, int x, int y, double angle = 0.0, const SDL_FPoint* center = nullptr, SDL_FRect* srcRect = nullptr, SDL_FRect* dstRect = nullptr, SDL_FlipMode flipMode = SDL_FLIP_NONE)
	{										
		SDL_RenderTextureRotated(Window::GetRenderer(), texture, srcRect, dstRect, angle, center, flipMode);
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
			int w;
			int h;
			Sprite(SDL_Texture* texture, int z = 0)
			{
				this->texture = texture;
				this->z = z;
				float _w;
				float _h;
				SDL_GetTextureSize(texture, & _w, & _h);
				this->w = (int)_w;
				this->h = (int)_h;
			}
		};	
		struct Rotatable
		{
			double angle = 0.0;
			SDL_FPoint* center = nullptr;
		};
		struct Collider
		{
			SDL_FRect rect;
			double offsetX = 0;
			double offsetY = 0;
			bool isStatic = false;
			bool colliding = false;
		};
		struct Physics
		{
			double accelerationX;
			double accelerationY;
			double terminalVelocity = 512;
			float bounce = 0.0f;
			bool onFloor = false;
			bool floating = false;
			double gravity = 1960.0;
			double friction = 0.8;
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
			int totalRows = 1;
			bool end = false;
			
		};
		struct Parallax
		{
			float x = 1.0f;
			float y = 1.0f;
			bool repeating = false;
		};
		struct UI
		{
			bool visible = false;
		};
		struct UISprite
		{
			SDL_Texture* texture;
			int w;
			int h;
			UISprite(SDL_Texture* texture)
			{
				this->texture = texture;
				float _w;
				float _h;
				SDL_GetTextureSize(texture, &_w, &_h);
				this->w = (int)_w;
				this->h = (int)_h;
			}
		};
		struct Button
		{
			SDL_Rect rect;
			bool pressed;
			bool down;
			bool hovering;
		};
		struct Label
		{
			std::string text = "";
			SDL_Color color = { 255U, 255U, 255U, 255U };
			float size = 21.0f;
				
		};
		struct Range
		{
			float value = 0.0f;
			float min = -1.0f;
			float max = 1.0f;
			Range(float value = 0.0f, float min = -1.0f, float max = 1.0f)
			{
				this->value = value;
				this->min = min;
				this->max = max;
			}
		};
		struct Audio
		{
			SDL_AudioStream* stream;
			bool looped = false;
			float startAfterLoop = 0.0f;
			SDL_AudioSpec spec;
			Uint8* wav_data = nullptr;
			Uint32 wav_len = 0;
			Audio(std::string path, bool looped = false ,float startAfterLoop = 0.0f)
			{
				SDL_AudioSpec spec;
				Uint8* wav_data = nullptr;
				Uint32 wav_len = 0;

				SDL_LoadWAV(path.c_str(), &spec, &wav_data, &wav_len);
				SDL_AudioStream* stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, nullptr, nullptr);
				SDL_PutAudioStreamData(stream, wav_data, wav_len);

				this->stream = stream;
				this->wav_len = wav_len;
				this->wav_data = wav_data;
				this->looped = looped;
				this->spec = spec;
			}

		};
		struct TileMap
		{
			std::vector<std::vector<int>> cells;

		};
		struct Tile
		{
			// id
		};
	};

	class System
	{
	public:
		static void Move(double elapsed)
		{
			
			auto& positions = Components<Component::Position>::GetStorage();
			const auto& velocities = Components<Component::Velocity>::GetStorage();

			for (auto& i : positions)										
			{
				int id = i.first;
				auto& pos = i.second;
				if (Component::Velocity* vel = Entity::GetComponent<Component::Velocity>(id))
				{
					pos.x += vel->x * elapsed;
					pos.y += vel->y * elapsed;
				}
			}

			
			/*
			auto& positions = Components<Component::Position>::Get();
			const auto& velocities = Components<Component::Velocity>::Get();
			auto& entities = Entity::entities;
			for (int i = 0; i < entities.size(); i++)
			{
				auto& pos = positions[i];
				auto& vel = velocities[i];
				pos.x += vel.x * elapsed;
				pos.y += vel.y * elapsed;
			}
			*/

		}
		static void Draw(int camX = 0, int camY = 0, int camW = 0, int camH = 0)
		{

			const std::unordered_map<int, Component::Sprite>& sprites = Components<Component::Sprite>::GetStorage();
			const std::unordered_map<int, Component::Position>& positions = Components<Component::Position>::GetStorage();
			const std::unordered_map<int, Component::Parallax>& parallaxes = Components<Component::Parallax>::GetStorage();
			const std::unordered_map<int, Component::Animation>& animations = Components<Component::Animation>::GetStorage();
			const std::unordered_map<int, Component::Rotatable>& rotatables = Components<Component::Rotatable>::GetStorage();

			
			std::vector<std::pair<int, Component::Sprite>> sortedSprites;
			for (auto& i : sprites)
			{
				std::pair<int, Component::Sprite> sprite = i;
				sortedSprites.push_back(sprite);
			}

			std::sort(sortedSprites.begin(), sortedSprites.end(), [](const std::pair<int, Component::Sprite>& a, const std::pair<int, Component::Sprite>& b) {return a.second.z < b.second.z;});

			
			for (auto& sprite : sortedSprites)
			{



				int entity = sprite.first;
				auto& texture = sprite.second.texture;
				auto it = positions.find(entity);
				int x = 0;
				int y = 0;
				if (it != positions.end())
				{
					x = it->second.x;
					y = it->second.y;
				}
				


				float w = sprite.second.w;
				float h = sprite.second.h;

				float parallaxX = 1.0f;
				float parallaxY = 1.0f;
				bool repeating = false;
				if (parallaxes.find(entity) != parallaxes.end())
				{
					auto& parallax = parallaxes.find(entity)->second;
					parallaxX = parallax.x;
					parallaxY = parallax.y;
					repeating = parallax.repeating;
				}

		

				float frameWidth = w;
				float frameHeight = h;
				auto animation = animations.find(entity);
				SDL_FRect srcRect;
				SDL_FRect dstRect;
				if (animation != animations.end())
				{
					auto& anim = animation->second;

					frameWidth = w / anim.maxFrames;
					frameHeight = h / anim.totalRows;

					srcRect.x = frameWidth * anim.frame;
					srcRect.y = frameHeight * anim.row;
					srcRect.w = frameWidth;
					srcRect.h = frameHeight;

					dstRect.x = (float)floor(x - camX * parallaxX);
					dstRect.y = (float)floor(y - camY * parallaxY);
					dstRect.w = srcRect.w;
					dstRect.h = srcRect.h;
				}

				else
				{
					srcRect.x = 0;
					srcRect.y = 0;
					srcRect.w = w;
					srcRect.h = h;


					dstRect.x = (float)floor(x - camX * parallaxX);
					dstRect.y = (float)floor(y - camY * parallaxY);
					dstRect.w = w;
					dstRect.h = h;
				}


				if (
					(dstRect.x + dstRect.w < 0) ||
					(dstRect.y + dstRect.h < 0) ||
					(dstRect.x > camW) ||
					(dstRect.y > camH)
					)
				{
					continue;
				}
				auto rotatable = rotatables.find(entity);
				double angel = 0.0;
				SDL_FPoint* point = nullptr;
				if (rotatable != rotatables.end())
				{
					auto& rotate = rotatable->second;
					angel = rotate.angle;
					point = rotate.center;
				}



				
				if (repeating)
				{

					float drawX = fmodf((x - camX * parallaxX), w);
					if (drawX > 0) { drawX -= w; }
					float drawY = (float)floor(y - camY * parallaxY);
					for (float offsetX = drawX; offsetX < camW; offsetX += w)
					{
						dstRect.x = offsetX;
						dstRect.y = drawY;
						dstRect.w = w;
						dstRect.h = h;
						TextureManager::Draw(texture, 0, 0, 0.0, NULL, &srcRect, &dstRect);
					}
					continue;
				}
					

				
			
				TextureManager::Draw(texture, 0, 0, angel, point, &srcRect, &dstRect);
				
		

			
				

			}
		}
		static void DrawUI(int camX = 0, int camY = 0, int camW = 0, int camH = 0)
		{

			const auto& sprites = Components<Component::Sprite>::GetStorage();
			const auto& positions = Components<Component::Position>::GetStorage();
			const auto& animations = Components<Component::Animation>::GetStorage();
			const auto& rotatables = Components<Component::Rotatable>::GetStorage();


			std::vector<std::pair<int, Component::Sprite>> sortedSprites;
			for (auto& i : sprites)
			{
				std::pair<int, Component::Sprite> sprite = i;
				sortedSprites.push_back(sprite);
			}

			std::sort(sortedSprites.begin(), sortedSprites.end(), [](const std::pair<int, Component::Sprite>& a, const std::pair<int, Component::Sprite>& b) {return a.second.z < b.second.z;});


			for (auto& sprite : sortedSprites)
			{



				int entity = sprite.first;

				auto it = positions.find(entity);
				int x = 0;
				int y = 0;
				if (it != positions.end())
				{
					x = it->second.x;
					y = it->second.y;
				}



				float w = sprite.second.w;
				float h = sprite.second.h;



				float frameWidth = w;
				float frameHeight = h;
				auto animation = animations.find(entity);
				SDL_FRect srcRect;
				SDL_FRect dstRect;
				if (animation != animations.end())
				{
					auto& anim = animation->second;

					frameWidth = w / anim.maxFrames;
					frameHeight = h / anim.totalRows;

					srcRect.x = frameWidth * anim.frame;
					srcRect.y = frameHeight * anim.row;
					srcRect.w = frameWidth;
					srcRect.h = frameHeight;

					dstRect.x = (float)floor(x);
					dstRect.y = (float)floor(y);
					dstRect.w = srcRect.w;
					dstRect.h = srcRect.h;
				}

				else
				{
					srcRect.x = 0;
					srcRect.y = 0;
					srcRect.w = w;
					srcRect.h = h;


					dstRect.x = (float)floor(x);
					dstRect.y = (float)floor(y);
					dstRect.w = w;
					dstRect.h = h;
				}


				if (
					(dstRect.x + dstRect.w < 0) ||
					(dstRect.y + dstRect.h < 0) ||
					(dstRect.x > camW) ||
					(dstRect.y > camH)
					)
				{
					continue;
				}




				auto rotatable = rotatables.find(entity);
				double angel = 0.0;
				SDL_FPoint* point = nullptr;
				if (rotatable != rotatables.end())
				{
					auto& rotate = rotatable->second;
					angel = rotate.angle;
					point = rotate.center;
				}

				TextureManager::Draw(sprite.second.texture, 0, 0, angel, point, &srcRect, &dstRect);

			}
		}
		

		static void UpdatePhysics(double elapsed)
		{
			auto& physics = Components<Component::Physics>::GetStorage();
			auto& velocities = Components<Component::Velocity>::GetStorage();

			for (auto& i : physics)
			{
				int entity = i.first;
				auto& physic = i.second;
				
				if (velocities.find(entity) != velocities.end())
				{
					auto& velocity = velocities.find(entity)->second;
					if (!physic.floating && !physic.onFloor)
					{
						velocity.y += physic.gravity * elapsed;
					}

					velocity.x += physic.accelerationX * elapsed;
					velocity.y += physic.accelerationY * elapsed;

					if (physic.onFloor)
					{
						velocity.x *= physic.friction;
					}
					

					if (velocity.y > physic.terminalVelocity) { velocity.y = physic.terminalVelocity; }
					if (velocity.y < -physic.terminalVelocity) { velocity.y = -physic.terminalVelocity; }

					physic.accelerationX = 0;
					physic.accelerationY = 0;

				}
			}
		}

		static void UpdateAnimationFrames(double elapsed)
		{
			auto& animations = Components<Component::Animation>::GetStorage();
			for (auto& i : animations)
			{
				auto& anim = i.second;
				anim.elapsed += elapsed * 1000.0f * anim.speed;
				if (anim.elapsed >= anim.frameTime || anim.end)
				{
					anim.elapsed -= anim.frameTime;
					anim.frame++;
					if (anim.looped)
					{
						anim.frame %= anim.maxFrames;
					}
					else
					{
						anim.frame = std::min(anim.frame, anim.maxFrames - 1);
					}
				}
				else
				{
					//anim.end = true;
				}
			}
		}
		static void SyncColliders()
		{
			auto& colliders = Components<Component::Collider>::GetStorage();
			const auto& positions = Components<Component::Position>::GetStorage();
			for (auto& i : colliders)
			{
				int entity = i.first;
				if (positions.find(entity) != positions.end())
				{
					auto& pos = positions.find(entity)->second;
					auto& collider = colliders[entity];
					collider.rect.x = pos.x + collider.offsetX;
					collider.rect.y = pos.y + collider.offsetY;
				}
			}
		}
		static void PlayAnimation(std::string state)
		{

		}

		static void Collide()
		{
			auto& colliders = Components<Component::Collider>::GetStorage();
			auto& positions = Components<Component::Position>::GetStorage();
			auto& velocities = Components<Component::Velocity>::GetStorage();
			auto& physics = Components<Component::Physics>::GetStorage();

			for (auto& kv : physics)
			{
				kv.second.onFloor = false;
			}

			SyncColliders();


			for (auto i = colliders.begin(); i != colliders.end(); ++i)
			{
				int entityA = i->first;
				auto& a = i->second;

				for (auto j = std::next(i); j != colliders.end(); ++j)
				{
					int entityB = j->first;
					auto& b = j->second;


					SDL_FRect A = a.rect;
					SDL_FRect B = b.rect;


					if (!SDL_HasRectIntersectionFloat(&A, &B)) { continue; }

					float dlx = (A.x + A.w) - B.x;
					float drx = (B.x + B.w) - A.x;
					float overlapX = std::min(dlx, drx);

					float duy = (A.y + A.h) - B.y;
					float ddy = (B.y + B.h) - A.y;
					float overlapY = std::min(duy, ddy);

					bool resolveX = overlapX < overlapY;

					auto moveEntity = [&](int id, float dx, float dy)
						{

							//auto cIt = colliders.find(id);
							auto pIt = positions.find(id);
							//if (cIt != colliders.end() && !cIt->second.isStatic)
							//{
								//cIt->second.rect.x += dx;
								//cIt->second.rect.y += dy;
								if (pIt != positions.end())
								{
									pIt->second.x += dx;
									pIt->second.y += dy;
								}
							//}
						};
					auto zeroVelAxis = [&](int id, bool xAxis, float bounce = 0.0f)
						{
							auto v = velocities.find(id);
							if (v == velocities.end()) return;
							if (xAxis) { v->second.x *= -bounce; }
							else { v->second.y *= -bounce; }
						};

					auto getPhys = [&](int id) -> Component::Physics*
						{
							auto p = physics.find(id);
							if (p == physics.end()) { return nullptr; }
							return &p->second;
						};


					if (resolveX)
					{
						float push = (A.x < B.x ? -overlapX : overlapX);

						if (!a.isStatic && b.isStatic)
						{
							moveEntity(entityA, push, 0);
							zeroVelAxis(entityA, true, getPhys(entityA) ? getPhys(entityA)->bounce : 0.0f);
						}
						else if (a.isStatic && !b.isStatic)
						{
							moveEntity(entityB, -push, 0);
							zeroVelAxis(entityB, true, getPhys(entityB) ? getPhys(entityB)->bounce : 0.0f);
						}
						else if (!a.isStatic && !b.isStatic)
						{
							moveEntity(entityA, push * 0.5f, 0);
							moveEntity(entityB, -push * 0.5f, 0);
							zeroVelAxis(entityA, false, getPhys(entityA) ? getPhys(entityA)->bounce : 0.0f);
							zeroVelAxis(entityB, false, getPhys(entityB) ? getPhys(entityB)->bounce : 0.0f);
						}
					}
					else
					{
						float push = (A.y < B.y ? -overlapY : overlapY);

						if (!a.isStatic && b.isStatic)
						{
							moveEntity(entityA, 0, push);
							zeroVelAxis(entityA, true, getPhys(entityA) ? getPhys(entityA)->bounce : 0.0f);


							if (A.y < B.y) 
							{ 
								if (getPhys(entityA)) 
								{
									getPhys(entityA)->onFloor = true; 
								} 
								positions[entityA].y = B.y - A.h;
							}
						}
						else if (a.isStatic && !b.isStatic)
						{
							moveEntity(entityB, 0, -push);
							zeroVelAxis(entityB, false, getPhys(entityB) ? getPhys(entityB)->bounce : 0.0f);

							if (B.y < A.y) { if (getPhys(entityB)) { getPhys(entityB)->onFloor = true; } }
						}
						else if (!a.isStatic && !b.isStatic)
						{
							moveEntity(entityA, 0, push * 0.5f);
							moveEntity(entityB, 0, -push * 0.5f);
							zeroVelAxis(entityA, false, getPhys(entityA) ? getPhys(entityA)->bounce : 0.0f);
							zeroVelAxis(entityB, false, getPhys(entityB) ? getPhys(entityB)->bounce : 0.0f);

							if (A.y < B.y) { if (getPhys(entityA)) { getPhys(entityA)->onFloor = true; } }
							else { if (getPhys(entityB)) { getPhys(entityB)->onFloor = true; } }
						}
					}

					a.colliding = true;
					b.colliding = true;

					
					if (A.y < B.y) { if (getPhys(entityA)) { getPhys(entityA)->onFloor = true; } }
				}
				
			}
		


			



			SyncColliders();

		}

		static SDL_FRect CreateCollidersFromTexture(SDL_Texture* texture, double x, double y)
		{
			float w = texture->w;
			float h = texture->h;
			SDL_FRect rect;
			rect.x = x;
			rect.y = y;
			rect.w = w;
			rect.h = h;
			return rect;	
		}


		static void DrawColliders(int camX, int camY)
		{
			const auto& colliders = Components<Component::Collider>::GetStorage();
			SDL_SetRenderDrawColor(Window::GetRenderer(), 255, 0, 0, 128U);
			//SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
			for (auto& i : colliders)
			{
				int entity = i.first;
				
				if (colliders.find(entity) != colliders.end())
				{
					SDL_FRect rect = i.second.rect;
					rect.x -= camX;
					rect.y -= camY;
					SDL_RenderRect(Window::GetRenderer(), &rect);
				}
			}
		}


		static void UpdateButtons()
		{
			auto& buttons = Components<Component::Button>::GetStorage();
			for (auto& i : buttons)
			{
				int entity = i.first;
				auto& button = i.second;
				button.pressed = false;

				
				float mouseX;
				float mouseY;
				SDL_GetMouseState(&mouseX, &mouseY);
					
				SDL_Rect rect;
				rect.x = mouseX;
				rect.y = mouseY;
				rect.w = 1;
				rect.h = 1;
				
				if (SDL_HasRectIntersection(&rect, &button.rect))
				{
					button.hovering = true;
					button.pressed = Input::IsMousePressed(SDL_BUTTON_LEFT);
					button.down = Input::IsMouseDown(SDL_BUTTON_LEFT);
					
				}
				else
				{
					button.hovering = false;
					button.pressed = false;
					button.down = false;
				}
			}
		}





		static void SyncButtonPositions()
		{
			auto& buttons = Components<Component::Button>::GetStorage();
			auto& positions = Components<Component::Position>::GetStorage();
			for (auto& i : buttons)
			{
				auto entity = i.first;
				auto& button = i.second;

				auto position = positions.find(entity);
				if (position != positions.end())
				{
					auto& pos = position->second;
					pos.x = button.rect.x;
					pos.y = button.rect.y;
				}
				

			}

		}
		static void PlayAudio(int entity)
		{
			auto& audios = Components<Component::Audio>::GetStorage();
			auto audio = audios.find(entity);
			if (audio != audios.end())
			{
				auto& au = audio->second;
				SDL_ResumeAudioStreamDevice(au.stream);
			}
		}
		static void ResetAudioAtPosition(int entity, float start = 0.0f)
		{
			auto& audios = Components<Component::Audio>::GetStorage();
			auto audio = audios.find(entity);
			if (audio == audios.end())
			{
				return;
			}


			auto& au = audio->second;
			
			int bytesPerSample = SDL_AUDIO_BYTESIZE(au.spec.format) * au.spec.channels;
			int bytesPerSecond = au.spec.freq * bytesPerSample;
			Uint32 startOffset = (Uint32)(start * bytesPerSecond);

			startOffset = std::min(startOffset, au.wav_len);
			startOffset = (startOffset / bytesPerSample) * bytesPerSample;


			std::cout << "Starting at offset: " << startOffset << " bytes (should be " << start << " seconds)\n";
			SDL_FlushAudioStream(au.stream);
			SDL_PutAudioStreamData(au.stream, au.wav_data + startOffset, au.wav_len - startOffset);
			SDL_ResumeAudioStreamDevice(au.stream);


		}
		static void UpdateAudio()
		{
			auto& audios = Components<Component::Audio>::GetStorage();
			for (auto& audio : audios)
			{
				int entity = audio.first;
				auto& au = audio.second;

				if (au.looped && SDL_GetAudioStreamQueued(au.stream) == 0)
				{
					std::cout << "Audio ended, restarting at " << au.startAfterLoop << " seconds\n";
					ResetAudioAtPosition(entity, au.startAfterLoop);
					//SDL_PutAudioStreamData(au.stream, au.wav_data, au.wav_len);
				}
			}


		}

	};
	template <typename T>
	class Components 
	{
		static std::vector<T> mStorage;
	public:
		static std::unordered_map<int, T> storage;
		static std::unordered_map<int, T>& GetStorage()
		{
			return storage;
		}
		static std::vector<T>& Get()
		{
			return mStorage;
		}
		template<typename T>
		static std::vector<std::pair<int, T*>> GetComponentsContaining(const std::vector<int>& entities)
		{
			std::vector<std::pair<int, T*>> res;

			for (int entity : entities)
			{
				auto it = storage.find(entity);
				if (it != storage.end())
				{
					res.push_back({ entity, &it->second });
				}
			}
			return res;
		}
	};
	class Entity
	{
		
		static int nextEntity;
	public:
		static std::vector<int> entities;
		static std::unordered_map<std::type_index, std::unordered_map<int, Component>> components;

		template<typename T>
		static void AddComponent(int entity, T component, std::unordered_map<int, T>& componentMap)
		{
			componentMap[entity] = component;
		};
		template<typename T>
		static void AddComponent(int entity, T component)
		{
			std::unordered_map<int, T>& storage = Components<T>::GetStorage();
			storage.insert_or_assign(entity, component);
		};
		template<typename T>
		static void Add(int entity, T component)
		{
			std::vector<T>& storage = Components<T>::Get();
			storage[entity] = component;
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
		template<typename T>
		static T* GetComponent(int entity)
		{
			auto& storage = Components<T>::GetStorage();
			auto it = storage.find(entity);
			if (it != storage.end())
			{
				return &it->second;
			}
			return nullptr;
		}

		static void DestroyEntity(int entity)
		{
			KillYourself(entity);
		}

		static void KillYourself(int entity)
		{
			std::unordered_map<int, Component::Position>& positions = Components<Component::Position>::GetStorage();
			std::unordered_map<int, Component::Velocity>& velocities = Components<Component::Velocity>::GetStorage();
			std::unordered_map<int, Component::Sprite>& sprites = Components<Component::Sprite>::GetStorage();
			std::unordered_map<int, Component::Rotatable>& rotatables = Components<Component::Rotatable>::GetStorage();
			std::unordered_map<int, Component::Collider>& collider = Components<Component::Collider>::GetStorage();
			std::unordered_map<int, Component::Physics>& physics = Components<Component::Physics>::GetStorage();
			std::unordered_map<int, Component::Animation>& animations = Components<Component::Animation>::GetStorage();
			std::unordered_map<int, Component::Parallax>& parallaxes = Components<Component::Parallax>::GetStorage();
			std::unordered_map<int, Component::UI>& uis = Components<Component::UI>::GetStorage();
			std::unordered_map<int, Component::UISprite>& uiSprites = Components<Component::UISprite>::GetStorage();
			std::unordered_map<int, Component::Button>& buttons = Components<Component::Button>::GetStorage();
			std::unordered_map<int, Component::Label>& labels = Components<Component::Label>::GetStorage();
			std::unordered_map<int, Component::Range>& ranges = Components<Component::Range>::GetStorage();
			std::unordered_map<int, Component::Audio>& audio = Components<Component::Audio>::GetStorage();
			std::unordered_map<int, Component::TileMap>& tilemMaps = Components<Component::TileMap>::GetStorage();
			std::unordered_map<int, Component::Tile>& tiles = Components<Component::Tile>::GetStorage();


			positions.erase(entity);
			velocities.erase(entity);
			sprites.erase(entity);
			rotatables.erase(entity);
			collider.erase(entity);
			physics.erase(entity);
			animations.erase(entity);
			parallaxes.erase(entity);
			uis.erase(entity);
			uiSprites.erase(entity);
			buttons.erase(entity);
			labels.erase(entity);
			ranges.erase(entity);
			audio.erase(entity);
			tilemMaps.erase(entity);
			tiles.erase(entity);
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
		
		std::vector<int> clouds;
		std::vector<int> trees;
		std::unordered_map<int, Component::Rotatable*> tree_rotations;
		Window* window;

		ECS::Component::Position* camera_position;
		ECS::Component::Position* player_position;
		ECS::Component::Velocity* player_velocity;
		ECS::Component::Sprite* player_sprite;
		ECS::Component::Animation* player_animation;
		ECS::Component::Physics* player_physics;
		ECS::Component::Rotatable* player_rotation;

		int buttonId;
		int range;
		ECS::Component::Position* range_position;
		ECS::Component::Button* button;

		
		void PlayAudio(std::string path)
		{
			SDL_AudioSpec spec;
			Uint8* wav_data = nullptr;
			Uint32 wav_len = 0;

			SDL_LoadWAV(path.c_str(), &spec, &wav_data, &wav_len);
			SDL_AudioStream* stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, nullptr, nullptr);
			SDL_PutAudioStreamData(stream, wav_data, wav_len);
			SDL_free(wav_data);
			SDL_ResumeAudioStreamDevice(stream);

		}
		int CreateFloor(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255U)
		{
			int ground = Entity::CreateEntity();
			auto ground_texture = TextureManager::MakeRectangleTexture(w, h, r, g, b, a);
			SDL_FRect ground_rect = { x, y, w, h };

			auto ground_collider = System::CreateCollidersFromTexture(ground_texture, ground_rect.x, ground_rect.y);
			Entity::AddComponent(ground, Component::Sprite{ ground_texture, -1 });
			Entity::AddComponent(ground, Component::Collider{ ground_rect, 0, 0, true });
			Entity::AddComponent(ground, Component::Position{ ground_rect.x, ground_rect.y });
			return ground;

		}
		int CreateRange(float radius, int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255U)
		{
			int range = Entity::CreateEntity();
			SDL_Texture* texture = TextureManager::CreateCircleTextureFill(radius, r, g, b, a);
			SDL_Rect rect;
			rect.x = x;
			rect.y = y;
			rect.w = (int)radius;
			rect.h = (int)radius;
			Entity::AddComponent(range, Component::Sprite{ texture, 694299 });
			Entity::AddComponent(range, Component::Position{ (double)x, (double)y });
			Entity::AddComponent(range, Component::Parallax{ 0.0, 0.0 });
			Entity::AddComponent(range, Component::Range{});
			Entity::AddComponent(range, Component::Button{ rect });
			return range;
		}
		int CreateSlider()
		{

		}
		int CreateRainbowSliderToChangeColorOfSomethingIdk()
		{

		}

		int CreateButton(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255U)
		{
			int button = Entity::CreateEntity();
			SDL_Rect rect;
			rect.x = x;
			rect.y = y;
			rect.w = w;
			rect.h = h;
			SDL_Texture* texture = TextureManager::MakeRectangleTexture(w, h, r, g, b, a);

			Entity::AddComponent(button, Component::Sprite{ texture, 69420 });
			Entity::AddComponent(button, Component::Position{ (double)x, (double)y });
			Entity::AddComponent(button, Component::Parallax{ 0.0, 0.0 });
			Entity::AddComponent(button, Component::Button{ rect });
			return button;
		}

		int CreateLabel(std::string string, int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255U)
		{
			int label = Entity::CreateEntity();
			SDL_Color color = { r, g, b, a };
			SDL_Surface* surface = nullptr;
			SDL_Texture* texture = nullptr;



			surface = TTF_RenderText_Blended(font, string.c_str(), 0, color);
			texture = SDL_CreateTextureFromSurface(Window::GetRenderer(), surface);
			SDL_DestroySurface(surface);

			float w, h;
			SDL_GetTextureSize(texture, &w, &h);

			Entity::AddComponent(label, Component::Label{ string, color });
			Entity::AddComponent(label, Component::Sprite{ texture, 69421 });
			Entity::AddComponent(label, Component::Position{ (double)x, (double)y });
			Entity::AddComponent(label, Component::Parallax{ 0.0f, 0.0f });
			return label;
		}

		int CreateButtonWithLabel(std::string string, int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255U)
		{
			int button = Entity::CreateEntity();
			int label = Entity::CreateEntity();
			SDL_Rect buttonRect = { x, y, w, h };
			SDL_Rect labelRect;
			SDL_Color bcolor = { r, g, b, a };
			SDL_Color lcolor = { 255U, 255U, 255U, 255U };
			SDL_Surface* surface = nullptr;
			SDL_Texture* ltexture = nullptr;
			SDL_Texture* btexture = TextureManager::MakeRectangleTexture(buttonRect.w, buttonRect.h, r, g, b, a);



			surface = TTF_RenderText_Blended(font, string.c_str(), 0, lcolor);
			ltexture = SDL_CreateTextureFromSurface(Window::GetRenderer(), surface);
			SDL_DestroySurface(surface);

			float lw, lh;
			SDL_GetTextureSize(ltexture, &lw, &lh);

			labelRect = { (int)((buttonRect.x + buttonRect.w / 2) - lw / 2), (int)((buttonRect.y + buttonRect.h / 2) - lh / 2), (int)lw, (int)lh };


			Entity::AddComponent(label, Component::Label{ string, lcolor });
			Entity::AddComponent(label, Component::Sprite{ ltexture, 69421 });
			Entity::AddComponent(label, Component::Position{ (double)labelRect.x, (double)labelRect.y });
			Entity::AddComponent(label, Component::Parallax{ 0.0f, 0.0f });

			Entity::AddComponent(button, Component::Label{ string, bcolor });
			Entity::AddComponent(button, Component::Sprite{ btexture, 69420 });
			Entity::AddComponent(button, Component::Position{ (double)x, (double)y });
			Entity::AddComponent(button, Component::Parallax{ 0.0, 0.0 });
			Entity::AddComponent(button, Component::Button{ buttonRect });
			return button;
		}
		void Init()
		{
		
			int mountains = Entity::CreateEntity();
			Entity::AddComponent(mountains, Component::Parallax{0.01f, 0.0f, true});
			Entity::AddComponent(mountains, Component::Position{});
			Entity::AddComponent(mountains, Component::Sprite{ TextureManager::Load("assets/textures/doesthisevenlooklikemountains.png"), -420 });



			int treesfar = Entity::CreateEntity();
			Entity::AddComponent(treesfar, Component::Parallax{ 0.15f, 0.01f, true });
			Entity::AddComponent(treesfar, Component::Position{});
			Entity::AddComponent(treesfar, Component::Sprite{ TextureManager::Load("assets/textures/doesthisevenlookliketreesfromfar.png"), -1 });


			int trees = Entity::CreateEntity();
			Entity::AddComponent(trees, Component::Parallax{ 0.25f, 0.1f, true });
			Entity::AddComponent(trees, Component::Position{});
			Entity::AddComponent(trees, Component::Sprite{ TextureManager::Load("assets/textures/doesthisevenlookliketrees.png"), -1 });

			int forestDay = Entity::CreateEntity();
			Entity::AddComponent(forestDay, Component::Audio{ "assets/audio/music/ForestDayMorning.wav", true, 10.0f});
			System::PlayAudio(forestDay);
			int sun = Entity::CreateEntity();
			Entity::AddComponent(sun, Component::Sprite{TextureManager::Load("assets/textures/sun.png"), -420});
			Entity::AddComponent(sun, Component::Position{});
			Entity::AddComponent(sun, Component::Parallax{0.00000001, 0.0000001 });
			CreateClouds(420);


			/*
			
			for (int i = 0; i < 420; i++)
			{
				int tree = Entity::CreateEntity();
				Entity::AddComponent(tree, Component::Sprite{ TextureManager::Load(renderer, "assets/textures/tree.png"), 1 }, Entity::sprites);
				Entity::AddComponent(tree, Component::Position{ (double)(rand() % 420), (double)(rand() % 420) }, Entity::positions);
				Entity::AddComponent(tree, Component::Rotatable{}, Entity::rotatables);
				trees.push_back(tree);
				tree_rotations[i] = Entity::GetComponent(tree, Entity::rotatables);
			}
			*/

			/*
			CreateFloor(0, 128, 32, 8);
			CreateFloor(64, 64, 32, 8);
			CreateFloor(192, 32, 32, 8);
			CreateFloor(256, 0, 32, 8);
			CreateFloor(128, -64, 32, 8);
			CreateFloor(64, -64, 32, 8);
			CreateFloor(0, -128, 32, 8);
			*/
			CreateFloor(-1000, 128, 2000, 512, 151, 107, 75, 255);
			buttonId = CreateButtonWithLabel("Meow", 100, 100, 200, 100, 255 / 2, 255 / 2, 255 / 2);

			range = CreateRange(10.0f, 300, 300, 0, 0, 0, 255);
			range_position = Entity::GetComponent<Component::Position>(range);
			Entity::AddComponent(camera, Component::Position{0, 0});
			
			/*
			Entity::AddComponent(world, Component::Position{ 0, 0 }, Entity::positions);
			Entity::AddComponent(world, Component::Sprite{ TextureManager::Load(renderer, "assets/textures/icon.png"), 69420 }, Entity::sprites);

			*/
			Entity::AddComponent(player, Component::Position{ 0, 0 });
			Entity::AddComponent(player, Component::Velocity{ 0, 0 });
			Entity::AddComponent(player, Component::Sprite{ TextureManager::Load("assets/textures/player_spritesheet.png"), 420 });
			Entity::AddComponent(player, Component::Animation{ 0, 0.0f, 100.0f, true, {}, 1.0f, 0, 0, 8 });
			Entity::AddComponent(player, Component::Physics{ 100.0, 0.0f});
			Entity::AddComponent(player, Component::Rotatable{});
			Entity::AddComponent(player, Component::Collider{ SDL_FRect{0, 0, 16, 32}, 8});

			
			camera_position = Entity::GetComponent<Component::Position>(camera);
			player_position = Entity::GetComponent<Component::Position>(player);
			player_velocity = Entity::GetComponent<Component::Velocity>(player);
			player_sprite = Entity::GetComponent<Component::Sprite>(player);
			player_animation = Entity::GetComponent<Component::Animation>(player);
			player_physics = Entity::GetComponent<Component::Physics>(player);
			player_rotation = Entity::GetComponent<Component::Rotatable>(player);

			button = Entity::GetComponent<Component::Button>(buttonId);
		}
		double speed;
		void Loop(double elapsed)
		{
			range_position->x = rand() % 420;																		
			range_position->y = rand() % 420;
			// process inputs -> process behaviors -> calculate velocities -> move entities -> resolve collisions -> render frame
			MoveClouds();

			double player_speed = 400.0;

			player_velocity->x = 0.0f;
			player_animation->state = "Idle";

			if (Input::IsKeyDown(SDL_SCANCODE_A))
			{
				player_velocity->x = -player_speed;
			}
			else if (Input::IsKeyDown(SDL_SCANCODE_D))
			{
				player_velocity->x = player_speed;
			}



			if (Input::IsKeyPressed(SDL_SCANCODE_SPACE) && player_physics->onFloor)
			{
				player_velocity->y = -640;
				player_physics->onFloor = false;
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

			if (Input::IsKeyPressed(SDL_SCANCODE_R))
			{
				player_position->x = 0.0;
				player_position->y = 0.0;
			}


			//player_rotation->angle += 1;
			for (auto& tree_rotation : tree_rotations)
			{
				tree_rotation.second->angle += rand() % 10;
			}

			if (button->pressed)
			{
				std::cout << "Meow\n";
			}
			float textureWidth, textureHeight;
			SDL_GetTextureSize(player_sprite->texture,  & textureWidth, & textureHeight);

			float frameWidth = textureWidth / player_animation->maxFrames;
			float frameHeight = textureHeight / player_animation->totalRows;

			float spriteCenterX = player_position->x + frameWidth / 2.0f;
			float spriteCenterY = player_position->y + frameHeight / 2.0f;
			camera_position->x = spriteCenterX - window->width/2.0f;
			camera_position->y = spriteCenterY - window->height/2.0f;

			


		}
		void PrepareFrame(double elapsed)
		{
			float textureWidth, textureHeight;
			SDL_GetTextureSize(player_sprite->texture, &textureWidth, &textureHeight);

			float frameWidth = textureWidth / player_animation->maxFrames;
			float frameHeight = textureHeight / player_animation->totalRows;

			float spriteCenterX = player_position->x + frameWidth / 2.0f;
			float spriteCenterY = player_position->y + frameHeight / 2.0f;
			camera_position->x = spriteCenterX - window->width / 2.0f;
			camera_position->y = spriteCenterY - window->height / 2.0f;
		}

		void CreateClouds(int howMany)
		{
			for (int i = 0; i < howMany; ++i)
			{
				int cloud = Entity::CreateEntity();
				auto cloudTexture = TextureManager::Load("assets/textures/clouds.png");
				SDL_SetTextureAlphaMod(cloudTexture, std::max(21, rand() % 255));
				Entity::AddComponent(cloud, Component::Position{(double)(rand() % Window::GetWindowWidth()), (double)(rand() % Window::GetWindowHeight())/2 });
				Entity::AddComponent(cloud, Component::Velocity{(double)(rand() % 21)});
				Entity::AddComponent(cloud, Component::Sprite{ cloudTexture, -420});
				Entity::AddComponent(cloud, Component::Parallax
					{ 
						(float)(0.01 + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (0.1 - 0.01)),
						(float)(0.01 + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (0.1 - 0.01)) 
					} 
					);
				Entity::AddComponent(cloud, Component::Animation{rand() % 2, 0.0f, std::numeric_limits<float>::infinity(), false, "", 1.0f, rand() % 2, 2, 2});
				clouds.push_back(cloud);
			}
		}																	
		void MoveClouds()
		{
			for (auto& i : clouds)
			{
				auto cloud_pos = Entity::GetComponent<Component::Position>(i);
				if (cloud_pos->x > Window::GetWindowWidth())
				{
					auto cloud_speed = Entity::GetComponent<Component::Velocity>(i);
					cloud_speed->x = rand() % 21;
					cloud_pos->x = -Window::GetWindowWidth();
				}
			}
		}



			


	};
};
int ECS::Entity::nextEntity = 0;
std::vector<int> ECS::Entity::entities = {};
template<typename T> std::unordered_map<int, T> ECS::Components<T>::storage;
template<typename T>  std::vector<T> ECS::Components<T>::mStorage;

int main()
{
	srand(time(NULL));
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
	{
		std::cerr << SDL_GetError();
	}

	if (!TTF_Init())
	{
		std::cout << "ERROR NO TTF IDK ";
	}

	font = TTF_OpenFont("assets/Andy Bold.ttf", 20.0f);
	Window window = Window();;

	window.CreateWindow("Hi!!!", 1280, 720);
	window.SetWindowIcon(TextureManager::LoadSurface("assets/textures/icon.png"));

	SDL_Event event;
	bool isRunning = true;

	ECS::Game game = ECS::Game();
	game.window = &window;
	game.Init();




	double elapsed = 0.016;
	while (isRunning)
	{
		auto begin = std::chrono::high_resolution_clock::now();

		// process inputs -> process behaviors -> calculate velocities -> move entities -> resolve collisions -> render frame


		for (auto i = Input::keysPressed.begin(); i != Input::keysPressed.end(); i++)
		{
			i->second = false;
		}

		for (auto i = Input::mousePressed.begin(); i != Input::mousePressed.end(); i++)
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

			case SDL_EVENT_MOUSE_BUTTON_DOWN:
				Input::mousePressed[event.button.button] = true;
				Input::mouseDown[event.button.button] = true;
				break;
			case SDL_EVENT_MOUSE_BUTTON_UP:
				Input::mouseDown[event.button.button] = false;
				break;
			}


		}

		ECS::System::UpdateButtons();
		game.Loop(elapsed);
		ECS::System::UpdatePhysics(elapsed);
		ECS::System::UpdateAnimationFrames(elapsed);
		ECS::System::UpdateAudio();
		ECS::System::Move(elapsed);
		ECS::System::Collide();
		ECS::System::SyncButtonPositions();
		SDL_SetRenderDrawColor(window.GetRenderer(), 135, 197, 255, SDL_ALPHA_OPAQUE);



		game.PrepareFrame(elapsed);
		SDL_RenderClear(window.GetRenderer());
		ECS::System::Draw(game.camera_position->x, game.camera_position->y, game.window->width, game.window->height);


		// remove this later
		ECS::System::DrawColliders(game.camera_position->x, game.camera_position->y);

		SDL_RenderPresent(window.GetRenderer());

		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> deltaTime = end - begin;

		int fps = 1.0 / deltaTime.count();
		std::cout << "FPS: " << fps << "\n";
		elapsed = (deltaTime.count());
	}
		SDL_Quit();
		return 0;
	
}


#endif