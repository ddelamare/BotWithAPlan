#pragma once
#include <vector>
namespace VectorHelpers
{
	template <typename T> static bool FoundInVector(std::vector<T> list, T elem)
	{
		return std::find(list.begin(), list.end(), elem) != list.end();
	}


	template <typename T> static void RemoveFromVector(std::vector<T>* list, T elem)
	{
		auto item = std::find(list->begin(), list->end(), elem);
		if (item != list->end())
			list->erase(item);
	}

}