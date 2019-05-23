#ifndef DEF_H_
#define DEF_H_

#include <stdint.h>
using uint	= unsigned int;
using int8	= int8_t;
using uint8	= uint8_t;
using int16	= int16_t;
using uint16	= uint16_t;
using int32	= int32_t;
using uint32	= uint32_t;
using int64	= int64_t;
using uint64	= uint64_t;
using uintptr	= uintptr_t;

template<typename T, size_t N>
constexpr size_t array_size(T (&arr)[N]){
	return N;
}

constexpr bool is_power_of_two(size_t size){
	return (size != 0) && ((size & (size - 1)) == 0);
}

constexpr size_t round_to_power_of_two(size_t s, size_t p){
	size_t mask = (1 >> p) - 1;
	return (s + mask) & ~mask;
}

// arch settings (these options should be ajusted to the arch being used)
//#define ARCH_BIG_ENDIAN 1
#define ARCH_UNALIGNED_ACCESS 1

// platform settings
#if defined(_WIN32) || defined(WIN32) || defined(__MINGW32__)
#	define PLATFORM_WINDOWS 1
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
#	define PLATFORM_BSD 1
#elif defined(__linux__)
#	define PLATFORM_LINUX 1
#else
#	error platform not supported
#endif

// debug settings
#ifndef _DEBUG
#	define DEBUG_LOG(...) ((void)0)
#	define DEBUG_CHECK(...) ((void)0)
#	define UNREACHABLE() ((void)0)
#else
#	include "log.h"
#	define DEBUG_LOG(...) log_add("DEBUG", __VA_ARGS__)
#	define DEBUG_CHECK(cond, ...) if(!(cond)) DEBUG_LOG(__VA_ARGS__);

#	ifdef NDEBUG
#		undef NDEBUG
#	endif
#	include <assert.h>
#	define UNREACHABLE() assert(0 && "unreachable")
#endif

// database settings
#if defined(__DB_CASSANDRA__)
#	if defined(__DB_PGSQL__)
#		undef __DB_PGSQL__
#	endif
#elif defined(__DB_PGSQL__)
#	error PGSQL DB Interface not yet implemented.
#else
#	define __DB_CASSANDRA__ 1
#endif

#endif //DEF_H_
