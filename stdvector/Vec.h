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

	//拷贝构造与拷贝赋值运算符
	Vec(const Vec& _vec);
	Vec& operator = (const Vec& _vec);
	//移动构造和移动赋值运算符
	Vec(Vec&& _vec);
	Vec& operator = (Vec&& _vec);


	void push_back(const T& _data);
	void clear();
	T& operator [] (int i) { return elements[i]; };
	

	size_t size() const { return firstFree - elements; };
	size_t capacity() const { return cap - elements; };
	

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

template<typename T> 
Vec<T>::Vec(const Vec& _vec)
{
	elements = alloc.allocate(_vec.capacity());
	firstFree = std::uninitialized_copy(_vec.elements, _vec.firstFree, elements);
	cap = elements + _vec.capacity();
	
}

template<typename T>
Vec<T>& Vec<T>::operator = (const Vec& _vec)
{
	size_t newCapacity = _vec.capacity();
	T* newElements = alloc.allocate(newCapacity);
	T* newFirstFree = std::uninitialized_copy(_vec.elements, _vec.firstFree, newElements);
	T* newCap = newElements + newCapacity;
	Free();
	elements = newElements;
	firstFree = newFirstFree;
	cap = newCap;
	return *this;
}

template <typename T>
Vec<T>::Vec(Vec&& _vec) : elements(_vec.elements),firstFree(_vec.firstFree),cap(_vec.cap)
{
	std::cout << "move constructor" << std::endl;
	//保证移后源是有效的状态
	_vec.elements = _vec.firstFree = _vec.cap = nullptr;
}

template <typename T>
Vec<T>& Vec<T>::operator = (Vec&& _vec)
{
	std::cout << "move operator = " << std::endl;
	T* newElements = _vec.elements;
	T* newFirstFree = _vec.firstFree;
	T* newCap = _vec.cap;
	//别忘了释放 = 运算符左边对象的资源
	Free();
	_vec.elements = _vec.firstFree = _vec.cap = nullptr;
	elements = newElements;
	firstFree = newFirstFree;
	cap = newCap;
	return *this;
}

template <typename T>
void Vec<T>::push_back(const T& _data)
{
	check();
	alloc.construct(firstFree++, _data);
}

template <typename T>
void Vec<T>::clear()
{
	Free();
	elements = firstFree = cap = nullptr;
}

template <typename T>
void Vec<T>::reallocate()
{
	size_t newCapacity = size() ? size() * 2 : 1;
	T* newElements = alloc.allocate(newCapacity);
	T* newFirstFree = newElements;
	for (T* it = elements; it != firstFree; ++it)
	{
		alloc.construct(newFirstFree++, std::move(*it));
	}
	//移动后马上销毁移后源
	Free();
	T* newCap = newElements + newCapacity;
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