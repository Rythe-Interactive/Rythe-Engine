#pragma once
/**
 * @file platform.hpp
 */

#pragma region //////////////////////////////////////// Utils ///////////////////////////////////////////

#if !defined(PROJECT_NAME)
#define PROJECT_NAME user_project
#endif

#define EXPAND(x) x
#define CALL(x, y) x(y)

#define NARGS_(_1, _2, _3, _4, _5 , _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, N, ...) N
#define NARGS(...) EXPAND(NARGS_(__VA_ARGS__, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1))

#define CONCAT(A, B) A ## B

#define CONCAT_DEFINE(A, B) CONCAT(A, B)

#define STRINGIFY_IMPL(x) #x
#define STRINGIFY(x) STRINGIFY_IMPL(x)

#define  L_NAME_1(x)                                                                    #x
#define  L_NAME_2(x, x2)                                                                #x , #x2
#define  L_NAME_3(x, x2, x3)                                                            #x , #x2 , #x3
#define  L_NAME_4(x, x2, x3, x4)                                                        #x , #x2 , #x3 , #x4
#define  L_NAME_5(x, x2, x3, x4, x5)                                                    #x , #x2 , #x3 , #x4 , #x5
#define  L_NAME_6(x, x2, x3, x4, x5, x6)                                                #x , #x2 , #x3 , #x4 , #x5 , #x6
#define  L_NAME_7(x, x2, x3, x4, x5, x6, x7)                                            #x , #x2 , #x3 , #x4 , #x5 , #x6 , #x7
#define  L_NAME_8(x, x2, x3, x4, x5, x6, x7, x8)                                        #x , #x2 , #x3 , #x4 , #x5 , #x6 , #x7 , #x8
#define  L_NAME_9(x, x2, x3, x4, x5, x6, x7, x8, x9)                                    #x , #x2 , #x3 , #x4 , #x5 , #x6 , #x7 , #x8 , #x9
#define L_NAME_10(x, x2, x3, x4, x5, x6, x7, x8, x9, x10)                               #x , #x2 , #x3 , #x4 , #x5 , #x6 , #x7 , #x8 , #x9 , #x10
#define L_NAME_11(x, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11)                          #x , #x2 , #x3 , #x4 , #x5 , #x6 , #x7 , #x8 , #x9 , #x10 , #x11
#define L_NAME_12(x, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12)                     #x , #x2 , #x3 , #x4 , #x5 , #x6 , #x7 , #x8 , #x9 , #x10 , #x11 , #x12
#define L_NAME_13(x, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13)                #x , #x2 , #x3 , #x4 , #x5 , #x6 , #x7 , #x8 , #x9 , #x10 , #x11 , #x12 , #x13
#define L_NAME_14(x, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14)           #x , #x2 , #x3 , #x4 , #x5 , #x6 , #x7 , #x8 , #x9 , #x10 , #x11 , #x12 , #x13 , #x14
#define L_NAME_15(x, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15)      #x , #x2 , #x3 , #x4 , #x5 , #x6 , #x7 , #x8 , #x9 , #x10 , #x11 , #x12 , #x13 , #x14 , #x15
#define L_NAME_16(x, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16) #x , #x2 , #x3 , #x4 , #x5 , #x6 , #x7 , #x8 , #x9 , #x10 , #x11 , #x12 , #x13 , #x14 , #x15 , #x16

 // turn: value0, value1, value2
 // into: "value0", "value1", "value2"
#define STRINGIFY_SEPERATE(...) EXPAND(CONCAT_DEFINE(L_NAME_, NARGS(__VA_ARGS__))(__VA_ARGS__))

