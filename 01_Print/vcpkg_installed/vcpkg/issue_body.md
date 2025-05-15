Package: atl:x64-windows@0

**Host Environment**

- Host: x64-windows
- Compiler: MSVC 19.16.27054.0
-    vcpkg-tool version: 2024-01-11-710a3116bbd615864eef5f9010af178034cb9b44
    vcpkg-scripts version: 53bef8994 2024-01-12 (1 year, 3 months ago)

**To Reproduce**

`vcpkg install `
**Failure logs**

```
CMake Error at C:/Users/IOKLO/AppData/Local/vcpkg/registries/git-trees/0cf33fe03c09b26209c1162fee2bb2c5538f8b0f/portfile.cmake:7 (message):
  Unable to locate 'atlbase.h'.  Ensure you have installed the Active
  Template Library (ATL) component of Visual Studio.
Call Stack (most recent call first):
  scripts/ports.cmake:170 (include)



```
**Additional context**

<details><summary>vcpkg.json</summary>

```
{
  "dependencies": [
    "llvm"
  ]
}

```
</details>
