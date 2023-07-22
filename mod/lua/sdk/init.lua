local hooks = require 'core.hooks'

local M = {}

local function loadlib(name)
  for k, v in pairs(require(name)) do
    M[k] = v
  end
end

local function create_interface(module, interface, cast)
  local interface_ptr, err = hooks.create_interface(module, interface, cast)

  if interface_ptr ~= nil then
    debug(("[Lua] Connected interface %s"):format(interface))
  else
    warn(("[Lua] Failed to connect interface %s (%s)"):format(interface, err))
  end

  return interface_ptr
end

loadlib('sdk.cvar')
M.cvar = create_interface('vstdlib.dll', M.CVAR_INTERFACE_VERSION, 'ICvar')

return M
