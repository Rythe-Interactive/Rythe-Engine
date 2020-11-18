#pragma once
#include <rendering/data/model.hpp>
#include <rendering/data/shader.hpp>
#include <rendering/components/renderable.hpp>
#include <rendering/components/light.hpp>
#include <rendering/components/camera.hpp>
#include <rendering/debugrendering.hpp>
#include <unordered_set>

using namespace legion::core::filesystem::literals;
using namespace std::literals::chrono_literals;

namespace legion::rendering
{
    class Renderer final : public System<Renderer>
    {
    public:
        sparse_map<model_handle, sparse_map<material_handle, std::vector<math::mat4>>> batches;
        std::vector<detail::light_data> lights;
        ecs::EntityQuery renderablesQuery;
        ecs::EntityQuery lightQuery;
        ecs::EntityQuery cameraQuery;
        std::atomic_bool initialized = false;
        app::gl_id modelMatrixBufferId;
        app::gl_id lightsBufferId;

        uint_max frameCount = 0;
        uint temp = 0;
        time::span totalTime;
        bool m_exit = false;

        static async::readonly_rw_spinlock debugLinesLock;
        static thread_local std::unordered_set<debug::debug_line>* localLines;
        static std::unordered_map<std::thread::id, std::unordered_set<debug::debug_line>*> debugLines;

        bool main_window_valid()
        {
            return m_ecs->world.has_component<app::window>();
        }

        app::window get_main_window()
        {
            return m_ecs->world.get_component_handle<app::window>().read();
        }

        static void startDebugDomain()
        {
            if (!localLines)
                localLines = new std::unordered_set<debug::debug_line>();
        }

        static void endDebugDomain()
        {
            size_type size = localLines->size();

            if (size == 0)
                return;

            std::thread::id id = std::this_thread::get_id();

            {
                async::readwrite_guard guard(debugLinesLock);

                if (debugLines[id])
                {
                    for (auto& line : *(debugLines[id]))
                    {
                        if (line.time > 0 && !localLines->count(line))
                            localLines->insert(line);
                    }

                    delete debugLines[id];
                }

                debugLines[id] = localLines;
                localLines = nullptr;
            }

            localLines = new std::unordered_set<debug::debug_line>();
            localLines->reserve(size);
        }

        void drawLine(debug::debug_line* line)
        {
            if (localLines->count(*line))
                localLines->erase(*line);
            localLines->insert(*line);
        }

        void debugRenderPass(const math::mat4& view, const math::mat4& projection, time::time_span<fast_time> deltaTime)
        {
            std::vector<debug::debug_line> lines;

            {
                async::readwrite_guard guard(debugLinesLock);
                if (debugLines.size() == 0)
                    return;

                std::vector<debug::debug_line> toRemove;
                for (auto& [threadId, domain] : debugLines)
                {
                    lines.insert(lines.end(), domain->begin(), domain->end());

                    for (auto& line : (*domain))
                    {
                        if (line.time == 0)
                            continue;

                        line.timeBuffer += deltaTime;

                        if (line.timeBuffer >= line.time)
                            toRemove.push_back(line);
                    }

                    for (auto line : toRemove)
                        domain->erase(line);
                }
            }

            static material_handle debugMaterial = MaterialCache::create_material("debug", "assets://shaders/debug.shs"_view);
            static app::gl_id vertexBuffer = -1;
            static size_type vertexBufferSize = 0;
            static app::gl_id colorBuffer = -1;
            static size_type colorBufferSize = 0;
            static app::gl_id vao = -1;

            if (debugMaterial == invalid_material_handle)
                return;

            if (vertexBuffer == -1)
                glGenBuffers(1, &vertexBuffer);

            if (colorBuffer == -1)
                glGenBuffers(1, &colorBuffer);

            if (vao == -1)
                glGenVertexArrays(1, &vao);

            std::unordered_map<bool, std::unordered_map<float, std::pair<std::vector<math::color>, std::vector<math::vec3>>>> lineBatches;

            for (auto& line : lines)
            {
                auto& [colors, vertices] = lineBatches[line.ignoreDepth][line.width];

                colors.push_back(line.color);
                colors.push_back(line.color);
                vertices.push_back(line.start);
                vertices.push_back(line.end);
            }

            debugMaterial.bind();

            glEnable(GL_LINE_SMOOTH);
            glBindVertexArray(vao);

            for (auto& [ignoreDepth, widthNdata] : lineBatches)
                for (auto& [width, lineData] : widthNdata)
                {
                    auto& [colors, vertices] = lineData;

                    ///------------ vertices ------------///
                    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

                    size_type vertexCount = vertices.size();
                    if (vertexCount > vertexBufferSize)
                    {
                        glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(math::vec3), 0, GL_DYNAMIC_DRAW);
                        vertexBufferSize = vertexCount;
                    }

                    glBufferSubData(GL_ARRAY_BUFFER, 0, vertexCount * sizeof(math::vec3), vertices.data());
                    glEnableVertexAttribArray(SV_POSITION);
                    glVertexAttribPointer(SV_POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);

                    ///------------ colors ------------///
                    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);

