#include "Common.h"

#include <random>

std::size_t s_MaxAlignment = alignof(std::max_align_t);
std::size_t s_NumOfRandAllocations = 1e5;
std::size_t s_MaxBlockSize = 4096;
std::size_t s_1GB = 1024 * 1024 * 1024;
std::vector<std::size_t> s_Sizes;
std::vector<std::size_t> s_RandomSizes;
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
