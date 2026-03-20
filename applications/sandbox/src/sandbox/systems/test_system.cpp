#include "../sandbox.hpp"

#include <core/program/program.hpp>

namespace sandbox
{
    RYTHE_DECLARE_PRIVATE_PROCESS_CHAIN(test_system, init_my_component);
    RYTHE_DECLARE_PRIVATE_PROCESS_CHAIN(test_system, destroy_my_component);

    RYTHE_DEFINE_SYSTEM(test_system)
    {
        using namespace rythe;
        using namespace rythe::core;
        using namespace rsl;
        using namespace rsl::literals;

        systemBuilder.create_process_chain(test_system_update_id)
                .after(some_other_system_update_id)
                .before(my_late_system_update_id)
                .interval(math::min(this_engine::get_context().get<physics_config>().updateInterval, 20_ms))
                .add_parallel_process(
                    [](process_context<
                            reads<transform /* archetype of: position, rotation, scale */, my_component>,
                            writes<my_other_component>> context)
                    {
                        const auto& [pos, rot, scal] = context.read<transform>();
                        const auto& myComp = context.read<my_component>();
                        auto& myOtherComp = context.write<my_other_component>();

                        myOtherComp.offset += myComp.rate * context.deltaTime.seconds() * math::sin(context.time.seconds()) * pos * rot * scal;
                    }
                )
                .add_sequential_process(
                    [](process_context<reads<my_other_component, hierarchy>, writes<position>> context)
                    {
                        const math::float3 offset = context.read<my_other_component>().offset;
                        const entity parent = context.read<hierarchy>().parent;
                        context.write<position>() = context.read<position>(parent) + offset;
                    }
                );

		systemBuilder.create_process_chain(test_system_init_my_component_id)
                .on_create<my_component>()
                .add_parallel_process(
                    [](process_context<writes<my_component>, emits<my_other_component>> context)
                    {
                        context.write<my_component>().rate = math::float3::one;
                        context.add_component(my_other_component{ .offset = math::float3::zero });
                    }
                );

        systemBuilder.create_process_chain(test_system_destroy_my_component_id)
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
