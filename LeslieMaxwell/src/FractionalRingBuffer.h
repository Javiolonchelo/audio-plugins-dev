// dBob Studio 2025
// Javier Rodrigo López
// javiolonchelo@gmail.com

#ifndef FRACTIONALRINGBUFFER_H
#define FRACTIONALRINGBUFFER_H
#include <memory>
#include <stdexcept>

template <typename T>
class FractionalRingBuffer
{
   public:
    FractionalRingBuffer()  = default;
    ~FractionalRingBuffer() = default;

    // method to prepare the buffer
    void prepare(const int size)
    {
        if (size <= 0) { throw std::invalid_argument("The size of the ring buffer must be greater than 0."); }

        buffer.reset(new T[size]);
        bufferSize = size;
        index      = 0;
        clear();
    }

    void push(const T& value)
    {
        if (buffer == nullptr) { throw std::runtime_error("Buffer not initialized. Call prepare() first."); }

        buffer[index] = value;
        index         = (index + 1) % bufferSize;
    }

    // T get(const int p)
    // {
    //     if (buffer == nullptr) { throw std::runtime_error("Buffer not initialized. Call prepare() first."); }
    //     if (p <= 0 || p >= bufferSize) { throw std::out_of_range("Position out of range."); }
    //
    //     const int idx = (index - p + bufferSize) % bufferSize;
    //     return buffer[idx];
    // }

    T get(const float p)
    {
        if (buffer == nullptr) { throw std::runtime_error("Buffer not initialized. Call prepare() first."); }
        if (p < 0.0f || p >= static_cast<float>(bufferSize)) { throw std::out_of_range("Position out of range."); }

        // Get the integer and fractional parts
        const int   intPart  = static_cast<int>(p);
        const float fracPart = p - static_cast<float>(intPart);

        // Linear interpolation between the two nearest samples
        const int idx1 = (index - intPart - 1 + bufferSize) % bufferSize;
        const int idx2 = (index - intPart - 2 + bufferSize) % bufferSize;
        return std::lerp(buffer[idx1], buffer[idx2], fracPart);
    }

    void clear()
    {
        if (buffer == nullptr) { throw std::runtime_error("Buffer not initialized. Call prepare() first."); }

        for (int i = 0; i < bufferSize; ++i) { buffer[i] = T(); }
        index = 0;
    }

   private:
    std::unique_ptr<T[]> buffer;

    int bufferSize = 0;
    int index      = 0;
};
#endif  // FRACTIONALRINGBUFFER_H
