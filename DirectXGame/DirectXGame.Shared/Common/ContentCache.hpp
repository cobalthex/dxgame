#pragma once

#include "Pch.hpp"
#include "Common/Helpers.hpp"
#include "IContent.hpp"

//Store content items by their name (std::wstring). Will automatically delete all content items on destruction
//Suported types are textures
class ContentCache
{
public:
	typedef std::string KeyType;
	typedef std::shared_ptr<IContent> ContentItem;

	template <class T>
	inline std::shared_ptr<T> Add(const KeyType& Key, T* Content) { cache[Key] = ContentItem(Content); return std::static_pointer_cast<T>(cache[Key]); } //Add an item to the cache
	template <class T>
	inline std::shared_ptr<T> Add(const KeyType& Key, const std::shared_ptr<T>& Content) { cache[Key] = std::static_pointer_cast<IContent>(Content); return std::static_pointer_cast<T>(cache[Key]); } //Add an item to the cache

	inline void Remove(const KeyType& Key) { cache.erase(Key); } //Remove an item from the cache

	inline ContentItem& operator[](const KeyType& Key) { return cache.at(Key); } //Retrieve a cache item (does not check existence)
	inline const ContentItem& operator[](const KeyType& Key) const { return cache.at(Key); } //Retrieve a cache item (does not check existence)
	inline bool Exists(const KeyType& Key) const { return (cache.find(Key) != cache.end()); } //Check to see if a cache item exists

	inline void Clear() { cache.clear(); } //Remove all items from the cache
	inline size_t Count() const { return cache.size(); } //Get the number of items in the cache

	//remove items that have no references left
	void Cleanup()
	{
		for (auto& itr = cache.begin(); itr != cache.end();)
		{
			if (itr->second.use_count() < 2)
				itr = cache.erase(itr);
			else
				itr++;
		}
	}

protected:
	std::map<KeyType, ContentItem> cache; //all of the content items stored, by name
};