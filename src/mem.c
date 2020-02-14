#include "mem.h"
#include "mem_cache.h"
#include "log.h"
#include "thread.h"

typedef enum cache_idx{
	CACHE32 = 0,
	CACHE64,
	CACHE128,
	CACHE256,
	CACHE512,
	CACHE1K,
	CACHE2K,
	CACHE4K,
	CACHE8K,
	CACHE16K,

	CACHE_FIRST = CACHE32,
	CACHE_LAST = CACHE16K,
	NUM_CACHES = CACHE_LAST + 1,
	FIRST_PO2 = 5,
	LAST_PO2 = 14,
} cache_idx_t;

static mutex_t mtx;
static struct {
	uint slots;
	uint stride;
	struct mem_cache *cache;
} cache_table[] = {
	// ~8K for small allocations
	[CACHE32] = {256, 32, NULL},
	[CACHE64] = {128, 64, NULL},
	[CACHE128] = {64, 128, NULL},
	[CACHE256] = {32, 256, NULL},
	[CACHE512] = {16, 512, NULL},
	[CACHE1K] = {8, 1024, NULL},

	// ~32K for medium allocations
	[CACHE2K] = {16, 2048, NULL},
	[CACHE4K] = {8, 4096, NULL},

	// ~128K for large allocations
	[CACHE8K] = {16, 8192, NULL},
	[CACHE16K] = {8, 16384, NULL},
};

bool mem_init(void){
	cache_idx_t i;
	mutex_init(&mtx);
	for(i = CACHE_FIRST; i <= CACHE_LAST; i += 1){
		cache_table[i].cache = mem_cache_create(
			cache_table[i].slots, cache_table[i].stride);
		ASSERT(cache_table[i].cache != NULL);
	}
	return true;
}

void mem_shutdown(void){
	cache_idx_t i;
	for(i = CACHE_FIRST; i <= CACHE_LAST; i += 1)
		mem_cache_destroy(cache_table[i].cache);
	mutex_destroy(&mtx);
}

static INLINE cache_idx_t
__size_to_cache_idx(size_t size){
	DEBUG_ASSERT(size > 0);
#ifdef COMPILER_ENV32
	int next_pow2 = 31 - _CLZ32(size)
		+ !IS_POWER_OF_TWO(size);
#else
	int next_pow2 = 63 - _CLZ64(size)
		+ !IS_POWER_OF_TWO(size);
#endif
	DEBUG_ASSERT(next_pow2 >= FIRST_PO2
		&& next_pow2 <= LAST_PO2);
	DEBUG_LOG("mem_alloc/free: requested = %d, got = %d",
			size, (1ULL << next_pow2));
	return next_pow2 - FIRST_PO2;
}

void *mem_alloc(size_t size){
	void *ptr;
	cache_idx_t cache = __size_to_cache_idx(size);
	mutex_lock(&mtx);
	ptr = mem_cache_alloc(cache_table[cache].cache);
	mutex_unlock(&mtx);
	DEBUG_ASSERT(ptr != NULL);
	return ptr;
}
void mem_free(size_t size, void *ptr){
	DEBUG_ASSERT(size > 0);
	bool ret;
	cache_idx_t cache = __size_to_cache_idx(size);
	mutex_lock(&mtx);
	ret = mem_cache_free(cache_table[cache].cache, ptr);
	mutex_unlock(&mtx);
	DEBUG_ASSERT(ret);
}
