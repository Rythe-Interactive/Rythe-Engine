#pragma once
#include <rendering/data/model.hpp>
#include <rendering/data/material.hpp>

namespace legion::rendering
{
    struct renderable
    {
        model_handle model;
        material_handle material;
    };
}