#define  pre_1(prefix, x)                                                                    prefix##x
#define  pre_2(prefix, x, x2)                                                                prefix##x , prefix##x2
#define  pre_3(prefix, x, x2, x3)                                                            prefix##x , prefix##x2, prefix##x3
#define  pre_4(prefix, x, x2, x3, x4)                                                        prefix##x , prefix##x2, prefix##x3, prefix##x4
#define  pre_5(prefix, x, x2, x3, x4, x5)                                                    prefix##x , prefix##x2, prefix##x3, prefix##x4, prefix##x5
#define  pre_6(prefix, x, x2, x3, x4, x5, x6)                                                prefix##x , prefix##x2, prefix##x3, prefix##x4, prefix##x5, prefix##x6
#define  pre_7(prefix, x, x2, x3, x4, x5, x6, x7)                                            prefix##x , prefix##x2, prefix##x3, prefix##x4, prefix##x5, prefix##x6, prefix##x7
#define  pre_8(prefix, x, x2, x3, x4, x5, x6, x7, x8)                                        prefix##x , prefix##x2, prefix##x3, prefix##x4, prefix##x5, prefix##x6, prefix##x7, prefix##x8
#define  pre_9(prefix, x, x2, x3, x4, x5, x6, x7, x8, x9)                                    prefix##x , prefix##x2, prefix##x3, prefix##x4, prefix##x5, prefix##x6, prefix##x7, prefix##x8, prefix##x9
#define pre_10(prefix, x, x2, x3, x4, x5, x6, x7, x8, x9, x10)                               prefix##x , prefix##x2, prefix##x3, prefix##x4, prefix##x5, prefix##x6, prefix##x7, prefix##x8, prefix##x9, prefix##x10
#define pre_11(prefix, x, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11)                          prefix##x , prefix##x2, prefix##x3, prefix##x4, prefix##x5, prefix##x6, prefix##x7, prefix##x8, prefix##x9, prefix##x10, prefix##x11
#define pre_12(prefix, x, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12)                     prefix##x , prefix##x2, prefix##x3, prefix##x4, prefix##x5, prefix##x6, prefix##x7, prefix##x8, prefix##x9, prefix##x10, prefix##x11, prefix##x12
#define pre_13(prefix, x, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13)                prefix##x , prefix##x2, prefix##x3, prefix##x4, prefix##x5, prefix##x6, prefix##x7, prefix##x8, prefix##x9, prefix##x10, prefix##x11, prefix##x12, prefix##x13
#define pre_14(prefix, x, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14)           prefix##x , prefix##x2, prefix##x3, prefix##x4, prefix##x5, prefix##x6, prefix##x7, prefix##x8, prefix##x9, prefix##x10, prefix##x11, prefix##x12, prefix##x13, prefix##x14
#define pre_15(prefix, x, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15)      prefix##x , prefix##x2, prefix##x3, prefix##x4, prefix##x5, prefix##x6, prefix##x7, prefix##x8, prefix##x9, prefix##x10, prefix##x11, prefix##x12, prefix##x13, prefix##x14, prefix##x15
#define pre_16(prefix, x, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16) prefix##x , prefix##x2, prefix##x3, prefix##x4, prefix##x5, prefix##x6, prefix##x7, prefix##x8, prefix##x9, prefix##x10, prefix##x11, prefix##x12, prefix##x13, prefix##x14, prefix##x15, prefix##x16

#define prepost_1(prefix, postfix, x)                             CAT(prefix##x, postfix)
#define prepost_2(prefix, postfix, x, x2)                         CAT(prefix##x, postfix) , CAT(prefix##x2, postfix)
#define prepost_3(prefix, postfix, x, x2, x3)                     CAT(prefix##x, postfix) , CAT(prefix##x2, postfix), CAT(prefix##x3, postfix)
#define prepost_4(prefix, postfix, x, x2, x3, x4)                 CAT(prefix##x, postfix) , CAT(prefix##x2, postfix), CAT(prefix##x3, postfix), CAT(prefix##x4, postfix)
#define prepost_5(prefix, postfix, x, x2, x3, x4, x5)             CAT(prefix##x, postfix) , CAT(prefix##x2, postfix), CAT(prefix##x3, postfix), CAT(prefix##x4, postfix), CAT(prefix##x5, postfix)
#define prepost_6(prefix, postfix, x, x2, x3, x4, x5, x6)         CAT(prefix##x, postfix) , CAT(prefix##x2, postfix), CAT(prefix##x3, postfix), CAT(prefix##x4, postfix), CAT(prefix##x5, postfix), CAT(prefix##x6, postfix)
#define prepost_7(prefix, postfix, x, x2, x3, x4, x5, x6, x7)     CAT(prefix##x, postfix) , CAT(prefix##x2, postfix), CAT(prefix##x3, postfix), CAT(prefix##x4, postfix), CAT(prefix##x5, postfix), CAT(prefix##x6, postfix), CAT(prefix##x7, postfix)
#define prepost_8(prefix, postfix, x, x2, x3, x4, x5, x6, x7, x8) CAT(prefix##x, postfix) , CAT(prefix##x2, postfix), CAT(prefix##x3, postfix), CAT(prefix##x4, postfix), CAT(prefix##x5, postfix), CAT(prefix##x6, postfix), CAT(prefix##x7, postfix), CAT(prefix##x8, postfix)

