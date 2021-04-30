#pragma once

#include <rendering/util/gui.hpp>
#include <imgui/imgui_internal.h>

#include <core/core.hpp>
#include <rendering/rendering.hpp>
#include <rendering/pipeline/gui/stages/imguirenderstage.hpp>
#include <rendering/systems/renderer.hpp>

LEGION_CLANG_SUPPRESS_WARNING_WITH_PUSH("-Wdeprecated-declarations")

namespace legion
{
    using namespace legion::rendering;
    using namespace legion::core::filesystem::literals;
    //using namespace legion::core::scenemanagement;

    class GuiTestSystem : public System<GuiTestSystem>
    {
        static bool captured;
    public:
        static bool isEditingText;

        static void CaptureKeyboard(bool cap = true)
        {
            captured = cap;
        }

        static void ReleaseKeyboard()
        {
            captured = false;
        }

        ecs::filter<camera, transform> cameraQuery;
        ecs::entity selected;

        math::mat4 view = math::mat4(1.0f);
        math::mat4 projection = math::mat4(1.0f);
        math::mat4 model = math::mat4(1.0f);

        void setup()
        {
            static_cast<DefaultPipeline*>(Renderer::getMainPipeline())->attachStage<ImGuiStage>();

            //gui code goes here
            ImGuiStage::addGuiRender<GuiTestSystem, &GuiTestSystem::onGUI>(this);

            app::window& window = ecs::world.get_component<app::window>();

            model_handle cubeModel;
            material_handle decalMaterial;

            {
                application::context_guard guard(window);

                cubeModel = ModelCache::create_model("cube", "assets://models/Cube.obj"_view);
                decalMaterial = MaterialCache::create_material("decal", "assets://shaders/decal.shs"_view);
                decalMaterial.set_param(SV_ALBEDO, TextureCache::create_texture("engine://resources/default/albedo"_view));
                decalMaterial.set_param(SV_NORMALHEIGHT, TextureCache::create_texture("engine://resources/default/normalHeight"_view));
                decalMaterial.set_param(SV_MRDAO, TextureCache::create_texture("engine://resources/default/MRDAo"_view));
                decalMaterial.set_param(SV_EMISSIVE, TextureCache::create_texture("engine://resources/default/emissive"_view));
                decalMaterial.set_param(SV_HEIGHTSCALE, 0.f);
                decalMaterial.set_param("skycolor", math::color(0.1f, 0.3f, 1.0f));
            }

            auto decalEntity = createEntity("Decal");
            selected = decalEntity;

            position pos(5.f, 0.f, 5.f);
            scale scal(3.f, 2.f, 3.f);
            rotation rot = angleAxis(math::quarter_pi<float>(), math::vec3::up);
            model = compose(scal, rot, pos);

            decalEntity.add_component<transform>(pos, rot, scal);
            decalEntity.add_component(gfx::mesh_renderer(decalMaterial, cubeModel));

        }

