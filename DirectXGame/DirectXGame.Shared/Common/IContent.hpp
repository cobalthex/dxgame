#pragma once

//The basis for items that can be cached in a content cache
class IContent
{
public:
	virtual ~IContent() { }
protected:
	IContent() { }
};