#pragma once
#include <rendering/data/mesh.hpp>
#include <rendering/components/renderable.hpp>
#include <rendering/components/camera.hpp>

namespace args::rendering
{
    class Renderer final : public System<Renderer>
    {
        sparse_map<mesh_handle, std::vector<math::mat4>> batches;

        ecs::EntityQuery renderablesQuery;
        ecs::EntityQuery cameraQuery;

        app::gl_id shaderId;
        app::gl_id modelMatrixBufferId;
        app::gl_id defaultVAO;
        app::gl_location modelMatrixAttrib;

        app::gl_location vertexLoc;
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

        void initData(const app::window& window)
        {
            glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);
            glDepthFunc(GL_GREATER);
            glDisable(GL_CULL_FACE);
            //glCullFace(GL_BACK);
            //glFrontFace(GL_CW);
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

            shaderId = glCreateProgram();

            const char* fragmentShader = "\
            #version 450\n\
\n\
            out vec4 fragment_color;\n\
\n\
            void main(void)\n\
            {\n\
                fragment_color = vec4(1, 0, 0, 1);\n\
            }";
            int fragShaderLength = strlen(fragmentShader);

            app::gl_id frag = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(frag, 1, &fragmentShader, &fragShaderLength);
            glCompileShader(frag);

            const char* vertexShader = "\
            #version 450\n\
\n\
            layout(location = 1) in vec3 vertex;\n\
            layout(location = 2) in mat4 modelMatrix;\n\
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

            glAttachShader(shaderId, frag);
            glAttachShader(shaderId, vert);

            glLinkProgram(shaderId);

            GLint numActiveUniforms = 0;
            glGetProgramiv(shaderId, GL_ACTIVE_UNIFORMS, &numActiveUniforms);

            GLint maxUniformNameLength = 0;
            glGetProgramiv(shaderId, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniformNameLength);
            GLchar uniformNameData[maxUniformNameLength];

            std::vector<std::string> uniformNames;

            for (int uniform = 0; uniform < numActiveUniforms; uniform++)
            {
                GLint arraySize = 0;
                GLenum type = 0;
                GLsizei actualLength = 0;
                glGetActiveUniform(shaderId, uniform, (GLsizei)maxUniformNameLength, &actualLength, &arraySize, &type, uniformNameData);

                uniformNames.push_back(std::string(uniformNameData));

            }

            GLint numActiveAttribs = 0;
            glGetProgramiv(shaderId, GL_ACTIVE_ATTRIBUTES, &numActiveAttribs);

            GLint maxAttribNameLength = 0;
            glGetProgramiv(shaderId, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttribNameLength);
            GLchar attribNameData[maxAttribNameLength];

            std::vector<std::string> attibuteNames;

            for (int attrib = 0; attrib < numActiveAttribs; ++attrib)
            {
                GLint arraySize = 0;
                GLenum type = 0;
                GLsizei actualLength = 0;
                glGetActiveAttrib(shaderId, attrib, (GLsizei)maxAttribNameLength, &actualLength, &arraySize, &type, attribNameData);

                attibuteNames.push_back(std::string(attribNameData));
            }

            modelMatrixAttrib = glGetAttribLocation(shaderId, "modelMatrix");
            vertexLoc = glGetAttribLocation(shaderId, "vertex");

            //camPosLoc = glGetUniformLocation(shaderId, "");
            viewProjLoc = glGetUniformLocation(shaderId, "viewProjectionMatrix");

            glGenVertexArrays(1, &defaultVAO);
            glBindVertexArray(defaultVAO);