#define  decltype_1(x)                                                                    decltype(x)
#define  decltype_2(x, x2)                                                                decltype(x) , decltype(x2)
#define  decltype_3(x, x2, x3)                                                            decltype(x) , decltype(x2), decltype(x3)
#define  decltype_4(x, x2, x3, x4)                                                        decltype(x) , decltype(x2), decltype(x3), decltype(x4)
#define  decltype_5(x, x2, x3, x4, x5)                                                    decltype(x) , decltype(x2), decltype(x3), decltype(x4), decltype(x5)
#define  decltype_6(x, x2, x3, x4, x5, x6)                                                decltype(x) , decltype(x2), decltype(x3), decltype(x4), decltype(x5), decltype(x6)
#define  decltype_7(x, x2, x3, x4, x5, x6, x7)                                            decltype(x) , decltype(x2), decltype(x3), decltype(x4), decltype(x5), decltype(x6), decltype(x7)
#define  decltype_8(x, x2, x3, x4, x5, x6, x7, x8)                                        decltype(x) , decltype(x2), decltype(x3), decltype(x4), decltype(x5), decltype(x6), decltype(x7), decltype(x8)
#define  decltype_9(x, x2, x3, x4, x5, x6, x7, x8, x9)                                    decltype(x) , decltype(x2), decltype(x3), decltype(x4), decltype(x5), decltype(x6), decltype(x7), decltype(x8), decltype(x9)
#define decltype_10(x, x2, x3, x4, x5, x6, x7, x8, x9, x10)                               decltype(x) , decltype(x2), decltype(x3), decltype(x4), decltype(x5), decltype(x6), decltype(x7), decltype(x8), decltype(x9) , decltype(x10)
#define decltype_11(x, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11)                          decltype(x) , decltype(x2), decltype(x3), decltype(x4), decltype(x5), decltype(x6), decltype(x7), decltype(x8), decltype(x9) , decltype(x10), decltype(x11)
#define decltype_12(x, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12)                     decltype(x) , decltype(x2), decltype(x3), decltype(x4), decltype(x5), decltype(x6), decltype(x7), decltype(x8), decltype(x9) , decltype(x10), decltype(x11), decltype(x12)
#define decltype_13(x, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13)                decltype(x) , decltype(x2), decltype(x3), decltype(x4), decltype(x5), decltype(x6), decltype(x7), decltype(x8), decltype(x9) , decltype(x10), decltype(x11), decltype(x12), decltype(x13)
#define decltype_14(x, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14)           decltype(x) , decltype(x2), decltype(x3), decltype(x4), decltype(x5), decltype(x6), decltype(x7), decltype(x8), decltype(x9) , decltype(x10), decltype(x11), decltype(x12), decltype(x13), decltype(x14)
#define decltype_15(x, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15)      decltype(x) , decltype(x2), decltype(x3), decltype(x4), decltype(x5), decltype(x6), decltype(x7), decltype(x8), decltype(x9) , decltype(x10), decltype(x11), decltype(x12), decltype(x13), decltype(x14), decltype(x15)
#define decltype_16(x, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16) decltype(x) , decltype(x2), decltype(x3), decltype(x4), decltype(x5), decltype(x6), decltype(x7), decltype(x8), decltype(x9) , decltype(x10), decltype(x11), decltype(x12), decltype(x13), decltype(x14), decltype(x15), decltype(x16)

