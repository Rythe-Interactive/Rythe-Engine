## REPO managment
### Git-Branch methodology
This repository employs a git flow based workflow, to incorporate your changes the following conditions must be true:
- Your change lives on either feature/your_change_name or bugfix/your_change_name.
- You must create a pull request.
- If your branch is a bugfix it must mention an issue.
- It must adhere to the code-style mentioned below.

Furthermore the following rules are to be met when attempting to merge:
- To merge into a develop branch, you require the approval of one peer reviewer of that team.
  - You can only merge from develop_<your_team> into develop.
- To merge into main you require the approval of a code-owner and the ci must pass (if applicable).
  - You can only merge from develop into main via a release branch.
- To merge into release you require the approval of a code-owner and the ci must pass on strict latest (if applicable) and your commit must be signed!.
  - You can only branch develop into a release branch, a release branch must be merged into main and develop directly.
- Hotfix branches are only allowed in special cases and must be communicated with a code owner.

## CODE style

### File-Names
- A file name cannot have any uppercase letters
- Headers must have the ending .hpp or .h respectively. (use hpp for c++ headers)
- Source must have the ending .cpp or .c respectively.


### Namespacing

Everything needs to go into the namespace `legion` and should be in it's own general namespace within that. Also use that as an indication where to put your file.

i.e.: The file /core/math/vector.hpp uses
`namespace legion::core::math { ... }`

Use the new c++17 convention for nested namespaces, namespace names may only have snake_case letters.

### Class/Struct Names

Types that provide a lot of functionality unlike "stl-likes" should be classes and start with an uppercase letter and continue in PascalCase.<br>
(examples: Window, LZMA2Adaptor, VK_WindowHelper)

POD types that are completely compatible with the stl should all be structs, lower-case and should stay that way.<br>
(examples: mat4f, vec2f, angle\<float\>)

"stl-likes" and primitive typedefs should use stl-like snake_case naming.<br>
(examples: entity_id, sparse_set, default_index_list)

### Class/Struct Members
`private` variables should start with `m_` and continued in camelCase.<br>
`public` variables should not have a prefix and use camelCase, the same applies to local variables.

Methods of non "stl-likes" should use camelCase naming and methods of "stl-likes" should use stl-like snake_case naming.

### Comments
In feature branches there is no comments enforcement, however in order to get a pull request accepted to any of the main branches doxygen style comments are mandatory. Check The Chapter "On the Topic of Doxygen" at the end of this document.


### Templates
When doing template-meta-programming make sure to use `constexpr` and `using` correctly.

### A Typical File
```cpp
#pragma once // we use pragma it is supported pretty much everywhere
#include <core/math/vector.hpp> //alway include from the root,makes the file more readable
#include <application/application.hpp> //even if it is your own directory

using namespace legion::core;

namespace legion::application // in cpp files you may use using namespace if you so desire
{

  Application::Application() : m_OtherVector { 1.0f, 2.0f } // use this constructor syntax (it's faster)
  {
    math::vec3f someVector = { 1.f, 2.f, 3.f };
  }
 ...
}
```

### On the Topic of Doxygen
To ensure that your doxygen comments are in line with the rest of the code base we have created some basic rules you should follow:

- We exclusively use `@` and `/**` to document our files. for example:
  ```cpp

  /** @brief Example for a function.
   *  @tparam T A quick example of what the template param does.
   *          (not when it can be inferred by a param)
   *  @param bar_parameter A quick description of what the param does.
   *         Additionally you might use [in] [out] [in/out] for
   *         clarity.
   *  @return int A quick explanation of what to expect from
   *          the function.
   *  @note Some additional noteworthy documentation details.
   *  @throw std::exception If your function is at risk of
   *         throwing an exception.
   */
  template <class T>
  int foo_function(int bar_parameter);
  ```

- To keep the file small we encourage you to drop any commands, when the function is small enough to be explained by `@brief`
alone. for example:
  ```cpp
  /** @brief Gets the size of the container.
   *  @return size_type The size of the container.
   */
  L_NODISCARD auto size() const noexcept
  {
    return m_container.size();
  }
  ```
  should really be
  ```cpp
  /** @brief Gets the size of the container.
   */
  L_NODISCARD auto size() const noexcept
  {
    return m_container.size();
  }
  ```
  To decide if a type is simple enough to drop info about extra commands, thing about how much text you would repeat & how complicated the types are you are taking or returning.

- Try to adhere to proper English. While this is not a super critical condition. we do appreciate good punctuation and spelling (we also make these mistakes at times). A good spell-check plugin for your favorite editor helps tremendously.

- Default Constructors & Destructors should not be documented, unless it does something very unexpected. In which case you should ask yourself if a constructor is the right place for your custom behavior