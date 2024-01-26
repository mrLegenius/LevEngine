#include "levpch.h"
#include "LuaComponentsBinder.h"

#include <Assets/AnimationAsset.h>
#include <Scene/Components/Animation/AnimatorComponent.h>

#include "MetaUtilities.h"
#include "ResourceManager.h"
#include "AI/Components/AIAgentComponent.h"
#include "AI/Components/AIAgentCrowdComponent.h"
#include "Assets/PrefabAsset.h"
#include "Audio/Audio.h"
#include "GUI/GUI.h"
#include "Input/Input.h"
#include "Kernel/Application.h"
#include "Kernel/Time/Time.h"
#include "Physics/Physics.h"
#include "Physics/Components/CharacterController.h"
#include "Physics/Components/Rigidbody.h"
#include "Renderer/DebugRender/DebugRender.h"
#include "Scene/Entity.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "Scene/Components/Camera/Camera.h"
#include "Scene/Components/MeshRenderer/MeshRenderer.h"
#include "Scene/Components/ScriptsContainer/ScriptsContainer.h"
#include "Scene/Components/Transform/Transform.h"

namespace LevEngine::Scripting
{
    void LuaComponentsBinder::CreateInputLuaBind(sol::state& lua)
    {
        lua.new_usertype<Input>(
            "Input",
            sol::no_constructor,
            "getMouseDelta", &Input::GetMouseDelta,
            "isKeyDown", [](int key)
            {
                return Input::IsKeyDown(static_cast<KeyCode>(key));
            },
            "isKeyUp", [](int key)
            {
                return Input::IsKeyUp(static_cast<KeyCode>(key));
            },
            "isKeyPressed", [](int key)
            {
                return Input::IsKeyPressed(static_cast<KeyCode>(key));
            },
            "isKeyReleased", [](int key)
            {
                return Input::IsKeyReleased(static_cast<KeyCode>(key));
            },
            "isMouseButtonPressed", [](int key)
            {
                return Input::IsMouseButtonPressed(static_cast<MouseButton>(key));
            },
            "isMouseButtonReleased", [](int key)
            {
                return Input::IsMouseButtonReleased(static_cast<MouseButton>(key));
            },
            "isMouseButtonDown", [](int key)
            {
                return Input::IsMouseButtonDown(static_cast<MouseButton>(key));
            },
            "isMouseButtonUp", [](int key)
            {
                return Input::IsMouseButtonUp(static_cast<MouseButton>(key));
            }
        );

        lua.new_enum(
            "MouseButton",
            "Left", MouseButton::Left,
            "Right", MouseButton::Right,
            "Middle", MouseButton::Middle);

        lua.new_enum<false>(
            "KeyCode",
            "None", KeyCode::None,
            "LeftButton", KeyCode::LeftButton,
            "RightButton", KeyCode::RightButton,
            "MiddleButton", KeyCode::MiddleButton,
            "MouseButtonX1", KeyCode::MouseButtonX1,
            "MouseButtonX2", KeyCode::MouseButtonX2,
            "MouseButtonX3", KeyCode::MouseButtonX3,
            "WheelUp", KeyCode::WheelUp,
            "WheelDown", KeyCode::WheelDown,
            "Back", KeyCode::Back,
            "Tab", KeyCode::Tab,
            "Enter", KeyCode::Enter,
            "Pause", KeyCode::Pause,
            "CapsLock", KeyCode::CapsLock,
            "Kana", KeyCode::Kana,
            "Kanji", KeyCode::Kanji);
        sol::table luaKeyCode = lua["KeyCode"];
        luaKeyCode.set(
            "Escape", KeyCode::Escape,
            "ImeConvert", KeyCode::ImeConvert,
            "ImeNoConvert", KeyCode::ImeNoConvert,
            "Space", KeyCode::Space,
            "PageUp", KeyCode::PageUp,
            "PageDown", KeyCode::PageDown,
            "End", KeyCode::End,
            "Home", KeyCode::Home,
            "Left", KeyCode::Left,
            "Up", KeyCode::Up,
            "Right", KeyCode::Right,
            "Down", KeyCode::Down,
            "Select", KeyCode::Select,
            "Print", KeyCode::Print,
            "Execute", KeyCode::Execute,
            "PrintScreen", KeyCode::PrintScreen,
            "Insert", KeyCode::Insert,
            "Delete", KeyCode::Delete,
            "Help", KeyCode::Help
        );
        luaKeyCode.set(
            "D0", KeyCode::D0,
            "D1", KeyCode::D1,
            "D2", KeyCode::D2,
            "D3", KeyCode::D3,
            "D4", KeyCode::D4,
            "D5", KeyCode::D5,
            "D6", KeyCode::D6,
            "D7", KeyCode::D7,
            "D8", KeyCode::D8,
            "D9", KeyCode::D9
        );
        luaKeyCode.set(
            "A", KeyCode::A,
            "B", KeyCode::B,
            "C", KeyCode::C,
            "D", KeyCode::D,
            "E", KeyCode::E,
            "F", KeyCode::F,
            "G", KeyCode::G,
            "H", KeyCode::H,
            "I", KeyCode::I,
            "J", KeyCode::J,
            "K", KeyCode::K,
            "L", KeyCode::L,
            "M", KeyCode::M,
            "N", KeyCode::N,
            "O", KeyCode::O,
            "P", KeyCode::P,
            "Q", KeyCode::Q,
            "R", KeyCode::R,
            "S", KeyCode::S,
            "T", KeyCode::T,
            "U", KeyCode::U,
            "V", KeyCode::V,
            "W", KeyCode::W,
            "X", KeyCode::X,
            "Y", KeyCode::Y,
            "Z", KeyCode::Z
        );

        luaKeyCode.set(
            "LeftWindows", KeyCode::LeftWindows,
            "RightWindows", KeyCode::RightWindows,
            "Apps", KeyCode::Apps,
            "Sleep", KeyCode::Sleep,
            "NumPad0", KeyCode::NumPad0,
            "NumPad1", KeyCode::NumPad1,
            "NumPad2", KeyCode::NumPad2,
            "NumPad3", KeyCode::NumPad3,
            "NumPad4", KeyCode::NumPad4,
            "NumPad5", KeyCode::NumPad5,
            "NumPad6", KeyCode::NumPad6,
            "NumPad7", KeyCode::NumPad7,
            "NumPad8", KeyCode::NumPad8,
            "NumPad9", KeyCode::NumPad9,
            "Multiply", KeyCode::Multiply,
            "Add", KeyCode::Add,
            "Separator", KeyCode::Separator,
            "Subtract", KeyCode::Subtract,
            "Decimal", KeyCode::Decimal,
            "Divide", KeyCode::Divide
        );
        luaKeyCode.set(
            "F1", KeyCode::F1,
            "F2", KeyCode::F2,
            "F3", KeyCode::F3,
            "F4", KeyCode::F4,
            "F5", KeyCode::F5,
            "F6", KeyCode::F6,
            "F7", KeyCode::F7,
            "F8", KeyCode::F8,
            "F9", KeyCode::F9,
            "F10", KeyCode::F10,
            "F11", KeyCode::F11,
            "F12", KeyCode::F12,
            "F13", KeyCode::F13,
            "F14", KeyCode::F14,
            "F15", KeyCode::F15,
            "F16", KeyCode::F16,
            "F17", KeyCode::F17,
            "F18", KeyCode::F18,
            "F19", KeyCode::F19,
            "F20", KeyCode::F20,
            "F21", KeyCode::F21,
            "F22", KeyCode::F22,
            "F23", KeyCode::F23,
            "F24", KeyCode::F24
        );
        luaKeyCode.set(
            "NumLock", KeyCode::NumLock,
            "Scroll", KeyCode::Scroll,
            "LeftShift", KeyCode::LeftShift,
            "RightShift", KeyCode::RightShift,
            "LeftControl", KeyCode::LeftControl,
            "RightControl", KeyCode::RightControl,
            "LeftAlt", KeyCode::LeftAlt,
            "RightAlt", KeyCode::RightAlt,
            "BrowserBack", KeyCode::BrowserBack,
            "BrowserForward", KeyCode::BrowserForward,
            "BrowserRefresh", KeyCode::BrowserRefresh,
            "BrowserStop", KeyCode::BrowserStop,
            "BrowserSearch", KeyCode::BrowserSearch,
            "BrowserFavorites", KeyCode::BrowserFavorites,
            "BrowserHome", KeyCode::BrowserHome,
            "VolumeMute", KeyCode::VolumeMute,
            "VolumeDown", KeyCode::VolumeDown,
            "VolumeUp", KeyCode::VolumeUp,
            "MediaNextTrack", KeyCode::MediaNextTrack,
            "MediaPreviousTrack", KeyCode::MediaPreviousTrack,
            "MediaStop", KeyCode::MediaStop,
            "MediaPlayPause", KeyCode::MediaPlayPause,
            "LaunchMail", KeyCode::LaunchMail,
            "SelectMedia", KeyCode::SelectMedia,
            "LaunchApplication1", KeyCode::LaunchApplication1,
            "LaunchApplication2", KeyCode::LaunchApplication2,
            "OemSemicolon", KeyCode::OemSemicolon,
            "OemPlus", KeyCode::OemPlus,
            "OemComma", KeyCode::OemComma,
            "OemMinus", KeyCode::OemMinus,
            "OemPeriod", KeyCode::OemPeriod,
            "OemQuestion", KeyCode::OemQuestion,
            "OemTilde", KeyCode::OemTilde,
            "ChatPadGreen", KeyCode::ChatPadGreen,
            "ChatPadOrange", KeyCode::ChatPadOrange,
            "OemOpenBrackets", KeyCode::OemOpenBrackets,
            "OemPipe", KeyCode::OemPipe,
            "OemCloseBrackets", KeyCode::OemCloseBrackets,
            "OemQuotes", KeyCode::OemQuotes,
            "Oem8", KeyCode::Oem8,
            "OemBackslash", KeyCode::OemBackslash,
            "ProcessKey", KeyCode::ProcessKey,
            "OemCopy", KeyCode::OemCopy,
            "OemAuto", KeyCode::OemAuto,
            "OemEnlW", KeyCode::OemEnlW,
            "Attn", KeyCode::Attn,
            "Crsel", KeyCode::Crsel,
            "Exsel", KeyCode::Exsel,
            "EraseEof", KeyCode::EraseEof,
            "Play", KeyCode::Play,
            "Zoom", KeyCode::Zoom,
            "Pa1", KeyCode::Pa1,
            "OemClear", KeyCode::OemClear
        );
    }

