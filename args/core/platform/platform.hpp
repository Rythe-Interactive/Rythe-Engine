#pragma once

#ifdef ARGS_WINDOWS
	#ifdef ARGS_INTERNAL
		#define ARGS_API __declspec(dllexport)
	#else
		#define ARGS_API __declspec(dllimport)
	#endif
#else
	#error Support for this platform is not complete yet.
#endif