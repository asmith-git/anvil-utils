#include "anvil/lutils/Message.hpp"


namespace anvil { namespace lutils { namespace msg {
	// Queue

	Queue::Queue() :
		_base_id(0u)
	{}

	Queue::~Queue() {
		Flush();
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

	void Queue::ProduceImplementation(Message* msgs, const size_t count) {
		if (count == 0u) return;

		std::exception_ptr exception;
		// Consume messages
		{
			std::lock_guard<decltype(_consumer_mutex)> lock(_consumer_mutex);
			for (Consumer* c : _consumers) {
				try {
					c->Consume(msgs, count);
				} catch (...) {
					exception = std::current_exception();
				}
			}

		}

		// Cleanup messages
		{
			const Message* const end = msgs + count;
			for (Message* m = msgs; m < end; ++m) {
				if (m->cleanup_flag) {
					try {
						m->producer->Cleanup(*m);
					} catch (...) {
						exception = std::current_exception();
					}
				}
			}
		}

		// If an exception was caught then throw it
		if (exception) std::rethrow_exception(exception);
	}

	void Queue::Produce(Producer& producer, Message* const msgs, const size_t count, const bool blocking) {
		{
			const Message* const end = msgs + count;
			for (Message* m = msgs; m < end; ++m) {
				m->id = _base_id++;
				m->producer = &producer;
			}
		}

		if (blocking) {
			Flush();
			ProduceImplementation(msgs, count);
		} else {
			std::lock_guard<decltype(_message_mutex)> lock(_message_mutex);
			_messages.reserve(_messages.size() + count);
			{
				const Message* const end = msgs + count;
				for (Message* m = msgs; m < end; ++m) {
					_messages.push_back(*m);
				}
			}
		}
	}

	void Queue::Flush() {
		if (_messages.empty()) return;

		PODVectorDynamic<Message> messages;
		{
			std::lock_guard<decltype(_message_mutex)> lock(_message_mutex);
			messages.swap(_messages);
		}
		ProduceImplementation(messages.data(), messages.size());
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
}}}