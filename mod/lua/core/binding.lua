local hooks = require 'core.hooks'

local ffi = require 'ffi'

local function assertf(v, fmt, ...)
  return v or error(string.format(fmt, ...))
end

hooks.declare "const char *INTERFACE_VERSION;"

-- Helper for loading SDK wrapper libraries.
local Binding = { ns = ffi.C }

-- Open binding library.
function Binding:open(path)
  self.__index = self
  local ns = ffi.load(path)
  return setmetatable({
    ns = ns,
    INTERFACE_VERSION = ffi.string(ns.INTERFACE_VERSION),
  }, self)
end

-- Bind method `name` of class `this`.
local function method(this, result, name, params)
  -- Remove 'virtual' or 'static' prefix.
  local virtual, static
  result, virtual = result:gsub('^virtual%s+', '')
  result, static = result:gsub('^static%s+', '')

  -- Add implicit 'this' parameter.
  if static == 1 then
    -- Not for static members.
  elseif params:match('%(%s*%)') or params:match('%(%s*void%s*%)') then
    params = ('(%s *)'):format(this)
  else
    params = ('(%s *, %s'):format(this, params:sub(2))
  end

  local cname = ('%s__%s'):format(this, name)

  -- Define C signature.
  local cdef = ('%s (*%s)%s;'):format(result, cname, params)
  hooks.declare(cdef)

  return cname
end

-- Bind methods of class `this`.
function Binding:methods(this, cppdef)
  local methods = {}
  local ctor = false

  -- Remove comments.
  cppdef = cppdef:gsub('//[^\n]*', '')
  cppdef = cppdef:gsub('/%*.-%*/', '')

  -- For each member function declaration...
  for result, name, params in string.gmatch(cppdef, '%s*([^%(]-)%s*([%a_][%w_]*)%s*(%b())[^;]*;') do
    -- Catch redeclaration (probably C++ overloaded method).
    assertf(methods[name] == nil, "%s::%s : Method redeclaration", this, name)

    -- Remove default arguments.
    params = params:gsub('=[^,)]*', '')

    if name == this then
      -- It's constructor.
      assertf(ctor == false, "%s::%s : Constructor redeclaration", this, this)
      hooks.declare(('%s *(*%s__new)%s;'):format(this, this, params))
      -- Also declare destructor.
      hooks.declare(('void (*%s__delete)(%s *);'):format(this, this))
      -- Constructor and destructors are internal.
      ctor = true
    else
      local cname = method(this, result, name, params)
      methods[name] = self.ns[cname]
    end
  end

  return methods, ctor
end

-- Bind class `name`, that inherits from `base` and has methods declared in `cppdef`.
function Binding:class(name, base, cppdef)
  -- Declare type.
  ffi.cdef(string.format('typedef struct %s %s;', name, name))
  -- Register methods.
  local methods, ctor = self:methods(name, cppdef)

  -- Class metatable.
  local mt = {}

  local constructor, destructor

  if ctor then
    constructor = self.ns[name .. '__new']
    destructor = self.ns[name .. '__delete']

    -- Set finalizer.
    mt.__gc = destructor
  end

  if not base then
    -- No inheritance, lookup is simple.
    mt.__index = methods
  else
    -- Either find own method or delegate the call to base class.
    local base_type = ffi.typeof(base .. '*')
    function mt.__index(_, key)
      return methods[key] or function(_self, ...)
        -- FFI does not know about inheritance, need to cast.
        local as_base = ffi.cast(base_type, _self)
        return as_base[key](as_base, ...)
      end
    end
  end

  -- Set metatable.
  ffi.metatype(name, mt)

  -- Return wrapper around constructor (if any).
  return (ctor or nil) and function(...)
    return ffi.gc(constructor(...), destructor)
  end
end

return Binding
