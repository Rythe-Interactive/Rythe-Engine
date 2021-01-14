#pragma once
#include <core/core.hpp>

namespace ext
{
    //boilerplate
    struct animation_event_base;
    template <class T> struct animation_event;
    //


    /**
     * @struct animation_event_base
     * @brief extension class for animation_events.
     * @note your custom animation-events should not have any other data, than
     *        the one provided through receive_param & enumerate_param
     */
    struct animation_event_base
    {
        /** @brief saves a key,value pair in the params list
         */
        void receive_param(const std::string& key, const std::string& value)
        {
            m_params[key] = value;
        }

        /** @brief retrieves a value by key
         */
        L_NODISCARD std::string get_param(const std::string& key) const noexcept
        {
            if (auto itr = m_params.find(key); itr != m_params.end())
            {
                return itr->second;
            }
            return "";
        }

        /** @brief retrieves all currently stored kv-pairs
         */
        L_NODISCARD const std::unordered_map<std::string, std::string>& enumerate_params() const noexcept
        {
            return m_params;
        }

        /** @brief clones the event
         */
        L_NODISCARD legion::events::event_base* surrogate_ctor() const;

    protected:
        friend struct animation;
        std::unordered_map<std::string, std::string> m_params;

    };

    /**
     * @struct animation_event
     * @brief Custom animation events should inherit from this
     * @tparam T Type of your custom event
     */
    template <class T>
    struct animation_event :
        legion::events::event<T>,
        animation_event_base{};

    /**@struct void_animation_event 
     * @brief default event that is emitted, when the type is not further specialized
     */
    struct void_animation_event : animation_event<void_animation_event> {};

    inline legion::events::event_base* animation_event_base::surrogate_ctor() const
    {
        auto* t = new void_animation_event();
        t->m_params = this->m_params;
        return t;
    }
}
