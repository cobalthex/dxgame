#pragma once

#include "Pch.hpp"
#include "CallbackId.hpp"

//The callback function
template <class CallbackArgsType>
using CallbackFunc = std::function<void(const CallbackArgsType& Args)>;

//Holds a collection of callbacks of a single type (such as event handlers)
//Each callback is stored under and ID that allows for deletion
template <class CallbackArgsType>
class CallbackCollection
{
public:
	typedef CallbackFunc<CallbackArgsType> CallbackFunc;

	CallbackCollection() : idCount(0), callbacks() { }
	virtual ~CallbackCollection() { }

	//Run the callbacks with the specified arguments
	void Run(const CallbackArgsType& Args) const
	{
		for (auto& i : callbacks)
			i.second(Args);
	}
	//Add a callback to the list of callbacks (Adds to the front) - Duplicates allowed
	inline CallbackId<CallbackArgsType> Add(const CallbackFunc& Callback)
	{
		callbacks[++idCount] = Callback;
		return CallbackId<CallbackArgsType>(idCount, this);
	}

	//Remove a callback from the list of callbacks
	//std::functions are dumb
	inline void Remove(const CallbackId<CallbackArgsType>& CallbackId)
	{
		if (CallbackId.collection == this)
			callbacks.erase(CallbackId.id);
	}

	//Get the callback from the specified ID
	inline CallbackFunc& At(const CallbackId<CallbackArgsType>& CallbackId)
	{
		return callbacks[CallbackId.id];
	}

	//Remove all callbacks
	inline void Clear() { callbacks.clear(); }
	//The current number of callbacks stored
	inline size_t Count() const { return callbacks.size(); }

	//Run the callbacks (alias of Run())
	inline void operator () (const CallbackArgsType& Args) const { Run(Args); }
	//Add a new callback (alias of Add())
	inline CallbackId<CallbackArgsType> operator += (const CallbackFunc& Callback) { return Add(Callback); }
	//Remove an existing callback (alias of Remove())
	inline void operator -= (const CallbackId<CallbackArgsType>& CallbackId) { Remove(CallbackId); }
	//Get the callback from the specified ID (alias of At())
	inline CallbackFunc& operator [] (const CallbackId<CallbackArgsType>& CallbackId) { return At(CallbackId); }

protected:
	std::map<id_t, CallbackFunc> callbacks; //a list of all of the callbacks stored

	id_t idCount; //the number of Ids (no ID may have an ID of zero)
};