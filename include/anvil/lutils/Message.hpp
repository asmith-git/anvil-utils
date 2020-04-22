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

namespace anvil { namespace msg {

	class Producer;
	class Consumer;
	class CommonBase;
	struct Message;

	class Queue {
	private:
		uint64_t _base_id;
		std::recursive_mutex _mutex;
		std::vector<Consumer*> _consumers;

		void Add(CommonBase&);
		void Remove(CommonBase&);
		void Produce(Message&);
	public:
		friend CommonBase;
		friend Consumer;
		friend Producer;

		Queue();
		~Queue();
	};

	enum : uint32_t {
		MSG_NULL = 0u						//!< Default message type
	};

	struct Message {
		Producer* producer;					//!< The msg::Producer that created this message
		void* data;							//!< Additional data that the producer appended to this message
		uint64_t id;						//!< An ID number unique to each message instance
		uint32_t type;						//!< Identifies which type of message this is
		struct {
			uint32_t cleanup_flag : 1;		//!< Set to true if the producer should cleanup data after all consumers have acknowledged the message
			uint32_t unused_flags : 31;		//!< Memory reserved for future use
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

		inline void Produce(Message& message) {
			message.producer = this;
			_queue.Produce(message);
		}
	public:
		friend Queue;
		Producer(Queue&);
		virtual ~Producer();
	};

	class Consumer : public CommonBase {
	protected:
		virtual void Consume(const Message&) = 0;
	public:
		friend Queue;
		Consumer(Queue&);
		virtual ~Consumer();
	};
}}

#endif
