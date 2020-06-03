#include <random>

static unsigned int SelectRandomly(int min, int max)
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_int_distribution<std::mt19937::result_type> dis(min, max);
	return dis(gen);
}