#define  colon_1(prefix, x)                                                                    prefix::x
#define  colon_2(prefix, x, x2)                                                                prefix::x , prefix::x2
#define  colon_3(prefix, x, x2, x3)                                                            prefix::x , prefix::x2, prefix::x3
#define  colon_4(prefix, x, x2, x3, x4)                                                        prefix::x , prefix::x2, prefix::x3, prefix::x4
#define  colon_5(prefix, x, x2, x3, x4, x5)                                                    prefix::x , prefix::x2, prefix::x3, prefix::x4, prefix::x5
#define  colon_6(prefix, x, x2, x3, x4, x5, x6)                                                prefix::x , prefix::x2, prefix::x3, prefix::x4, prefix::x5, prefix::x6
#define  colon_7(prefix, x, x2, x3, x4, x5, x6, x7)                                            prefix::x , prefix::x2, prefix::x3, prefix::x4, prefix::x5, prefix::x6, prefix::x7
#define  colon_8(prefix, x, x2, x3, x4, x5, x6, x7, x8)                                        prefix::x , prefix::x2, prefix::x3, prefix::x4, prefix::x5, prefix::x6, prefix::x7, prefix::x8
#define  colon_9(prefix, x, x2, x3, x4, x5, x6, x7, x8, x9)                                    prefix::x , prefix::x2, prefix::x3, prefix::x4, prefix::x5, prefix::x6, prefix::x7, prefix::x8, prefix::x9
#define colon_10(prefix, x, x2, x3, x4, x5, x6, x7, x8, x9, x10)                               prefix::x , prefix::x2, prefix::x3, prefix::x4, prefix::x5, prefix::x6, prefix::x7, prefix::x8, prefix::x9, prefix::x10
#define colon_11(prefix, x, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11)                          prefix::x , prefix::x2, prefix::x3, prefix::x4, prefix::x5, prefix::x6, prefix::x7, prefix::x8, prefix::x9, prefix::x10, prefix::x11
#define colon_12(prefix, x, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12)                     prefix::x , prefix::x2, prefix::x3, prefix::x4, prefix::x5, prefix::x6, prefix::x7, prefix::x8, prefix::x9, prefix::x10, prefix::x11, prefix::x12
#define colon_13(prefix, x, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13)                prefix::x , prefix::x2, prefix::x3, prefix::x4, prefix::x5, prefix::x6, prefix::x7, prefix::x8, prefix::x9, prefix::x10, prefix::x11, prefix::x12, prefix::x13
#define colon_14(prefix, x, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14)           prefix::x , prefix::x2, prefix::x3, prefix::x4, prefix::x5, prefix::x6, prefix::x7, prefix::x8, prefix::x9, prefix::x10, prefix::x11, prefix::x12, prefix::x13, prefix::x14
#define colon_15(prefix, x, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15)      prefix::x , prefix::x2, prefix::x3, prefix::x4, prefix::x5, prefix::x6, prefix::x7, prefix::x8, prefix::x9, prefix::x10, prefix::x11, prefix::x12, prefix::x13, prefix::x14, prefix::x15
#define colon_16(prefix, x, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16) prefix::x , prefix::x2, prefix::x3, prefix::x4, prefix::x5, prefix::x6, prefix::x7, prefix::x8, prefix::x9, prefix::x10, prefix::x11, prefix::x12, prefix::x13, prefix::x14, prefix::x15, prefix::x16

