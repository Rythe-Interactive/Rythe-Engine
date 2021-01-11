#include "animation.hpp"
#include <tinygltf/json.hpp>

namespace ext
{
    namespace detail
    {
        inline core::math::vec3 vec3_from_json_array(nlohmann::json::array_t a)
        {
            return core::math::vec3{ a[0].get<float>(), a[1].get<float>(), a[2].get<float>() };
        }

        inline core::math::quat quat_from_json_array(nlohmann::json::array_t a)
        {
            return core::math::quat{ a[0].get<float>(), a[1].get<float>(), a[2].get<float>(), a[3].get<float>() };
        }
    }

    void animation::from_resource(animation* anim, const core::filesystem::basic_resource& resource)
    {

        //introduce convenience usings
        using json = nlohmann::json;
        using jptr = nlohmann::json_pointer<json>;


        //parse the file as json
        json j = json::parse(resource);


        //clear previous animation
        anim->position_key_frames.clear();
        anim->rotation_key_frames.clear();
        anim->scale_key_frames.clear();


        //create pointers to the data we want
        const auto posptr = jptr("/animation/positions");
        const auto rotptr = jptr("/animation/rotations");
        const auto sclptr = jptr("/animation/scales");
        const auto evtptr = jptr("/animation/events");
        const auto loopingptr = jptr("/animation/looping");




        //read the looping value from the json
        anim->looping = j.value(loopingptr, false);


        //get access to the arrays
        auto positions = j.value(posptr, json::array());
        auto rotations = j.value(rotptr, json::array());
        auto scales = j.value(sclptr, json::array());
        auto events = j.value(evtptr, json::array());

        //parse position arrays
        for (auto& elem : positions)
        {
            const auto dur = elem.find("duration");
            const auto pl = elem.find("payload");

            if (dur != elem.end() && pl != elem.end())
            {
                //insert position keyframe
                anim->position_key_frames.emplace_back(dur->get<int>(),
                    core::position(detail::vec3_from_json_array(
                        pl->get<json::array_t>())));
            }
        }

        //parse rotations
        for (auto& elem : rotations)
        {
            const auto dur = elem.find("duration");
            const auto pl = elem.find("payload");

            if (dur != elem.end() && pl != elem.end())
            {
                //insert rotation keyframe
                anim->rotation_key_frames.emplace_back(dur->get<int>(),
                    core::rotation(detail::quat_from_json_array(
                        pl->get<json::array_t>())));
            }
        }

        //parse scales
        for (auto& elem : scales)
        {
            const auto dur = elem.find("duration");
            const auto pl = elem.find("payload");

            if (dur != elem.end() && pl != elem.end())
            {
                //insert scale keyframe
                anim->scale_key_frames.emplace_back(dur->get<int>(),
                    core::scale(detail::vec3_from_json_array(pl->get<json::array_t>())));
            }
        }

        //parse events
        for (auto& elem : events)
        {
            id_type id = invalid_id;
            float trigger = std::numeric_limits<float>::max();
            std::unordered_map<std::string, std::string> meta;
            for (auto& [key, value] : elem.items())
            {
                if (key == "id") {
                    id = value.get<id_type>();
                }
                else if(key == "trigger")
                {
                    trigger = value.get<float>();
                }
                else {
                    meta[key] = value.get<std::string>();
                }
            }
            if(id != invalid_id && trigger != std::numeric_limits<float>::max())
            {
                auto ptr = std::shared_ptr<animation_event_base>(
                    std::make_shared<void_animation_event>());
                ptr->m_params = meta;
                auto pair = std::make_pair(std::move(ptr),id);

                anim->events.emplace_back(trigger,std::move(pair));
            }

        }
    }

    void animation::to_resource(core::filesystem::basic_resource* resource, const animation& anim)
    {
        using json = nlohmann::json;

        json j;
        j["animation"] = json::object(
            {
                {"positions",json::array()},
                {"rotations",json::array()},
                {"scales",json::array()},
                {"events",json::array()},
                {"looping",anim.looping}
            }
        );

        for (const auto& [duration, pos] : anim.position_key_frames) {
            j["animation"]["positions"] += json::object({ {"duration",duration},{"payload",json::array({pos.x,pos.y,pos.z})} });
        }
        for (const auto& [duration, rot] : anim.rotation_key_frames) {
            j["animation"]["rotations"] += json::object({ {"duration",duration},{"payload",json::array({rot.w,rot.x,rot.y,rot.z})} });
        }
        for (const auto& [duration, sc] : anim.scale_key_frames) {
            j["animation"]["scales"] += json::object({ {"duration",duration},{"payload",json::array({sc.x,sc.y,sc.z})} });
        }

        for (const auto& [trig, ev] : anim.events) {

            const auto& [evptr,id] = ev;
            auto evobj = json::object({{"id",id},{"trigger",trig}});
            for(auto& [key,value]: evptr->m_params)
            {
                evobj[key] = value;
            }

            j["animation"]["events"] += evobj;


        }

        *(resource) = core::filesystem::basic_resource(j.dump(4));
    }
}

