//Copyright 2020 Adam G. Smith
//
//Licensed under the Apache License, Version 2.0 (the "License");
//you may not use this file except in compliance with the License.
//You may obtain a copy of the License at
//
//http ://www.apache.org/licenses/LICENSE-2.0
//
//Unless required by applicable law or agreed to in writing, software
//distributed under the License is distributed on an "AS IS" BASIS,
//WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//See the License for the specific language governing permissions and
//limitations under the License.

#ifndef ANVIL_LUTILS_MESSAGE_HPP
#define ANVIL_LUTILS_MESSAGE_HPP

#include <cstdint>
#include <vector>
#include <mutex>
#include <atomic>
#include "anvil/lutils/PODVector.hpp"
#include "anvil/lutils/IDGenerator.hpp"

namespace anvil { namespace lutils { namespace msg {

	class Producer;
	class Consumer;
	class CommonBase;

	enum : uint32_t {
		MSG_NULL = 0u						//!< Default message type
	};

	struct Message {
		Producer* producer;					//!< The msg::Producer that created this message
		void* data;							//!< Additional data that the producer appended to this message
		uint64_t id;						//!< An ID number unique to each message instance
		uint32_t type;						//!< Identifies which type of message this is
		uint16_t sub_type;					//!< Indentifies a message subtype
		struct {
			uint16_t cleanup_flag : 1;		//!< Set to true if the producer should cleanup data after all consumers have acknowledged the message
			uint16_t unused_flags : 15;		//!< Memory reserved for future use
		};

		Message() :
			producer(nullptr),
			data(nullptr),
			id(0u),
			type(MSG_NULL),
			cleanup_flag(0u),
			unused_flags(0u)
		{}
	};

	class Queue {
	private:
		std::atomic_uint32_t _recursion_counter;
#ifdef ANVIL_LUTILS_REUSE_MESSAGE_IDS
		IDGeneratorSelector<uint64_t, true, false, true> _id_generator;
#else
		IDGeneratorSelector<std::atomic_uint64_t, false, false, false> _id_generator;
#endif
		std::recursive_mutex _consumer_mutex;
		std::recursive_mutex _message_mutex;
		PODVectorDynamic<Consumer*> _consumers;
		PODVectorDynamic<Message> _messages;
		const bool _in_order;

		void Add(CommonBase&);
		void Remove(CommonBase&);
		void ProduceImplementation(Message* const msgs, const size_t count);
		void Initialise(Producer& producer, Message* const msgs, const size_t count);
		void Produce(Message* const msgs, const size_t count, const bool blocking);
	public:
		friend CommonBase;
		friend Consumer;
		friend Producer;

		Queue(const bool in_order = true);
		~Queue();

		size_t Flush();
	};

	class CommonBase {
	protected:
		Queue& _queue;
	public:
		CommonBase(Queue&);
		virtual ~CommonBase();

		inline Queue& GetQueue() const throw() {
			return _queue;
		}
	};

	class Producer : public CommonBase {
	protected:
		virtual void Cleanup(Message&) = 0;

		void Produce(Message* const msgs, const size_t count, const bool blocking = true) {
			_queue.Initialise(*this, msgs, count);
			_queue.Produce(msgs, count, blocking);
		}
	public:
		friend Queue;
		Producer(Queue&);
		virtual ~Producer();
	};

	class Consumer : public CommonBase {
	protected:
		virtual void Consume(const Message* const msgs, const size_t count) = 0;
	public:
		friend Queue;
		Consumer(Queue&);
		virtual ~Consumer();
	};
}}}

#endif
