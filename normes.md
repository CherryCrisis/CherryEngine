# **Normes**

## Coding norme


Every resources, comments, and sources must be write in english.

*C++*
---

Headers must end with .hpp extension

```c++
#include "my_header.hpp"
```

* Macros must be written in uppercase SNAKE_CASE and must start with *CC_* to mimic namespaces.

* Namespaces must start with *CC*.

* Functions, structures, classes, interfaces and namespaces must be written in PascalCase.

* Variables must be written in lowercase snake_case.

* Member variables (private, protected and public) must start with *m_* except for primitive types (like Vectors) and for variables which will be used by the user (ex: Texture.width)

* Const ref must be used when the input parameter is bigger than a ref.

* Forward declarations must be used when possible.

* Variables, return types, fonction names, and other keywords must be aligned when possible.

* Private, protected and public flags must be ordered in this way except if the code does not allow it.

* Member variables, constructors, destructors and member functions, static member variables must be ordred in this way except when the code does not allow it.

* Event and Properties must start with uppercase letter.

```c++
#define CC_MACRO

namespace CCEngine
{
    struct Model;    

    struct Texture
    {
    protected:
        std::string m_filePath;

    public:
        float       m_width = 0.f;
        float       m_height = 0.f;
        Property    Filepath;
    }

    class ResourcesManager
    {
    private:
        std::vector<Texture> m_textures;
        Event<...> m_OnReload;

    public:
        ResourcesManager();
        ~ResourcesManager();

        Texture LoadTexture(const std::string& path);
        void AddModel(Model* modelToAdd);

        static int resourcesCount;
    }

    float GetDeltaTime();
}
```

* Includes must be ordered in this way:
    - Precompiled headers first
    - Main header of the current file
    - std headers
    - External libraries headers
    - Singleton headers
    - Other headers

    Each of these categories must be spaced with the others.

* Constructors must use chained constructors when possible.

* Namespaces must encapsulate all the implementations.

* using namespace is forbidden except for Maths.

* Long mathematical operations must be aligned and written on the line. Operators should not be at the beginning of the line, prefer to put them at the end.

```c++
#include "pch.h"

#include "my_header.hpp"

#include <vector>

#include <imgui.h>

#include "debug_manager.hpp"

#include "texture.hpp"

namespace CCEngine
{
    GLTexture::GLTexture(...)
        : Texture(...)
    {
        ...
    }

    void MyFunction()
    {
        float longOperation = HyperLongNameForAFunctionToCalculateSomething(...) +
                              HyperLongNameForAFunctionToCalculateSomething(...) /
                              HyperLongNameForAFunctionToCalculateSomething(...);
    }
}
```

* Interfaces must be prefixed with *I*
* Abstract classes and structures must be prefixed with *A*
* Enum types and enum classes must be prefixed with *E*
* Booleans must start with *has/have* or *is/are*

```c++
struct IDamageableInterface
{
    virtual OnDamage() = 0;
}

class AShape
{
    protected:
        float m_volume = 0.f;

    public:
        float GetVolume() const = 0;
};

enum class EType
{
    TYPE_1 = 0,
    TYPE_2 = 1,
    TYPE_3 = 2,
};
```

*Mathematics norme*
---
* Mathematical primitives must implement static functions and member functions when possible. 

* Functions must be inlined except for constexpr and templated functions.

* Member functions must be flagged as const when possible.

* For operators and other mathematical formulas, function parameter naming conventions should follow this rule:
    - When there is less than three parameters and when the primitives are of the same type, the left operand must be named *lhs* and the right operand *rhs*.

```c++
struct Vector2
{
    ...

    inline float Dot(const Vector2& rhs);
    inline static float Dot(const Vector2& lhs, const Vector2& rhs);
}
```

---
## Git
Branche must be name with their feature and must be writte in snake_case except for the acronyms (like UI, GUI...)

```
[resources_manager] ...
[renderer] ...
[UI] ...
```

New features and fixes must be separeted with semicolons

```
[branch] Added feature1, feature2; Fixed featureX;
```

Push to master every Friday at 2:30 p.m. with a working build (which compiles)

---
## Folder tree

Cherry Chrisis files :
- headers (.hpp) in "include"
- source files (.cpp) in "src"
- interfaces (.i for SWIG) in "interface"

External files :
- external headers (.hpp, .h) in "external/include"
- external implementations  (.cpp) in "external/src"
- external static libraries (.lib) in "lib"
- external DLLs (.dll) in "dll"