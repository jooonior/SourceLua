# SourceLua

Source SDK 2013 plugin that embeds Lua into the engine.

Documentation is written for TF2 specifically, but the plugin should work in
any game compatible with SDK APIs.

## Setup

1. Download the [latest release](https://github.com/jooonior/SourceLua/releases/latest/download/SourceLua.zip) and extract it into your `tf/custom` directory.
2. Add `lua_file init.lua` to your `autoexec.cfg`.

## Commands

### `lua`

Usage: `lua "chunk"` or `lua ...`

Runs `chunk` as Lua code and prints the returned value. When given more than
one argument, executes the entire command line as single chunk.

Following example illustrates how quotes are handled:

```
] lua "return 'hello world' -- comment"  // ~> return 'hello world' -- comment
hello world
] lua "return 'hello world'" -- comment  // ~> "return 'hello world'" -- comment
<LUA PARSE ERROR>
] lua return "hello world" -- comment    // ~> return "hello world" -- comment
hello world
] lua return 'here is a semicolon -> ;'  // ~> return 'here is a semicolon -> 
<LUA PARSE ERROR>
Unknown command "'"
] lua return "here is a semicolon -> ;"  // ~> return "here is a semicolon -> ;"
here is a semicolon -> ;
```

### `lua_file`

Usage: `lua_file <file path> [args]...`

Loads and runs a Lua file. Unless absolute, `file path` is resolved relative to
engine search paths.

Optional `args` are forwarded to the script and can be accessed from Lua through
the `arg` global table (`arg[0]` is the resolved full path, `arg[1]` first
argument, `arg[n]` _n_-th argument).

### `lua_restart`

Restarts the Lua execution environment.

### `lua_version`

Prints plugin version and information about Lua binaries.

## Lua

This plugin uses [LuaJIT](https://luajit.org/) to run Lua code. All standard
libraries and extensions that LuaJIT offers are available. 

### New/Changed Functions

#### `print(...)`

Prints to engine console as well as to standard output.

#### `warn(...)`

Equivalent to `print(...)` but the engine console output is colored red.

#### `debug(...)`

Equivalent to `print(...)` but prints only when developer mode is enabled
(with `developer 1`).

### Utility Scripts

#### `init.lua`

Sets up the Lua environment to better integrate with the Source engine. Namely
adds engine search paths to `package.path` and `package.cpath`, so that `require`
searches directories the engine would.

```
] lua require 'X'
module 'X' not found:
  no file 'tf/custom/A/lua/X.lua'
  no file 'tf/custom/A/lua/X/init.lua'
  no file 'tf/custom/B/lua/X.lua'
  no file 'tf/custom/B/lua/X/init.lua'
  no file 'tf/lua/X.lua'
  no file 'tf/lua/X/init.lua'
```

### Source Engine APIs

The plugin does not provide bindings to SDK functions, those are kept in
separate libraries. Even without those, the LuaJIT FFI module enables you to
call external C functions and can be used to consume engine interfaces (even
non-public ones) directly from Lua. Included `core` Lua module aims to make
this process a bit easier.

#### Interfaces and Virtual Functions

The `core.hooks` module provides functions to query engine interfaces and call
their methods, given you know what you're looking for.

```lua
local hooks = require 'core.hooks'
-- As long as you know the interface name and where it comes from...
local engine = hooks.create_interface('engine.dll', 'VEngineClient014')
-- and also the vtable index and signature of the function you want to call...
engine:__bind(102, 'void ExecuteClientCmd(const char *cmd)')
-- then it's simple!
engine:ExecuteClientCmd('echo hello world')
```

#### SDK Bindings

The `sdk` module contains bindings for Source SDK 2013. Their downside is that
they rely on compiled binaries and require an exact ABI match against the SDK
(and can't be easily monkeypatched for newer APIs).

```lua
local sdk = require 'sdk'
local sv_cheats = sdk.cvar:FindVar('sv_cheats')
sv_cheats:SetIntValue(1)
```

Only a small subset of what is offered by the SDK is currently implemented.

## Good Luck!

- Get familiar with the [LuaJIT FFI library](https://luajit.org/ext_ffi.html),
  you will need it.

- There's very little type safety! Casting is often required and mistakes lead
  to segfaults.
