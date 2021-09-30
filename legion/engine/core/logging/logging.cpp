#include <core/logging/logging.hpp>


namespace legion::core::log
{
    impl& impl::get()
    {
        static impl instance;
        return instance;
    }

}
