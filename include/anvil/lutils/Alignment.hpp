//Copyright 2019 Adam G. Smith
//
//Licensed under the Apache License, Version 2.0 (the "License");
//you may not use this file except in compliance with the License.
//You may obtain a copy of the License at
//
//http ://www.apache.org/licenses/LICENSE-2.0
//
//Unless required by applicable law or agreed to in writing, software
//distributed under the License is distributed on an "AS IS" BASIS,
//WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//See the License for the specific language governing permissions and
//limitations under the License.

#ifndef ANVIL_LUTILS_ALIGNMENT_HPP
#define ANVIL_LUTILS_ALIGNMENT_HPP

#ifndef ANVIL_CACHE_LINE
#define ANVIL_CACHE_LINE 64
#endif

#ifndef ANVIL_LEVEL_1_CACHE
#define ANVIL_LEVEL_1_CACHE 32768 // 32 KiB
#endif

#ifndef ANVIL_LEVEL_2_CACHE
#define ANVIL_LEVEL_2_CACHE (ANVIL_LEVEL_1_CACHE * 2)
#endif

#ifndef ANVIL_LEVEL_3_CACHE
#define ANVIL_LEVEL_3_CACHE (ANVIL_LEVEL_2_CACHE * 2)
#endif

#ifndef ANVIL_PAGE_SIZE
#define ANVIL_PAGE_SIZE 4096 // 4 KiB
#endif

namespace anvil {
	enum {
		CACHE_LINE_BYTES = ANVIL_CACHE_LINE,
		CACHE_LEVEL_1_BYTES = ANVIL_LEVEL_1_CACHE,
		CACHE_LEVEL_2_BYTES = ANVIL_LEVEL_2_CACHE,
		CACHE_LEVEL_3_BYTES = ANVIL_LEVEL_3_CACHE,
		PAGE_SIZE_BYTES = ANVIL_PAGE_SIZE
	};

	static_assert((CACHE_LEVEL_1_BYTES % CACHE_LINE_BYTES) == 0, "Level 1 cache size must be a multiple of cache line size");
	static_assert((CACHE_LEVEL_2_BYTES % CACHE_LINE_BYTES) == 0, "Level 2 cache size must be a multiple of cache line size");
	static_assert((CACHE_LEVEL_3_BYTES % CACHE_LINE_BYTES) == 0, "Level 3 cache size must be a multiple of cache line size");
}

#endif
