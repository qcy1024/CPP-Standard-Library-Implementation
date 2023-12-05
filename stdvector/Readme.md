# std::vector的实现

std::vector 并不是线程安全的。Vec.h 尝试实现C++标准库std::vector，并且最终目标尝试将其实现为线程安全的容器。

私有成员及已实现接口如下：

```c++
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
```



## 私有成员

私有成员包括用于实现Vec的一些数据成员以及一些用于实现类接口的私有函数。模板参数T为容器存放的元素类型；三个T*类型的私有数据成员element, firstFree, cap分别指向已分配的第一个元素的指针、第一个可分配的空间指针以及预留空间最后一个位置的下一个位置的指针。



## 已实现接口

1.构造函数与拷贝控制

1）Vec();
构造函数，构造一个空容器。

2）Vec(const Vec& _vec);
拷贝构造。

3）Vec& operator = (const Vec& _vec);
拷贝复制运算符，能正确地处理自赋值。

4）Vec(Vec&& _vec);
移动构造，保证移后源有效，可以对移后源进行赋值或销毁操作。

5）Vec& operator = (Vec&& _vec);
移动赋值运算符，移后源有效。

6）~Vec();
析构函数。析构时销毁所有容器中的元素，并释放分配的所有内存空间。

7）void resize(size_type n);
与标准库功能相同。接受一个表示大小的参数n，将容器中元素的数量置为n。若原来容器中的元素不足n，会在末尾补足元素，补充的元素采用默认构造函数；若原来容器中的元素超过了n，在末尾将元素截断。

8）void resize(size_type n,const T& value);
与标准库功能相同。还接受一个const T& 的value值，若原来容器中的元素不足n，在补足时，将补充的元素值初始化为value。

9）void push_back(const T& _data);
与标准库功能相同。在容器末尾加入一个元素，初始化为_data。

10）void clear();
与标准库功能相同。清空容器。

11）T& operator [] (size_type i);
与标准库功能相同。返回下标为i的元素的引用。

12）T* begin();
与标准库功能相同。返回指向首元素的迭代器。（迭代器暂时不知道怎么实现的，暂时就假设迭代器类型就是T*）。

13）T* end();
与标准库功能相同。返回指向尾元素的下一个位置的迭代器。

14）size_type size();
与标准库功能相同。返回容器中已有的元素数量。

15）size_type capacity();
与标准库功能相同。返回容器目前已经分配了的内存的总容量(一共能容纳多少的T类型的元素)。

16）bool empty();
与标准库功能相同。若容器为空，返回true; 否则返回false。

17）T& front();
与标准库功能相同。返回容器首元素的引用。

18）T& back();
与标准库功能相同。返回容器尾元素的引用。

