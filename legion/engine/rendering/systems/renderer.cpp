#include <rendering/systems/renderer.hpp>
#include <rendering/debugrendering.hpp>
#include <Optick/optick.h>

namespace legion::rendering
{
    delegate<RenderPipelineBase* (app::window&)> Renderer::m_pipelineProvider;
    RenderPipelineBase* Renderer::m_currentPipeline;

    void Renderer::debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, L_MAYBEUNUSED const void* userParam)
    {
        if (id == 131185) // Filter out annoying Nvidia message of: Buffer you made will use VRAM because you told us that you want it to allocate VRAM.
            return;

        OPTICK_EVENT();
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

        switch (severity)
        {
        case GL_DEBUG_SEVERITY_HIGH:
            log::error("[{}-{}] {}: {}", s, t, id, message);
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            log::warn("[{}-{}] {}: {}", s, t, id, message);
            break;
        case GL_DEBUG_SEVERITY_LOW:
            log::debug("[{}-{}] {}: {}", s, t, id, message);
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            log::debug("[{}-{}] {}: {}", s, t, id, message);
            break;
        default:
            log::debug("[{}-{}] {}: {}", s, t, id, message);
            break;
        }
    }

    void Renderer::debugCallbackARB(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, L_MAYBEUNUSED const void* userParam)
    {
        OPTICK_EVENT();
        if (!log::impl::thread_names.count(std::this_thread::get_id()))
            log::impl::thread_names[std::this_thread::get_id()] = "OpenGL";

        cstring s;
        switch (source)
        {
        case GL_DEBUG_SOURCE_API_ARB:
            s = "OpenGL";
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB:
            s = "Window system";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB:
            s = "Shader compiler";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY_ARB:
            s = "Third party";
            break;
        case GL_DEBUG_SOURCE_APPLICATION_ARB:
            s = "Application";
            break;
        case GL_DEBUG_SOURCE_OTHER_ARB:
            s = "Other";
            break;
        default:
            s = "Unknown";
            break;
        }

        cstring t;

        switch (type)
        {
        case GL_DEBUG_TYPE_ERROR_ARB:
            t = "Error";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB:
            t = "Deprecation";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:
            t = "Undefined behavior";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE_ARB:
            t = "Performance";
            break;
        case GL_DEBUG_TYPE_PORTABILITY_ARB:
            t = "Portability";
            break;
        case GL_DEBUG_TYPE_OTHER_ARB:
            t = "Misc";
            break;
        default:
            t = "Unknown";
            break;
        }

        switch (severity)
        {
        case GL_DEBUG_SEVERITY_HIGH_ARB:
            log::error("[{}-{}] {}: {}", s, t, id, message);
            break;
        case GL_DEBUG_SEVERITY_MEDIUM_ARB:
            log::warn("[{}-{}] {}: {}", s, t, id, message);
            break;
        case GL_DEBUG_SEVERITY_LOW_ARB:
            log::debug("[{}-{}] {}: {}", s, t, id, message);
            break;
        default:
            log::debug("[{}-{}] {}: {}", s, t, id, message);
            break;
        }
    }

    void Renderer::debugCallbackAMD(GLuint id, GLenum category, GLenum severity, GLsizei length, const GLchar* message, L_MAYBEUNUSED void* userParam)
    {
        OPTICK_EVENT();
        if (!log::impl::thread_names.count(std::this_thread::get_id()))
            log::impl::thread_names[std::this_thread::get_id()] = "OpenGL";

        cstring c;
        switch (category)
        {
        case GL_DEBUG_CATEGORY_API_ERROR_AMD:
            c = "OpenGL";
            break;
        case GL_DEBUG_CATEGORY_WINDOW_SYSTEM_AMD:
            c = "Window system";
            break;
        case GL_DEBUG_CATEGORY_DEPRECATION_AMD:
            c = "Deprecation";
            break;
        case GL_DEBUG_CATEGORY_UNDEFINED_BEHAVIOR_AMD:
            c = "Undefined behavior";
            break;
        case GL_DEBUG_CATEGORY_PERFORMANCE_AMD:
            c = "Performance";
            break;
        case GL_DEBUG_CATEGORY_SHADER_COMPILER_AMD:
            c = "Shader compiler";
            break;
        case GL_DEBUG_CATEGORY_APPLICATION_AMD:
            c = "Application";
            break;
        case GL_DEBUG_CATEGORY_OTHER_AMD:
            c = "Other";
            break;
        default:
            c = "Unknown";
            break;
        }

        switch (severity)
        {
        case GL_DEBUG_SEVERITY_HIGH_AMD:
            log::error("[{}] {}: {}", c, id, message);
            break;
        case GL_DEBUG_SEVERITY_MEDIUM_AMD:
            log::warn("[{}] {}: {}", c, id, message);
            break;
        case GL_DEBUG_SEVERITY_LOW_AMD:
            log::debug("[{}] {}: {}", c, id, message);
            break;
        default:
            log::debug("[{}] {}: {}", c, id, message);
            break;
        }
    }

    bool Renderer::initContext(const app::window& window)
    {
        OPTICK_EVENT();
        if (!gladLoadGLLoader((GLADloadproc)app::ContextHelper::getProcAddress))
        {
            log::error("Failed to load OpenGL");
            return false;
        }

        glEnable(GL_DEBUG_OUTPUT);

        if (GLAD_GL_AMD_debug_output)
        {
            glDebugMessageCallbackAMD(&Renderer::debugCallbackAMD, nullptr);
        }
        else if (GLAD_GL_KHR_debug)
        {
            glDebugMessageCallback(&Renderer::debugCallback, nullptr);
        }
        else if (GLAD_GL_ARB_debug_output)
        {
            glDebugMessageCallbackARB(&Renderer::debugCallbackARB, nullptr);
        }

        log::info("loaded OpenGL version: {}.{}", GLVersion.major, GLVersion.minor);

        glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_GREATER);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CW);

        const unsigned char* vendor = glGetString(GL_VENDOR);
        const unsigned char* renderer = glGetString(GL_RENDERER);
        const unsigned char* version = glGetString(GL_VERSION);
        const unsigned char* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);

        GLint major, minor;
        glGetIntegerv(GL_MAJOR_VERSION, &major);
        glGetIntegerv(GL_MINOR_VERSION, &minor);

        log::info("Initialized Renderer\n\tCONTEXT INFO\n\t----------------------------------\n\tGPU Vendor:\t{}\n\tGPU:\t\t{}\n\tGL Version:\t{}\n\tGLSL Version:\t{}\n\t----------------------------------\n", vendor, renderer, version, glslVersion);
        return true;
    }

    void Renderer::setThreadPriority()
    {
#ifdef LEGION_WINDOWS
        if (SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST))
        {
            log::info("Acquired realtime priority for rendering thread.");
        }
        else
        {
            DWORD dwError = GetLastError();
            /*if (dwError == ERROR_THREAD_)
                log::info("Rendering thread is already in realtime priority mode.");
            else*/
            log::warn("Rendering thread failed to enter realtime priority mode: {}", dwError);
        }