    void LuaComponentsBinder::CreateTransformLuaBind(sol::state& lua)
    {
        lua.new_usertype<Transform>(
            "Transform",
            "type_id", &entt::type_hash<Transform>::value,
            sol::call_constructor,
            sol::factories(
                []()
                {
                    return Transform();
                },
                [](const Entity entity)
                {
                    return Transform(entity);
                }),
            //Position
            "getLocalPosition", &Transform::GetLocalPosition,
            "setLocalPosition", &Transform::SetLocalPosition,
            "getWorldPosition", &Transform::GetWorldPosition,
            "setWorldPosition", &Transform::SetWorldPosition,
            //Scale
            "getLocalScale", &Transform::GetLocalScale,
            "setLocalScale", &Transform::SetLocalScale,
            "getWorldScale", &Transform::GetWorldScale,
            "setWorldScale", &Transform::SetWorldScale,
            //Rotation
            "getLocalRotation", &Transform::GetLocalRotation,
            "setLocalRotation", &Transform::SetLocalRotation,
            "getWorldRotation", &Transform::GetWorldRotation,
            "setWorldRotation", &Transform::SetWorldRotation,
            //Direction
            "getForwardDirection", &Transform::GetForwardDirection,
            "getRightDirection", &Transform::GetRightDirection,
            "getUpDirection", &Transform::GetUpDirection,
            //Parenting
            "getChild", [](const Transform& transform, const int index)
            {
                const auto children = transform.GetChildren();
                LEV_ASSERT(index < children.size() && index > -1)
                return children[index];
            },
            "removeChild", &Transform::RemoveChild,
            "setParent", sol::overload(
                &Transform::SetParent,
                [](Transform& transform, const Entity entity)
                {
                    transform.SetParent(entity);
                }),
            "getParent", &Transform::GetParent,
            "getChildrenCount", &Transform::GetChildrenCount,
            //Movement
            "move", &Transform::Move,
            "moveForward", &Transform::MoveForward,
            "moveBackward", &Transform::MoveBackward,
            "moveRight", &Transform::MoveRight,
            "moveLeft", &Transform::MoveLeft,
            "moveUp", &Transform::MoveUp,
            "moveDown", &Transform::MoveDown
        );
    }

