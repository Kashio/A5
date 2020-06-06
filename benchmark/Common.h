#ifndef COMMON_H
#define COMMON_H

#include <vector>

extern std::size_t s_MaxAlignment;
extern std::size_t s_NumOfRandAllocations;
extern std::size_t s_MaxBlockSize;
extern std::size_t s_1GB;
extern std::vector<std::size_t> s_Sizes;
extern std::size_t s_SizesAgg;
extern std::vector<std::size_t> s_RandomSizes;
extern std::size_t s_RandomSizesAgg;
extern std::vector<std::size_t> s_DeallocationIndices;

namespace Utils
{
	int SelectRandomly(int min, int max);
};

#endif // !COMMON_H
