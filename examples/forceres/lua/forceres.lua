--[[
 Lua implementation of the SourceRes plugin.
 Registers console command `forceres` that allows resizing the engine window
 to arbitrary resolutions. Load with `lua require 'forceres'`.
--]]

local hooks = require 'core.hooks'
local sdk = require 'sdk'

local ffi = require 'ffi'

-- Declare required data structures.
hooks.declare [[
  typedef struct {
    int width;
    int height;
    int bpp;
    int refreshRate;
  } vmode_s;
]]

-- Query required engine interface.
local engine = assert(hooks.create_interface('engine.dll', 'VEngineClient014'))
-- Bind required virtual functions.
engine:__bind(89, 'void GetVideoModes(int &nCount, vmode_s *&pModes)')
engine:__bind(102, 'void ExecuteClientCmd(const char *szCmdString)')

local function find_video_mode(count, modes, width, height)
  for i = 0, count - 1 do
    local mode = modes[i]
    if mode.width == width and mode.height == height then
      return mode
    end
  end
  return nil
end

local function add_video_mode(count, modes)
  if count >= 512 then
    return nil, "Can't register any more video modes"
  end

  -- This is the address where engine stores the number of video modes.
  local internal_count = ffi.cast('int *', modes) - 1
  if internal_count[0] ~= count then
    return nil, "Can't find internal video mode counter"
  end

  internal_count[0] = count + 1
  return modes[count]
end

local function forceres_callback(args)
  args = ffi.cast('CCommand &', args)

  -- Check argument count.
  if args:ArgC() ~= 3 then
    warn "Usage: forceres <width> <height>"
    return
  end

  -- Parse arguments.
  local width = tonumber(ffi.string(args:Arg(1)))
  local height = tonumber(ffi.string(args:Arg(2)))

  -- Allocate buffers.
  local count = ffi.new 'int[1]'
  local modes = ffi.new 'vmode_s *[1]'

  engine:GetVideoModes(count, modes)

  -- Get values from buffers.
  count = count[0]
  modes = modes[0]

  -- Try to found mode with matching resolution.
  if find_video_mode(count, modes, width, height) == nil then
    -- If no mode matches, add new one.
    local mode, err = add_video_mode(count, modes)
    -- Can't use `error` or `assert` because this function is called from external code.
    if mode == nil then
      warn(err)
      return
    end

    -- Set desired with and height, and sensible defaults for other properies.
    mode.width = width
    mode.height = height
    mode.bpp = modes[0].bpp
    mode.refreshRate = modes[0].refreshRate

    print(('Registered new video mode with resolution (%i, %i)'):format(width, height))
  end

  -- Invoke command to change resolution. 
  engine:ExecuteClientCmd(('mat_setvideomode %i %i 1'):format(width, height))
end

-- Create the command object.
local forceres = sdk.ConCommand('forceres', forceres_callback, 'Force windowed resolution', 0, nil)

-- Associate a finalizer to unregister the command before it gets garbage collected.
ffi.gc(forceres, function(self)
  -- Cast to base class is needed because FFI typechecking does not account for inheritance.
  local as_base = ffi.cast('ConCommandBase *', self)
  sdk.cvar:UnregisterConCommand(as_base)
end)

-- Register the command.
sdk.cvar:RegisterConCommand(ffi.cast('ConCommandBase *', forceres))

-- Return the command to ensure it won't get garbage collected, since values
-- returned from required modules are stored in the `package.loaded` table.
return forceres
