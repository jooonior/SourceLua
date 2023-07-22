local binding = require 'core.binding'
local hooks = require 'core.hooks'

local clib = binding:open(assert(package.searchpath('sdk/cvar', package.cpath)))

local M = {}

M.CVAR_INTERFACE_VERSION = clib.INTERFACE_VERSION

hooks.forward {
  'ICvar',
  'ConCommandBase',
  'CCommand',
  'ConCommand',
  'ConVar',

  'IConVar',
  'ICvarQuery',
  'IConsoleDisplayFunc',
  'characterset_t',
}

hooks.declare [[
  typedef int CVarDLLIdentifier_t;
  typedef void (*FnChangeCallback_t)(IConVar *var, const char *pOldValue, float flOldValue);
]]

-- public/icvar.h
M.ICvar = clib:class('ICvar', nil, [[
  // Allocate a unique DLL identifier
  virtual CVarDLLIdentifier_t AllocateDLLIdentifier() = 0;

  // Register, unregister commands
  virtual void            RegisterConCommand( ConCommandBase *pCommandBase ) = 0;
  virtual void            UnregisterConCommand( ConCommandBase *pCommandBase ) = 0;
  virtual void            UnregisterConCommands( CVarDLLIdentifier_t id ) = 0;

  // If there is a +<varname> <value> on the command line, this returns the value.
  // Otherwise, it returns NULL.
  virtual const char*     GetCommandLineValue( const char *pVariableName ) = 0;

  // Try to find the cvar pointer by name
  virtual ConCommandBase *FindCommandBase( const char *name ) = 0;
  virtual ConVar         *FindVar ( const char *var_name ) = 0;
  virtual ConCommand     *FindCommand( const char *name ) = 0;

  // Get first ConCommandBase to allow iteration
  virtual ConCommandBase  *GetCommands( void ) = 0;

  // Install a global change callback (to be called when any convar changes)
  virtual void            InstallGlobalChangeCallback( FnChangeCallback_t callback ) = 0;
  virtual void            RemoveGlobalChangeCallback( FnChangeCallback_t callback ) = 0;
  virtual void            CallGlobalChangeCallbacks( ConVar *var, const char *pOldString, float flOldValue ) = 0;

  // Install a console printer
  virtual void            InstallConsoleDisplayFunc( IConsoleDisplayFunc* pDisplayFunc ) = 0;
  virtual void            RemoveConsoleDisplayFunc( IConsoleDisplayFunc* pDisplayFunc ) = 0;
  // virtual void            ConsoleColorPrintf( const Color& clr, PRINTF_FORMAT_STRING const char *pFormat, ... ) const FMTFUNCTION( 3, 4 ) = 0;
  // virtual void            ConsolePrintf( PRINTF_FORMAT_STRING const char *pFormat, ... ) const FMTFUNCTION( 2, 3 ) = 0;
  // virtual void            ConsoleDPrintf( PRINTF_FORMAT_STRING const char *pFormat, ... ) const FMTFUNCTION( 2, 3 ) = 0;

  // Reverts cvars which contain a specific flag
  virtual void            RevertFlaggedConVars( int nFlag ) = 0;

  // Method allowing the engine ICvarQuery interface to take over
  // A little hacky, owing to the fact the engine is loaded
  // well after ICVar, so we can't use the standard connect pattern
  virtual void            InstallCVarQuery( ICvarQuery *pQuery ) = 0;

  virtual bool            IsMaterialThreadSetAllowed( ) const = 0;
  virtual void            QueueMaterialThreadSetValue( ConVar *pConVar, const char *pValue ) = 0;
  virtual void            QueueMaterialThreadSetIntValue( ConVar *pConVar, int nValue ) = 0;
  virtual void            QueueMaterialThreadSetFloatValue( ConVar *pConVar, float flValue ) = 0;
  virtual bool            HasQueuedMaterialThreadConVarSets() const = 0;
  virtual int             ProcessQueuedMaterialThreadConVarSets() = 0;
]])

-- public/tier1/convar.h
M.ConCommandBase = clib:class('ConCommandBase', nil, [[
  ConCommandBase( const char *pName, const char *pHelpString = 0, int flags = 0 );

  virtual  bool               IsCommand( void ) const;

  // Check flag
  virtual bool                IsFlagSet( int flag ) const;
  // Set flag
  virtual void                AddFlags( int flags );

  // Return name of cvar
  virtual const char          *GetName( void ) const;

  // Return help text for cvar
  virtual const char          *GetHelpText( void ) const;

  // Deal with next pointer
  // const ConCommandBase     *GetNext( void ) const;
  ConCommandBase              *GetNext( void );

  virtual bool                IsRegistered( void ) const;

  // Returns the DLL identifier
  virtual CVarDLLIdentifier_t GetDLLIdentifier() const;
]])

-- public/tier1/convar.h
M.CCommand = clib:class('CCommand', nil, [[
  CCommand( int nArgC, const char **ppArgV );
  bool Tokenize( const char *pCommand, characterset_t *pBreakSet = NULL );
  void Reset();

  int ArgC() const;
  const char **ArgV() const;
  const char *ArgS() const;                    // All args that occur after the 0th arg, in string form
  const char *GetCommandString() const;        // The entire command in string form, including the 0th arg
  // const char *operator[]( int nIndex ) const;    // Gets at arguments
  const char *Arg( int nIndex ) const;        // Gets at arguments

  // Helper functions to parse arguments to commands.
  const char* FindArg( const char *pName ) const;
  int FindArgInt( const char *pName, int nDefaultVal ) const;

  static int MaxCommandLength();
  static characterset_t* DefaultBreakSet();
]])

-- public/tier1/convar.h
hooks.declare [[
  typedef void ( *FnCommandCallback_t )( const CCommand &command );
  typedef int  ( *FnCommandCompletionCallback )( const char *partial, char commands[64][64] );
]]

-- public/tier1/convar.h
M.ConCommand = clib:class('ConCommand', 'ConCommandBase', [[
  ConCommand( const char *pName, FnCommandCallback_t callback,
              const char *pHelpString = 0, int flags = 0, FnCommandCompletionCallback completionFunc = 0 );

  // virtual int AutoCompleteSuggest( const char *partial, CUtlVector< CUtlString > &commands );

  virtual bool CanAutoComplete( void );

  // Invoke the function
  virtual void Dispatch( const CCommand &command );
]])

-- public/tier1/iconvar.h
hooks.declare [[
  typedef void ( *FnChangeCallback_t )( IConVar *var, const char *pOldValue, float flOldValue );
]]

-- public/tier1/convar.h
M.ConVar = clib:class('ConVar', 'ConCommandBase', [[
  ConVar( const char *pName, const char *pDefaultValue, int flags,
          const char *pHelpString, bool bMin, float fMin, bool bMax, float fMax,
          FnChangeCallback_t callback );

  // Install a change callback (there shouldn't already be one....)
  void InstallChangeCallback( FnChangeCallback_t callback );

  // Retrieve value
  float                       GetFloat( void ) const;
  int                         GetInt( void ) const;
  bool                        GetBool() const;
  char const                 *GetString( void ) const;

  virtual void                SetValue( const char *value );
  virtual void                SetFloatValue( float value );  // C++ SetValue
  virtual void                SetIntValue( int value );  // C++ SetValue

  // Reset to default value
  void                        Revert( void );

  // True if it has a min/max setting
  bool                        GetMin( float& minVal ) const;
  bool                        GetMax( float& maxVal ) const;
  const char                 *GetDefault( void ) const;
  void                        SetDefault( const char *pszDefault );
]])

return M