                    size_type colorCount = colors.size();
                    if (colorCount > colorBufferSize)
                    {
                        glBufferData(GL_ARRAY_BUFFER, colorCount * sizeof(math::color), 0, GL_DYNAMIC_DRAW);
                        colorBufferSize = colorCount;
                    }

                    glBufferSubData(GL_ARRAY_BUFFER, 0, colorCount * sizeof(math::color), colors.data());

                    auto colorAttrib = debugMaterial.get_attribute("color");

                    if (colorAttrib == invalid_attribute)
                    {
                        glBindBuffer(GL_ARRAY_BUFFER, 0);
                        break;
                    }

                    colorAttrib.set_attribute_pointer(4, GL_FLOAT, GL_FALSE, 0, 0);

                    ///------------ camera ------------///
                    glUniformMatrix4fv(SV_VIEW, 1, false, math::value_ptr(view));
                    glUniformMatrix4fv(SV_PROJECT, 1, false, math::value_ptr(projection));

                    glLineWidth(width + 1);

                    if (ignoreDepth)
                        glDisable(GL_DEPTH_TEST);

                    glDrawArraysInstanced(GL_LINES, 0, vertices.size(), colors.size());

                    if (ignoreDepth)
                        glEnable(GL_DEPTH_TEST);
                    colorAttrib.disable_attribute_pointer();
                    glBindBuffer(GL_ARRAY_BUFFER, 0);
                }

            glBindVertexArray(0);

            glDisable(GL_LINE_SMOOTH);

