#pragma once

///
/// This system is not used in the project and just here for an example!!!
///

#include <core/core.hpp>

class MySystem final : public lgn::System<MySystem>
{
public:
    auto to_string(lgn::channel_format format)
    {
        switch (format)
        {
        case lgn::channel_format::eight_bit:
            return "8bit";
        case lgn::channel_format::sixteen_bit:
            return "16bit";
        case lgn::channel_format::float_hdr:
            return "hdr";
        }
        return "unknown";
    }

    auto to_string(lgn::image_components components)
    {
        switch (components)
        {
        case lgn::image_components::grey:
            return "grey";
        case lgn::image_components::grey_alpha:
            return "grey alpha";
        case lgn::image_components::rgb:
            return "rgb";
        case lgn::image_components::rgba:
            return "rgba";
        }
        return "unknown";
    }

    void setup()
    {
        using namespace legion;
        auto result = assets::load<image>(fs::view("engine://resources/default/albedo"));
        if (!result)
            log::error("{}", result.error());

        if (result.has_warnings())
            for (auto& warn : result.warnings())
                log::warn(warn);

        auto val = result.except([](exception& error) {return assets::asset<image>{}; });

        if(result)
        {
            auto img = result.value();
            log::debug("\nname: {}\nid: {}\nresolution: {}\nformat: {}\ncomponents: {}",
                img.name(), img.id(), img->resolution(), to_string(img->format()), to_string(img->components()));
        }
    }

    void update(lgn::time::span deltaTime)
    {
        raiseEvent<lgn::events::exit>();
    }
};