    void LuaComponentsBinder::CreateMeshRendererComponentLuaBind(sol::state& lua)
    {
        lua.new_usertype<MeshRendererComponent>(
            "MeshRendererComponent",
            "type_id", &entt::type_hash<MeshRendererComponent>::value,
            sol::call_constructor,
            sol::factories(
                []()
                {
                    return MeshRendererComponent{};
                }),
            "enabled", &MeshRendererComponent::enabled,
            "castShadow", &MeshRendererComponent::castShadow
        );
    }

    void LuaComponentsBinder::CreateCameraComponentLuaBind(sol::state& lua)
    {
        lua.new_usertype<CameraComponent>(
            "CameraComponent",
            "type_id", &entt::type_hash<CameraComponent>::value,
            sol::call_constructor,
            sol::factories(
                []()
                {
                    return CameraComponent{};
                }),
            "isMain", &CameraComponent::IsMain,
            "fixAspectRatio", &CameraComponent::FixedAspectRatio
        );
    }

    void LuaComponentsBinder::CreateScriptsContainerLuaBind(sol::state& lua)
    {
        lua.new_usertype<ScriptsContainer>(
            "ScriptsContainer",
            "type_id", &entt::type_hash<ScriptsContainer>::value,
            sol::call_constructor,
            sol::factories(
                []()
                {
                    return ScriptsContainer{};
                }),
            sol::meta_function::index, &ScriptsContainer::Get,
            sol::meta_function::new_index, &ScriptsContainer::Set
        );
    }

