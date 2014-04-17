//
//  ringbuffer.h
//  mhmm
//
//  Created by Jules Francoise on 10/04/2014.
//
//

#ifndef mhmm_ringbuffer_h
#define mhmm_ringbuffer_h

#include <vector>
#include <exception>
#include <stdexcept>

using namespace std;

/**
 * @ingroup Utility
 * @class RingBuffer
 * @brief Simple RingBuffer Class
 * @details Multichannel
 * @tparam T Data type
 * @tparam channels number of channels
 * @todo remove template parameters?
 */
template <typename T, int channels>
class RingBuffer {
public:
    /**
     * @brief Constructor
     * @param length length of the ringbuffer
     */
	RingBuffer(unsigned int length = 1)
    {
        length_ = length;
        for (int c=0; c<channels; c++) {
            data[c].resize(length_);
        }
        current_index_ = 0;
        full_ = false;
    }
    
    /**
     * @brief Destructor
     */
	~RingBuffer()
    {
        for (int c=0; c<channels; c++) {
            data[c].clear();
        }
    }
    
    /**
     * @brief Access data by index & channel
     * @return value at the given index and channel
     */
    T operator()(unsigned int channel, unsigned int index) const
    {
        if (channel >= channels)
            throw out_of_range("Ringbuffer: channel out of bounds");
        unsigned int m = full_ ? length_ : current_index_;
        if (index >= m)
            throw out_of_range("Ringbuffer: index out of bounds");
        return data[channel][index];
    }
	
    /**
     * @brief Clear the content of the buffer
     */
	void clear()
    {
        current_index_ = 0;
        full_ = false;
    }
    
    /**
     * @brief Add an element to the buffer (single-channel method)
     * @param value element to add to the buffer
     * @throws invalid_argument if the buffer is multi-channel
     */
    void push(T const value)
    {
        if (channels > 1)
            throw invalid_argument("You must pass a vector or array");
        data[0][current_index_] = value;
        current_index_++;
        if (current_index_ == length_)
            full_ = true;
        current_index_ %= length_;
    }
    
    /**
     * @brief Add an element to the buffer (multi-channel method)
     * @param value element to add to the buffer
     */
	void push(T const *value)
    {
        for (int c=0; c<channels; c++)
        {
            data[c][current_index_] = value[c];
        }
        current_index_++;
        if (current_index_ == length_)
            full_ = true;
        index %= length_;
    }
    
    /**
     * @brief Add an element to the buffer (multi-channel method)
     * @param value element to add to the buffer
     */
    void push(vector<T> const &value)
    {
        for (int c=0; c<channels; c++)
        {
            data[c][current_index_] = value[c];
        }
        current_index_++;
        if (current_index_ == length_)
            full_ = true;
        current_index_ %= length_;
    }
    
    /**
     * @brief Get the size of the ringbuffer
     * @return size of the ringbuffer (length)
     */
	unsigned int size() const
    {
        return length_;
    }
    
    /**
     * @brief Get the actual size of the ringbuffer (< size() if the buffer is not full)
     * @return actual size of the ringbuffer (length)
     */
    unsigned int size_t() const
    {
        return (full_ ? length_ : current_index_);
    }
    
    /**
     * @brief Resize the buffer to a specific length
     * @param length target length of the ringbuffer
     */
	void resize(unsigned int length)
    {
        if (length == length_) return;
        if (length > length_) {
            full_ = false;
        } else if (current_index_ >= length) {
            full_ = true;
            current_index_ = 0;
        }
        length_ = length;
        for (int c=0; c<channels; c++) {
            data[c].resize(length_);
        }
    }
    
    /**
     * @brief Compute the mean of the buffer
     * @return vector containing the mean of the buffer
     */
    vector<T> mean() const
    {
        vector<T> _mean(channels, 0.0);
        int size = full_ ? length_ : current_index_;
        for (int c=0; c<channels; c++)
        {
            for (int i=0; i<size; i++) {
                _mean[c] += data[c][i];
            }
            _mean[c] /= T(size);
        }
        return _mean;
    }
	
protected:
    /**
     * @brief buffer data
     */
    vector<T> data[channels];

    /**
     * @brief length of the buffer
     */
	unsigned int length_;

    /**
     * @brief current index in the buffer
     */
	unsigned int current_index_;

    /**
     * Defines if the ringbuffer is already full
     */
	bool full_;
};


#endif