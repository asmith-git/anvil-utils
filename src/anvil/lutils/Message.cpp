#include "anvil/lutils/Message.hpp"


namespace anvil { namespace msg {
	// Queue

	Queue::Queue() :
		_base_id(0u)
	{}

	Queue::~Queue() {

	}

	void Queue::Add(CommonBase& base) {
		Consumer* consumer = dynamic_cast<Consumer*>(&base);
		if (consumer) {
			std::lock_guard<decltype(_mutex)> lock(_mutex);
			_consumers.push_back(consumer);
		}
	}

	void Queue::Remove(CommonBase& base) {
		Consumer* consumer = dynamic_cast<Consumer*>(&base);
		if (consumer) {
			std::lock_guard<decltype(_mutex)> lock(_mutex);
			_consumers.erase(std::find(_consumers.begin(), _consumers.end(), consumer));
		}
	}

	void Queue::Produce(Message& message) {
		std::lock_guard<decltype(_mutex)> lock(_mutex);

		message.id = _base_id++;
		std::exception_ptr exception;
		for (Consumer* c : _consumers) {
			try {
				c->Consume(message);
			} catch (...) {
				exception = std::current_exception();
			}
		}

		if(message.cleanup_flag) message.producer->Cleanup(message);

		if (exception) std::rethrow_exception(exception);
	}

	// CommonBase

	CommonBase::CommonBase(Queue& queue) :
		_queue(queue)
	{
		_queue.Add(*this);
	}

	CommonBase::~CommonBase() {
		_queue.Remove(*this);
	}

	// Producer

	Producer::Producer(Queue& queue) :
		CommonBase(queue)
	{}

	Producer::~Producer() {

	}

	// Consumer

	Consumer::Consumer(Queue& queue) :
		CommonBase(queue)
	{}

	Consumer::~Consumer() {

	}
}}