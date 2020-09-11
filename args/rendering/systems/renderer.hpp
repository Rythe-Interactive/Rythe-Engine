#pragma once
#include <rendering/data/model.hpp>
#include <rendering/data/shader.hpp>
#include <rendering/components/renderable.hpp>
#include <rendering/components/camera.hpp>

using namespace args::core::filesystem::literals;

namespace args::rendering
{
    class Renderer final : public System<Renderer>
    {
        sparse_map<model_handle, sparse_map<material_handle, std::vector<math::mat4>>> batches;

        ecs::EntityQuery renderablesQuery;
        ecs::EntityQuery cameraQuery;
        bool initialized = false;
        app::gl_id modelMatrixBufferId;

        virtual void setup()
        {
            createProcess<&Renderer::render>("Rendering");
            renderablesQuery = createQuery<renderable, position, rotation, scale>();
            cameraQuery = createQuery<camera, position, rotation, scale>();
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
                            t = "Deprication";
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
                            log::info("[{}-{}] {}", s, t, message);
                            break;
                        case GL_DEBUG_SEVERITY_NOTIFICATION:
                            log::debug("[{}-{}] {}", s, t, message);
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

            math::ivec2 viewportSize = app::ContextHelper::getFramebufferSize(window);
            glViewport(0, 0, viewportSize.x, viewportSize.y);

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
            glBufferData(GL_ARRAY_BUFFER, 65536, nullptr, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            initialized = true;
            return true;
        }

        void render(time::time_span<fast_time> deltaTime)
        {
            (void)deltaTime;

            if (!m_ecs->world.has_component<app::window>())
                return;

            app::window window = m_ecs->world.get_component_handle<app::window>().read();

            async::readwrite_guard guard(*window.lock);
            app::ContextHelper::makeContextCurrent(window);

            if (!initialized)
                if (!initData(window))
                    return;

            glClearColor(0.3f, 0.5f, 1.0f, 1.0f);
            glClearDepth(0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glDisable(GL_CULL_FACE);

            batches.clear();

            auto camEnt = cameraQuery[0];

            math::mat4 viewProj;
            math::compose(viewProj, camEnt.get_component_handle<scale>().read(), camEnt.get_component_handle<rotation>().read(), camEnt.get_component_handle<position>().read());
            viewProj = camEnt.get_component_handle<camera>().read().projection * math::inverse(viewProj);


            for (auto ent : renderablesQuery)
            {
                renderable rend = ent.get_component_handle<renderable>().read();

                math::mat4 modelMatrix;
                math::compose(modelMatrix, ent.get_component_handle<scale>().read(), ent.get_component_handle<rotation>().read(), ent.get_component_handle<position>().read());
                batches[rend.model][rend.material].push_back(modelMatrix);
            }

            for (int i = 0; i < batches.size(); i++)
            {
                auto modelH = batches.keys()[i];
                if (!modelH.is_buffered())
                    modelH.buffer_data(modelMatrixBufferId);

                model mesh = modelH.get_model();
                if (mesh.submeshes.empty())
                    continue;

                for (int j = 0; j < batches.dense()[i].size(); j++)
                {
                    auto material = batches.dense()[i].keys()[j];
                    auto instances = batches.dense()[i].dense()[j];
                    glBindBuffer(GL_ARRAY_BUFFER, modelMatrixBufferId);
                    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(math::mat4) * instances.size(), instances.data());
                    glBindBuffer(GL_ARRAY_BUFFER, 0);

                    material.bind();
                    material.set_param<math::mat4>("viewProjectionMatrix", viewProj);
                    material.prepare();
                    glBindVertexArray(mesh.vertexArrayId);
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.indexBufferId);

                    for (auto submesh : mesh.submeshes)
                        glDrawElementsInstanced(GL_TRIANGLES, (GLuint)submesh.indexCount, GL_UNSIGNED_INT, (GLvoid*)submesh.indexOffset, (GLsizei)instances.size());

                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
                    glBindVertexArray(0);
                    material.release();
                }
            }

            app::ContextHelper::makeContextCurrent(nullptr);
        }
    };
}
