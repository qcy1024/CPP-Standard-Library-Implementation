#ifndef __VEC__H
#define __VEC__H

#include <memory>


template <typename T>
class Vec
{
public:

	/*�ӿ�*/
	//cap - elementsʱ������������nullptr�����ֵ��0.
	Vec() : elements(nullptr), firstFree(nullptr), cap(nullptr) {};


	void push_back(const T& _data);
	T& operator [] (int i) { return elements[i]; };
	

	size_t size() { return firstFree - elements; };
	size_t capacity() { return cap - elements; };
	

//private
public:
	std::allocator<T> alloc;
	//ʹ������T*��Ա��elements��¼Vec��һ��Ԫ�ص�ָ�룻
	T* elements;
	//firstFree��¼��һ���ɷ���ռ��ָ�룬���ռ�������firstFree==cap
	T* firstFree;
	//��¼Ԥ���ռ����һ��λ�õ���һ��λ�õ�ָ�롣
	T* cap;

	/*���ߺ���*/
	//����ʱVec�������ռ�����ʱ�����·������Ŀռ�
	void reallocate();
	//����ʱVec�������ռ��Ƿ����������������͵���reallocate()
	void check();
	//���ٶ����ͷſռ�
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
	//ע������if���б�Ҫ�ģ����elements��û�б����죬destroy(nullptr)�ᵼ��δ�������Ϊ��
	//���캯���н�elements��ʼ��Ϊnullptr����һ��push_back��ʱ��һ���ᵼ�¶�nullptr��destroy��Ϊ��
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
