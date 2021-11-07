#pragma once

#include <cassert>

#include <core/platform/platform.hpp>

/**
 * @file assert.hpp
 */

#if defined(LEGION_VALIDATE)

/**@def assert_msg
 * @brief same as assert, but you can include a message
 * @param msg the message you want to include when the assertion fails
 * @param expr the expression to be asserted
 */
#define assert_msg(msg,expr) assert(((void)(msg),(expr)))

#else

#undef assert

#define assert_msg(msg, expr) ((void)0)
#define assert(expr) ((void)0)

#endif
#include <core/common/exception.hpp>
