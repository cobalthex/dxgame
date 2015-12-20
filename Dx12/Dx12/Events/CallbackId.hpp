#include "Pch.hpp"

typedef size_t id_t;

template <class CallbackArgsType> class CallbackCollection;

//A simple way to uniquely identify callbacks
template <class CallbackArgsType>
struct CallbackId
{
	CallbackId(id_t Id = 0) : id(Id), collection(nullptr) { }
	CallbackId(const CallbackId& Id) : id(Id.id), collection(Id.collection) { }
	CallbackId(CallbackId&& Id) : id(Id.id), collection(Id.collection) { Id.id = 0; Id.collection = nullptr; }

	CallbackId& operator = (const CallbackId& Id)
	{
		id = Id.id;
		collection = Id.collection;

		return *this;
	}

	operator id_t () const { return id; }

protected:
	id_t id;
	CallbackCollection<CallbackArgsType>* collection = nullptr;

	CallbackId(id_t Id, CallbackCollection<CallbackArgsType>* const CallbackCollection) : id(Id), collection(CallbackCollection) { }
	friend class CallbackCollection<CallbackArgsType>;
};

//A callback ID that will automatically remove the callback from its collection when this goes out of scope
template <class CallbackArgsType>
struct ScopedCallbackId : public CallbackId<CallbackArgsType>
{
	ScopedCallbackId& operator = (const CallbackId& Id)
	{
		CallbackId::operator=(Id);

		return *this;
	}
	~ScopedCallbackId()
	{
		if (collection != nullptr)
			collection->Remove(*this);
		id = 0;
		collection = nullptr;
	}

	//Disable the automatic removal (not undoable)
	void Release() { collection = nullptr; }

protected:
};