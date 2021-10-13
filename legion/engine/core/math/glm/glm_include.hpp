#pragma once
#include <core/platform/platform.hpp>

#define GLM_FORCE_SWIZZLE_COMPAT
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED

#if !defined(DOXY_EXCLUDE)
#include "glm.hpp"
#include "ext.hpp"
#endif