        // BuildTree creates a rudimentary Entity View, as entities do currently not have the ability to be named
        // This is going to display IDs, which isn't great but still gives a little insight
        // In how the Scene is currently structured, it will also try to show the names of the components of the
        // entities, which makes identifying them easier
        //
        bool BuildTree(ecs::entity handle)
        {
            static bool isEditingName = false;
            static char buffer[512];

            if (ImGui::TreeNode(reinterpret_cast<void*>(handle->id), "%s", handle->name.c_str()))
            {
                if (ImGui::IsItemClicked())
                {
                    selected = handle;
                }

                if (handle->name.empty())
                {
                    if (handle == ecs::world)
                        handle->name = "World";
                    else if (selected.has_component<camera>())
                        handle->name = "camera";
                    else
                        handle->name = "Entity " + std::to_string(handle->id);

                    if (selected == handle && isEditingName)
                        strcpy(buffer, handle->name.data());
                }

                if (!handle.has_component<camera>())
                {
                    if (selected == handle && isEditingName)
                    {
                        if (ImGui::InputText(" ###", buffer, 512, ImGuiInputTextFlags_EnterReturnsTrue))
                        {
                            handle->name = buffer;
                            isEditingName = false;
                        }
                        if (ImGui::IsItemClicked())
                        {
                            selected = handle;
                        }
                    }
                    else
                    {
                        ImGui::Text("");
                    }

                    ImGui::SameLine();
                    if (ImGui::Button("Change Name"))
                    {
                        if (selected == handle && isEditingName)
                        {
                            handle->name = buffer;
                            isEditingName = false;
                        }
                        else
                        {
                            if (isEditingName)
                            {
                                selected->name = buffer;
                            }
                            selected = handle;
                            isEditingName = true;
                            strcpy(buffer, handle->name.data());
                        }
                    }
                }
                else
                    ImGui::Text("");

                if (handle != ecs::world && !handle.has_component<camera>()/* && !handle.has_component<scene>()*/)
                {
                    ImGui::SameLine();
                    if (ImGui::Button("Destroy"))
                    {
                        handle.destroy();
                        ImGui::TreePop();
                        return false;
                    }
                }

                if (selected != handle)
                {
                    ImGui::SameLine();
                    if (ImGui::Button("Select"))
                    {
                        if (isEditingName)
                        {
                            selected->name = buffer;
                        }
                        isEditingName = false;
                        selected = handle;
                    }
                }

                ImGui::Separator();

                if (ImGui::TreeNode("Children"))
                {
                    if (ImGui::IsItemClicked())
                    {
                        selected = handle;
                    }

                    ImGui::SameLine();
                    if (ImGui::Button("Add child"))
                    {
                        (void)createEntity(handle);
                    }

                    std::vector<ecs::entity> toErase;
                    for (auto child : handle)
                    {
                        if (!BuildTree(child))
                            toErase.push_back(child);
                    }

                    if (toErase.size())
                    {
                        for (auto child : toErase)
                            handle.remove_child(child);
                    }

                    ImGui::TreePop();
                }
                else if (ImGui::IsItemClicked())
                {
                    selected = handle;
                }


                if (ImGui::TreeNode("Components")) {
                    if (ImGui::IsItemClicked())
                    {
                        selected = handle;
                    }
                    for (id_type id : handle.component_composition())
                    {
                        ImGui::Text("%s", ecs::Registry::getFamilyName(id).c_str());
                        if (ImGui::IsItemClicked())
                        {
                            selected = handle;
                        }
                    }

                    ImGui::TreePop();
                }
                else
                {
                    if (ImGui::IsItemClicked())
                    {
                        selected = handle;
                    }
                }
                ImGui::TreePop();
            }
            return true;
        }

        void DisplayFileHandling()
        {
            if (ImGui::BeginMainMenuBar())
            {
                //if (ImGui::BeginMenu("File"))
                //{
                //    if (ImGui::BeginMenu("Save Scene"))
                //    {
                //        if (!SceneManager::currentScene)
                //        {
                //            SceneManager::currentScene = SceneManager::create_scene();
                //        }
                //        auto sceneEntity = SceneManager::currentScene.entity;
                //        std::string sceneName = sceneEntity.get_name();

                //        std::string text = "Save scene as:";
                //        text += sceneName;
                //        if (ImGui::Button(text.c_str()))
                //        {
                //            SceneManager::create_scene(sceneName, sceneEntity);
                //        }
                //        ImGui::EndMenu();
                //    }

                //    if (ImGui::BeginMenu("Load Scene"))
                //    {
                //        for (auto& [id, name] : SceneManager::sceneNames)
                //        {
                //            if (id && ImGui::MenuItem(name.c_str()))
                //            {
                //                SceneManager::load_scene(name);
                //            }
                //        }
                //        ImGui::EndMenu();
                //    }
                //    ImGui::EndMenu();
                //}
                ImGui::EndMainMenuBar();
            }
        }

