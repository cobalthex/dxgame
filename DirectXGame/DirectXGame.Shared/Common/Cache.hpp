#pragma once

#include "Pch.hpp"

//A simple cache system that can store, by name, a single cache item
template <typename KeyType, class StorageType>
class Cache
{
public:
	typedef std::shared_ptr<StorageType> CacheItem;

	Cache() : cache() { }
	virtual ~Cache() { }

	virtual size_t Count() const { return cache.size(); }

	virtual CacheItem& Load(const KeyType& Key) = 0; //Load an item from the cache (and fetch it if it does not exist)
	virtual CacheItem& Add(const KeyType& Key, const CacheItem& Item) { cache[Key] = Item; return cache[Key]; } //Add an existing item to the cache

	virtual bool Has(const KeyType& Key) const { return (cache.find(Key) != cache.end()); }

	//Remove all items from the cache
	virtual void Clear() { cache.clear(); }

	//Go through the cache and remove items that no longer have any references
	virtual void Cleanup()
	{
		for (auto& itr = cache.begin(); itr != cache.end();)
		{
			if (itr->second.unique())
				itr = cache.erase(itr);
			else
				itr++;
		}
	}

protected:
	std::map<KeyType, CacheItem> cache;
};