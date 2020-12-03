#include <rendering/pipeline/default/mainrenderstage.hpp>

namespace legion::rendering
{
    void MainRenderState::setup()
    {
    }

    void MainRenderState::render(app::window& context, camera& cam, time::span deltaTime)
    {
        auto* batches = m_pipeline->get_meta<sparse_map<model_handle, sparse_map<material_handle, std::vector<math::mat4>>>>("batches");

        auto fbo = m_pipeline->getFramebuffer("main");
        fbo.bind();


        fbo.release();
    }
}