#define  dot_1(prefix, x)                                                                    prefix.x
#define  dot_2(prefix, x, x2)                                                                prefix.x , prefix.x2
#define  dot_3(prefix, x, x2, x3)                                                            prefix.x , prefix.x2, prefix.x3
#define  dot_4(prefix, x, x2, x3, x4)                                                        prefix.x , prefix.x2, prefix.x3, prefix.x4
#define  dot_5(prefix, x, x2, x3, x4, x5)                                                    prefix.x , prefix.x2, prefix.x3, prefix.x4, prefix.x5
#define  dot_6(prefix, x, x2, x3, x4, x5, x6)                                                prefix.x , prefix.x2, prefix.x3, prefix.x4, prefix.x5, prefix.x6
#define  dot_7(prefix, x, x2, x3, x4, x5, x6, x7)                                            prefix.x , prefix.x2, prefix.x3, prefix.x4, prefix.x5, prefix.x6, prefix.x7
#define  dot_8(prefix, x, x2, x3, x4, x5, x6, x7, x8)                                        prefix.x , prefix.x2, prefix.x3, prefix.x4, prefix.x5, prefix.x6, prefix.x7, prefix.x8
#define  dot_9(prefix, x, x2, x3, x4, x5, x6, x7, x8, x9)                                    prefix.x , prefix.x2, prefix.x3, prefix.x4, prefix.x5, prefix.x6, prefix.x7, prefix.x8, prefix.x9
#define dot_10(prefix, x, x2, x3, x4, x5, x6, x7, x8, x9, x10)                               prefix.x , prefix.x2, prefix.x3, prefix.x4, prefix.x5, prefix.x6, prefix.x7, prefix.x8, prefix.x9, prefix.x10
#define dot_11(prefix, x, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11)                          prefix.x , prefix.x2, prefix.x3, prefix.x4, prefix.x5, prefix.x6, prefix.x7, prefix.x8, prefix.x9, prefix.x10, prefix.x11
#define dot_12(prefix, x, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12)                     prefix.x , prefix.x2, prefix.x3, prefix.x4, prefix.x5, prefix.x6, prefix.x7, prefix.x8, prefix.x9, prefix.x10, prefix.x11, prefix.x12
#define dot_13(prefix, x, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13)                prefix.x , prefix.x2, prefix.x3, prefix.x4, prefix.x5, prefix.x6, prefix.x7, prefix.x8, prefix.x9, prefix.x10, prefix.x11, prefix.x12, prefix.x13
#define dot_14(prefix, x, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14)           prefix.x , prefix.x2, prefix.x3, prefix.x4, prefix.x5, prefix.x6, prefix.x7, prefix.x8, prefix.x9, prefix.x10, prefix.x11, prefix.x12, prefix.x13, prefix.x14
#define dot_15(prefix, x, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15)      prefix.x , prefix.x2, prefix.x3, prefix.x4, prefix.x5, prefix.x6, prefix.x7, prefix.x8, prefix.x9, prefix.x10, prefix.x11, prefix.x12, prefix.x13, prefix.x14, prefix.x15
#define dot_16(prefix, x, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16) prefix.x , prefix.x2, prefix.x3, prefix.x4, prefix.x5, prefix.x6, prefix.x7, prefix.x8, prefix.x9, prefix.x10, prefix.x11, prefix.x12, prefix.x13, prefix.x14, prefix.x15, prefix.x16

 // turn: value, 0, 1, 2
 // into: value0, value1, value2
#define CAT_PREFIX(prefix, ...) EXPAND(CONCAT_DEFINE(pre_, NARGS(__VA_ARGS__))(prefix, __VA_ARGS__))

// turn: value, p, 0, 1, 2
// into: value0p, value1p, value2p
#define CAT_PREPOSTFIX(prefix, postfix, ...) EXPAND(CONCAT_DEFINE(prepost_, NARGS(__VA_ARGS__))(prefix, postfix, __VA_ARGS__))

// turn: 0, 1, 2
// into: decltype(0), decltype(1), decltype(2)
#define decltypes_count(count, ...) EXPAND(CONCAT_DEFINE(decltype_, count)(__VA_ARGS__))
#define decltypes(...) EXPAND(CONCAT_DEFINE(decltype_, NARGS(__VA_AGRS__))(__VA_ARGS__))

// turn: Foo, x, y, z
// into: Foo::x, Foo::y, Foo::z
#define colon_access_count(count, prefix, ...) EXPAND(CONCAT_DEFINE(colon_, count)(prefix, __VA_ARGS__))
#define colon_access(prefix, ...) EXPAND(CONCAT_DEFINE(colon_, NARGS(__VA_ARGS__))(prefix, __VA_ARGS__))

// turn: foo, x, y, z
// into: foo.x, foo.y, foo.z
#define dot_access_count(count, prefix, ...) EXPAND(CONCAT_DEFINE(dot_, count)(prefix, __VA_ARGS__))
#define dot_access(prefix, ...) EXPAND(CONCAT_DEFINE(dot_, NARGS(__VA_ARGS__))(prefix, __VA_ARGS__))

#define RULE_OF_5(type)\
type() = default;\
type(const type&) = default;\
type(type&&) = default;\
type& operator=(const type&) = default;\
type& operator=(type&&) = default;

#define LEGION_DEBUG_VALUE 1
#define LEGION_RELEASE_VALUE 2

