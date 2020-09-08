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
        sparse_map<model_handle, std::vector<math::mat4>> batches;

        ecs::EntityQuery renderablesQuery;
        ecs::EntityQuery cameraQuery;

        app::gl_id shaderId;
        app::gl_id modelMatrixBufferId;

        shader_handle shdr;

        //app::gl_location camPosLoc;
        app::gl_location viewProjLoc;


        virtual void setup()
        {
            createProcess<&Renderer::render>("Rendering");
            renderablesQuery = createQuery<renderable, position, rotation, scale>();
            cameraQuery = createQuery<camera, position, rotation, scale>();

            shaderId = 0;

            //glGenBuffers(1, &lightsBufferId);
            //glBindBuffer(GL_UNIFORM_BUFFER, lightsBufferId);
            //glBufferData(GL_UNIFORM_BUFFER, sizeof(LightData) * MAX_LIGHT_COUNT, NULL, GL_DYNAMIC_DRAW);
            //glBindBufferBase(GL_UNIFORM_BUFFER, 0, lightsBufferId);
            //glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }

        bool initData(const app::window& window)
        {
            if (!gladLoadGLLoader((GLADloadproc)app::ContextHelper::getProcAddress))
            {
                std::cout << "Failed to load OpenGL" << std::endl;
                return false;
            }
            else
            {
                glEnable(GL_DEBUG_OUTPUT);
                glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
                    {
                        cstring s;
                        switch (source)
                        {
                        case GL_DEBUG_SOURCE_API:
                            s = "GL_DEBUG_SOURCE_API";
                            break;
                        case GL_DEBUG_SOURCE_SHADER_COMPILER:
                            s = "GL_DEBUG_SOURCE_SHADER_COMPILER";
                            break;
                        case GL_DEBUG_SOURCE_THIRD_PARTY:
                            s = "GL_DEBUG_SOURCE_THIRD_PARTY";
                            break;
                        case GL_DEBUG_SOURCE_APPLICATION:
                            s = "GL_DEBUG_SOURCE_APPLICATION";
                            break;
                        case GL_DEBUG_SOURCE_OTHER:
                            s = "GL_DEBUG_SOURCE_OTHER";
                            break;
                        default:
                            s = "UNKNOWN SOURCE";
                            break;
                        }

                        cstring t;

                        switch (type)
                        {
                        case GL_DEBUG_TYPE_ERROR:
                            t = "GL_DEBUG_TYPE_ERROR";
                            break;
                        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
                            t = "GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR";
                            break;
                        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
                            t = "GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR";
                            break;
                        case GL_DEBUG_TYPE_PERFORMANCE:
                            t = "GL_DEBUG_TYPE_PERFORMANCE";
                            break;
                        case GL_DEBUG_TYPE_PORTABILITY:
                            t = "GL_DEBUG_TYPE_PORTABILITY";
                            break;
                        case GL_DEBUG_TYPE_MARKER:
                            t = "GL_DEBUG_TYPE_MARKER";
                            break;
                        case GL_DEBUG_TYPE_PUSH_GROUP:
                            t = "GL_DEBUG_TYPE_PUSH_GROUP";
                            break;
                        case GL_DEBUG_TYPE_POP_GROUP:
                            t = "GL_DEBUG_TYPE_POP_GROUP";
                            break;
                        case GL_DEBUG_TYPE_OTHER:
                            t = "GL_DEBUG_TYPE_OTHER";
                            break;
                        default:
                            t = "UNKNOWN TYPE";
                            break;
                        }

                        cstring sev;
                        switch (severity)
                        {
                        case GL_DEBUG_SEVERITY_HIGH:
                            sev = "GL_DEBUG_SEVERITY_HIGH ";
                            break;
                        case GL_DEBUG_SEVERITY_MEDIUM:
                            sev = "GL_DEBUG_SEVERITY_MEDIUM ";
                            break;
                        case GL_DEBUG_SEVERITY_LOW:
                            sev = "GL_DEBUG_SEVERITY_LOW ";
                            break;
                        case GL_DEBUG_SEVERITY_NOTIFICATION:
                            sev = "GL_DEBUG_SEVERITY_NOTIFICATION ";
                            break;
                        default:
                            sev = "UNKNOWN SEVERITY";
                            break;
                        }


                        //std::printf("GL CALLBACK: %s source = %s type = %s, severity = %s, message = %s\n", (type == GL_DEBUG_TYPE_ERROR ? " GL ERROR " : ""), s, t, sev, message);
                    }, nullptr);
                std::cout << "loaded OpenGL version: " << GLVersion.major << '.' << GLVersion.minor << std::endl;
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

            std::cout << "Initialized Renderer\n\tCONTEXT INFO\n\t----------------------------------\n\tGPU Vendor:\t" << vendor << "\n\tGPU:\t\t" << renderer << "\n\tGL Version:\t" << version << "\n\tGLSL Version:\t" << glslVersion << "\n\t----------------------------------\n";

            shdr = shader_cache::create_shader("wireframe", "basic:/shaders/wireframe.glsl"_view);

            shaderId = -1;

          /*  shaderId = glCreateProgram();

            const char* fragmentShader = "\
            #version 450\n\
\n\
            in vec3 barycentricCoords;\n\
            out vec4 fragment_color;\n\
\n\
            void main(void)\n\
            {\n\
                vec3 deltas = fwidth(barycentricCoords);\n\
                vec3 adjustedCoords = smoothstep(deltas, 2* deltas, barycentricCoords);\n\
                float linePresence = min(adjustedCoords.x, min(adjustedCoords.y, adjustedCoords.z));\n\
                if(linePresence > 0.9)\n\
                    discard;\n\
                \n\
                fragment_color = vec4(vec3(0), 1);\n\
            }";
            int fragShaderLength = strlen(fragmentShader);

            app::gl_id frag = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(frag, 1, &fragmentShader, &fragShaderLength);
            glCompileShader(frag);

            const char* vertexShader = "\
            #version 450\n\
\n\
            layout(location = 8)in vec3 vertex;\n\
            layout(location = 12)in mat4 modelMatrix;\n\
            uniform	mat4 viewProjectionMatrix;\n\
\n\
            void main(void)\n\
            {\n\
                gl_Position = viewProjectionMatrix * modelMatrix * vec4(vertex, 1.f);\n\
            }";
            int vertShaderLength = strlen(vertexShader);

            app::gl_id vert = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vert, 1, &vertexShader, &vertShaderLength);
            glCompileShader(vert);

            const char* geometryShader = "\
            #version 450\n\
\n\
            layout(triangles)in;\n\
            layout(triangle_strip, max_vertices = 3)out;\n\
            \n\
            out vec3 barycentricCoords;\n\
            \n\
            void main(void)\n\
            {\n\
                gl_Position = gl_in[0].gl_Position;\n\
                barycentricCoords = vec3(1, 0, 0);\n\
                EmitVertex();\n\
                gl_Position = gl_in[1].gl_Position;\n\
                barycentricCoords = vec3(0, 1, 0);\n\
                EmitVertex();\n\
                gl_Position = gl_in[2].gl_Position;\n\
                barycentricCoords = vec3(0, 0, 1);\n\
                EmitVertex();\n\
                EndPrimitive();\n\
            }";
            int geometryShaderLength = strlen(geometryShader);

            app::gl_id geom = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geom, 1, &geometryShader, &geometryShaderLength);
            glCompileShader(geom);

            glAttachShader(shaderId, frag);
            glAttachShader(shaderId, vert);
            glAttachShader(shaderId, geom);

            glLinkProgram(shaderId);*/

            //camPosLoc = glGetUniformLocation(shaderId, "");
            //viewProjLoc = glGetUniformLocation(shaderId, "viewProjectionMatrix");

            glGenBuffers(1, &modelMatrixBufferId);
            glBindBuffer(GL_ARRAY_BUFFER, modelMatrixBufferId);
            glBufferData(GL_ARRAY_BUFFER, 65536, nullptr, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            return true;
        }

        void render(time::time_span<fast_time> deltaTime)
        {
            (void)deltaTime;

            if (!m_ecs->world.has_component<app::window>())
                return;

            app::window window = m_ecs->world.get_component_handle<app::window>().read();

            app::ContextHelper::makeContextCurrent(window);

            if (shaderId == 0)
                if (!initData(window))
                    return;

            auto printErrors = []()
            {
                GLenum error = glGetError();
                int i = 0;
                while (error || i > 10)
                {
                    std::cout << error << std::endl;
                    error = glGetError();
                    i++;
                }
            };

            glClearColor(0.3f, 0.5f, 1.0f, 1.0f);
            glClearDepth(0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
            glDepthFunc(GL_GREATER);

            batches.clear();

            auto camEnt = cameraQuery[0];

            //math::vec3 camPos = camEnt.get_component<position>().read();

            math::mat4 viewProj;
            math::compose(viewProj, camEnt.get_component_handle<scale>().read(), camEnt.get_component_handle<rotation>().read(), camEnt.get_component_handle<position>().read());
            viewProj = camEnt.get_component_handle<camera>().read().projection * math::inverse(viewProj);


            for (auto ent : renderablesQuery)
            {
                renderable rend = ent.get_component_handle<renderable>().read();

                math::mat4 modelMatrix;
                math::compose(modelMatrix, ent.get_component_handle<scale>().read(), ent.get_component_handle<rotation>().read(), ent.get_component_handle<position>().read());
                batches[rend.model].push_back(modelMatrix);
            }

            //std::cout << "created " << batches.size() << " batches" << std::endl;

            for (int i = 0; i < batches.size(); i++)
            {
                auto handle = batches.keys()[i];
                if (!handle.is_buffered())
                    handle.buffer_data(modelMatrixBufferId);

                model mesh = handle.get_model();
                if (mesh.submeshes.empty())
                    continue;

                auto instances = batches.dense()[i];
                glBindBuffer(GL_ARRAY_BUFFER, modelMatrixBufferId);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(math::mat4) * instances.size(), instances.data());
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                //glUseProgram(shaderId);
                shdr.bind();
                shdr.get_uniform<math::mat4>("viewProjectionMatrix").set_value(viewProj);
                //glUniformMatrix4fv(viewProjLoc, 1, GL_FALSE, math::value_ptr(viewProj));

                glBindVertexArray(mesh.vertexArrayId);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.indexBufferId);

                printErrors();
                for (auto submesh : mesh.submeshes)
                    glDrawElementsInstanced(GL_TRIANGLES, (GLuint)submesh.indexCount, GL_UNSIGNED_INT, (GLvoid*)submesh.indexOffset, (GLsizei)instances.size());

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
                glBindVertexArray(0);
                glUseProgram(0);
            }
        }
    };
}
