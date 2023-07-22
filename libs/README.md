# Source SDK wrapper libraries for LuaJIT FFI

## Building

```
cmake . -B build -G "Visual Studio 12 2013" -D SOURCE_SDK_2013=.../source-sdk-2013/mp/src
cmake --build build --config Release
```

## Lua Binding

See `mod/lua/sdk/cvar.lua` for how to bind to compiled libraries.