    void LuaComponentsBinder::CreateRigidbodyLuaBind(sol::state& lua)
    {
        lua.new_usertype<Rigidbody>(
            "Rigidbody",
            "type_id", &entt::type_hash<Rigidbody>::value,
            sol::call_constructor,
            sol::factories(
                []()
                {
                    return Rigidbody{};
                }),
            "addForce", sol::overload(
                &Rigidbody::AddForce,
                [](const Rigidbody& rigidbody, Vector3 vector)
                {
                    rigidbody.AddForce(vector);
                }),
            "initialize", [](Rigidbody& rigidbody, const Entity entity)
            {
                rigidbody.Initialize(entity);
            },
            "getTriggerEnterBuffer", &Rigidbody::GetTriggerEnterBuffer,
            "getTriggerStayBuffer", &Rigidbody::GetTriggerStayBuffer,
            "getTriggerExitBuffer", &Rigidbody::GetTriggerExitBuffer,
            "getCollisionEnterBuffer", &Rigidbody::GetCollisionEnterBuffer,
            "getCollisionStayBuffer", &Rigidbody::GetCollisionStayBuffer,
            "getCollisionExitBuffer", &Rigidbody::GetCollisionExitBuffer,
            "getLayer", [](const Rigidbody& rigidbody)
            {
                return static_cast<int>(rigidbody.GetLayer());
            },
            "setLayer", [](const Rigidbody& rigidbody, int Layer)
            {
                rigidbody.SetLayer(static_cast<FilterLayer>(Layer));
            }, 
            "getSphereRadius",&Rigidbody::GetSphereRadius,
            "setSphereRadius",&Rigidbody::SetSphereRadius,
            "getCapsuleRadius",&Rigidbody::GetCapsuleRadius,
            "setCapsuleRadius",&Rigidbody::SetCapsuleRadius,
            "getCapsuleHalfHeight",&Rigidbody::GetCapsuleHalfHeight,
            "setCapsuleHalfHeight",&Rigidbody::SetCapsuleHalfHeight,
            "getBoxHalfExtents",&Rigidbody::GetBoxHalfExtents,
            "setBoxHalfExtents",&Rigidbody::SetBoxHalfExtents
        );

        lua.new_enum(
            "ForceMode",
            "Force", Rigidbody::ForceMode::Force,
            "Impulse", Rigidbody::ForceMode::Impulse,
            "Acceleration", Rigidbody::ForceMode::Acceleration,
            "VelocityChange", Rigidbody::ForceMode::VelocityChange
        );

        lua.new_usertype<Collision>(
            "Collision",
            "entity", &Collision::Entity,
            "contactCount", &Collision::ContactCount,
            "points", &Collision::Points,
            "normals", &Collision::Normals,
            "impulses", &Collision::Impulses,
            "separations", &Collision::Separations);
    }

    void LuaComponentsBinder::CreateCharacterControllerLuaBind(sol::state& lua)
    {
        lua.new_usertype<CharacterController>(
            "CharacterController",
            "type_id", &entt::type_hash<CharacterController>::value,
            sol::call_constructor,
            sol::factories(
                []()
                {
                    return CharacterController{};
                }),
            "move", &CharacterController::Move,
            "jump", &CharacterController::Jump,
            "getGravityScale", &CharacterController::GetGravityScale,
            "setGravityScale", &CharacterController::SetGravityScale,
            "isGrounded", &CharacterController::IsGrounded,
            "getCollisionHitBuffer", &CharacterController::GetCollisionHitBuffer,
            "getLayer", [](const CharacterController& characterController)
            {
                return static_cast<int>(characterController.GetLayer());
            },
            "setLayer", [](CharacterController& characterController, int Layer)
            {
                characterController.SetLayer(static_cast<FilterLayer>(Layer));
            }
        );

        lua.new_usertype<ControllerColliderHit>(
            "ControllerColliderHit",
            "entity", &ControllerColliderHit::Entity,
            "point", &ControllerColliderHit::Point,
            "normal", &ControllerColliderHit::Normal,
            "moveDirection", &ControllerColliderHit::MoveDirection,
            "moveLength", &ControllerColliderHit::MoveLength);
    }

    void LuaComponentsBinder::CreateAnimatorComponentLuaBind(sol::state& lua)
    {
        lua.new_usertype<AnimatorComponent>(
            "AnimatorComponent",
            "type_id", &entt::type_hash<AnimatorComponent>::value,
            sol::call_constructor,
            sol::factories(
                []()
                {
                    return AnimatorComponent{};
                }),
            "getAnimationClip", [](const AnimatorComponent& animatorComponent)
            {
                return animatorComponent.GetAnimationClipConst()->GetAddress().c_str();
            },
            "setAnimationClip", [](AnimatorComponent& animatorComponent, const std::string& assetAddress)
            {
                animatorComponent.SetAnimationClip(
                    ResourceManager::LoadAsset<AnimationAsset>(assetAddress.c_str()));
            },
            "playAnimation", &AnimatorComponent::PlayAnimation
        );
    }

