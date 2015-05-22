/***************************************
* @file     obj_circularbuffer.h
* @brief    环形缓冲区-支持存储class数据类型(class必须有赋值函数)，只能一个一个读写数据
* @details  出处 https://github.com/afeinberg/circular_buffer
* @author   phata, wqvbjhc@gmail.com
* @date     2014-5-20
****************************************/
#ifndef OBJ_CIRCULAR_BUFFER_H
#define OBJ_CIRCULAR_BUFFER_H

#include <cstdlib>
#include <memory>
template<typename T, class Alloc = std::allocator<T> >
class ObjCircularBuffer
{
public:
    explicit ObjCircularBuffer(size_t capacity,
                               bool overflow = false,
							   const Alloc& alloc = Alloc()):capacity_(capacity),
							   overflow_(overflow),
							   alloc_(alloc),
							   size_(0),
							   head_(0),
							   tail_(0)
	{
		buffer_ = alloc_.allocate(capacity_);
	}

	virtual ~ObjCircularBuffer(){
		clear();
		alloc_.deallocate(buffer_, capacity_);
	}

	size_t size() const{
		return size_;
	}

	size_t capacity() const{
		return capacity_;
	}

	bool empty() const{
		return size_ == 0;
	}

	bool full() const{
		return size_ >= capacity_;
	}

	bool push_back(const T& item){
		bool ret = false;
		if (overflow_ || !full()) {
			alloc_.construct(&buffer_[tail_], item);
			++tail_;
			tail_ %= capacity_;
			++size_;
			ret = true;
		}
		return ret;
	}

	bool front(T& item) const{
		bool ret = false;
		if (!empty()) {
			item = buffer_[head_];
			ret = true;
		}
		return ret;
	}

	bool pop_front(){
		bool ret = false;
		if (!empty()) {
			alloc_.destroy(&buffer_[head_]);
			++head_;
			head_ %= capacity_;
			--size_;
			ret = true;
		}
		return ret;
	}

	void clear(){
		while (!empty()) {
			pop_front();
		}
	}

private:
    size_t capacity_;
    bool overflow_;
    Alloc alloc_;
    size_t size_;
    size_t head_;
    size_t tail_;
    T* buffer_;
private://Noncopyable
    ObjCircularBuffer(const ObjCircularBuffer&);
    const ObjCircularBuffer& operator=(const ObjCircularBuffer&);
};

#endif // OBJ_CIRCULAR_BUFFER_H