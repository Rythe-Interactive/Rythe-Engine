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

Everything needs to go into the namespace `rythe` and should be in it's own general namespace within that. Also use that as an indication where to put your file.

i.e.: The file /core/math/vector.hpp uses
`namespace rythe::core::math { ... }`

Use the new c++17 convention for nested namespaces, namespace names may only have snake_case letters.

### Class/Struct Names

Types that provide a lot of functionality should be classes should use stl-like snake_case naming.<br>
(examples: window, lzma2_adaptor, vk_window_helper, sparse_set, default_index_list)

POD types that are completely compatible with the stl should all be structs, lower-case and should stay that way.<br>
(examples: float4x4, float2, angle\<float\>, entity_id)

### Class/Struct Members
`private` variables should start with `m_` and continued in camelCase.<br>
`public` variables should not have a prefix and use camelCase, the same applies to local variables.

Methods of should use stl-like snake_case naming.

### Comments
There is no comments enforcement.


### Templates
When doing template-meta-programming make sure to use `constexpr` and `using` correctly.

### A Typical File
```cpp
#pragma once // we use pragma it is supported pretty much everywhere
#include <core/math/vector.hpp> // always include from the root, makes the file more readable
#include <application/application.hpp> // even if it is your own directory

// in cpp files you may use using namespace if you so desire
using namespace rythe::core;

namespace rythe
{
  application::application() : m_otherVector { 1.0f, 2.0f } // use this constructor syntax (it's faster)
  {
    math::float3 someVector = { 1.f, 2.f, 3.f };
  }
 ...
}
```