    void LuaComponentsBinder::CreateLuaEntityBind(sol::state& lua, Scene* scene)
    {
        using namespace Scripting;
        using namespace entt::literals;
        if (const sol::optional<sol::table> entityTable = lua["Entity"];
            entityTable != sol::nullopt)
        {
            auto table = entityTable.value();
            table.set_function(sol::call_constructor,
                               sol::factories(
                                   [&]()
                                   {
                                       return Entity{scene->CreateEntity()};
                                   },
                                   [&](String name)
                                   {
                                       return Entity{scene->CreateEntity(name)};
                                   }
                               ));
        }
        else
        {
            lua.new_usertype<Entity>(
                "Entity",
                sol::call_constructor,
                sol::factories(
                    [&]()
                    {
                        return Entity{scene->CreateEntity()};
                    },
                    [&](String name)
                    {
                        return Entity{scene->CreateEntity(name)};
                    }
                ),
                "add_component", [](Entity& entity, const sol::table& scriptComponent,
                                    sol::this_state state) -> sol::object
                {
                    if (!scriptComponent.valid())
                    {
                        return sol::lua_nil_t{};
                    }

                    const auto component = InvokeMetaFunction(
                        GetIdType(scriptComponent),
                        "add_component"_hs,
                        entity,
                        scriptComponent,
                        state);

                    return component ? component.cast<sol::reference>() : sol::lua_nil_t{};
                },
                "has_component", [](Entity& entity, const sol::table& scriptComponent)
                {
                    const auto result = InvokeMetaFunction(
                        GetIdType(scriptComponent),
                        "has_component"_hs,
                        entity);

                    return result ? result.cast<bool>() : false;
                },
                "get_component", [](Entity& entity, const sol::table& scriptComponent, sol::this_state state)
                {
                    const auto component = InvokeMetaFunction(
                        GetIdType(scriptComponent),
                        "get_component"_hs,
                        entity,
                        state);

                    return component ? component.cast<sol::reference>() : sol::lua_nil_t{};
                },
                "remove_component", [](Entity& entity, const sol::table& scriptComponent)
                {
                    InvokeMetaFunction(
                        GetIdType(scriptComponent),
                        "remove_component"_hs,
                        entity);
                },
                "get_or_add_component", [](Entity& entity, const sol::table& scriptComponent, sol::this_state state)
                {
                    const auto component = InvokeMetaFunction(
                        GetIdType(scriptComponent),
                        "get_or_add_component"_hs,
                        entity,
                        state);

                    return component ? component.cast<sol::reference>() : sol::lua_nil_t{};
                },
                "name", [](const Entity& entity)
                {
                    return entity.GetName().c_str();
                },
                "uuid", [](const Entity& entity)
                {
                    return entity.GetUUID();
                },
                "isValid", [](const Entity& entity)
                {
                    return static_cast<bool>(entity);
                }
            );
        }
    }

    void LuaComponentsBinder::CreateSceneManagerBind(sol::state& lua)
    {
        lua.new_usertype<SceneManager>(
            "SceneManager",
            sol::no_constructor,
            "getActiveScene", &SceneManager::GetActiveScene,
            "getActiveScenePath", []()
            {
                return SceneManager::GetActiveScenePath().string();
            },
            "loadSceneFromPath", [](const std::string& path)
            {
                return SceneManager::RequestSceneLoad(path);
            }
        );
    }

    void LuaComponentsBinder::CreateSceneBind(sol::state& lua)
    {
        lua.new_usertype<Scene>(
            "Scene",
            sol::no_constructor,
            "destroyEntity", [](const Ref<Scene>& scene, Entity entity)
            {
                scene->DestroyEntity(entity);
            }
        );
    }

    void LuaComponentsBinder::CreatePrefabBind(sol::state& lua)
    {
        lua.new_usertype<PrefabAsset>(
            "Prefab",
            sol::call_constructor,
            sol::factories(
                [](const std::string& prefabName)
                {
                    return ResourceManager::LoadAsset<PrefabAsset>(String(prefabName.c_str()));
                }),
            "instantiate", sol::overload(
                [](PrefabAsset& prefabAsset, const Ref<Scene>& scene, const Entity parent) -> Entity
                {
                    return prefabAsset.Instantiate(scene, parent);
                },
                [](PrefabAsset& prefabAsset, const Ref<Scene>& scene) -> Entity
                {
                    return prefabAsset.Instantiate(scene);
                })
        );
    }

    void LuaComponentsBinder::CreateAudioBind(sol::state& lua)
    {
        lua.new_usertype<Audio>(
            "Audio",
            sol::no_constructor,
            "playOneShot",
            [](const std::string& eventName, const Entity entity)
            {
                return Audio::PlayOneShot(String{eventName.c_str()}, entity);
            }
        );
    }

    void LuaComponentsBinder::CreatePrintBind(sol::state& lua)
    {
        lua.set_function("print", [&lua](sol::variadic_args va)
        {
            std::string out;
            for (auto v : va)
            {
                if (auto optionalString = v.as<sol::optional<std::string>>())
                {
                    out.append(optionalString.value());
                    out.append(" ");
                }
                else
                {
                    auto result = lua["tostring"](v);
                    if (result.valid())
                    {
                        std::string string = result;
                        out.append(string);
                        out.append(" ");
                    }
                }
            }
            Log::CoreInfo(out);
        });
    }

    void LuaComponentsBinder::CreateGUIBind(sol::state& lua)
    {
        lua.new_usertype<GUI>(
            "GUI",
            "drawCircle", sol::overload(
                &GUI::DrawCircle,
                [](const Vector2& pos, float radius, const Color& color)
                {
                    GUI::DrawCircle(pos, radius, color);
                }
            ),
            "getWindowSize", &GUI::GetWindowSize,
            "drawString", [](const Vector2& pos, const std::string& text, const float size, const Color& color)
            {
                GUI::DrawString(pos, String{text.c_str()}, size, color);
            }
        );
    }

