#include "Common.h"

#include <random>

std::size_t s_MaxAlignment = alignof(std::max_align_t);
std::size_t s_NumOfRandAllocations = 1000;
std::size_t s_MaxBlockSize = 8192;
std::size_t s_30MB = 1024 * 1024 * 30;
std::vector<std::size_t> s_Sizes;
std::size_t s_SizesAgg;
std::vector<std::size_t> s_RandomSizes;
std::size_t s_RandomSizesAgg;
std::vector<std::size_t> s_DeallocationIndices;

namespace Utils
{
	int SelectRandomly(int min, int max)
	{
		static std::random_device rd;
		static std::mt19937 gen(rd());
		std::uniform_int_distribution<std::mt19937::result_type> dis(min, max);
		return dis(gen);
	}
};