#if defined(DOXY_EXCLUDE)
#define NDOXY(...)
#define CNDOXY(...)
#else
#define NDOXY(args...) args
#define CNDOXY(args...) , args
#endif

#if defined(_DEBUG) || defined(DEBUG)
/**@def LEGION_DEBUG
 * @brief Defined in debug mode.
 */
#define LEGION_DEBUG
#define LEGION_CONFIGURATION LEGION_DEBUG_VALUE
#else
/**@def LEGION_RELEASE
 * @brief Defined in release mode.
 */
#define LEGION_RELEASE 
#define LEGION_CONFIGURATION LEGION_RELEASE_VALUE
#endif

#if (!defined(LEGION_LOW_POWER) && !defined(LEGION_HIGH_PERFORMANCE))
 /**@def LEGION_HIGH_PERFORMANCE
  * @brief Automatically defined if LEGION_LOW_POWER was not defined. It makes Legion ask the hardware's full attention to run as fast as possible.
  * @note Define LEGION_LOW_POWER to run Legion with minimal resources instead.
  */
#define LEGION_HIGH_PERFORMANCE
#endif

  /**@def LEGION_PURE
   * @brief Marks a function as pure virtual.
   */
#define LEGION_PURE =0

   /**@def LEGION_IMPURE
    * @brief Marks a function as overridable but default implemented.
    */
#define LEGION_IMPURE {}

    /**@def LEGION_IMPURE_RETURN
     * @brief Marks a function as overridable but default implemented with certain default return value.
     * @param x value the function should return.
     */
#define LEGION_IMPURE_RETURN(x) { return (x); }

#if !defined(LEGION_MIN_THREADS)
#define LEGION_MIN_THREADS 5
#endif

#pragma endregion

#pragma region /////////////////////////////////// Operating system /////////////////////////////////////
#if defined(_WIN64)
 /**@def LEGION_WINDOWS
  * @brief Defined when compiling for Windows.
  */
#define LEGION_WINDOWS

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#define NOMINMAX
#include <Windows.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <objbase.h>
#elif defined(__linux__)
 /**@def LEGION_LINUX
  * @brief Defined when compiling for Linux.
  */
#define LEGION_LINUX

#include <sys/prctl.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sched.h>
#include <errno.h>
#endif
#pragma endregion

#pragma region //////////////////////////////////// Detect compiler /////////////////////////////////////
#if defined(__clang__)
  // clang
#define LEGION_CLANG

#if defined(__GNUG__) || (defined(__GNUC__) && defined(__cplusplus))
#define LEGION_CLANG_GCC
#elif defined(_MSC_VER)
#define LEGION_CLANG_MSVC
#endif

#define L_PAUSE_INSTRUCTION __builtin_ia32_pause
#elif defined(__GNUG__) || (defined(__GNUC__) && defined(__cplusplus))
  // gcc
#define LEGION_GCC
#define L_PAUSE_INSTRUCTION __builtin_ia32_pause
#elif defined(_MSC_VER)
  // msvc
#define LEGION_MSVC
#define L_PAUSE_INSTRUCTION _mm_pause
#endif
#pragma endregion

#pragma region ////////////////////////////////// Compiler specifics ////////////////////////////////////

#if defined(LEGION_CLANG) || defined(LEGION_GCC)
#define L_PAUSE_INSTRUCTION __builtin_ia32_pause
#elif defined(LEGION_MSVC)
#define L_PAUSE_INSTRUCTION _mm_pause
#else
#define L_PAUSE_INSTRUCTION
#endif

#if !defined(__FULL_FUNC__)
#if defined(LEGION_CLANG) || defined(LEGION_GCC)
#define __FULL_FUNC__ __PRETTY_FUNCTION__
#elif defined(LEGION_MSVC)
#define __FULL_FUNC__ __FUNCSIG__
#else
#define __FULL_FUNC__ __func__
#endif
#endif