    void LuaComponentsBinder::CreateTimeBind(sol::state& lua)
    {
        auto timestep_multiplication_overload = sol::overload(
            [](const Timestep& v1, const Timestep& v2)
            {
                return v1 * v2;
            },
            [](const Timestep& v1, float value)
            {
                return v1 * value;
            },
            [](float value, const Timestep& v1)
            {
                return v1 * value;
            }
        );

        auto timestep_addition_overload = sol::overload(
            [](const Timestep& v1, const Timestep& v2)
            {
                return v1 + v2;
            }
        );

        lua.new_usertype<Timestep>(
            "Timestep",
            sol::call_constructor,
            sol::constructors<Timestep(double)>(),
            "getMilliseconds", &Timestep::GetMilliseconds,
            "getSeconds", &Timestep::GetSeconds,
            sol::meta_function::addition, timestep_addition_overload,
            sol::meta_function::multiplication, timestep_multiplication_overload,
            sol::meta_function::to_string, [](const Timestep& timestep)
            {
                return "Timestep " + std::to_string(timestep.GetMilliseconds());
            }
        );

        lua.new_usertype<Time>(
            "Time",
            "getTimeSinceStartup", &Time::GetTimeSinceStartup,
            "getFrameNumber", &Time::GetFrameNumber,
            "getScaledDeltaTime", &Time::GetScaledDeltaTime,
            "GetUnscaledDeltaTime", &Time::GetUnscaledDeltaTime
        );
    }

    void LuaComponentsBinder::CreatePhysicsBind(sol::state& lua)
    {
        lua.new_enum(
            "FilterLayer",
            "None",   FilterLayer::None,
            "Layer0", FilterLayer::Layer0,
            "Layer1", FilterLayer::Layer1,
            "Layer2", FilterLayer::Layer2,
            "Layer3", FilterLayer::Layer3,
            "Layer4", FilterLayer::Layer4,
            "Layer5", FilterLayer::Layer5,
            "Layer6", FilterLayer::Layer6,
            "Layer7", FilterLayer::Layer7,
            "Layer8", FilterLayer::Layer8,
            "Layer9", FilterLayer::Layer9,
            "All",    FilterLayer::All
        );

        lua.new_usertype<RaycastHit>(
            "RaycastHit",
            sol::call_constructor,
            sol::factories(
                []()
                {
                    RaycastHit{};
                }
            ),
            "isSuccessful", &RaycastHit::IsSuccessful,
            "entity", &RaycastHit::Entity,
            "point", &RaycastHit::Point,
            "normal", &RaycastHit::Normal,
            "distance", &RaycastHit::Distance
        );

        auto physics = lua["Physics"].get_or_create<sol::table>();
        physics.set_function(
            "getGravity", []()
            {
                return Application::Get().GetPhysics().GetGravity();
            }
        );

        physics.set_function(
            "raycast",
            sol::overload(
                [](const Vector3 origin, const Vector3 direction, const float maxDistance, const FilterLayer layerMask)
                {
                    return Application::Get().GetPhysics().Raycast(
                        origin,
                        direction,
                        maxDistance,
                        layerMask
                    );
                },
                [](const Vector3 origin, const Vector3 direction, const float maxDistance)
                {
                    return Application::Get().GetPhysics().Raycast(origin, direction, maxDistance);
                }
            )
        );

        physics.set_function(
            "sphereCast",
            sol::overload(
                [](
                    const Vector3 origin,
                    const float radius,
                    const Vector3 direction,
                    const float maxDistance,
                    const FilterLayer layerMask
                )
                {
                    return Application::Get().GetPhysics().SphereCast(
                        origin,
                        radius,
                        direction,
                        maxDistance,
                        layerMask
                    );
                },
                [](const Vector3 origin, const float radius, const Vector3 direction, const float maxDistance)
                {
                    return Application::Get().GetPhysics().SphereCast(origin, radius, direction, maxDistance);
                }
            )
        );

        physics.set_function(
            "capsuleCast",
            sol::overload(
                [](
                    const Vector3 origin,
                    const Quaternion orientation,
                    const float radius,
                    const float halfHeight,
                    const Vector3 direction,
                    const float maxDistance,
                    const FilterLayer layerMask
                )
                {
                    return Application::Get().GetPhysics().CapsuleCast(
                        origin,
                        orientation,
                        radius,
                        halfHeight,
                        direction,
                        maxDistance,
                        layerMask
                    );
                },
                [](
                    const Vector3 origin,
                    const Quaternion orientation,
                    const float radius,
                    const float halfHeight,
                    const Vector3 direction,
                    const float maxDistance
                )
                {
                    return Application::Get().GetPhysics().CapsuleCast(
                        origin,
                        orientation,
                        radius,
                        halfHeight,
                        direction,
                        maxDistance
                    );
                }
            )
        );

        physics.set_function(
            "boxCast",
            sol::overload(
                [](
                    const Vector3 origin,
                    const Quaternion orientation,
                    const Vector3 halfExtents,
                    const Vector3 direction,
                    const float maxDistance,
                    const FilterLayer layerMask
                )
                {
                    return Application::Get().GetPhysics().BoxCast(
                        origin,
                        orientation,
                        halfExtents,
                        direction,
                        maxDistance,
                        layerMask
                    );
                },
                [](
                    const Vector3 origin,
                    const Quaternion orientation,
                    const Vector3 halfExtents,
                    const Vector3 direction,
                    const float maxDistance
                )
                {
                    return Application::Get().GetPhysics().BoxCast(origin, orientation, halfExtents, direction, maxDistance);
                }
            )
        );
    }

