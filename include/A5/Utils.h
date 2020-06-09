#ifndef A5_UTILS_H
#define A5_UTILS_H

#include <cstddef>

namespace A5
{
	namespace Utils
	{
		constexpr std::size_t Log2(std::size_t x)
		{
			return x == 1 ? 0 : 1 + Log2(x / 2);
		}
	}
}

#endif // !A5_UTILS_H