#if defined(LEGION_CLANG)
#define LEGION_PRAGMA_TO_STR(x) _Pragma(#x)
#define LEGION_CLANG_SUPPRESS_WARNING_PUSH _Pragma("clang diagnostic push")
#define LEGION_CLANG_SUPPRESS_WARNING(w) LEGION_PRAGMA_TO_STR(clang diagnostic ignored w)
#define LEGION_CLANG_SUPPRESS_WARNING_POP _Pragma("clang diagnostic pop")
#define LEGION_CLANG_SUPPRESS_WARNING_WITH_PUSH(w)                                                \
    LEGION_CLANG_SUPPRESS_WARNING_PUSH LEGION_CLANG_SUPPRESS_WARNING(w)
#else
#define LEGION_CLANG_SUPPRESS_WARNING_PUSH
#define LEGION_CLANG_SUPPRESS_WARNING(w)
#define LEGION_CLANG_SUPPRESS_WARNING_POP
#define LEGION_CLANG_SUPPRESS_WARNING_WITH_PUSH(w)
#endif

#if defined(LEGION_GCC)
#define LEGION_PRAGMA_TO_STR(x) _Pragma(#x)
#define LEGION_GCC_SUPPRESS_WARNING_PUSH _Pragma("GCC diagnostic push")
#define LEGION_GCC_SUPPRESS_WARNING(w) LEGION_PRAGMA_TO_STR(GCC diagnostic ignored w)
#define LEGION_GCC_SUPPRESS_WARNING_POP _Pragma("GCC diagnostic pop")
#define LEGION_GCC_SUPPRESS_WARNING_WITH_PUSH(w)                                                  \
    LEGION_GCC_SUPPRESS_WARNING_PUSH LEGION_GCC_SUPPRESS_WARNING(w)
#else
#define LEGION_GCC_SUPPRESS_WARNING_PUSH
#define LEGION_GCC_SUPPRESS_WARNING(w)
#define LEGION_GCC_SUPPRESS_WARNING_POP
#define LEGION_GCC_SUPPRESS_WARNING_WITH_PUSH(w)
#endif

#if defined(LEGION_MSVC)
#define LEGION_MSVC_SUPPRESS_WARNING_PUSH __pragma(warning(push))
#define LEGION_MSVC_SUPPRESS_WARNING(w) __pragma(warning(disable : w))
#define LEGION_MSVC_SUPPRESS_WARNING_POP __pragma(warning(pop))
#define LEGION_MSVC_SUPPRESS_WARNING_WITH_PUSH(w)                                                 \
    LEGION_MSVC_SUPPRESS_WARNING_PUSH LEGION_MSVC_SUPPRESS_WARNING(w)
#else
#define LEGION_MSVC_SUPPRESS_WARNING_PUSH
#define LEGION_MSVC_SUPPRESS_WARNING(w)
#define LEGION_MSVC_SUPPRESS_WARNING_POP
#define LEGION_MSVC_SUPPRESS_WARNING_WITH_PUSH(w)
#endif

#if defined (LEGION_MSVC)
#define L_WARNING(desc) __pragma(message(__FILE__ "(" STRINGIFY(__LINE__) ") : warning: " #desc))
#define L_ERROR(desc) __pragma(message(__FILE__ "(" STRINGIFY(__LINE__) ") : error: " #desc))
#elif defined(LEGION_GCC) || defined(LEGION_CLANG)
#define L_WARNING(desc) _Pragma(STRINGIFY(GCC warning desc))
#define L_ERROR(desc) _Pragma(STRINGIFY(GCC error desc))
#endif


LEGION_CLANG_SUPPRESS_WARNING("-Wdocumentation-unknown-command")
LEGION_CLANG_SUPPRESS_WARNING("-Wdocumentation")
LEGION_CLANG_SUPPRESS_WARNING("-Wextra-semi-stmt")
LEGION_CLANG_SUPPRESS_WARNING("-Wextra-semi")
LEGION_CLANG_SUPPRESS_WARNING("-Wunused-function")
LEGION_CLANG_SUPPRESS_WARNING("-Wcovered-switch-default")
LEGION_CLANG_SUPPRESS_WARNING("-Wexit-time-destructors")
LEGION_CLANG_SUPPRESS_WARNING("-Wglobal-constructors")
LEGION_CLANG_SUPPRESS_WARNING("-Wgnu-anonymous-struct")
LEGION_CLANG_SUPPRESS_WARNING("-Wnested-anon-types")
LEGION_CLANG_SUPPRESS_WARNING("-Wunused-macros")
LEGION_CLANG_SUPPRESS_WARNING("-Wunused-member-function")
LEGION_CLANG_SUPPRESS_WARNING("-Wc++98-c++11-c++14-compat")
LEGION_CLANG_SUPPRESS_WARNING("-Wc++98-c++11-compat")
LEGION_CLANG_SUPPRESS_WARNING("-Wc++98-compat-pedantic")
LEGION_CLANG_SUPPRESS_WARNING("-Wc++98-compat")
LEGION_CLANG_SUPPRESS_WARNING("-Wc++11-compat")
LEGION_CLANG_SUPPRESS_WARNING("-Wc++14-compat")

