/// @ref gtx_matrix_decompose
/// @file glm/gtx/matrix_decompose.hpp
///
/// @see core (dependence)
///
/// @defgroup gtx_matrix_decompose GLM_GTX_matrix_decompose
/// @ingroup gtx
///
/// Include <glm/gtx/matrix_decompose.hpp> to use the features of this extension.
///
/// Decomposes a model matrix to translations, rotation and scale components

#pragma once

// Dependencies
#include "../mat4x4.hpp"
#include "../vec3.hpp"
#include "../vec4.hpp"
#include "../geometric.hpp"
#include "../gtc/quaternion.hpp"
#include "../gtc/matrix_transform.hpp"

#if GLM_MESSAGES == GLM_ENABLE && !defined(GLM_EXT_INCLUDED)
#   ifndef GLM_ENABLE_EXPERIMENTAL
#       pragma message("GLM: GLM_GTX_matrix_decompose is an experimental extension and may change in the future. Use #define GLM_ENABLE_EXPERIMENTAL before including it, if you really want to use it.")
#   else
#       pragma message("GLM: GLM_GTX_matrix_decompose extension included")
#   endif
#endif

namespace legion::core::math
{
    /// @addtogroup gtx_matrix_decompose
    /// @{

    /// Decomposes a model matrix to translations, rotation and scale components
    /// @see gtx_matrix_decompose
    template<typename T, qualifier Q>
    GLM_FUNC_DECL bool decompose(
        mat<4, 4, T, Q> const& modelMatrix,
        vec<3, T, Q> & scale, qua<T, Q> & orientation, vec<3, T, Q> & translation, vec<3, T, Q> & skew, vec<4, T, Q> & perspective);

    template<typename T, qualifier Q>
    GLM_FUNC_DECL void compose(
        mat<4, 4, T, Q> & modelMatrix,
        vec<3, T, Q> const& scale, qua<T, Q> const& orientation, vec<3, T, Q> const& translation);

    template<typename T, qualifier Q>
    GLM_FUNC_DECL mat<4, 4, T, Q> compose(
        vec<3, T, Q> const& scale, qua<T, Q> const& orientation, vec<3, T, Q> const& translation);

    template<typename T, qualifier Q>
    GLM_FUNC_DECL bool decompose(
        mat<4, 4, T, Q> const& modelMatrix,
        vec<3, T, Q> & scale, qua<T, Q> & orientation, vec<3, T, Q> & translation);

    /// @}
}//namespace legion::core::math

#include "matrix_decompose.inl"