        template<typename Vec>
        bool DisplayVec(cstring name, Vec& value)
        {
            if constexpr (std::is_same_v<typename Vec::value_type, float>)
            {
                return ImGui::InputScalarN(name, ImGuiDataType_Float, math::value_ptr(value), Vec::length());
            }
            else if constexpr (std::is_same_v<typename Vec::value_type, int>)
            {
                return ImGui::InputScalarN(name, ImGuiDataType_S32, math::value_ptr(value), Vec::length());
            }
            else if constexpr (std::is_same_v<typename Vec::value_type, bool>)
            {
                ImGuiWindow* window = ImGui::GetCurrentWindow();
                if (window->SkipItems)
                    return false;

                ImGuiContext& g = *ImGui::GetCurrentContext();
                bool value_changed = false;
                ImGui::BeginGroup();
                ImGui::PushID(name);
                ImGui::PushMultiItemsWidths(Vec::length(), ImGui::CalcItemWidth());

                for (int i = 0; i < Vec::length(); i++)
                {
                    ImGui::PushID(i);
                    if (i > 0)
                        ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);

                    std::string label = std::string("##") + name + std::to_string(i);

                    value_changed |= ImGui::Checkbox(label.c_str(), &value[i]);

                    ImGui::PopID();
                    ImGui::PopItemWidth();
                }
                ImGui::PopID();

                const char* label_end = ImGui::FindRenderedTextEnd(name);
                if (name != label_end)
                {
                    ImGui::SameLine(0.0f, g.Style.ItemInnerSpacing.x);
                    ImGui::TextEx(name, label_end);
                }

                ImGui::EndGroup();
                return value_changed;
            }
        }

        template<typename T>
        bool DisplayValue(cstring name, T& value)
        {
            if constexpr (std::is_same_v<T, float>)
            {
                return ImGui::InputFloat(name, &value);
            }
            else if constexpr (std::is_same_v<T, bool>)
            {
                return ImGui::Checkbox(name, &value);
            }
            else if constexpr (std::is_same_v<T, int>)
            {
                return ImGui::InputInt(name, &value);
            }
            else
            {
                return DisplayVec<T>(name, value);
            }
        }

        bool DisplayParamEditor(material_handle material, const std::string& name, const GLenum& type)
        {
            if (common::starts_with(name, "lgn_"))
                return false;

            ImGui::Text("%s:", name.c_str());
            ImGui::SameLine();

            std::string label = "##" + name;

            switch (type)
            {
            case GL_SAMPLER_2D:
            {
                auto value = material.get_param<texture_handle>(name);
                auto& tex = value.get_texture();
                static char buffer[512];
                strcpy(buffer, tex.path.data());

                if (ImGui::InputText(label.c_str(), buffer, 512, ImGuiInputTextFlags_EnterReturnsTrue))
                {
                    if (tex.path != buffer)
                    {
                        std::string newTex = buffer;
                        value = TextureCache::create_texture(fs::view(newTex));
                        material.set_param<texture_handle>(name, value);
                        return true;
                    }
                }
            }
            break;
            case GL_FLOAT:
            {
                float value = material.get_param<float>(name);
                if (DisplayValue(label.c_str(), value))
                {
                    material.set_param(name, value);
                    return true;
                }
            }
            break;
            case GL_FLOAT_VEC2:
            {
                math::vec2 value = material.get_param<math::vec2>(name);
                if (DisplayValue(label.c_str(), value))
                {
                    material.set_param(name, value);
                    return true;
                }
            }
            break;
            case GL_FLOAT_VEC3:
            {
                math::vec3 value = material.get_param<math::vec3>(name);
                if (DisplayValue(label.c_str(), value))
                {
                    material.set_param(name, value);
                    return true;
                }
            }
            break;
            case GL_FLOAT_VEC4:
            {
                math::vec4 value = material.get_param<math::vec4>(name);
                if (DisplayValue(label.c_str(), value))
                {
                    material.set_param(name, value);
                    return true;
                }
            }
            break;
            case GL_UNSIGNED_INT:
            {
                int value = (uint)material.get_param<uint>(name);
                if (DisplayValue(label.c_str(), value))
                {
                    material.set_param(name, value);
                    return true;
                }
            }
            break;
            case GL_INT:
            {
                int value = material.get_param<int>(name);
                if (DisplayValue(label.c_str(), value))
                {
                    material.set_param(name, value);
                    return true;
                }
            }
            break;
            case GL_INT_VEC2:
            {
                math::ivec2 value = material.get_param<math::ivec2>(name);
                if (DisplayValue(label.c_str(), value))
                {
                    material.set_param(name, value);
                    return true;
                }
            }
            break;
            case GL_INT_VEC3:
            {
                math::ivec3 value = material.get_param<math::ivec3>(name);
                if (DisplayValue(label.c_str(), value))
                {
                    material.set_param(name, value);
                    return true;
                }
            }
            break;
            case GL_INT_VEC4:
            {
                math::ivec4 value = material.get_param<math::ivec4>(name);
                if (DisplayValue(label.c_str(), value))
                {
                    material.set_param(name, value);
                    return true;
                }
            }
            break;
            case GL_BOOL:
            {
                bool value = material.get_param<bool>(name);
                if (DisplayValue(label.c_str(), value))
                {
                    material.set_param(name, value);
                    return true;
                }
            }
            break;
            case GL_BOOL_VEC2:
            {
                math::bvec2 value = material.get_param<math::bvec2>(name);
                if (DisplayValue(label.c_str(), value))
                {
                    material.set_param(name, value);
                    return true;
                }
            }
            break;
            case GL_BOOL_VEC3:
            {
                math::bvec3 value = material.get_param<math::bvec3>(name);
                if (DisplayValue(label.c_str(), value))
                {
                    material.set_param(name, value);
                    return true;
                }
            }
            break;
            case GL_BOOL_VEC4:
            {
                math::bvec4 value = material.get_param<math::bvec4>(name);
                if (DisplayValue(label.c_str(), value))
                {
                    material.set_param(name, value);
                    return true;
                }
            }
            break;
            case GL_FLOAT_MAT2:
            {
                math::mat2 value = material.get_param<math::mat2>(name);
                bool changed = false;
                std::string label0 = label + '0';
                changed |= DisplayValue(label0.c_str(), value[0]);
                std::string label1 = label + '1';
                changed |= DisplayValue(label1.c_str(), value[1]);
                if (changed)
                {
                    material.set_param(name, value);
                    return true;
                }
            }
            break;
            case GL_FLOAT_MAT3:
            {
                math::mat3 value = material.get_param<math::mat3>(name);
                bool changed = false;
                std::string label0 = label + '0';
                changed |= DisplayValue(label0.c_str(), value[0]);
                std::string label1 = label + '1';
                changed |= DisplayValue(label1.c_str(), value[1]);
                std::string label2 = label + '2';
                changed |= DisplayValue(label2.c_str(), value[2]);
                if (changed)
                {
                    material.set_param(name, value);
                    return true;
                }
            }
            break;
            case GL_FLOAT_MAT4:
            {
                math::mat4 value = material.get_param<math::mat4>(name);
                bool changed = false;
                std::string label0 = label + '0';
                changed |= DisplayValue(label0.c_str(), value[0]);
                std::string label1 = label + '1';
                changed |= DisplayValue(label1.c_str(), value[1]);
                std::string label2 = label + '2';
                changed |= DisplayValue(label2.c_str(), value[2]);
                std::string label3 = label + '3';
                changed |= DisplayValue(label3.c_str(), value[3]);
                if (changed)
                {
                    material.set_param(name, value);
                    return true;
                }
            }
            break;
            default:
                ImGui::Text("Unknown parameter type");
            }
            return false;
        }

