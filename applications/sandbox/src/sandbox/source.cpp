#define RYTHE_ENTRY
#define RSL_DEFAULT_LOG_SEVERITY debug

#if defined(NDEBUG)
#define RYTHE_KEEP_CONSOLE
#endif

#include <core/core.hpp>
#include <rsl/logging>
#include <rsl/type_traits>

template <typename T>
struct foo {};

rsl::result<void> RYTHE_CCONV init_program(rythe::core::program& program)
{
    using namespace rythe;
    program.add_engine_instance();

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

    return rsl::okay;
}
