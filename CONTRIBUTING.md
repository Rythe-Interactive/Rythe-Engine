## REPO managment
### Git-Branch methodology
This repository employs a git flow based workflow, to incorporate your changes the following conditions must be true:
- your change lives on either feature/your_change_name or bugfix/your_change_name.
- you must create a pull request.
- if your branch is a bugfix it must mention an issue.
- it must adhere to the code-style mentioned below.

Furthermore the following rules are to be met when attempting to merge:
- to merge into a develop branch, you require the approval of one peer reviewer of that team.
  - you can only merge from develop_<your_team> into develop
- to merge into main you require the approval of a code-owner and the ci must pass (if applicable).
  - you can only merge from develop into main via a release branch
- to merge into release you require the approval of a code-owner and the ci must pass on strict latest (if applicable) and your commit must be signed!.
  - you can only branch develop into a release branch, a release branch must be merged into main and develop directly
- hotfix branches are only allowed in special cases and must be communicated with a code owner.

## CODE style

### File-Names
- A file name cannot have any uppercase letters
- Headers must have the ending .hpp or .h respectively (use hpp for c++ headers)
- Source must have the ending .cpp or .c respectively


### Namespacing

everything needs to go into the namespace `args` and should be in it's own general namespace within that. also use that as an indication where to put your file.

i.e.: The file /core/math/vector.hpp uses
`namespace args::core::math { ... }`

Use the new c++17 convention for nested namespaces, namespace names may only have snake_case letters

### Class/Struct Names

types that provide a lot of functionality unlike "stl-likes" should be classes and start with an uppercase letter and continue in PascalCase<br>
(examples: Window, LZMA2Adaptor, VK_WindowHelper)

POD types that are completely compatible with the stl should all be structs, lower-case and should stay that way<br>
(examples: mat4f, vec2f, angle<float\>)

"stl-likes" and primitive typedefs should use stl-like snake_case naming<br>
(examples: entity_id, sparse_set, default_index_list)

### Class/Struct Members
`private` variables should start with `m_` and continued in PascalCase.<br>
`public` variables should not have a prefix and use camelCase, the same applies to local variables.

methods of non "stl-likes" should use camelCase naming and "stl-likes" should use stl-like snake_case naming

### Comments
In feature branches there is no comments enforcement, however in order to get a pull request accepted to any of the main branches doxygen style comments are mandatory.

### Templates
when doing template-meta-programming make sure to use `constexpr` and `using` correctly.

### A Typical File
```c++
#pragma once // we use pragma it is supported pretty much everywhere
#include <core/math/vector.hpp> //alway include from the root,makes the file more readable
#include <application/application.hpp> //even if it is your own directory

using namespace args::core;

namespace args::application // in cpp files you may use using namespace if you so desire
{

  Application::Application() : m_OtherVector { 1.0f, 2.0f } // use this constructor syntax (it's faster)
  {
    math::vec3f someVector = { 1.f, 2.f, 3.f };
  }
 ...
}
```
