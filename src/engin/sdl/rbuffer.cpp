#include "rbuffer.h"

ringbuffer::ringbuffer(size_t size)
    : buffersize(size + 1),
      buffer(new uint32_t[size + 1]()),
      readbuffer(new uint32_t[size]),
      readpos(0),
      writepos(0) {}

// 可读数据量
size_t ringbuffer::readable() const {
    if (writepos >= readpos)
        // 顺序
        return writepos - readpos;
    else
        // 环绕
        return (buffersize - readpos) + writepos;
}

// 可写入容量
size_t ringbuffer::available() const {
    // 预留一个单元区分满和空
    return buffersize - readable() - 1;
}

// 写数据
bool ringbuffer::write(const uint32_t* data, size_t size) {
    if (size > available()) {
        return false;
    }

    for (size_t i = 0; i < size; ++i) {
        buffer.get()[writepos] = data[i];
        // 环绕写指针
        writepos = (writepos + 1) % buffersize;
    }
    return true;
}

// 读数据
bool ringbuffer::read(uint32_t*& data, size_t size) {
    if (size > readable()) {
        data = nullptr;
        return false;
    }
    // 拷贝数据到 readbuffer
    for (size_t i = 0; i < size; ++i) {
        readbuffer.get()[i] = buffer.get()[readpos];
        // 环绕读指针
        readpos = (readpos + 1) % buffersize;
    }
    // 返回 readbuffer 的指针
    data = readbuffer.get();
    return true;
}