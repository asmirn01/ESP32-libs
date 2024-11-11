#pragma once
#include "CircularBuffer.hpp"

template <typename T, uint16_t BUFFER_SIZE> class StatAverager {
public:
	T getAverageValue() const {
		if (_buffer.size() == 0) {
			return T();
		}
		T average;
		for (buffer_index_t i = 0; i < _buffer.size(); i++) {
			average += _buffer[i];
		}
		average /= _buffer.size();
		return average;
	}

	bool consume(T value) {
		_buffer.push(value);
		return _buffer.isFull();
	}

	bool isBufferFull() const { return _buffer.isFull(); }
	bool isBufferEmpty() const { return _buffer.isEmpty(); }

private:
	CircularBuffer<T, BUFFER_SIZE> _buffer;
	using buffer_index_t = typename decltype(_buffer)::index_t;
};