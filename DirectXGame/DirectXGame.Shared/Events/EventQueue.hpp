#pragma once

#include "Pch.hpp"
#include "Event.hpp"
#include "CallbackCollection.hpp"

//A simple, thread-safe, event queue that is designed to handle events of any time
template <class EventType = Event>
class EventQueue
{
public:
	EventQueue()
	{
		Event* Assert_EventType_Is_Of_Type_Event = (EventType*)nullptr; //make sure that EventType is a derivative of Event
	}
	EventQueue(EventQueue&& EventQueue)
	{
		events = std::move(EventQueue.events);
		handlers = std::move(EventQueue.handlers);
		queueMutex = std::move(EventQueue.queueMutex);
		popWait = std::move(EventQueue.popWait);
	}
	EventQueue& operator = (EventQueue&& EventQueue)
	{
		if (this != &EventQueue)
		{
			events = std::move(EventQueue.events);
			handlers = std::move(EventQueue.handlers);
		}
		return *this;
	}
	virtual ~EventQueue() = default;

	//Pass an event to the queue
	inline void Enqueue(const EventType& Event)
	{
		EnqueueSafe(Event);
	}
	//Return the first event in the queue after removing it
	inline EventType Dequeue()
	{
		return DequeueSafe();
	}
	//Clear all events in the queue
	inline void Clear()
	{
		ClearSafe();
	}
	//The number of events currently waiting in the queue
	inline size_t Count() const { return events.size(); }

	//Cycle through and handle all events in the queue, removing them as they get used
	//Note, for events with multiple repetitions, time gets updated after each call
	virtual void Cycle()
	{
		TimePoint time = Timer::now();
		size_t size = events.size(), n = 0; //only parse those originally in the queue, as not to create a possible infinite loop

		while (!events.empty()) //TODO: make sure thread-safe
		{
			//allow for handler to add new events by removing the first beforehand
			auto i = DequeueSafe();
			
			Evaluate(i, time);

			if (n++ > size)
				break; //finished original set
		}
	}

	//Evaluate a single item on the queue
	//Allows for multithreaded processing
	inline virtual void RunOnce()
	{
		auto ev = DequeueSafe();
		Evaluate(ev);
	}

	//All of the event handlers that can receive events
	CallbackCollection<EventType> handlers = CallbackCollection<EventType>();
	typedef ScopedCallbackId<EventType> HandlerId; //Alias of callback ID for event handler. Automatically scoped

protected:
	std::queue<EventType> events = std::queue<EventType>();

	//Evaluate a single event at a given time
	virtual void Evaluate(EventType& Event, const TimePoint& Time = Timer::now())
	{
		//the Event is ready to be parsed, and if not, re-add
		auto finalTime = Event.time + Event.delay;

		if (Time >= finalTime)
			handlers.Run(Event);
		else
		{
			EnqueueSafe(Event);
			return;
		}

		if (--Event.repetitions > 0)
		{
			Event.time = Time;
			EnqueueSafe(Event);
		}
	}

	//thread-safe operations for the queue

	std::mutex queueMutex;
	std::condition_variable popWait;

	EventType DequeueSafe()
	{
		std::unique_lock<std::mutex> lock(queueMutex);

		//do nothing while the queue is empty (if waiting for dequeuing)
		while (events.empty())
			popWait.wait(lock);

		auto m = events.front();
		events.pop();
		return m;
	}

	void EnqueueSafe(const EventType& Event)
	{
		std::unique_lock<std::mutex> lock(queueMutex);

		events.push(Event);

		lock.unlock();
		popWait.notify_one();
	}
	void EnqueueSafe(const EventType&& Event)
	{
		std::unique_lock<std::mutex> lock(queueMutex);

		events.push(std::move(Event));

		lock.unlock();
		popWait.notify_one();
	}

	void ClearSafe()
	{
		std::unique_lock<std::mutex> lock(queueMutex);

		events = std::queue<EventType>();

		lock.unlock();
		popWait.notify_one();
	}

};