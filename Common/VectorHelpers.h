#pragma once
#include <vector>
namespace VectorHelpers
{
	template <typename T> static bool FoundInVector(std::vector<T> list, T elem)
	{
		return std::find(list.begin(), list.end(), elem) != list.end();
	}
}