            glGenBuffers(1, &modelMatrixBufferId);
            glBindBuffer(GL_ARRAY_BUFFER, modelMatrixBufferId);
            glBufferData(GL_ARRAY_BUFFER, 65536, nullptr, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        void render(time::time_span<fast_time> deltaTime)
        {
            (void)deltaTime;

            if (!m_ecs->world.has_component<app::window>())
                return;

            app::window window = m_ecs->world.get_component<app::window>().read();

            app::ContextHelper::makeContextCurrent(window);

            auto printErrors = []()
            {
                GLenum error = glGetError();
                while (error)
                {
                    std::cout << error << std::endl;
                    error = glGetError();
                }
            };

            if (!shaderId)
                initData(window);

            glClearColor(0.3f, 0.5f, 1.0f, 1.0f);
            glClearDepth(0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            glDepthFunc(GL_GREATER);

            batches.clear();

            auto camEnt = cameraQuery[0];

            //math::vec3 camPos = camEnt.get_component<position>().read();

            math::mat4 viewProj;
            math::compose(viewProj, camEnt.get_component<scale>().read(), camEnt.get_component<rotation>().read(), camEnt.get_component<position>().read());
            viewProj = camEnt.get_component<camera>().read().projection * math::inverse(viewProj);


            for (auto ent : renderablesQuery)
            {
                renderable rend = ent.get_component<renderable>().read();

                math::mat4 modelMatrix;
                math::compose(modelMatrix, ent.get_component<scale>().read(), ent.get_component<rotation>().read(), ent.get_component<position>().read());
                batches[rend.mesh].push_back(modelMatrix);
            }

            std::cout << "created " << batches.size() << " batches" << std::endl;

            for (int i = 0; i < batches.size(); i++)
            {
                auto handle = batches.keys()[i];
                if (!handle.is_buffered())
                    handle.buffer_data();

                mesh mesh = handle.get_mesh();
                if (mesh.submeshes.empty())
                    continue;

                auto instances = batches.dense()[i];

                glUseProgram(shaderId);

                //glUniform1i(location of lightCount, (int)lights.size());

                glUniformMatrix4fv(viewProjLoc, 1, GL_FALSE, math::value_ptr(viewProj));

                // set other uniforms
                printErrors();
                glBindBuffer(GL_ARRAY_BUFFER, mesh.vertexBufferId);
                glEnableVertexAttribArray(vertexLoc);
                glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
                printErrors();

                //glBindBuffer(GL_ARRAY_BUFFER, normalBufferId);
                //glEnableVertexAttribArray(pNormalsAttrib->GetLocation());
                //glVertexAttribPointer(pNormalsAttrib->GetLocation(), 3, GL_FLOAT, GL_TRUE, 0, 0);

                //glBindBuffer(GL_ARRAY_BUFFER, uvBufferId);
                //glEnableVertexAttribArray(pUVsAttrib->GetLocation());
                //glVertexAttribPointer(pUVsAttrib->GetLocation(), 2, GL_FLOAT, GL_FALSE, 0, 0);

                //glBindBuffer(GL_ARRAY_BUFFER, tangentBufferId);
                //glEnableVertexAttribArray(pTangentsAttrib->GetLocation());
                //glVertexAttribPointer(pTangentsAttrib->GetLocation(), 3, GL_FLOAT, GL_TRUE, 0, 0);

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.submeshes[0].indexBufferId);
                printErrors();

                /*glBindBuffer(GL_UNIFORM_BUFFER, lightsBufferId);
                glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightData) * lights.size(), lights.data());*/

                glBindBuffer(GL_ARRAY_BUFFER, modelMatrixBufferId);
                printErrors();

                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(math::mat4) * instances.size(), instances.data());
                printErrors();

                glEnableVertexAttribArray(modelMatrixAttrib + 0);
                glEnableVertexAttribArray(modelMatrixAttrib + 1);
                glEnableVertexAttribArray(modelMatrixAttrib + 2);
                glEnableVertexAttribArray(modelMatrixAttrib + 3);
                printErrors();

                glVertexAttribPointer(modelMatrixAttrib + 0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(math::vec4), (GLvoid*)0);
                glVertexAttribPointer(modelMatrixAttrib + 1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(math::vec4), (GLvoid*)(sizeof(math::vec4)));
                glVertexAttribPointer(modelMatrixAttrib + 2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(math::vec4), (GLvoid*)(2 * sizeof(math::vec4)));
                glVertexAttribPointer(modelMatrixAttrib + 3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(math::vec4), (GLvoid*)(3 * sizeof(math::vec4)));
                printErrors();

                glVertexAttribDivisor(modelMatrixAttrib + 0, 1);
                glVertexAttribDivisor(modelMatrixAttrib + 1, 1);
                glVertexAttribDivisor(modelMatrixAttrib + 2, 1);
                glVertexAttribDivisor(modelMatrixAttrib + 3, 1);
                printErrors();

                glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)mesh.submeshes[0].indexCount, GL_UNSIGNED_INT, (GLvoid*)0, (GLsizei)instances.size());
                printErrors();

                glDisableVertexAttribArray(modelMatrixAttrib + 0);
                glDisableVertexAttribArray(modelMatrixAttrib + 1);
                glDisableVertexAttribArray(modelMatrixAttrib + 2);
                glDisableVertexAttribArray(modelMatrixAttrib + 3);
                printErrors();

                ////fix for serious performance issue
                //if (pTangentsAttrib && pTangentsAttrib->IsValid()) glDisableVertexAttribArray(pTangentsAttrib->GetLocation());
                //if (pUVsAttrib && pUVsAttrib->IsValid()) glDisableVertexAttribArray(pUVsAttrib->GetLocation());
                //if (pNormalsAttrib && pNormalsAttrib->IsValid()) glDisableVertexAttribArray(pNormalsAttrib->GetLocation());
                glDisableVertexAttribArray(vertexLoc);
                printErrors();

                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
                printErrors();

                // glBindBuffer(GL_UNIFORM_BUFFER, 0);
                glUseProgram(0);
                printErrors();
            }
        }
    };
}
