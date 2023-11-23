#ifndef __VEC__H
#define __VEC__H

#include <memory>


template <typename T>
class Vec
{
public:

	/*接口*/
	//cap - elements时，若两个都是nullptr，结果值是0.
	Vec() : elements(nullptr), firstFree(nullptr), cap(nullptr) {};


	void push_back(const T& _data);
	T& operator [] (int i) { return elements[i]; };
	

	size_t size() { return firstFree - elements; };
	size_t capacity() { return cap - elements; };
	

//private
public:
	std::allocator<T> alloc;
	//使用三个T*成员，elements记录Vec第一个元素的指针；
	T* elements;
	//firstFree记录第一个可分配空间的指针，若空间已满，firstFree==cap
	T* firstFree;
	//记录预留空间最后一个位置的下一个位置的指针。
	T* cap;

	/*工具函数*/
	//当此时Vec的连续空间已满时，重新分配更大的空间
	void reallocate();
	//检查此时Vec的连续空间是否已满，若已满，就调用reallocate()
	void check();
	//销毁对象并释放空间
	void Free();
};

template <typename T>
void Vec<T>::push_back(const T& _data)
{
	check();
	alloc.construct(firstFree++, _data);
}

template <typename T>
void Vec<T>::reallocate()
{
	size_t newCapacity = size() ? size() * 2 : 1;
	T* newElements = alloc.allocate(newCapacity);
	T* newFirstFree = std::uninitialized_copy(elements, firstFree, newElements);
	T* newCap = newElements + newCapacity;
	Free();
	elements = newElements;
	firstFree = newFirstFree;
	cap = newCap;
}

template <typename T>
void Vec<T>::Free()
{
	//注意这里if是有必要的，如果elements还没有被构造，destroy(nullptr)会导致未定义的行为。
	//构造函数中将elements初始化为nullptr，第一次push_back的时候一定会导致对nullptr的destroy行为。
	if (elements == nullptr) return;
	T* p = firstFree;
	while (p != elements)
		alloc.destroy(--p);
	alloc.deallocate(elements, capacity());
	return;
}

template <typename T>
void Vec<T>::check()
{
	if (firstFree == cap)
		reallocate();
}

#endif // !__VEC__H
