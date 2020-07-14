#pragma once
#include <cassert>

/**
 * @file common.hpp
 * @brief collection of common utilities that don't fall into a specific category
 */


/**@def assert_msg
 * @brief same as assert, but you can include a message
 * @param msg the message you want to include when the assertion fails
 * @param expr the expression to be asserted
 */
#define assert_msg(msg,expr) assert(((void)(msg),(expr)))