            debugMaterial.release();
        }

        virtual void setup()
        {
            scheduling::ProcessChain::subscribeToChainStart<&Renderer::startDebugDomain>();
            scheduling::ProcessChain::subscribeToChainEnd<&Renderer::endDebugDomain>();
            debug::setEventBus(m_eventBus);

            bindToEvent<debug::debug_line, &Renderer::drawLine>();

            bindToEvent<events::exit, &Renderer::onExit>();
            createProcess<&Renderer::render>("Rendering");
            renderablesQuery = createQuery<renderable, position, rotation, scale>();
            lightQuery = createQuery<light, position, rotation>();
            cameraQuery = createQuery<camera, position, rotation, scale>();

            m_scheduler->sendCommand(m_scheduler->getChainThreadId("Rendering"), [](void* param)
                {
                    Renderer* self = reinterpret_cast<Renderer*>(param);
                    log::trace("Waiting on main window.");

                    while (!self->main_window_valid())
                        std::this_thread::yield();

                    app::window window = self->get_main_window();

                    log::trace("Initializing context.");

                    async::readwrite_guard guard(*window.lock);
                    app::ContextHelper::makeContextCurrent(window);

                    bool result = self->initData(window);

                    app::ContextHelper::makeContextCurrent(nullptr);

                    if (!result)
                        log::error("Failed to initialize context.");

                    self->initialized.store(result, std::memory_order_release);
                }, this);

            while (!initialized.load(std::memory_order_acquire))
                std::this_thread::yield();
        }

        void onExit(events::exit* event)
        {
            m_exit = true;
        }

        bool initData(const app::window& window)
        {
            if (!gladLoadGLLoader((GLADloadproc)app::ContextHelper::getProcAddress))
            {
                log::error("Failed to load OpenGL");
                return false;
            }
            else
            {
                glEnable(GL_DEBUG_OUTPUT);

                glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
                    {
                        if (!log::impl::thread_names.count(std::this_thread::get_id()))
                            log::impl::thread_names[std::this_thread::get_id()] = "OpenGL";

                        cstring s;
                        switch (source)
                        {
                        case GL_DEBUG_SOURCE_API:
                            s = "OpenGL";
                            break;
                        case GL_DEBUG_SOURCE_SHADER_COMPILER:
                            s = "Shader compiler";
                            break;
                        case GL_DEBUG_SOURCE_THIRD_PARTY:
                            s = "Third party";
                            break;
                        case GL_DEBUG_SOURCE_APPLICATION:
                            s = "Application";
                            break;
                        case GL_DEBUG_SOURCE_OTHER:
                            s = "Other";
                            break;
                        default:
                            s = "Unknown";
                            break;
                        }

                        cstring t;

                        switch (type)
                        {
                        case GL_DEBUG_TYPE_ERROR:
                            t = "Error";
                            break;
                        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
                            t = "Deprecation";
                            break;
                        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
                            t = "Undefined behavior";
                            break;
                        case GL_DEBUG_TYPE_PERFORMANCE:
                            t = "Performance";
                            break;
                        case GL_DEBUG_TYPE_PORTABILITY:
                            t = "Portability";
                            break;
                        case GL_DEBUG_TYPE_MARKER:
                            t = "Marker";
                            break;
                        case GL_DEBUG_TYPE_PUSH_GROUP:
                            t = "Push";
                            break;
                        case GL_DEBUG_TYPE_POP_GROUP:
                            t = "Pop";
                            break;
                        case GL_DEBUG_TYPE_OTHER:
                            t = "Misc";
                            break;
                        default:
                            t = "Unknown";
                            break;
                        }

                        cstring sev;
                        switch (severity)
                        {
                        case GL_DEBUG_SEVERITY_HIGH:
                            log::error("[{}-{}] {}", s, t, message);
                            break;
                        case GL_DEBUG_SEVERITY_MEDIUM:
                            log::warn("[{}-{}] {}", s, t, message);
                            break;
                        case GL_DEBUG_SEVERITY_LOW:
                            log::debug("[{}-{}] {}", s, t, message);
                            break;
                        case GL_DEBUG_SEVERITY_NOTIFICATION:
                            log::trace("[{}-{}] {}", s, t, message);
                            break;
                        default:
                            log::debug("[{}-{}] {}", s, t, message);
                            break;
                        }
                    }, nullptr);
                log::info("loaded OpenGL version: {}.{}", GLVersion.major, GLVersion.minor);
            }

            glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);
            glDepthFunc(GL_GREATER);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            glFrontFace(GL_CW);
            glEnable(GL_MULTISAMPLE);

            const unsigned char* vendor = glGetString(GL_VENDOR);
            const unsigned char* renderer = glGetString(GL_RENDERER);
            const unsigned char* version = glGetString(GL_VERSION);
            const unsigned char* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);

            GLint major, minor;
            glGetIntegerv(GL_MAJOR_VERSION, &major);
            glGetIntegerv(GL_MINOR_VERSION, &minor);

            log::info("Initialized Renderer\n\tCONTEXT INFO\n\t----------------------------------\n\tGPU Vendor:\t{}\n\tGPU:\t\t{}\n\tGL Version:\t{}\n\tGLSL Version:\t{}\n\t----------------------------------\n", vendor, renderer, version, glslVersion);

            glGenBuffers(1, &modelMatrixBufferId);
            glBindBuffer(GL_ARRAY_BUFFER, modelMatrixBufferId);
            glBufferData(GL_ARRAY_BUFFER, sizeof(math::mat4) * 1024, nullptr, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glGenBuffers(1, &lightsBufferId);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightsBufferId);
            glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(detail::light_data) * 1024, nullptr, GL_DYNAMIC_DRAW);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SV_LIGHTS, lightsBufferId);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

            return true;
        }

        void render(time::time_span<fast_time> deltaTime)
        {
            if (!m_ecs->world.has_component<app::window>() || m_exit)
                return;

            //waitForSync();

            time::clock renderClock;
            renderClock.start();

            app::window window = m_ecs->world.get_component_handle<app::window>().read();

            async::readwrite_guard guard(*window.lock);
            app::ContextHelper::makeContextCurrent(window);

            math::ivec2 viewportSize = app::ContextHelper::getFramebufferSize(window);
            if (viewportSize.x != 0 && viewportSize.y != 0)
            {

                glViewport(0, 0, viewportSize.x, viewportSize.y);
                glClearColor(0.3f, 0.5f, 1.0f, 1.0f);
                glClearDepth(0.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                batches.clear();
                lights.clear();

                auto camEnt = cameraQuery[0];

                position camPos = camEnt.get_component_handle<position>().read();
                rotation camRot = camEnt.get_component_handle<rotation>().read();

                math::mat4 view(1.f);
                math::compose(view, camEnt.get_component_handle<scale>().read(), camRot, camPos);
                view = math::inverse(view);

                math::mat4 projection = camEnt.get_component_handle<camera>().read().get_projection(((float)viewportSize.x) / viewportSize.y);

                camera::camera_input cam_input_data(view, projection, camPos, 0, camRot.forward());

                for (auto ent : renderablesQuery)
                {
                    renderable rend = ent.get_component_handle<renderable>().read();

                    if (rend.material == invalid_material_handle)
                    {
                        log::warn("Entity {} has an invalid material.", ent.get_id());
                        continue;
                    }
                    if (rend.model == invalid_model_handle)
                    {
                        log::warn("Entity {} has an invalid model.", ent.get_id());
                        continue;
                    }

                    transform transf = ent.get_component_handles<transform>();
                    batches[rend.model][rend.material].push_back(transf.get_local_to_world_matrix());
                }

                for (auto ent : lightQuery)
                {
                    light lght = ent.read_component<light>();

                    lights.push_back(lght.get_light_data(ent.get_component_handle<position>(), ent.get_component_handle<rotation>()));
                }

                glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightsBufferId);
                glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(detail::light_data) * lights.size(), lights.data());
                glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

                for (auto [modelHandle, instancesPerMaterial] : batches)
                {
                    if (!modelHandle.is_buffered())
                        modelHandle.buffer_data(modelMatrixBufferId);

                    model mesh = modelHandle.get_model();
                    if (mesh.submeshes.empty())
                    {
                        log::warn("Empty mesh found.");
                        continue;
                    }

                    for (auto [material, instances] : instancesPerMaterial)
                    {
                        glBindBuffer(GL_ARRAY_BUFFER, modelMatrixBufferId);
                        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(math::mat4) * instances.size(), instances.data());
                        glBindBuffer(GL_ARRAY_BUFFER, 0);

                        cam_input_data.bind(material);
                        if (material.has_param<uint>(SV_LIGHT_COUNT))
                            material.set_param<uint>(SV_LIGHT_COUNT, lights.size());
                        material.bind();

                        glBindVertexArray(mesh.vertexArrayId);
                        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.indexBufferId);
                        glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightsBufferId);

                        for (auto submesh : mesh.submeshes)
                            glDrawElementsInstanced(GL_TRIANGLES, (GLuint)submesh.indexCount, GL_UNSIGNED_INT, (GLvoid*)(submesh.indexOffset * sizeof(uint)), (GLsizei)instances.size());

                        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
                        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
                        glBindVertexArray(0);
                        material.release();
                    }
                }

                debugRenderPass(view, projection, deltaTime);
            }
            else
            {
                log::error("Invalid frame-buffer size {}", viewportSize);
            }

            app::ContextHelper::makeContextCurrent(nullptr);
            auto elapsed = renderClock.end();

            /* if (temp < 3)
             {
                 temp++;
                 log::debug("render took: {:.3f}ms\tdeltaTime: {:.3f}ms fps: {:.3f}", elapsed.milliseconds(), deltaTime.milliseconds(), 1.0 / deltaTime);
             }
             else
             {
                 frameCount++;
                 totalTime += deltaTime;
                 log::debug("render took: {:.3f}ms\tdeltaTime: {:.3f}ms fps: {:.3f} average: {:.3f}", elapsed.milliseconds(), deltaTime.milliseconds(), 1.0 / deltaTime, 1.0 / (totalTime / frameCount));
             }*/
        }
    };
}