    void LuaComponentsBinder::CreateDebugRenderBind(sol::state& lua)
    {
        auto debugRender = lua["DebugRender"].get_or_create<sol::table>();

        debugRender.set_function(
            "drawCube",
            sol::overload(
                sol::resolve<void(Vector3, Vector3, Color)>(&DebugRender::DrawCube),
                sol::resolve<void(Vector3, Vector3, Color, float)>(&DebugRender::DrawCube)
            )
        );

        debugRender.set_function(
            "drawWireCube",
            sol::overload(
                sol::resolve<void(Vector3, Vector3, Color)>(&DebugRender::DrawWireCube),
                sol::resolve<void(Vector3, Vector3, Color, float)>(&DebugRender::DrawWireCube),
                sol::resolve<void(Vector3, Quaternion, Vector3, Color)>(&DebugRender::DrawWireCube),
                sol::resolve<void(Vector3, Quaternion, Vector3, Color, float)>(&DebugRender::DrawWireCube)
            )
        );

        debugRender.set_function(
            "drawSphere",
            sol::overload(
                sol::resolve<void(Vector3, float, Color)>(&DebugRender::DrawSphere),
                sol::resolve<void(Vector3, float, Color, float)>(&DebugRender::DrawSphere)
            )
        );

        debugRender.set_function(
            "drawWireSphere",
            sol::overload(
                sol::resolve<void(Vector3, float, Color)>(&DebugRender::DrawWireSphere),
                sol::resolve<void(Vector3, float, Color, float)>(&DebugRender::DrawWireSphere)
            )
        );

        debugRender.set_function(
            "drawWireCapsule",
            sol::overload(
                sol::resolve<void(Vector3, Quaternion, float, float, Color)>(&DebugRender::DrawWireCapsule),
                sol::resolve<void(Vector3, Quaternion, float, float, Color, float)>(&DebugRender::DrawWireCapsule)
            )
        );

        debugRender.set_function(
            "drawPoint",
            sol::overload(
                sol::resolve<void(Vector3, Color)>(&DebugRender::DrawPoint),
                sol::resolve<void(Vector3, Color, float)>(&DebugRender::DrawPoint)
            )
        );

        debugRender.set_function(
            "drawPointStar",
            sol::overload(
                sol::resolve<void(Vector3, Color)>(&DebugRender::DrawPointStar),
                sol::resolve<void(Vector3, Color, float)>(&DebugRender::DrawPointStar)
            )
        );

        debugRender.set_function(
            "drawLine",
            sol::overload(
                sol::resolve<void(Vector3, Vector3, Color)>(&DebugRender::DrawLine),
                sol::resolve<void(Vector3, Vector3, Color, float)>(&DebugRender::DrawLine)
            )
        );

        debugRender.set_function(
            "drawRay",
            sol::overload(
                sol::resolve<void(Vector3, Vector3, Color)>(&DebugRender::DrawRay),
                sol::resolve<void(Vector3, Vector3, Color, float)>(&DebugRender::DrawRay)
            )
        );

        debugRender.set_function(
            "drawCircle",
            sol::overload(
                sol::resolve<void(Vector3, float, Quaternion, Color)>(&DebugRender::DrawCircle),
                sol::resolve<void(Vector3, float, Quaternion, Color, float)>(&DebugRender::DrawCircle)
            )
        );

        debugRender.set_function(
            "drawGrid",
            sol::resolve<void(Vector3, Vector3, Vector3, uint32_t, uint32_t, float, Color)>(&DebugRender::DrawGrid)
        );
    }

    void LuaComponentsBinder::CreateAIAgentCrowdComponentLuaBind(sol::state& lua)
    {
        lua.new_usertype<AIAgentCrowdComponent>(
            "AIAgentCrowdComponent",
            "type_id", &entt::type_hash<AIAgentCrowdComponent>::value,
            sol::call_constructor,
            sol::factories(
                []()
                {
                    return AIAgentCrowdComponent{};
                }),
            "addAgent", &AIAgentCrowdComponent::AddAgent,
            "getRandomPointOnNavMesh", &AIAgentCrowdComponent::GetRandomPointOnNavMesh,
            "getRandomPointAroundCircle", &AIAgentCrowdComponent::GetRandomPointAroundCircle,
            "getNearestPoint", &AIAgentCrowdComponent::GetNearestPoint
        );
    }

