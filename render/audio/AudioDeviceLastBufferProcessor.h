#pragma once
#include "AudioDeviceBufferProcessor.h"

class AudioDeviceLastBufferProcessor : public AudioDeviceBufferProcessor
{
private:
    virtual void clear_buffer() override;
protected:
    uint8_t * mpTargetBuffer;
    uint32_t mTargetBufferSize;
    uint32_t mTargetBufferValidSize;

    uint8_t * mpCacheBuffer;
    uint32_t mCacheBufferSize;
    uint32_t mCacheReadedSize;

    void add_cache_read_size(uint32_t size);

    void set_cache_buffer(uint8_t * buff, uint32_t size);

    bool is_cache_buffer_empty();

    virtual uint32_t get_cache_buffer_size() override;

    uint8_t * get_cache_buffer();

public:
    AudioDeviceLastBufferProcessor(/* args */);
    ~AudioDeviceLastBufferProcessor() = default;

    void set_target_buffer(uint8_t * buffer, uint32_t size);

    virtual bool fill_buffer_by_cache(int volume) = 0;

    bool is_target_buffer_full();
private:
    static const int CACHE_INCREACE_SIZE;
    uint32_t mCacheArrayLen;
};
