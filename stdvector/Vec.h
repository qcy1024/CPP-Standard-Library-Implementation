#ifndef __VEC__H
#define __VEC__H

#include <memory>


template <typename T>
class Vec
{
public:
	//Vec<T>的迭代器类型暂时不知道怎么实现的，先假设就是T*
	typedef T* iterator;
	//标准库std::vector<T>::size_type暂时不知道怎么实现的，先假设为size_t
	typedef typename size_t size_type;

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
	T& front() const { return *elements; };
	T& back() const { return *(elements + size() - 1); };


	T* begin() const { return elements; };
	T* end() const { return firstFree; };
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
	//销毁对象并释放空间.Free()会析构size()大小的所有对象，以及回收已经分配的capacity()大小的内存空间。
	void Free();
};

//拷贝构造函数
template<typename T> 
Vec<T>::Vec(const Vec& _vec)
{
	elements = alloc.allocate(_vec.capacity());
	firstFree = std::uninitialized_copy(_vec.elements, _vec.firstFree, elements);
	cap = elements + _vec.capacity();
	
}

//拷贝复制运算符
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

//移动构造函数
template <typename T>
Vec<T>::Vec(Vec&& _vec) : elements(_vec.elements),firstFree(_vec.firstFree),cap(_vec.cap)
{
	std::cout << "move constructor" << std::endl;
	//保证移后源是有效的状态
	_vec.elements = _vec.firstFree = _vec.cap = nullptr;
}

//移动赋值运算符
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
bool operator == (const Vec<T>& lhs, const Vec<T>& rhs)
{
	if (lhs.size() != rhs.size())
		return false;
	T* it1 = lhs.begin();
	T* it2 = rhs.begin();
	T* itEnd1 = lhs.end();
	T* itEnd2 = rhs.end();
	while (it1 != itEnd1 && it2 != itEnd2 && *it1 == *it2)
	{
		it1++;
		it2++;
	}
	if (it1 == itEnd1 && it2 == itEnd2) return true;
	return false;
	//下面注释掉的方法要求T类型定义了!=运算符
 	//for (; it1 != lhs.end() && it2 != rhs.end();  ++it1, ++it2)
	//{
	//	if (*it1 != *it2) return false;
	//}
}

template <typename T> 
bool operator != (const Vec<T>& lhs, const Vec<T>& rhs)
{
	//用==运算符实现!=运算符
	return !(lhs == rhs);
}


/*以下是非接口的类工具函数*/
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