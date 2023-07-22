require 'core.hooks.virtual'  -- declare FFI types

local ffi = require 'ffi'

local M = {}

-- Wraps `ffi.cdef` with better error messages.
function M.declare(cdef)
  local ok, err = pcall(ffi.cdef, cdef);
  if not ok then
    error(('%s\n%s'):format(err, cdef))
  end
end

-- Quickly declare forward declarations for multiple types.
function M.forward(names)
  for _, name in ipairs(names) do
    M.declare(('typedef struct %s %s;'):format(name, name))
  end
end

ffi.cdef [[
  // Windows API
  void* GetModuleHandleA(const char *);
  void* GetProcAddress(void *, const char *);
]]

local CreateInterfaceFn = ffi.typeof 'void* (*)(const char *pName, int *pReturnCode)'

-- Create Source SDK interface. `cast` defaults to `'virtual'` (see `core.hooks.virtual`).
function M.create_interface(module, interface, cast)
  local module_handle = ffi.C.GetModuleHandleA(module)
  if module_handle == nil then
    return nil, ("Module '%s' not found"):format(module)
  end

  local interface_factory = ffi.cast(
    CreateInterfaceFn,
    ffi.C.GetProcAddress(module_handle, 'CreateInterface')
  )
  if interface_factory == nil then
    return nil, ("Module '%s' does not expose 'CreateInterface' API"):format(module)
  end

  local interface_handle = interface_factory(interface, nil)
  if interface_handle == nil then
    return nil, ("Interface '%s' not found in module '%s'"):format(interface, module)
  end

  return ffi.cast((cast or 'virtual') .. '*', interface_handle)
end

return M
