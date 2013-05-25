#ifdef EVENTS_EXPORTS
#define EVENTS_API __declspec(dllexport)
#else
#define EVENTS_API __declspec(dllimport)
#endif