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
			std::lock_guard<decltype(_consumer_mutex)> lock(_consumer_mutex);
			_consumers.push_back(consumer);
		}
	}

	void Queue::Remove(CommonBase& base) {
		Consumer* consumer = dynamic_cast<Consumer*>(&base);
		if (consumer) {
			std::lock_guard<decltype(_consumer_mutex)> lock(_consumer_mutex);
			_consumers.erase(std::find(_consumers.begin(), _consumers.end(), consumer));
		}
	}

	void Queue::ForceProduce(Message* msgs, const size_t count) {
		std::lock_guard<decltype(_consumer_mutex)> lock(_consumer_mutex);

		const Message* const end = msgs + count;
		for (Message* m = msgs; m < end; ++m) {
			std::exception_ptr exception;
			for (Consumer* c : _consumers) {
				try {
					c->Consume(*m);
				} catch (...) {
					exception = std::current_exception();
				}
			}

			if (m->cleanup_flag) m->producer->Cleanup(*m);

			if (exception) std::rethrow_exception(exception);
		}

	}

	void Queue::Produce(Message& message, const bool blocking) {
		message.id = _base_id++;

		if (blocking) {
			Flush();
			ForceProduce(&message, 1u);
		} else {
			std::lock_guard<decltype(_message_mutex)> lock(_message_mutex);
			_messages.push_back(message);
		}
	}

	void Queue::Flush() {
		if (_messages.empty()) return;

		std::vector<Message> messages;
		{
			std::lock_guard<decltype(_message_mutex)> lock(_message_mutex);
			messages.swap(_messages);
		}
		ForceProduce(messages.data(), messages.size());
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