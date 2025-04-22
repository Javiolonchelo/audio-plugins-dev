// dBob Studio 2025
// Javier Rodrigo López
// javiolonchelo@gmail.com

#ifndef FRACTIONALRINGBUFFER_H
#define FRACTIONALRINGBUFFER_H
#include <cassert>
#include <memory>

/**
 *
 * @tparam T Type of the samples stored in the buffer.
 */
template <typename T>
class FractionalRingBuffer
{
   public:
    FractionalRingBuffer()  = default;
    ~FractionalRingBuffer() = default;

    /**
     * Prepares the buffer for use. This sets the size of the buffer, clears it, and initializes the index of the write pointer.
     * @param size Size of the buffer in samples.
     */
    void prepare(const int size)
    {
        assert(size > 0);

        buffer.reset(new T[size]);
        bufferSize = size;
        index      = 0;
        clear();
    }

    /**
     * Inserts a value into the buffer at the current position and moves the write pointer forward one position.
     * @param value Value to be inserted.
     */
    void push(const T& value)
    {
        assert(buffer != nullptr);

        buffer[index] = value;
        index         = (index + 1) % bufferSize;
    }

    /**
     * Retrieves a value from the buffer at a specified position.
     * This emulates the behavior of a delay line with the following transfer function: \f[ H(z) = z^{-p} \f]
     *
     * @param pos Position relative to the current index.
     * @return Value at the specified position.
     */
    T get(const int pos)
    {
        assert(buffer != nullptr);
        assert(pos > 0 && pos < bufferSize);

        const int idx = (index - pos + bufferSize) % bufferSize;
        return buffer[idx];
    }

    /**
     * Retrieves a value which is a result of linear interpolation between two samples in the buffer. The decimal part of the position is used to determine
     * the interpolation weight.
     * This emulates the behavior of a fractional delay line with the following transfer function: \f[ H(z) = z^{-\left(M + \text{frac} \right)} \f]
     * @param pos Fractional position relative to the current index.
     * @return Linear interpolated value at the specified position.
     * @see std::lerp
     */
    T get(const float pos)
    {
        assert(buffer != nullptr);
        assert(pos >= 0.0f && pos < static_cast<float>(bufferSize));

        // Get the integer and fractional parts
        const int   intPart  = static_cast<int>(pos);
        const float fracPart = pos - static_cast<float>(intPart);

        // Linear interpolation between the two nearest samples
        const int idx1 = (index - intPart + bufferSize) % bufferSize;
        const int idx2 = (index - intPart - 1 + bufferSize) % bufferSize;
        return std::lerp(buffer[idx1], buffer[idx2], fracPart);
    }

    /**
     * Get the size of the buffer.
     * @return Size of the buffer.
     */
    [[nodiscard]] int getSize() const { return bufferSize; }

    /**
     * Clears the buffer by setting all values to their default state.
     */
    void clear()
    {
        assert(buffer != nullptr);
        for (int i = 0; i < bufferSize; ++i) { buffer[i] = T(); }
    }

   private:
    std::unique_ptr<T[]> buffer;

    int bufferSize = 0;
    int index      = 0;
};
#endif  // FRACTIONALRINGBUFFER_H
