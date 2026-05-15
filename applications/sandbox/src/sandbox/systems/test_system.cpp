#include "../sandbox.hpp"

#include <core/program/program.hpp>

namespace sandbox
{
    [[rsl_reflect(rythe::system_function), maybe_unused]] rsl::result<void> test_system(rythe::core::process_graph_builder& processGraph)
    {
        using namespace rythe;
        using namespace rsl;

        processGraph.create_process_chain("test_system_update")
                .after("some_other_update")
                .before("my_late_system_update")
                .interval(math::min(this_engine::get_context().get_or_emplace<physics_config>().updateInterval, 20_ms))
                .add_parallel_process(
                    [](process_context<
                        reads<transform /* archetype of: position, rotation, scale */, my_component>,
                        writes<my_other_component>
                        > context)
                    {
                        const auto& [pos, rot, scal] = context.read<transform>();
                        const auto& myComp = context.read<my_component>();
                        auto& myOtherComp = context.write<my_other_component>();

                        myOtherComp.offset += myComp.rate * context.deltaTime.seconds() * math::sin(context.time.seconds()) * pos.value * rot.value * scal.value;
                    }
                )
                .add_sequential_process(
                    [](process_context<
                        reads<my_other_component, hierarchy>,
                        writes<position>
                    > context)
                    {
                        const math::float3 offset = context.read<my_other_component>().offset;
                        const entity parent = context.read<hierarchy>().parent;
                        context.write<position>() = context.read<position>(parent).value + offset;
                    }
                );

        processGraph.create_process_chain("init_my_component")
                .on_create<my_component>()
                .add_parallel_process(
                    [](process_context<writes<my_component>, emits<my_other_component>> context)
                    {
                        context.write<my_component>().rate = math::float3::one;
                        context.add_component(my_other_component{ .offset = math::float3::zero });
                    }
                );

        processGraph.create_process_chain("destroy_my_component")
                .on_destroy<my_component>()
                .add_parallel_process(
                    [](process_context<destroys<my_other_component>> context)
                    {
                        context.remove_component<my_other_component>();
                    }
                );

        return rsl::okay;
    }
} // namespace sandbox