LEGION_GCC_SUPPRESS_WARNING("-Wc++11-compat")
LEGION_GCC_SUPPRESS_WARNING("-Wc++14-compat")


#if defined(LEGION_GCC) || defined(LEGION_CLANG)
#define L_ALWAYS_INLINE __attribute__((always_inline))
#elif defined(LEGION_MSVC)
#define L_ALWAYS_INLINE __forceinline
#else
#define L_ALWAYS_INLINE
#endif

#if (defined(LEGION_WINDOWS) && !defined(LEGION_WINDOWS_USE_CDECL)) || defined (DOXY_INCLUDE)
    /**@def LEGION_CCONV
     * @brief the calling convention exported functions will use in the args engine
     */
#define LEGION_CCONV __fastcall
#elif defined(LEGION_MSVC)
#define LEGION_CCONV __cdecl
#else
#define LEGION_CCONV
#endif

#pragma endregion

#pragma region ////////////////////////////////// Language convention ///////////////////////////////////

/**@def LEGION_CPP17V
 * @brief the version number of c++17 as long
 */
#define LEGION_CPP17V 201703L

/**@def NO_MANGLING
 * @brief exports functions with C style names instead of C++ mangled names
 */
#define NO_MANGLING extern "C"

/**@def LEGION_FUNC
 * @brief export setting + calling convention used by the engine
 */
#define LEGION_FUNC LEGION_CCONV

/**@def LEGION_INTERFACE
 * @brief un-mangled function name +  export setting + calling convention used by the engine
 */
#define LEGION_INTERFACE NO_MANGLING LEGION_CCONV 

#if !defined(__FUNC__)
#define __FUNC__ __func__ 
#endif

#pragma endregion

#pragma region ///////////////////////////////////// Attributes /////////////////////////////////////////

#if defined(__has_cpp_attribute)|| defined(DOXY_INCLUDE) 
 /**@def L_HASCPPATTRIB
  * @brief checks if a certain attribute exists in this version of c++
  * @param x attribute you want to test for
  * @return true if attribute exists
  */
#  define L_HASCPPATTRIB(x) __has_cpp_attribute(x)
#else
#  define L_HASCPPATTRIB(x) 0
#endif

#if __cplusplus >= LEGION_CPP17V || L_HASCPPATTRIB(fallthrough) || defined(DOXY_INCLUDE)
#define L_FALLTHROUGH [[fallthrough]]
#else
#define L_FALLTHROUGH
#endif

#if __cplusplus >= LEGION_CPP17V || L_HASCPPATTRIB(maybe_unused) || defined(DOXY_INCLUDE)
 /**@def L_MAYBEUNUSED
  * @brief [[maybe_unused]]
  */
#define L_MAYBEUNUSED [[maybe_unused]]
#else
#define L_MAYBEUNUSED
#endif

#if __cplusplus >= LEGION_CPP17V || L_HASCPPATTRIB(nodiscard) || defined(DOXY_INCLUDE)
/**@def L_NODISCARD
 * @brief Marks a function as "nodiscard" meaning that result must be captured and should not be discarded.
 */
#define L_NODISCARD [[nodiscard]]
#else
#define L_NODISCARD
#endif

#if __cplusplus > LEGION_CPP17V || L_HASCPPATTRIB(noreturn) || defined(DOXY_INCLUDE)
 /**@def L_NORETURN
  * @brief Marks a function as "noreturn" meaning that the function will never finish, or terminate the application
  */
#define L_NORETURN [[noreturn]]
#else
#define L_NORETURN
#endif
#pragma endregion