        void onGUI(L_MAYBEUNUSED app::window& context, camera& cam, const camera::camera_input& camInput, L_MAYBEUNUSED time::span deltaTime)
        {
            //if (!SceneManager::currentScene)
              //  SceneManager::currentScene = SceneManager::create_scene();

            ImGuiIO& io = ImGui::GetIO();

            if (captured)
            {
                io.WantCaptureKeyboard = true;
                isEditingText = false;
            }
            else
                isEditingText = io.WantCaptureKeyboard;

            setProjectionAndView(io.DisplaySize.x / io.DisplaySize.y, cam, camInput);

            using namespace imgui;
            gizmo::SetOrthographic(false);

            DisplayFileHandling();

            std::string windowName = "Entities";

            if (selected)
            {
                windowName += "  (selected:";

                if (selected->name.empty())
                {
                    if (selected == ecs::world)
                        selected->name = "World";
                    else if (selected.has_component<camera>())
                        selected->name = "camera";
                    else
                        selected->name = "Entity " + std::to_string(selected->id);
                }

                windowName += selected->name;

                windowName += ")";
            }

            windowName += "###";

            if (base::Begin(windowName.c_str()))
            {
                BuildTree(ecs::world);
            }
            base::End();

            static bool showGizmo = true;

            gizmo::BeginFrame();

            base::Begin("Edit Entity");
            if (selected)
            {
                if (selected.has_component<camera>())
                {
                    ImGui::Text("Changing camera transform not allowed.");
                }
                else if (selected.has_component<position>() && selected.has_component<rotation>() && selected.has_component<scale>())
                {
                    if (base::RadioButton("Show Gizmo", showGizmo))
                        showGizmo = !showGizmo;

                    if (showGizmo)
                    {
                        position& pos = selected.get_component<position>();
                        rotation& rot = selected.get_component<rotation>();
                        scale& scal = selected.get_component<scale>();
                        model = compose(scal, rot, pos);
                        gizmo::EditTransform(value_ptr(view), value_ptr(projection), value_ptr(model), true);
                        decompose(model, scal, rot, pos);
                    }
                }
                else if (ImGui::Button("Add Transform"))
                {
                    selected.add_component<transform>();
                }

                if (selected.has_component<mesh_renderable>())
                {
                    static ecs::entity lastSelected;
                    static char modelBuffer[512];
                    static char shaderBuffer[512];
                    static char variantBuffer[512];

                    mesh_renderer& renderer = selected.get_component<mesh_renderer>();
                    mesh_filter& mesh = selected.get_component<mesh_filter>();

                    std::string modelPath;

                    if (mesh.id != invalid_id)
                    {
                        auto [lock, data] = mesh.get();
                        async::readonly_guard guard(lock);
                        modelPath = data.filePath;
                    }

                    ImGui::Text("Model:");
                    if (lastSelected != selected)
                        strcpy(modelBuffer, modelPath.data());
                    ImGui::SameLine();
                    if (ImGui::InputText("##model", modelBuffer, 512, ImGuiInputTextFlags_EnterReturnsTrue))
                    {
                        if (modelPath != modelBuffer)
                        {
                            modelPath = modelBuffer;
                            auto copy = default_mesh_settings;
                            copy.contextFolder = filesystem::view(modelPath).parent();
                            mesh.id = ModelCache::create_model(modelPath, filesystem::view(modelPath), copy).id;
                        }
                    }

                    ImGui::Text("Material:");
                    auto shader = renderer.material.get_shader();
                    std::string shaderPath = shader.get_path();
                    if (lastSelected != selected)
                        strcpy(shaderBuffer, shaderPath.data());

                    ImGui::SameLine();
                    if (ImGui::InputText("##shader", shaderBuffer, 512, ImGuiInputTextFlags_EnterReturnsTrue))
                    {
                        if (shaderPath != shaderBuffer)
                        {
                            shaderPath = shaderBuffer;
                            std::string materialName = std::to_string(selected->id) + shaderPath;
                            shader = ShaderCache::create_shader(fs::view(shaderPath));
                            renderer.material = MaterialCache::create_material(materialName, shader);
                        }
                    }

                    ImGui::Text("Variant:");
                    auto& variant = shader.get_variant(renderer.material.current_variant());
                    std::string variantName = variant.name;
                    if (lastSelected != selected)
                        strcpy(variantBuffer, variantName.data());

                    ImGui::SameLine();
                    if (ImGui::InputText("##variant", variantBuffer, 512, ImGuiInputTextFlags_EnterReturnsTrue))
                    {
                        if (variantName != variantBuffer)
                        {
                            variantName = variantBuffer;
                            if (renderer.material.has_variant(variantName))
                            {
                                renderer.material.set_variant(variantName);
                            }
                        }
                    }

                    auto currentVariant = renderer.material.current_variant();
                    if (currentVariant == 0)
                        currentVariant = nameHash("default");


                    for (auto& [variantId, variantInfo] : shader.get_uniform_info())
                    {
                        if (variantId != currentVariant)
                            continue;

                        for (auto& [name, location, type] : variantInfo)
                        {
                            DisplayParamEditor(renderer.material, name, type);
                        }
                    }

                    if (lastSelected != selected)
                        lastSelected = selected;
                }
                else if (ImGui::Button("Add Renderable"))
                {
                    selected.add_component<gfx::mesh_renderable>();
                }
            }
            else
            {
                ImGui::Text("Select an entity to edit its transform.");
            }
            base::End();
        }

        void setProjectionAndView(float aspect, const camera& cam, const camera::camera_input& camInput)
        {
            view = camInput.view;
            projection = math::perspective(math::deg2rad(cam.fov * aspect), aspect, cam.nearz, cam.farz);
        }
    };

}

LEGION_CLANG_SUPPRESS_WARNING_POP
