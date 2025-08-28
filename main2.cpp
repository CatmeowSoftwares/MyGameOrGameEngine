	 #include <iostream>  
	#include <stdio.h>

	#include <SDL3/SDL.h> 
	#include <SDL3/SDL_render.h>
	#include <SDL3_image/SDL_image.h>
	//#include <SDL_mixer.h>
	#include <SDL3_ttf/SDL_ttf.h>

	#include <vector>
	#include <unordered_map>

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



		static void Draw(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y, double angle = 0.0, const SDL_FPoint* center = nullptr, SDL_FRect* srcRect = nullptr, SDL_FRect* dstRect = nullptr, SDL_FlipMode flipMode = SDL_FLIP_NONE)
		{
			float textureWidth, textureHeight;
			SDL_GetTextureSize(texture, &textureWidth, &textureHeight);												
			SDL_RenderTextureRotated(renderer, texture, srcRect, dstRect, angle, center, flipMode);
		}
	};

	class Window
	// REPLACE WITH OPENGL OR VULKAN WHEN POSSIBLE SO I CAN USE SHADERS
	{	
		SDL_Window* window = nullptr;
		SDL_Renderer* renderer = nullptr;


		static bool isRunning;
	public:

		int width = 0;
		int height = 0;

		static void ForceQuit()
		{
			isRunning = false;
		}
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
	bool Window::isRunning = true;

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
				int totalRows = 1;
				bool end = false;
			
			};

			struct Parallax
			{
				float x = 1.0f;
				float y = 1.0f;;
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
			struct Audio
			{
				SDL_AudioStream* stream;
				bool looped = false;
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
			static void Draw(SDL_Renderer* renderer, std::unordered_map<int, Component::Sprite>& sprites, std::unordered_map<int, Component::Animation>& animations, std::unordered_map<int, Component::Parallax>& parallaxes, std::unordered_map<int, Component::Rotatable>& rotatables ,std::unordered_map<int, Component::Position>& positions, int camX = 0, int camY = 0, int camW = 0, int camH = 0)
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
					int entity = sprite.first;
					int x = positions[entity].x;
					int y = positions[entity].y;
					float w;
					float h;
					SDL_GetTextureSize(sprite.second.texture, &w, &h);
				
					float parallaxX = 1.0f;
					float parallaxY = 1.0f;


					if (parallaxes.find(entity) != parallaxes.end())
					{
						auto& parallax = parallaxes[entity];
						parallaxX = parallax.x;
						parallaxY = parallax.y;
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

						dstRect.x = (float)floor(x- camX * parallaxX);
						dstRect.y = (float)floor(y- camY * parallaxY);
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

					TextureManager::Draw(renderer, sprite.second.texture,0, 0, angel, point ,&srcRect, &dstRect);
				
				}
			}


			static void UpdatePhysics(std::unordered_map<int, Component::Physics>& physics, std::unordered_map<int, Component::Velocity>& velocities, double elapsed)
			{
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

			static void UpdateAnimationFrames(std::unordered_map<int, Component::Animation>& animations, double elapsed)
			{
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
			static void SyncColliders(std::unordered_map<int, Component::Collider>& colliders, std::unordered_map<int, Component::Position>& positions)
			{
				for (auto& i : colliders)
				{
					int entity = i.first;
					if (positions.find(entity) != positions.end())
					{
						auto& pos = positions[entity];
						auto& collider = colliders[entity];
						collider.rect.x = pos.x + collider.offsetX;
						collider.rect.y = pos.y + collider.offsetY;
					}
				}
			}
			static void PlayAnimation(std::string state)
			{

			}

			static void Collide(std::unordered_map<int, Component::Collider>& colliders,
								std::unordered_map<int, Component::Position>& positions,
								std::unordered_map<int, Component::Velocity>& velocities,
								std::unordered_map<int, Component::Physics>& physics)
			{


				for (auto& kv : physics)
				{
					kv.second.onFloor = false;
				}

				SyncColliders(colliders, positions);


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
		


			



				SyncColliders(colliders, positions);

			}

			static SDL_FRect CreateCollidersFromTexture(SDL_Texture* texture, double x, double y)
			{
				float w;
				float h;
				SDL_GetTextureSize(texture, &w, &h);
				SDL_FRect rect;
				rect.x = x;
				rect.y = y;
				rect.w = w;
				rect.h = h;
				return rect;	
			}

			static SDL_Texture* MakeRectangleTexture(SDL_Renderer* renderer, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255)
			{
				SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, w, h);
				SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
				SDL_SetRenderTarget(renderer, texture);
				SDL_SetRenderDrawColor(renderer, r, g, b, a);
				SDL_RenderClear(renderer);
				SDL_SetRenderTarget(renderer, nullptr);
				return texture;
			}

			static void DrawColliders(SDL_Renderer* renderer, const std::unordered_map<int, Component::Collider>& colliders, int camX, int camY)
			{
				SDL_SetRenderDrawColor(renderer, 255, 0, 0, 128U);
				//SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
				for (auto& i : colliders)
				{
					int entity = i.first;
				
					if (colliders.find(entity) != colliders.end())
					{
						SDL_FRect rect = i.second.rect;
						rect.x -= camX;
						rect.y -= camY;
						SDL_RenderRect(renderer, &rect);
					}
				}
			}


			static void UpdateButtons(std::unordered_map<int, Component::Button>& buttons)
			{
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


		};
		class Entity
		{
		
			static int nextEntity;
		public:
			static std::vector<int> entities;
			static std::unordered_map<int, std::unordered_map<int, Component>> components;
			static std::unordered_map<int, Component::Velocity> velocities;
			static std::unordered_map<int, Component::Position> positions;
			static std::unordered_map<int, Component::Sprite> sprites;
			static std::unordered_map<int, Component::Collider> colliders;
			static std::unordered_map<int, Component::Animation> animations;
			static std::unordered_map<int, Component::Physics> physics;
			static std::unordered_map<int, Component::Rotatable> rotatables;
			static std::unordered_map<int, Component::Parallax> parallaxes;
			static std::unordered_map<int, Component::Button> buttons;
			static std::unordered_map<int, Component::Label> labels;
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
			template<typename T>
			static T* GetComponent(int entity)
			{
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
			ECS::Component::Button* button;

			void PlayAudio()
			{
				SDL_AudioSpec spec;
				char* wav_path = NULL;
				//SDL_LoadWAV(const char* path, SDL_AudioSpec * spec, Uint8 * *audio_buf, Uint32 * audio_len);
				SDL_AudioStream* stream = NULL;
				Uint8* wav_data = NULL;
				Uint32 wav_len = 0;


				SDL_asprintf(&wav_path, "assets/audio/music/ForestDayMorning.wav", SDL_GetBasePath());

				SDL_free(wav_path);  /* done with this string. */

				/* Create our audio stream in the same format as the .wav file. It'll convert to what the audio hardware wants. */
				stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);

				SDL_ResumeAudioStreamDevice(stream);
			}
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
			int CreateFloor(SDL_Renderer* renderer, int x, int y, int w, int h)
			{
				int ground = Entity::CreateEntity();
				auto ground_texture = System::MakeRectangleTexture(renderer, w, h, 128, 0, 128);
				SDL_FRect ground_rect = { x, y, w, h };

				auto ground_collider = System::CreateCollidersFromTexture(ground_texture, ground_rect.x, ground_rect.y);
				Entity::AddComponent(ground, Component::Sprite{ ground_texture, -1 }, Entity::sprites);
				Entity::AddComponent(ground, Component::Collider{ ground_rect, 0, 0, true }, Entity::colliders);
				Entity::AddComponent(ground, Component::Position{ ground_rect.x, ground_rect.y }, Entity::positions);
				return ground;

			}


			int CreateButton(SDL_Renderer* renderer, int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255U)
			{
				int button = Entity::CreateEntity();
				SDL_Rect rect;
				rect.x = x;
				rect.y = y;
				rect.w = w;
				rect.h = h;
				SDL_Texture* texture = System::MakeRectangleTexture(renderer, w, h, r, g, b, a);

				Entity::AddComponent(button, Component::Sprite{ texture, 69420 }, Entity::sprites);
				Entity::AddComponent(button, Component::Position{ (double)x, (double)y }, Entity::positions);
				Entity::AddComponent(button, Component::Parallax{ 0.0, 0.0 }, Entity::parallaxes);
				Entity::AddComponent(button, Component::Button{ rect }, Entity::buttons);
				return button;
			}

			int CreateLabel(SDL_Renderer* renderer, std::string string, int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255U)
			{
				int label = Entity::CreateEntity();
				SDL_Color color = { r, g, b, a };
				SDL_Surface* surface = nullptr;
				SDL_Texture* texture = nullptr;



				surface = TTF_RenderText_Blended(font, string.c_str(), 0, color);
				texture = SDL_CreateTextureFromSurface(renderer, surface);
				SDL_DestroySurface(surface);

				float w, h;
				SDL_GetTextureSize(texture, &w, &h);

				Entity::AddComponent(label, Component::Label{ string, color }, Entity::labels);
				Entity::AddComponent(label, Component::Sprite{ texture, 69421 }, Entity::sprites);
				Entity::AddComponent(label, Component::Position{ (double)x, (double)y }, Entity::positions);
				Entity::AddComponent(label, Component::Parallax{ 0.0f, 0.0f }, Entity::parallaxes);
				return label;
			}

			int CreateButtonWithLabel(SDL_Renderer* renderer, std::string string, int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255U)
			{
				int button = Entity::CreateEntity();
				int label = Entity::CreateEntity();
				SDL_Rect buttonRect = { x, y, w, h };
				SDL_Rect labelRect;
				SDL_Color bcolor = { r, g, b, a };
				SDL_Color lcolor = { 255U, 255U, 255U, 255U };
				SDL_Surface* surface = nullptr;
				SDL_Texture* ltexture = nullptr;
				SDL_Texture* btexture = System::MakeRectangleTexture(renderer, buttonRect.w, buttonRect.h, r, g, b, a);



				surface = TTF_RenderText_Blended(font, string.c_str(), 0, lcolor);
				ltexture = SDL_CreateTextureFromSurface(renderer, surface);
				SDL_DestroySurface(surface);

				float lw, lh;
				SDL_GetTextureSize(ltexture, &lw, &lh);

				labelRect = { (int)((buttonRect.x + buttonRect.w / 2) - lw/2), (int)((buttonRect.y + buttonRect.h / 2) - lh/2), (int)lw, (int)lh };


				Entity::AddComponent(label, Component::Label{ string, lcolor }, Entity::labels);
				Entity::AddComponent(label, Component::Sprite{ ltexture, 69421 }, Entity::sprites);
				Entity::AddComponent(label, Component::Position{ (double)labelRect.x, (double)labelRect.y }, Entity::positions);
				Entity::AddComponent(label, Component::Parallax{ 0.0f, 0.0f }, Entity::parallaxes);

				Entity::AddComponent(button, Component::Label{ string, bcolor }, Entity::labels);
				Entity::AddComponent(button, Component::Sprite{ btexture, 69420 }, Entity::sprites);
				Entity::AddComponent(button, Component::Position{ (double)x, (double)y }, Entity::positions);
				Entity::AddComponent(button, Component::Parallax{ 0.0, 0.0 }, Entity::parallaxes);
				Entity::AddComponent(button, Component::Button{ buttonRect }, Entity::buttons);
				return button;
			}
			void Init(SDL_Renderer* renderer)
			{
				PlayAudio("assets/audio/music/ForestDayMorning.wav");
				int sun = Entity::CreateEntity();
				Entity::AddComponent(sun, Component::Sprite{TextureManager::Load(renderer, "assets/textures/sun.png"), -420}, Entity::sprites);
				Entity::AddComponent(sun, Component::Position{}, Entity::positions);
				Entity::AddComponent(sun, Component::Parallax{0.00000001, 0.0000001 }, Entity::parallaxes);
				CreateClouds(420, renderer);


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


				CreateFloor(renderer, 0, 128, 32, 8);
				CreateFloor(renderer, 64, 64, 32, 8);
				CreateFloor(renderer, 192, 32, 32, 8);
				CreateFloor(renderer, 256, 0, 32, 8);
				CreateFloor(renderer, 128, -64, 32, 8);
				CreateFloor(renderer, 64, -64, 32, 8);
				CreateFloor(renderer, 0, -128, 32, 8);


				buttonId = CreateButtonWithLabel(renderer, "Meow", 100, 100, 200, 100, 255 / 2, 255 / 2, 255 / 2);


				Entity::AddComponent(camera, Component::Position{0, 0}, Entity::positions);
			
				/*
				Entity::AddComponent(world, Component::Position{ 0, 0 }, Entity::positions);
				Entity::AddComponent(world, Component::Sprite{ TextureManager::Load(renderer, "assets/textures/icon.png"), 69420 }, Entity::sprites);

				*/
				Entity::AddComponent(player, Component::Position{ 0, 0 }, Entity::positions);
				Entity::AddComponent(player, Component::Velocity{ 0, 0 }, Entity::velocities);
				Entity::AddComponent(player, Component::Sprite{ TextureManager::Load(renderer, "assets/textures/player_spritesheet.png"), 420 }, Entity::sprites);
				Entity::AddComponent(player, Component::Animation{ 0, 0.0f, 100.0f, true, {}, 1.0f, 0, 0, 8 }, Entity::animations);
				Entity::AddComponent(player, Component::Physics{ 100.0, 0.0f}, Entity::physics);
				Entity::AddComponent(player, Component::Rotatable{}, Entity::rotatables);
				Entity::AddComponent(player, Component::Collider{ SDL_FRect{0, 0, 16, 32}, 8}, Entity::colliders);

			
				camera_position = Entity::GetComponent(camera, Entity::positions);
				player_position = Entity::GetComponent(player, Entity::positions);
				player_velocity = Entity::GetComponent(player, Entity::velocities);
				player_sprite = Entity::GetComponent(player, Entity::sprites);
				player_animation = Entity::GetComponent(player, Entity::animations);
				player_physics = Entity::GetComponent(player, Entity::physics);
				player_rotation = Entity::GetComponent(player, Entity::rotatables);

				button = Entity::GetComponent(buttonId, Entity::buttons);
				Entity::GetComponent<Component::Position>(1);
			}
			double speed;
			void Loop(SDL_Renderer* renderer, double elapsed)
			{
			
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
			void PrepareFrame(SDL_Renderer* renderer, double elapsed)
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

			void CreateClouds(int howMany, SDL_Renderer* renderer)
			{
				for (int i = 0; i < howMany; ++i)
				{
					int cloud = Entity::CreateEntity();
					auto cloudTexture = TextureManager::Load(renderer, "assets/textures/clouds.png");
					SDL_SetTextureAlphaMod(cloudTexture, std::max(21, rand() % 255));
					Entity::AddComponent(cloud, Component::Position{(double)(rand() % 420), (double)(rand() % 420) }, Entity::positions);
					Entity::AddComponent(cloud, Component::Velocity{(double)(rand() % 69)}, Entity::velocities);
					Entity::AddComponent(cloud, Component::Sprite{ cloudTexture, -420}, Entity::sprites);
					Entity::AddComponent(cloud, Component::Parallax
						{ 
							(float)(0.01 + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (0.1 - 0.01)),
							(float)(0.01 + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (0.1 - 0.01)) 
						}, 
						Entity::parallaxes);
					Entity::AddComponent(cloud, Component::Animation{rand() % 2, 0.0f, 0.0f, false, "", 1.0f, rand() % 2, 2, 2}, Entity::animations);
					clouds.push_back(cloud);
				}
			}
			void MoveClouds()
			{
				for (auto& i : clouds)
				{
					auto cloud_pos = Entity::GetComponent(i, Entity::positions);
					if (cloud_pos->x > 1280)
					{
						auto cloud_speed = Entity::GetComponent(i, Entity::velocities);
						cloud_speed->x = rand() % 69;
						cloud_pos->x = -420;
					}
				}
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
	std::unordered_map<int, ECS::Component::Physics> ECS::Entity::physics = {};
	std::unordered_map<int, ECS::Component::Rotatable> ECS::Entity::rotatables = {};
	std::unordered_map<int, ECS::Component::Parallax> ECS::Entity::parallaxes = {};
	std::unordered_map<int, ECS::Component::Button> ECS::Entity::buttons = {};
	std::unordered_map<int, ECS::Component::Label> ECS::Entity::labels = {};


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

		SDL_Event event;
		bool isRunning = true;

		ECS::Game game = ECS::Game();
		game.window = &window;
		game.Init(window.GetRenderer());

	
		

		double elapsed = 0.016;
		while (isRunning)
		{
			
			// process inputs -> process behaviors -> calculate velocities -> move entities -> resolve collisions -> render frame
			auto begin = std::chrono::high_resolution_clock::now();
		
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
		
			ECS::System::UpdateButtons(ECS::Entity::buttons);
			game.Loop(window.GetRenderer(), elapsed);
			ECS::System::UpdatePhysics(ECS::Entity::physics, ECS::Entity::velocities, elapsed);
			ECS::System::UpdateAnimationFrames(ECS::Entity::animations, elapsed);
			ECS::System::Move(ECS::Entity::velocities, ECS::Entity::positions, elapsed);
			ECS::System::Collide(ECS::Entity::colliders, ECS::Entity::positions, ECS::Entity::velocities, ECS::Entity::physics);
			SDL_SetRenderDrawColor(window.GetRenderer(), 135, 197, 255, SDL_ALPHA_OPAQUE);
		


			game.PrepareFrame(window.GetRenderer(), elapsed);
			SDL_RenderClear(window.GetRenderer());
			ECS::System::Draw(window.GetRenderer(), ECS::Entity::sprites, ECS::Entity::animations, ECS::Entity::parallaxes, ECS::Entity::rotatables,ECS::Entity::positions, game.camera_position->x, game.camera_position->y,	game.window->width, game.window->height);
		
		
			// remove this later
			ECS::System::DrawColliders(window.GetRenderer(), ECS::Entity::colliders, game.camera_position->x, game.camera_position->y);


			SDL_RenderPresent(window.GetRenderer());

			auto end = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> deltaTime = end - begin;
			//std::cout << "FPS: " << 1.0 / deltaTime.count() << "\n";
			elapsed = (deltaTime.count());

		
		}

		SDL_Quit();
		return 0;  
	}


#endif