    void LuaComponentsBinder::CreateRuleBind(sol::state& lua)
    {
        lua.new_usertype<Rule>(
             "Rule",
             sol::call_constructor,
             sol::constructors<Rule(const std::string&)>(),
             "addCondition", sol::overload(
             [](Rule& rule, const std::string& id, const std::string& attribute, const std::string& operation, bool value)
             {
                 rule.AddCondition(id.c_str(), attribute.c_str(), operation.c_str(), value);
             },
             [](Rule& rule, const std::string& id, const std::string& attribute, const std::string& operation, float value)
             {
                 rule.AddCondition(id.c_str(), attribute.c_str(), operation.c_str(), value);
             },
             [](Rule& rule, const std::string& id, const std::string& attribute, const std::string& operation, const Vector3& value)
             {
                 rule.AddCondition(id.c_str(), attribute.c_str(), operation.c_str(), value);
             },
             [](Rule& rule, const std::string& id, const std::string& attribute, const std::string& operation, const String& value)
             {
                 rule.AddCondition(id.c_str(), attribute.c_str(), operation.c_str(), value);
             }));
    }

    void LuaComponentsBinder::CreateAIAgentComponentLuaBind(sol::state& lua)
    {
        lua.new_usertype<AIAgentComponent>(
            "AIAgentComponent",
            "type_id", &entt::type_hash<AIAgentComponent>::value,
            sol::call_constructor,
            sol::factories(
                []()
                {
                    return AIAgentComponent{};
                }),
            "getCrowd", &AIAgentComponent::GetCrowd,
            "addRule", &AIAgentComponent::AddRule,
            "match", [](AIAgentComponent& agentComponent)
            {
                return agentComponent.Match().c_str();
            },
            "initRBS", &AIAgentComponent::InitRBS,
            "isRBSInited", &AIAgentComponent::IsRBSInited,
            "findEntitiesInVisibleScope", &AIAgentComponent::FindEntitiesInVisibleScope,
            "findClosestEntityInVisibleScope", &AIAgentComponent::FindClosestEntityInVisibleScope,
            "setMoveTarget", &AIAgentComponent::SetMoveTarget,
            //Bool facts
            "setFactAsBool", [](AIAgentComponent& agentComponent, const std::string& id, const std::string& attribute, bool value)
            {
                agentComponent.SetFactAsBool(MakePair(id.c_str(), attribute.c_str()), value);
            },
            "hasBoolFact", [](AIAgentComponent& agentComponent, const std::string& id, const std::string& attribute)
            {
                return agentComponent.HasBoolFact(MakePair(id.c_str(), attribute.c_str()));
            },
            "getFactAsBool", [](AIAgentComponent& agentComponent, const std::string& id, const std::string& attribute)
            {
                return agentComponent.GetFactAsBool(MakePair(id.c_str(), attribute.c_str()));
            },
            //Number facts
            "setFactAsNumber", [](AIAgentComponent& agentComponent, const std::string& id, const std::string& attribute, float value)
            {
                agentComponent.SetFactAsNumber(MakePair(id.c_str(), attribute.c_str()), value);
            },
            "hasNumberFact", [](AIAgentComponent& agentComponent, const std::string& id, const std::string& attribute)
            {
                return agentComponent.HasNumberFact(MakePair(id.c_str(), attribute.c_str()));
            },
            "getFactAsNumber", [](AIAgentComponent& agentComponent, const std::string& id, const std::string& attribute)
            {
                return agentComponent.GetFactAsNumber(MakePair(id.c_str(), attribute.c_str()));
            },
            //Vector3 facts
            "setFactAsVector3", [](AIAgentComponent& agentComponent, const std::string& id, const std::string& attribute, Vector3 value)
            {
                agentComponent.SetFactAsVector3(MakePair(id.c_str(), attribute.c_str()), value);
            },
            "hasVector3Fact", [](AIAgentComponent& agentComponent, const std::string& id, const std::string& attribute)
            {
                return agentComponent.HasVector3Fact(MakePair(id.c_str(), attribute.c_str()));
            },
            "getFactAsVector3", [](AIAgentComponent& agentComponent, const std::string& id, const std::string& attribute)
            {
                return agentComponent.GetFactAsVector3(MakePair(id.c_str(), attribute.c_str()));
            },
            //String facts
            "setFactAsString", [](AIAgentComponent& agentComponent, const std::string& id, const std::string& attribute, const std::string& value)
            {
                agentComponent.SetFactAsString(MakePair(id.c_str(), attribute.c_str()), value.c_str());
            },
            "hasStringFact", [](AIAgentComponent& agentComponent, const std::string& id, const std::string& attribute)
            {
                return agentComponent.HasStringFact(MakePair(id.c_str(), attribute.c_str()));
            },
            "getFactAsString", [](AIAgentComponent& agentComponent, const std::string& id, const std::string& attribute)
            {
                return agentComponent.GetFactAsString(MakePair(id.c_str(), attribute.c_str())).c_str();
            }
        );
    }
}