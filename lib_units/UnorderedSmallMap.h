#pragma once

#include <vector>
#include <algorithm>
#include <utility>
//#include <memory_resource>

template <typename K, typename V>
class UnorderedSmallMap {
	using Item = std::pair<K, V>;
	std::vector<Item> data;

public:
	using iterator = typename std::vector<Item>::iterator;
	using const_iterator = typename std::vector<Item>::const_iterator;

	auto find(this auto&& self, const K& key) {
		return std::find_if(self.data.begin(), self.data.end(),
			[&key](const auto& item) { return item.first == key; });
	}

	bool contains(const K& key) const {
		return find(key) != data.end();
	}

	std::pair<iterator, bool> try_emplace(const K& key) {
		auto it = find(key);
		if (it != data.end()) return { it, false };
		data.emplace_back(key, V{});
		return { std::prev(data.end()), true };
	}

	// Удаление по ключу
	size_t erase(const K& key) {
		auto it = find(key);
		if (it == data.end()) return 0;

		// Если элемент не последний, заменяем его последним
		if (it != std::prev(data.end())) {
			*it = std::move(data.back());
		}
		data.pop_back();
		return 1;
	}

	// Эффективное массовое удаление по условию
	template <typename Predicate>
	size_t erase_if(Predicate pred) {
		if (data.empty()) return 0;
		size_t count = 0;
		while (pred(data.back())) {
			count++;
			data.pop_back();
			if (data.empty()) return count;
		}
		for (auto i = std::ssize(data) - 2; i >= 0; --i) {
			if (pred(data[i])) {
				count++;
				data[i] = std::move(data.back());
				data.pop_back();
			}
		}
		return count;
	}

	auto begin(this auto&& self) { return self.data.begin(); }
	auto end(this auto&& self) { return self.data.end(); }
	auto empty() const { return data.empty(); }
	auto size() const noexcept { return data.size(); }
	void reserve(size_t n) { data.reserve(n); }
};
