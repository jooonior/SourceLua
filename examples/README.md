# Examples of Lua Plugins

Each plugin is in its own directory. To try one out, copy it into your
`tf/custom` folder.

## Plugin Structure

```
tf/custom/
├── simple_plugin/
│   └── lua/
│       └── simple_plugin.lua  -- loaded by `require 'simple_plugin'`
└── complex_plugin/
    └── lua/
        └── complex_plugin/
            ├── init.lua       -- loaded by `require 'complex_plugin'`
            └── utils.lua
```
