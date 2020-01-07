#include "slab.h"
#include "log.h"
#include "system.h"

#include <stdlib.h>
#include <stdio.h>

#define FREENODE_NEXT(ptr) (*(void**)(ptr))

struct slab{
	uint32 stride;
	uint32 capacity;
	uint32 offset;
	void *mem;
	void *freelist;
};

#define PTR_ALIGNMENT		(sizeof(void*))
#define PTR_ALIGNMENT_MASK	(sizeof(void*) - 1)
struct slab *slab_create(uint32 slots, uint32 stride){
	uint32 capacity, mem_offset;
	uint32 alignment, alignment_mask;
	struct slab *s;

	// check if stride has the minimum alignment requirement
	if((stride & PTR_ALIGNMENT_MASK) != 0){
		LOG_WARNING("slab_create: ajusting stride to have"
			" the minimum alignment of %u", PTR_ALIGNMENT);
		stride = (stride + PTR_ALIGNMENT_MASK) & ~PTR_ALIGNMENT_MASK;
	}

	// if stride is a power of two, it can be used as
	// a better alignment
	if(IS_POWER_OF_TWO(stride))
		alignment = stride;
	else
		alignment = PTR_ALIGNMENT;

	// properly align the offset to memory
	alignment_mask = alignment - 1;
	mem_offset = sizeof(struct slab);
	if((mem_offset & alignment_mask) != 0)
		mem_offset = (mem_offset + alignment_mask) & ~alignment_mask;

	// allocate and initialize slab
	capacity = slots * stride;
	s = sys_aligned_malloc(mem_offset + capacity, alignment);
	s->stride = stride;
	s->capacity = capacity;
	s->offset = 0;
	s->mem = OFFSET_POINTER(s, mem_offset);
	s->freelist = NULL;
	return s;
}

void slab_destroy(struct slab *s){
	sys_aligned_free(s);
}

void *slab_alloc(struct slab *s){
	void *ptr = NULL;
	if(s->freelist != NULL){
		ptr = s->freelist;
		s->freelist = FREENODE_NEXT(s->freelist);
		return ptr;
	}
	if(s->offset < s->capacity){
		ptr = OFFSET_POINTER(s->mem, s->offset);
		s->offset += s->stride;
	}
	return ptr;
}

bool slab_free(struct slab *s, void *ptr){
	// check if ptr belongs to this slab
	if(ptr < s->mem || ptr > OFFSET_POINTER(s->mem, s->capacity))
		return false;

	// return memory to slab
	uint32 last_offset = s->offset - s->stride;
	if(ptr == OFFSET_POINTER(s->mem, last_offset)){
		s->offset = last_offset;
	}else{
		FREENODE_NEXT(ptr) = s->freelist;
		s->freelist = ptr;
	}
	return true;
}

bool slab_is_full(struct slab *s){
	return s->offset >= s->capacity && s->freelist == NULL;
}

bool slab_is_empty(struct slab *s){
	uint32 count;
	void *freenode;
	if(s->offset == 0)
		return true;

	count = 0;
	freenode = s->freelist;
	while(freenode != NULL){
		count += s->stride;
		freenode = FREENODE_NEXT(freenode);
	}

	if(s->offset == count){
		s->offset = 0;
		s->freelist = NULL;
		return true;
	}
	return false;
}

void slab_report(struct slab *s){
	void *ptr;
	LOG("memory block report:");
	LOG("\tstride = %ld", s->stride);
	LOG("\tcapacity = %ld", s->capacity);
	LOG("\toffset = %ld", s->offset);
	LOG("\tbase: %p", s->mem);
	LOG("\tfreelist:");
	for(ptr = s->freelist; ptr != NULL; ptr = *(void**)ptr)
		LOG("\t\t* %p", ptr);
}