#endif // LEGION_WINDOWS
    }

    void Renderer::setup()
    {
        OPTICK_EVENT();
        RenderPipelineBase::m_ecs = m_ecs;
        RenderPipelineBase::m_scheduler = m_scheduler;
        RenderPipelineBase::m_eventBus = m_eventBus;

        RenderStageBase::m_ecs = m_ecs;
        RenderStageBase::m_scheduler = m_scheduler;
        RenderStageBase::m_eventBus = m_eventBus;

        bindToEvent<events::exit, &Renderer::onExit>();

        createProcess<&Renderer::render>("Rendering");

        m_scheduler->sendCommand(m_scheduler->getChainThreadId("Rendering"), [&]()
            {
                OPTICK_EVENT("Initialization");
                log::trace("Waiting on main window.");

                while (!world.has_component<app::window>())
                    std::this_thread::yield();

                app::window window = world.get_component_handle<app::window>().read();

                log::trace("Initializing context.");

                bool result = false;

                {
                    app::context_guard guard(window);
                    if (!guard.contextIsValid())
                    {
                        log::error("Failed to initialize context.");
                        return;
                    }
                    result = initContext(window);
                }

                if (!result)
                    log::error("Failed to initialize context.");
                else
                    setThreadPriority();
            }).wait();
    }

    void Renderer::onExit(events::exit* event)
    {
        OPTICK_EVENT();
        m_exiting.store(true, std::memory_order_release);
    }

    void Renderer::render(time::span deltatime)
    {
        OPTICK_EVENT();

        if (m_pipelineProvider.isNull())
            return;

        static auto cameraQuery = createQuery<camera>();
        cameraQuery.queryEntities();
        for (auto ent : cameraQuery)
        {
            auto cam = ent.get_component_handle<camera>().read();
            app::window win = cam.targetWindow.read();
            if (!win)
                win = m_ecs->world.get_component_handle<app::window>().read();
            if (!win)
                continue;

            math::ivec2 viewportSize;
            {
                if (!app::WindowSystem::windowStillExists(win.handle))
                    continue;
                app::context_guard guard(win);
                if (!guard.contextIsValid())
                    continue;

                viewportSize = win.framebufferSize();
            }

            if (viewportSize.x == 0 || viewportSize.y == 0)
                continue;

            position camPos = ent.get_component_handle<position>().read();
            rotation camRot = ent.get_component_handle<rotation>().read();
            scale camScale = ent.get_component_handle<scale>().read();

            math::mat4 view(1.f);
            math::compose(view, camScale, camRot, camPos);
            view = math::inverse(view);

            math::mat4 projection = cam.get_projection(((float)viewportSize.x) / viewportSize.y);

            camera::camera_input cam_input_data(view, projection, camPos, camRot.forward(), cam.nearz, cam.farz, viewportSize);

            if (!m_exiting.load(std::memory_order_relaxed))
            {
                m_currentPipeline = m_pipelineProvider(win);
                m_currentPipeline->render(win, cam, cam_input_data, deltatime);
            }
        }
    }

    L_NODISCARD RenderPipelineBase* Renderer::getPipeline(app::window& context)
    {
        OPTICK_EVENT();
        if (m_pipelineProvider.isNull())
            return nullptr;

        if (context == app::invalid_window)
            return nullptr;

        return m_pipelineProvider(context);
    }

    L_NODISCARD RenderPipelineBase* Renderer::getCurrentPipeline()
    {
        return m_currentPipeline;
    }

    L_NODISCARD RenderPipelineBase* Renderer::getMainPipeline()
    {
        OPTICK_EVENT();
        if (m_pipelineProvider.isNull())
            return nullptr;

        auto context = world.get_component_handle<app::window>().read();
        if (context == app::invalid_window)
            return nullptr;

        return m_pipelineProvider(context);
    }

}
