#include <rendering/pipeline/default/stages/meshbatchingstage.hpp>

namespace  legion::rendering
{
    void MeshBatchingStage::setup(app::window& context)
    {
        OPTICK_EVENT();
        create_meta<sparse_map<material_handle, sparse_map<model_handle, std::vector<math::mat4>>>>("mesh batches");
    }

    void MeshBatchingStage::render(app::window& context, camera& cam, const camera::camera_input& camInput, time::span deltaTime)
    {
        OPTICK_EVENT();
        (void)deltaTime;
        (void)camInput;
        (void)cam;
        (void)context;

        static id_type batchesId = nameHash("mesh batches");
        auto* batches = get_meta<sparse_map<material_handle, sparse_map<model_handle, std::vector<math::mat4>>>>(batchesId);

        static ecs::filter<position, rotation, scale, mesh_filter, mesh_renderer> renderablesQuery{};

        auto& positions = renderablesQuery.get<position>();
        auto& rotations = renderablesQuery.get<rotation>();
        auto& scales = renderablesQuery.get<scale>();
        auto& filters = renderablesQuery.get<mesh_filter>();
        auto& renderers = renderablesQuery.get<mesh_renderer>();

        {
            OPTICK_EVENT("Clear instances");
            for (auto [_, models] : *batches)
                for (auto [_, instances] : models)
                    instances.clear();
        }

        {
            OPTICK_EVENT("Calculate instances");
            for (int i = 0; i < renderablesQuery.size(); i++)
            {
                OPTICK_EVENT("instance");
                (*batches)[renderers[i].get().material][model_handle{ filters[i].get().id }].push_back(math::compose(scales[i].get(), rotations[i].get(), positions[i].get()));
            }
        }
    }

    priority_type MeshBatchingStage::priority()
    {
        return setup_priority;
    }

}
