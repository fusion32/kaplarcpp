#ifndef KAPLAR_ENDIAN_H_
#define KAPLAR_ENDIAN_H_ 1

#include "common.h"

static INLINE uint16 swap_u16(uint16 x){
	return (x & 0xFF00) >> 8
		| (x & 0x00FF) << 8;
}
static INLINE uint32 swap_u32(uint32 x){
	return (x & 0xFF000000) >> 24
		| (x & 0x00FF0000) >> 8
		| (x & 0x0000FF00) << 8
		| (x & 0x000000FF) << 24;
}
static INLINE uint64 swap_u64(uint64 x){
	return (x & 0xFF00000000000000) >> 56
		| (x & 0x00FF000000000000) >> 40
		| (x & 0x0000FF0000000000) >> 24
		| (x & 0x000000FF00000000) >> 8
		| (x & 0x00000000FF000000) << 8
		| (x & 0x0000000000FF0000) << 24
		| (x & 0x000000000000FF00) << 40
		| (x & 0x00000000000000FF) << 56;
}

#if ARCH_BIG_ENDIAN
#define u16_be_to_cpu(x)	(x)
#define u16_le_to_cpu(x)	swap_u16(x)
#define u32_be_to_cpu(x)	(x)
#define u32_le_to_cpu(x)	swap_u32(x)
#define u64_be_to_cpu(x)	(x)
#define u64_le_to_cpu(x)	swap_u64(x)
#define u16_cpu_to_be(x)	(x)
#define u16_cpu_to_le(x)	swap_u16(x)
#define u32_cpu_to_be(x)	(x)
#define u32_cpu_to_le(x)	swap_u32(x)
#define u64_cpu_to_be(x)	(x)
#define u64_cpu_to_le(x)	swap_u64(x)
#else //ARCH_BIG_ENDIAN
#define u16_be_to_cpu(x)	swap_u16(x)
#define u16_le_to_cpu(x)	(x)
#define u32_be_to_cpu(x)	swap_u32(x)
#define u32_le_to_cpu(x)	(x)
#define u64_be_to_cpu(x)	swap_u64(x)
#define u64_le_to_cpu(x)	(x)
#define u16_cpu_to_be(x)	swap_u16(x)
#define u16_cpu_to_le(x)	(x)
#define u32_cpu_to_be(x)	swap_u32(x)
#define u32_cpu_to_le(x)	(x)
#define u64_cpu_to_be(x)	swap_u64(x)
#define u64_cpu_to_le(x)	(x)
#endif //ARCH_BIG_ENDIAN

#endif //KAPLAR_ENDIAN_H_
