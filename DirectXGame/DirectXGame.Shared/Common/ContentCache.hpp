#pragma once

#include "Pch.hpp"
#include "Common/Helpers.hpp"
#include "Graphics/Textures/Texture2D.hpp"

//Store content items by their name (std::wstring). Will automatically delete all content items on destruction
//Suported types are textures
class ContentCache
{
public:
	typedef std::string KeyType;

	void* operator[](const KeyType& Key) const { return (void*)cache.at(Key).value; }
	bool Exists(const KeyType& Key) const { return (cache.find(Key) != cache.end()); }

	void Clear() { cache.clear(); } //Remove all items from the cache
	size_t Count() const { return cache.size(); } //Get the number of items in the cache

	void Remove(const KeyType& Key) { cache.erase(Key); } //Remove an item from the cache

protected:
	//A single content item in the cache
	struct ContentItem
	{
		enum class Type
		{
			Texture2D
		};

		ContentItem(Type Ty) : type(Ty) { }
		inline virtual ~ContentItem()
		{
			switch (type)
			{
			case Type::Texture2D:
				((Texture2D*)value)->~Texture2D();
				break;
			}
		}
		Type type;

		uint8_t value[SizeofLargestType<Texture2D>::size];
	};

	std::map<KeyType, ContentItem> cache; //all of the content items stored, by name

public:
	inline Texture2D* GetTexture2D(const KeyType& Key) const { return (Texture2D*)(cache.at(Key).value); }
	template <typename... TArgs>
	Texture2D* CreateTexture2D(const KeyType& Key, TArgs&&... Args)
	{
		auto i = cache.emplace(Key, ContentItem::Type::Texture2D);
		auto& v = (*i.first).second;
		new (v.value) Texture2D(std::forward<TArgs>(Args)...);
		return (Texture2D*)v.value;
	}
	Texture2D* CreateTexture2D(const KeyType& Key, Texture2D&& Texture)
	{
		auto i = cache.emplace(Key, ContentItem::Type::Texture2D);
		auto& v = (*i.first).second;
		new (v.value) Texture2D(Texture);
		return (Texture2D*)v.value;
	}
};