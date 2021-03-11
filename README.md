# Dependencies
* [anvil-utils](https://github.com/asmith-git/anvil-utils) (If ANVIL_DISABLE_LUTILS is not defined)

# Usage Example
```cpp
#include "anvil/byte-pipe/BytePipe.hpp"

// This is the example data structure that we want to serialise with byte pipes
struct Ball {
  // Serialisable variables
	float x;
	float y;
	uint8_t colour[4u];
	float radius;

  // Define a globaly unique POD ID number for POD support
	enum { POD_ID = 1000 };

	// Define locally unique component ID numbers for object support
	enum : uint16_t {
		COMPONENT_X,
		COMPONENT_Y,
		COMPONENT_RADIUS,
		COMPONENT_COLOUR
	};

};
```
## Serialisation
```cpp
// Using the basic serialisation functions
void SerialiseMethod1(const Ball& ball, anvil::BytePipe::Parser& serialiser) {
	// Ball is serialised as an object
	serialiser.OnObjectBegin(4u);

	// Write the X position
	serialiser.OnComponentID(Ball::COMPONENT_X);
	serialiser.OnPrimativeF32(ball.x);

	// Write the Y position
	serialiser.OnComponentID(Ball::COMPONENT_Y);
	serialiser.OnPrimativeF32(ball.y);

	// Write the radius
	serialiser.OnComponentID(Ball::COMPONENT_RADIUS);
	serialiser.OnPrimativeF32(ball.radius);

	// Write the colour as an array
	serialiser.OnComponentID(Ball::COMPONENT_COLOUR);
	serialiser.OnArrayBegin(4u);
	serialiser.OnPrimativeU8(ball.colour[0u]);
	serialiser.OnPrimativeU8(ball.colour[1u]);
	serialiser.OnPrimativeU8(ball.colour[2u]);
	serialiser.OnPrimativeU8(ball.colour[3u]);
	serialiser.OnArrayEnd();

	// End of serialisation
	serialiser.OnObjectEnd();
}

// The serialisation code can be simplified using the helper functions
void SerialiseMethod2(const Ball& ball, anvil::BytePipe::Parser& serialiser) {
	serialiser.OnObjectBegin(4u);
	serialiser.OnPrimative(Ball::COMPONENT_X, ball.x);
	serialiser.OnPrimative(Ball::COMPONENT_Y, ball.y);
	serialiser.OnPrimative(Ball::COMPONENT_RADIUS, ball.radius);
	serialiser.OnPrimativeArray(Ball::COMPONENT_COLOUR, ball.colour, 4u);
	serialiser.OnObjectEnd();
}

// You could also serial Ball as a POD structure.
// This is faster, but will break if you change the definition of any of the variables
void SerialiseMethod3(const Ball& ball, anvil::BytePipe::Parser& serialiser) {
	serialiser.OnUserPOD(Ball::POD_ID, sizeof(Ball), &ball);
}
```
## Deserialisation
```cpp
// Interfacing directly with the deserialiser interface is a bit more complicated
// but this is how an fast implementation of Ball's deserialiser would look like.
class BallParser final : public anvil::BytePipe::Parser {
private:
	Ball _ball;				//!< The ball struture that is deserialised.
	void* _next_value;		//!< The next variable in _ball that will be set.
	uint32_t _array_index;	//!< The index of the next variable in Ball::colour

	// Copying this object is not allowed
	BallDeserialiser(BallDeserialiser&&) = delete;
	BallDeserialiser(const BallDeserialiser&) = delete;

	// Check that nobody has changed the definitions of variables while we weren't looking
	static_assert(std::is_same<decltype(Ball::x), float>::value, "Expected Ball::x to be a float");
	static_assert(std::is_same<decltype(Ball::y), float>::value, "Expected Ball::y to be a float");
	static_assert(std::is_same<decltype(Ball::radius), float>::value, "Expected Ball::radius to be a float");
	enum { COLOUR_ARRAY_SIZE = sizeof(Ball::colour) };
	static_assert(std::is_same<uint8_t[COLOUR_ARRAY_SIZE], decltype(Ball::colour)>::value, "Expected Ball::colour to be an array of unsigned bytes");
public:
	BallParser() :
		_array_index(0u),
		_next_value(nullptr)
	{}

	~BallParser() {

	}

	Ball Get() {
		return _ball;
	}

	// Inherited from Parser

	void OnPipeOpen() final {
		// Reset the parse state
		_array_index = 0u;
		_next_value = nullptr;
	}

	void OnPipeClose() final {
		// Do nothing
	}

	void OnPrimativeU8(const uint8_t value) final {
		// Act like the value was a float
		OnPrimativeF32(value);
	}

	void OnPrimativeU16(const uint16_t value) final {
		// Act like the value was a float
		OnPrimativeF32(value);
	}

	void OnPrimativeU32(const uint32_t value) final {
		// Act like the value was a float
		OnPrimativeF32(value);
	}

	void OnPrimativeU64(const uint64_t value) final {
		// Act like the value was a float
		OnPrimativeF32(value);
	}

	void OnPrimativeS8(const int8_t value) final {
		// Act like the value was a float
		OnPrimativeF32(value);
	}

	void OnPrimativeS16(const int16_t value) final {
		// Act like the value was a float
		OnPrimativeF32(value);
	}

	void OnPrimativeS32(const int32_t value) final {
		// Act like the value was a float
		OnPrimativeF32(value);
	}

	void OnPrimativeS64(const int64_t value) final {
		// Act like the value was a float
		OnPrimativeF32(value);
	}

	void OnPrimativeF32(const float value) final {
		// Check which variable is being updated
		if (_next_value == nullptr) {
			throw std::runtime_error("BallDeserialiser::OnPrimativeF32 : No name was set");
		} else if (_next_value == _ball.colour) {
			// Write the value into the correct array index
			_ball.colour[_array_index++] = static_cast<uint8_t>(value);

			// If there hasn't been 4 elements set then don't reset the next variable pointer
			if (_array_index < COLOUR_ARRAY_SIZE) return;
		} else {
			// We can directly write the value into these variables because they are the same type
			*static_cast<float*>(_next_value) = value;
		}

		// The next value must be specified by the caller
		_next_value = nullptr;
	}

	void OnPrimativeF64(const double value) final {
		// Act like the value was a float
		OnPrimativeF32(value);
	}

	void OnPrimativeBool(const bool value) final {
		// Act like the value was a float
		OnPrimativeF32(value);
	}

	void OnPrimativeC8(const char value) final {
		throw std::runtime_error("BallDeserialiser::OnPrimativeC8 : Ball doesn't have any values that are strings");
	}

	void OnPrimativeString(const char* value, const uint32_t length) final {
		throw std::runtime_error("BallDeserialiser::OnPrimativeString : Ball doesn't have any values that are strings");
	}

	void OnNull() final {
		// Act like 0 was set
		OnPrimativeF32(0.f);
	}

	void OnArrayBegin(const size_t size) final {
		if(size != 4u) throw std::runtime_error("BallDeserialiser::OnArrayBegin : Less colour values than expected");

		// Reset the index counter
		_array_index = 0u;
	}

	void OnArrayEnd() final {
		if (_array_index != COLOUR_ARRAY_SIZE) throw std::runtime_error("BallDeserialiser::EndArray : Less colour values than expected");
	}

	void OnObjectBegin(const size_t size) final {
		throw std::runtime_error("BallDeserialiser::OnObjectBegin : Ball doesn't have any values that are objects");
	}

	void OnObjectEnd() final {
		throw std::runtime_error("BallDeserialiser::OnObjectEnd : Ball doesn't have any values that are objects");
	}

	void OnComponentID(const uint16_t component_id) final {	
		// Because ball is a simple POD structue we will only
		// store a pointer to the next variable that will be set

		switch (component_id) {
		case Ball::COMPONENT_X:
			// Point to the X position
			_next_value = &_ball.x;
			break;
		case Ball::COMPONENT_Y:
			// Point to the Y position
			_next_value = &_ball.y;
			break;
		case Ball::COMPONENT_RADIUS:
			// Point to the radius
			_next_value = &_ball.radius;
			break;
		case Ball::COMPONENT_COLOUR:
			// Point to the first index of the RGBA colour array
			_next_value = _ball.colour;
			break;
		default:
			// Ball doesn't have any other values
			throw std::runtime_error("BallDeserialiser::OnComponentID : Invalid component ID '" + std::to_string(component_id) + "'");
		}
	}


	void OnUserPOD(const uint32_t type, const uint32_t bytes, const void* data) final {
		// Check that the POD is Ball
		if(type != Ball::POD_ID) throw std::runtime_error("BallDeserialiser::OnUserPOD : Invalid POD ID '" + std::to_string(type) + "'");

		// Check that the memory layout matches what we expect a Ball to look like
		if (bytes != sizeof(Ball)) throw std::runtime_error("BallDeserialiser::OnUserPOD : Ball is wrong size '" + std::to_string(bytes) + "'");

		// Copy the ball
		_ball = *static_cast<const Ball*>(data);
	}

};

// The objects header provides a DOM (document object model) style interface that can
// simplify deserialisation but introduces overheads due to requiring all memory being 
// copied and converted into objects
Ball DeserialiseMethod2(anvil::BytePipe::Value& value) {
	if (value.GetType() != anvil::BytePipe::TYPE_OBJECT) throw std::runtime_error("DeserialiseMethod2 : Ball must be an object");

	Ball ball;

	// Read the primative variables
	ball.x = value.GetValue(Ball::COMPONENT_X).GetF32();
	ball.y = value.GetValue(Ball::COMPONENT_Y).GetF32();
	ball.radius = value.GetValue(Ball::COMPONENT_RADIUS).GetF32();

	// Read the colour array
	anvil::BytePipe::Value& colour = value.GetValue(Ball::COMPONENT_COLOUR);
	ball.colour[0u] = colour.GetValue(0u).GetU8();
	ball.colour[1u] = colour.GetValue(1u).GetU8();
	ball.colour[2u] = colour.GetValue(2u).GetU8();
	ball.colour[3u] = colour.GetValue(3u).GetU8();

	return ball;
}
```
