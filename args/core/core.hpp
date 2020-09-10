#pragma once

/**
 * @file core.hpp
 * @brief Single include header for Args-Core
 */

#include <core/platform/platform.hpp>

#include <core/math/math.hpp>
#include <core/common/common.hpp>
#include <core/types/types.hpp>
#include <core/time/time.hpp>
#include <core/async/async.hpp>
#include <core/containers/containers.hpp>
#include <core/ecs/ecs.hpp>
#include <core/scheduling/scheduling.hpp>
#include <core/events/events.hpp>
#include <core/entry/entry_point.hpp>
#include <core/engine/engine.hpp>

#include <core/filesystem/filesystem.hpp>
#include <core/data/data.hpp>

namespace args
{
    using namespace core;
}
