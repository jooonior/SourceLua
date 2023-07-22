assert(arg[0], "Can't determine root directory")

local ffi = require 'ffi'

-- Set our path as `package.path`.

local root = string.gsub(arg[0], '[^/\\]*$', '')
package.path = ('%slua/?.lua;%slua/?/init.lua'):format(root, root)

-- Now we can `require` our modules.

local hooks = require 'core.hooks'

-- Query engine filesystem interface and required methods.

local FILESYSTEM_INTERFACE_VERSION = 'VFileSystem022'
local fs = assert(hooks.create_interface('FileSystem_Stdio.dll', FILESYSTEM_INTERFACE_VERSION))
fs:__bind(13, 'int GetSearchPath(const char *pathID, bool bGetPackFiles, char *pDest, int maxLenInChars)')

-- Wrapper around FFI for `IFileSystem::GetSearchPath`.
local function get_search_path()
  local length = fs:GetSearchPath(nil, false, nil, 0)
  local buffer = ffi.new('char[?]', length)
  fs:GetSearchPath(nil, false, buffer, length)
  return ffi.string(buffer)
end

-- Add every mounted directory to `package.path` and `package.cpath`.

local paths, cpaths = {}, {}

for mount in get_search_path():gmatch('[^;]+') do
  if not mount:match('.vpk$') then
    table.insert(paths, mount .. 'lua/?.lua')
    table.insert(paths, mount .. 'lua/?/init.lua')
    table.insert(cpaths, mount .. 'lua/?.dll')
  end
end

package.path = table.concat(paths, ';')
package.cpath = table.concat(cpaths, ';')
