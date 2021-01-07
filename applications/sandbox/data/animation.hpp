#pragma once
#include <vector>
#include <core/defaults/defaultcomponents.hpp>
#include <cstdint>
#include <optional>
#include <tinygltf/json.hpp>

namespace ext
{
    using namespace legion;
    struct animation
    {
        bool running = false;
        bool looping = false;
        std::vector<std::pair<int, position>>   position_key_frames;
        std::vector<std::pair<int, rotation>>   rotation_key_frames;
        std::vector<std::pair<int, scale>>      scale_key_frames;

        index_type p_index = 0;
        index_type r_index = 0;
        index_type s_index = 0;

        float p_accumulator = 0.0f;
        float r_accumulator = 0.0f;
        float s_accumulator = 0.0f;

    };

    inline void serialize_animation(fs::view disk_location, const animation& anim)
    {
        using json = nlohmann::json;

        json j;
        j["animation"] = json::object(
            {
                {"positions",json::array()},
                {"rotations",json::array()},
                {"scales",json::array()},
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

        disk_location.set(filesystem::basic_resource(j.dump(4)));
    }

    
    inline math::vec3 vec3_from_json_array(nlohmann::json::array_t a)
    {
        return math::vec3{ a[0].get<float>(), a[1].get<float>(), a[2].get<float>() };
    }

    inline math::quat quat_from_json_array(nlohmann::json::array_t a)
    {
        return math::quat{ a[0].get<float>(), a[1].get<float>(), a[2].get<float>(), a[3].get<float>() };
    }

    inline void deserialize_animation(fs::view disk_location, animation& anim)
    {
        using json = nlohmann::json;
        using jptr = nlohmann::json_pointer<json>;

        json j = json::parse(disk_location.get().except([](auto err) { return fs::basic_resource("{}"); }));

        anim.position_key_frames.clear();
        anim.rotation_key_frames.clear();
        anim.scale_key_frames.clear();

        const auto posptr = jptr("/animation/positions");
        const auto rotptr = jptr("/animation/rotations");
        const auto sclptr = jptr("/animation/scales");
        const auto loopingptr = jptr("/animation/looping");
        
        anim.looping = j.value(loopingptr,false); 

        auto positions = j.value(posptr, json::array());
        auto rotations = j.value(rotptr, json::array());
        auto scales = j.value(sclptr, json::array());

        for (auto& elem : positions)
        {
            const auto dur = elem.find("duration");
            const auto pl = elem.find("payload");

            if (dur != elem.end() && pl != elem.end())
            {
                anim.position_key_frames.emplace_back(dur->get<int>(), position(vec3_from_json_array(pl->get<json::array_t>())));
            }
        }
        
        for (auto& elem : rotations)
        {
            const auto dur = elem.find("duration");
            const auto pl = elem.find("payload");

            if (dur != elem.end() && pl != elem.end())
            {
                anim.rotation_key_frames.emplace_back(dur->get<int>(), rotation(quat_from_json_array(pl->get<json::array_t>())));
            }
        }
        for (auto& elem : scales)
        {
            const auto dur = elem.find("duration");
            const auto pl = elem.find("payload");

            if (dur != elem.end() && pl != elem.end())
            {
                anim.scale_key_frames.emplace_back(dur->get<int>(), scale(vec3_from_json_array(pl->get<json::array_t>())));
            }
        }
    }
}
