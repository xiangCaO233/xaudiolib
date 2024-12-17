#ifndef X_RINGBUFFER_H
#define X_RINGBUFFER_H

#include <memory>

class ringbuffer {
    // 缓冲区本体
    std::shared_ptr<uint32_t> buffer;
    // 读取缓冲区
    std::shared_ptr<uint32_t> readbuffer;
    // 缓冲区大小（包含1个空闲单元）
    size_t buffersize;
    // 读取指针
    size_t readpos;
    // 写入指针
    size_t writepos;

   public:
    ringbuffer(size_t size);
    ~ringbuffer() = default;

    // 可读数据量
    size_t readable() const;
    // 可写入容量
    size_t available() const;
    // 写数据
    bool write(const uint32_t* data, size_t size);
    // 读数据
    bool read(uint32_t*& data, size_t size);
};

#endif  // X_RINGBUFFER_H
