#pragma once
#include <core/core.hpp>

namespace legion::editor
{
    struct menu_option_base
    {
        const std::string name;

        menu_option_base(const std::string& menuName) : name(menuName) {}

        virtual void execute() LEGION_PURE;
    };

    template<typename executable>
    struct menu_option : public menu_option_base
    {
    private:
        executable m_exec;

    public:
        menu_option(const std::string& name, executable&& exec) : menu_option_base(name), m_exec(exec) {}
        menu_option(const std::string& name, const executable& exec) : menu_option_base(name), m_exec(exec) {}

        virtual void execute() override
        {
            m_exec();
        }
    };
}
