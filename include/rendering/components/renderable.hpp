#pragma once
#include <rendering/data/model.hpp>
#include <rendering/data/material.hpp>
#include <core/serialization/serializationUtil.hpp>

namespace legion::rendering
{
    struct renderable
    {
        model_handle model;
        material_handle material;

        template<typename Archive>
        void serialize(Archive& archive);
    };

    template<typename Archive>
    void renderable::serialize(Archive& archive)
    {
        archive(this->model,this->material);
    }
}
