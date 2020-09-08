namespace args::core::ecs
{
    template<typename component_type, typename ...component_types>
    inline std::tuple<component_handle<component_type>, component_handle<component_types>...> archetype<component_type, component_types...>::create(EcsRegistry* registry, id_type entityId)
    {
        return registry->createComponents<component_type, component_types...>(entityId);
    }
}
