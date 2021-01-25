#pragma once
#include <core/types/primitives.hpp>
#include <core/types/type_util.hpp>
#include <core/platform/platform.hpp>
#include <core/async/ring_sync_lock.hpp>
#include <unordered_map>

/**
 * @file event.hpp
 */

namespace legion::core::events
{
    namespace detail
    {
        static async::rw_spinlock eventNameLock;
        static std::unordered_map<id_type, std::string> eventNames;

        template<typename T>
        id_type reportEventType()
        {
            eventNames[typeHash<T>()] = nameOfType<T>();
            return typeHash<T>();
        }
    }

    /**@class event_base
     * @brief Base class of all events for polymorphic storage.
     */
    struct event_base
    {
        /**@brief Returns a value that decides whether the event should be kept in the bus or destroyed after all subscribers have been notified.
         */
        virtual bool persistent() LEGION_IMPURE_RETURN(false);

        /**@brief Returns a value that decides whether more than one of this event type should be able to exist.
         *		  If true and an event of this type already exists then the event-bus will only notify the subscribers and nothing else.
         */
        virtual bool unique() LEGION_IMPURE_RETURN(true);

        virtual ~event_base() = default;

        /**@brief Get's the type id of an event. (only available in event_base)
         */
        virtual id_type get_id() LEGION_PURE;
    };

    /**@class event
     * @brief Base class all user defined events should inherit from. This ensures that each event has it's type id and that it can statically be requested.
     */
    template<typename Self>
    struct event : public event_base
    {
        friend 	class EventBus;

        inline static const id_type id = detail::reportEventType<Self>();

        virtual ~event() = default;
    private:
        virtual id_type get_id()
        {
            return id;
        }
    };
}
