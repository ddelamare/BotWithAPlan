#pragma once
#include <vector>
namespace VectorHelpers
{
	template <typename T> static bool FoundInVector(std::vector<T> list, T elem)
	{
		return std::find(list.begin(), list.end(), elem) != list.end();
	}

	template <typename T, typename U> static U GetFromPairVector(std::vector<std::pair<T, U>> list, T key)
	{
		for (std::pair<T, U> pair : list)
		{
			if (pair.first == key)
			{
				return pair.second;
			}
		}
		return U();
	}


	template <typename T> static void RemoveFromVector(std::vector<T>* list, T elem)
	{
		auto item = std::find(list->begin(), list->end(), elem);
		if (item != list->end())
			list->erase(item);
	}

	// Returns elements in the first list that are not in the second list
	template <typename T> static std::vector<T> FindMissingInVector(std::vector<T> elems, std::vector<T> list)
	{
		auto missing = std::vector<T>();
		for (auto &elem : elems) {
			if (!FoundInVector(list, elem))
			{
				missing.push_back(elem);
			}
		}
		return missing;
	}

}