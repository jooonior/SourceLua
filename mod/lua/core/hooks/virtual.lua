-- FFI definitions for calling virtual member functions of objects.

local ffi = require 'ffi'

ffi.cdef [[
  struct vfunc;
  typedef struct { struct vfunc **__vt; } virtual;
]]

ffi.metatype('struct vfunc', {
  __index = {
    -- Cast self to given signature. Either `:as("result", "params")` or `:as "result(params)"`.
    -- Automatically corrects calling convention and inserts `this *` as first parameter.
    as = function(self, result, params)
      if params == nil then
        -- Parse 'result' as full function type.
        result, params = string.match(result, '([^(]+)(%b())')
      end

      -- Insert 'this *' as first parameter.
      if params:match('%(%s*%)') or params:match('%(%s*void%s*%)') then
        params = '(virtual *)'
      else
        params = ('(virtual *, %s'):format(params:sub(2))
      end

      -- Cast to member function pointer.
      local cast = ('%s (__thiscall *)%s'):format(result, params)
      return ffi.cast(cast, self)
    end
  }
})

-- Holds bound methods for all `cdata<virtual *>` instances (`cdata` does not allow adding keys).
local virtual = setmetatable({}, {
  __index = function(self, k)
    local v = {}
    self[k] = v
    return v
  end,
  -- Don't prevent `cdata<virtual *>` instances from being garbage collected.
  __mode = 'k',
})

-- Binds virtual function at index `i` with given `signature`.
local function bind(self, i, signature)
  -- Parse function signature.
  local result, name, params = string.match(signature, '%s*([^%(]-)%s*([%a_][%w_]*)%s*(%b())')
  virtual[self][name] = self.__vt[i]:as(result, params)
end

ffi.metatype('virtual', {
  __index = function(self, k)
    return k == '__bind' and bind or virtual[self][k]
  end,
})

-- So that you can access the table if you want to.
return virtual
