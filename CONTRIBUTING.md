## CODE style

### File-Names
- A file name cannot have any uppercase letters
- Headers must have the ending .hpp or .h respectively (use hpp for c++ headers)
- Source must have the ending .cpp or .c respectively


### Namespacing

everything needs to go into the namespace `args` and should be in it's own general namespace within that. also use that as an indication where to put your file.

i.e.: The file /math/vector.hpp uses
`namespace args::math { ... }`

Use the new c++17 convention for nested namespaces, namespace names may only have lower-case letters

### Class/Struct Names

types that provide a lot of functionality unlike "stl-likes" should be classes and start with an uppercase letter and continue in PascalCase<br>
(examples: Window, LZMA2Adaptor, VK_WindowHelper)

POD types that are completely compatible with the stl should all be structs, lower-case and should stay that way<br>
(examples: mat4f, vec2f, angle<float\>)

"stl-likes" and primitive typedefs should use stl-like snake_case naming<br>
(examples: entity_id, sparse_set, default_index_list)

### Class/Struct Members
`private` variables should start with `m_` and continued in PascalCase.<br>
`public` variables should not have a prefix and use camelCase.

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

  Application::Application() : m_other_vector { 1.0f, 2.0f } // use this constructor syntax (it's faster)
  {
    math::vec3f some_vector = { 1.f, 2.f, 3.f };
  }
 ...
}
```
