#define RYTHE_ENTRY
#define RSL_DEFAULT_LOG_SEVERITY debug

#if defined(NDEBUG)
#define RYTHE_KEEP_CONSOLE
#endif

#include <core/core.hpp>
#include <rsl/logging>
#include <rsl/type_traits>

#include "sandbox.hpp"

template <typename T>
struct foo {};

namespace sandbox
{
    [[rsl_reflect(rythe::system_function), maybe_unused]] rsl::result<void> test_system(rythe::core::process_graph& processGraph);
}

rsl::result<void> RYTHE_CCONV init_program(rythe::core::program& program)
{
    using namespace rythe;
    using namespace sandbox;
    program.add_engine_instance().bind();

    constexpr static rsl::constexpr_string A = "Something";
    constexpr static rsl::constexpr_string B = "Other";
    constexpr static rsl::constexpr_string result = A + B;

    rsl::log::debug(A);
    rsl::log::debug(B);
    rsl::log::debug(result);

    constexpr static rsl::constexpr_string hello_world = "hello world";
    constexpr static rsl::constexpr_string shorten = hello_world.filter_if([&](const rsl::size_type i) { return ' ' != hello_world[i]; });
    constexpr static rsl::constexpr_string optimal = shorten.refit<shorten.size() + 1>();

    rsl::log::debug(hello_world);
    rsl::log::debug(shorten);
    rsl::log::debug(optimal);
    rsl::log::debug("{} {}", shorten.capacity(), optimal.capacity());

    constexpr rsl::constexpr_string typeName = rsl::type_name<rsl::dynamic_string>();
    constexpr auto shrunk = typeName.refit<typeName.size() + 1>();
    constexpr rsl::id_type typeHash = rsl::type_id<rsl::dynamic_string>();

    rsl::log::debug(
            "type info: {} : {}, {}, {}, {}",
            rsl::string_view(typeName),
            typeHash,
            typeName.size(),
            typeName.capacity(),
            shrunk.capacity()
            );

    using process_info = process_func_info<decltype(
            [](process_context<
                    reads<transform /* archetype of: position, rotation, scale */, my_component>,
                    writes<my_other_component>, emits<position>, destroys<scale, rotation>> context)
    {
        const auto& [pos, rot, scal] = context.read<transform>();
        const auto& myComp = context.read<my_component>();
        auto& myOtherComp = context.write<my_other_component>();

        myOtherComp.offset += myComp.rate * context.deltaTime.seconds() * math::sin(context.time.seconds()) * pos.value *
                rot.value * scal.value;
    })>::context_type;

    rsl::log::debug("reading: {}", rsl::type_name<process_info::reading_components>());
    rsl::log::debug("writing: {}", rsl::type_name<process_info::writing_components>());
    rsl::log::debug("emitting: {}", rsl::type_name<process_info::emitting_components>());
    rsl::log::debug("destroying: {}", rsl::type_name<process_info::destroying_components>());
    process_graph ctx{};
    return test_system(ctx);
}
