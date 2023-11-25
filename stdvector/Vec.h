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

	//���������뿽����ֵ�����
	Vec(const Vec& _vec);
	Vec& operator = (const Vec& _vec);
	//�ƶ�������ƶ���ֵ�����
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
	//��֤�ƺ�Դ����Ч��״̬
	_vec.elements = _vec.firstFree = _vec.cap = nullptr;
}

template <typename T>
Vec<T>& Vec<T>::operator = (Vec&& _vec)
{
	std::cout << "move operator = " << std::endl;
	T* newElements = _vec.elements;
	T* newFirstFree = _vec.firstFree;
	T* newCap = _vec.cap;
	//�������ͷ� = �������߶������Դ
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
	//�ƶ������������ƺ�Դ
	Free();
	T* newCap = newElements + newCapacity;
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