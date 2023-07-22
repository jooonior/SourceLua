/* Macros for generating C API wrappers around C++ member functions.
 * struct Person {
 *     Person(const char *name);
 *     void Say(const char *what);
 * };
 * EXPORT_CLASS(Person, const char *) // Person *(*Person__new)(const char *); void (*Person__delete)(Person *);
 * EXPORT_METHOD(Person, Say)         // void (*Person__Say)(Person *, const char *);
 */

#define LIB_EXPORT extern "C" __declspec(dllexport)

// Exports given string as 'INTERFACE_VERSION'.
#define EXPORT_INTERFACE_VERSION(Version) LIB_EXPORT const char *INTERFACE_VERSION = Version;

#pragma region EXPORT_STRUCT / EXPORT_CLASS

template<typename T, typename Signature>
struct wrap_struct;

// This signature indirection exists so that the macro allows specifying parameter names.

template<typename T, typename... Args>
struct wrap_struct<T, void(Args...)>
{
	static T *constructor(Args... args) { return new T(args...); }
	static void destructor(T *t) { delete t; }
};

// Generates and exports C wrappers for constructor and destructor. Expects constructor parameter types as __VA_ARGS__.
#define EXPORT_STRUCT(Name, ...) \
	static void __##Name##__signature(__VA_ARGS__); \
	using __##Name##__wrapper = wrap_struct<Name, decltype(__##Name##__signature)>; \
	LIB_EXPORT auto Name##__new = __##Name##__wrapper::constructor; \
	LIB_EXPORT auto Name##__delete = __##Name##__wrapper::destructor;

// Generates and exports C wrappers for constructor and destructor.
#define EXPORT_CLASS(Name, ...) EXPORT_STRUCT(Name, __VA_ARGS__)

#pragma endregion

#pragma region EXPORT_METHOD

template<typename T>
struct wrap_method;

// Needs two layers of templates to compile in VS2013.
// https://stackoverflow.com/questions/29919987/

template<typename R, typename C, typename... Args>
struct wrap_method<R(C:: *)(Args...)>
{
	template<R(C:: *t)(Args...)>
	static R method(C *c, Args... args) { return (c->*t)(args...); }
};

template<typename R, typename C, typename... Args>
struct wrap_method<R(C:: *)(Args...) const>
{
	template<R(C:: *t)(Args...) const>
	static R method(const C *c, Args... args) { return (c->*t)(args...); }
};

template<typename R, typename... Args>
struct wrap_method<R(*)(Args...)>
{
	template<R(*t)(Args...)>
	static R method(Args... args) { return t(args...); }
};

// Default arguments for macros: https://stackoverflow.com/a/56038661
#define FUNC_CHOOSER(_f0, _f1, _f2, _f3, _f4, _f5, _f6, _f7, _f8, _f9, _f10, _f11, _f12, _f13, _f14, _f15, _f16, ...) _f16
#define FUNC_RECOMPOSER(argsWithParentheses) FUNC_CHOOSER argsWithParentheses
#define CHOOSE_FROM_ARG_COUNT(F, ...) FUNC_RECOMPOSER((__VA_ARGS__, \
            F##_16, F##_15, F##_14, F##_13, F##_12, F##_11, F##_10, F##_9, F##_8,\
            F##_7, F##_6, F##_5, F##_4, F##_3, F##_2, F##_1, ))
#define NO_ARG_EXPANDER(FUNC) ,,,,,,,,,,,,,,,,FUNC ## _0
#define MACRO_CHOOSER(FUNC, ...) CHOOSE_FROM_ARG_COUNT(FUNC, NO_ARG_EXPANDER __VA_ARGS__ (FUNC))
#define MULTI_MACRO(FUNC, ...) MACRO_CHOOSER(FUNC, __VA_ARGS__)(__VA_ARGS__)

// Generates and exports C wrapper for member function. Overloaded methods need signature as third argument.
#define EXPORT_METHOD_AS(Class, Name, Method, ...) MULTI_MACRO(_EXPORT_METHOD_AS, Class, Name, Method, __VA_ARGS__)

// Generates and exports C wrapper for member function. Overloaded methods need signature as third argument.
#define EXPORT_METHOD(Class, Method, ...) EXPORT_METHOD_AS(Class, Method, Method, __VA_ARGS__)

#define _EXPORT_METHOD_AS_3(Class, Name, Method) _EXPORT_METHOD_AS_4(Class, Name, Method, decltype(&Class::Method))
#define _EXPORT_METHOD_AS_4(Class, Name, Method, Signature) \
	LIB_EXPORT auto Class##__##Name = wrap_method<Signature>::method<&Class::Method>;

#pragma endregion
