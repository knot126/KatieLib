/**
 * Automated way of inserting code before a given instruction
 */

#if defined(__arm__)

static inline uint32_t kt_short_jump(void *from, void *to) {
	const size_t pcoffset = (((size_t)to - (size_t)from) - 8) >> 2;
	return 0xea000000 | (pcoffset & 0xffffff); // b <imm24>
}

#elif defined(__aarch64__)

static inline uint32_t kt_short_jump(void *from, void *to) {
	LogI("Gen Jump from %p to %p", from, to);
	const size_t pcoffset = ((size_t)to - (size_t)from) >> 2;
	return 0x14000000 | (pcoffset & 0x3ffffff); // b <imm26>
}

#else
#error "Unsupported platform"
#endif


static uint32_t *kt_insert_code(uint32_t *at, const void *code, size_t code_size) {
	/**
	 * "Inserts" instructions at a certain code point by replacing what `at`
	 * points to with a branch to the new code block. The new code block has
	 * the requested inserted instructions, then the original instruction which
	 * the branch replaced, then a branch back to the next instruction after the
	 * `at` instruction.
	 */
	
	void * const new_block = YipAllocate(YIP_MEMORY_REGION_PRE_SEGMENT, code_size + 8);
	
	if (!new_block) {
		return NULL;
	}
	
	// First the user's code
	memcpy(new_block, code, code_size);
	
	// Place the original instruction
	((uint32_t *)new_block)[(code_size >> 2)] = at[0];
	
	// New jump
	uint32_t * const jump_back = ((uint32_t *)new_block) + (code_size >> 2) + 1;
	jump_back[0] = kt_short_jump(jump_back, at + 1);
	
	// Jump to the new block
	at[0] = kt_short_jump(at, new_block);
	
	return jump_back - 1;
}
