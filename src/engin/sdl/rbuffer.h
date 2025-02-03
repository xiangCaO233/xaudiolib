#ifndef X_RINGBUFFER_H
#define X_RINGBUFFER_H

#include <memory>

class ringbuffer {
  // 缓冲区本体
  std::shared_ptr<float[]> buffer;
  // 读取缓冲区
  std::shared_ptr<float[]> readbuffer;
  // 缓冲区大小（包含1个空闲单元）
  size_t buffersize;
  // 读取指针
  size_t readpos;
  // 写入指针
  size_t writepos;

 public:
  explicit ringbuffer(size_t size);
  ~ringbuffer();

  // 可读数据量
  [[nodiscard]] size_t readable() const;
  // 可写入容量
  [[nodiscard]] size_t available() const;
  // 写数据
  bool write(const float* data, size_t size);
  bool write(float value, size_t size);
  // 读数据
  bool read(float*& data, size_t size);
};

#endif  // X_RINGBUFFER_H
