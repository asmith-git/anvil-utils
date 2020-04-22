#include "anvil/lutils/Message.hpp"


namespace anvil { namespace lutils { namespace msg {
	// Queue

	Queue::Queue(const bool in_order) :
		_recursion_counter(0u),
		_base_id(0u),
		_in_order(in_order)
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
		++_recursion_counter;

		std::exception_ptr exception;
		try {
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
		} catch (...) {
			exception = std::current_exception();
		}

		--_recursion_counter;

		// If an exception was caught then throw it
		if (exception) std::rethrow_exception(exception);
	}

	void Queue::Initialise(Producer& producer, Message* const msgs, const size_t count) {
		const Message* const end = msgs + count;
		for (Message* m = msgs; m < end; ++m) {
			m->id = _base_id++;
			m->producer = &producer;
		}
	}

	void Queue::Produce(Message* const msgs, const size_t count, const bool blocking) {
		if (blocking) {
			// If the buffer guarantees ordering of messages
			if (_in_order) {
				// If this message is produced during the production of another message then use non-blocking mode
				if (_recursion_counter > 0u) goto NON_BLOCKING;

				// Flush any messages that go before this one
				Flush();
			}

			// Send messages to consumers
			ProduceImplementation(msgs, count);

			// Flush any messages that were generated as a result of the messages just consumed
			while (Flush() > 0u);
		} else {
	NON_BLOCKING:
			// Push to the queue of tasks waiting to be flushed
			std::lock_guard<decltype(_message_mutex)> lock(_message_mutex);
			_messages.push_back_many(msgs, count);
		}
	}

	size_t Queue::Flush() {
		if (_messages.empty()) return 0u;

		PODVectorDynamic<Message> messages;
		{
			std::lock_guard<decltype(_message_mutex)> lock(_message_mutex);
			messages.swap(_messages);
		}
		const size_t size = messages.size();
		ProduceImplementation(messages.data(), size);
		return size;
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