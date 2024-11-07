# 语句
1. `auto &word : dictionary`和`auto word : dictionary`
   1. `auto &word : dictionary`
   这是一个引用。word 是 dictionary 中元素的引用。
   修改 word 将直接修改 dictionary 中的元素。
   不会拷贝元素，效率更高，尤其是当元素类型较大时。 
   2. `auto word : dictionary`
   这是一个值。word 是 dictionary 中元素的副本。
   修改 word 不会影响 dictionary 中的元素。
   每次迭代都会拷贝元素，可能导致性能开销，尤其是当元素类型较大时。
   3. `const auto &word : dictionary`
   只读不修改元素，相较于2，不会拷贝内存，也防止了修改
2. `static`: 关键字具有多种用途
   1. 静态局部变量
   局部变量使用 `static` 关键字修饰后，变量在函数第一次调用时被初始化，并在程序的生命周期内保持其值。局部静态变量的作用域仅限于其所在的函数，但其生命周期在整个程序运行期间。
   2. 静态成员变量
   静态成员变量属于类而不是类的某个对象。所有对象共享同一个静态成员变量。它必须在类外进行定义和初始化。
   ```cpp
    class MyClass {
    public:
        static int staticVar; // 静态成员变量声明
    };
    int MyClass::staticVar = 0; // 静态成员变量定义和初始化
   ```
   3. 静态成员函数
   静态成员函数可以在没有对象实例的情况下调用。它们只能访问静态成员变量或其他静态成员函数。
   ```cpp
   class MyClass {
    public:
        static void staticFunction() {
            std::cout << "Static function called" << std::endl;
        }
    };

    int main() {
        MyClass::staticFunction(); // 调用静态成员函数
        return 0;
    }
   ```
   4. 静态全局变量和函数
   在全局范围内，`static` 关键字用于限制变量或函数的作用域，使其仅在定义它的文件中可见。这是实现内部链接的一个方式。


# 1.`<cctype>`
    函数名称   返回值
    isalnum()  如果参数是字母数字，即字母或者数字，函数返回true
    isalpha()  如果参数是字母，函数返回true
    iscntrl()  如果参数是控制字符，函数返回true
    isdigit()  如果参数是数字（0－9），函数返回true
    isgraph()  如果参数是除空格之外的打印字符，函数返回true
    islower()  如果参数是小写字母，函数返回true
    isprint()  如果参数是打印字符（包括空格），函数返回true
    ispunct()  如果参数是标点符号，函数返回true
    isspace()  如果参数是标准空白字符，如空格、换行符、水平或垂直制表符，函数返回true
    isupper()  如果参数是大写字母，函数返回true
    isxdigit() 如果参数是十六进制数字，即0－9、a－f、A－F，函数返回true

    tolower()  如果参数是大写字符，返回其小写，否则返回该参数
    toupper()  如果参数是小写字符，返回其大写，否则返回该参数

# 2.`<string>`
给字符串`s` 添加 n 个字符 `'c'`，有几种不同的方法可以实现
1. 使用 `append` 方法:`s.append(n, 'c')`;
2. 使用 `operator+=`:`s += std::string(n, 'c');`
3. 使用 `insert` 方法: `s.insert(s.end(), n, 'c');`
   `string& insert(int pos ,const char*s);`
   `string& insert(int pos ,const string& str);`
   `string& insert(int pos ,int n ,char c);` //在指定位置插入n个字符c,包含pos
   `string& erase(int pos ,int n=npos);`  //删除从pos开始的n个字符,
4. 使用 `push_back` 方法:
     for (int i = 0; i < n; ++i) {
        s.push_back('c');
    }
5. 截取
   从字符串中截取想要的字串
   `string substr(int pos=0,int n=npos)const; `//返回由pos开始的n个字符串组成的字符串（包含第pos位置的字符）
6. 将对象转换为整数`stoi`
    `std::stoi` 是 C++11 引入的标准库函数，可以将 `std::string` 对象转换为整数。与 `atoi` 不同的是，`std::stoi` 可以处理 C++ 的 `std::string` 类型，并且提供了更好的错误处理机制。
    ```cpp
    std::string str = "211";
    int num = std::stoi(str);//=211
    ```
7. 判断一个字符串是否包含一个字符串
   1. `std::string::find`:返回子字符串在母字符串中的起始位置索引，如果子字符串不存在，则返回 `std::string::npos`。
   ```cpp
   if (str.find(substr) != std::string::npos) {
        std::cout << "The string contains the substring." << std::endl;
    } else {
        std::cout << "The string does not contain the substring." << std::endl;
    }
   ```
   2. `s.find(str,pos)`:是用来寻找从下标pos开始(包括pos处字符)匹配str的位置
   3. `s.find_first_of（str） 和 s.find_last_of（str）`:找到目标字符在字符串中第一次出现和最后一次出现的位置
8. 判断一个字符串是否包含一个字符
   1. `std::string::find` 方法不仅可以用于查找子字符串，还可以用于查找单个字符。如果字符存在于字符串中，`std::string::find` 方法会返回该字符在字符串中的索引位置；否则，它会返回 `std::string::npos`
   2. 使用 `std::count`:`std::count` 是 `<algorithm>` 头文件中的一个函数，用于计算字符在字符串中出现的次数。如果返回值大于 0，表示字符存在于字符串中
   ```cpp
  bool containsChar(const std::string& str, char ch) {
    return std::count(str.begin(), str.end(), ch) > 0;
  }
9. 判断字符是否相等
  string字符串比较
  比较方式： 字符串比较是按字符的ascll码进行对比
  = 返回  0
  > 返回  1
  < 返回 -1
  
  	int compare(const string &s)const;
  	int compare(const char* s)const;
10.  删除字符最后一个字符
    `pop_back()`:str.pop_back()
11.  判断字符是否为空
    `empty()`:str.empty();返回一个bool值，如果长度为0，返回true;反之返回false
12.  清空字符串
    `clear()`:str.clear();  
13. 查看尾元素
    `back()`方法返回对字符串中最后一个字符的引用。需要注意的是，在调用`back()`方法之前，应该确保字符串不为空，否则会引发未定义行为。
    ```cpp
    if (!str.empty()) {
        char last_char = str.back();
        std::cout << "The last character of the string is: " << last_char << std::endl;
    } else {
        std::cout << "The string is empty." << std::endl;
    }
    ```
    或者使用
    ```cpp
    if (!str.empty()) {
        char last_char = str[str.size() - 1];}
    ```
14.  数字类型转换为字符串  long 10 (n)
    `std::to_string`
    ```cpp
    int a = 42;
    double b = 3.14; 
    std::string str_a = std::to_string(a);
    std::string str_b = std::to_string(b);
    std::string result = "The value of a is " + str_a + " and the value of b is " + str_b;
    ```
15. 删除字符erase O(n)
    1. 删除指定位置的字符
      `iterator erase(size_t pos = 0, size_t len = npos);`
      pos：要删除的子字符串的起始位置
      len：要删除的字符的数量。默认为 npos，表示从 pos 开始到字符串末尾的所有字符。
    2. 删除指定位置的字符（迭代器版本）
      `iterator erase(iterator position);`
    3. 删除指定范围内的字符
      `iterator erase(iterator first, iterator last);`
      first：要删除的起始位置的迭代器。
      last：要删除的结束位置的迭代器（不包括此位置）。
16. `strstr`
用于在一个字符串中查找另一个字符串的第一次出现
```cpp
char *strstr(const char *haystack, const char *needle);
```
参数说明
  haystack: 要搜索的字符串（即在其中查找子串的字符串）。
  needle: 要查找的子串。

返回值
  如果在 haystack 中找到了 needle 的第一次出现，strstr 返回指向 needle 在 haystack 中的起始位置的指针。
  如果 needle 没有出现在 haystack 中，strstr 返回 NULL。
  如果 needle 是一个空字符串，strstr 返回 haystack 的起始位置（即原字符串的指针）。


# 3.`<iostream>`
1. typeid().name();查看数据类型
    cout << typeid(a).name() << endl; //int
# 4.`<vector>`
头文件：` #include<vector>`

`vector`与普通函数的区别：数组是静态空间，而`vector`是动态扩展；

动态扩展：并不是在原空间之后续接新空间，而是找更大的内存空间，然后将原始数据拷贝到新空间，然后释放原有空间。

`vector`容器的迭代器是支持随机访问的迭代器；

1. `vector`构造函数
函数原型：  
  `vector<T> v;`  // 默认构造
  `vector(v.begin(),v.end());`  // 将v.begin()到v.end()区间的元素拷贝
  `vector(n,elem);`  //将n个elem拷贝
  `vector(const vector &vec);`
	 
2. vector赋值
函数原型：
  `vector& operator=(const vector& vec);`
  `assign(beg,end);` //将[beg,end)区间中的数据拷贝赋值
  `assign(n,elem);` //将n个elem拷贝给本身

3. `vector`容量和大小
函数原型：

  `empty(); `  //判断容器释放为空
  `capacity();` //容器的容量
  `size();` //返回容器中的元素的个数
  `resize(int num)`  //重新定义容器的长度，若容器变长，则以默认值0进行填充  若变短，则末尾超出容器的元素被删除
  `resize(int num,elem) `//以elem进行填充


3. `vector`插入和删除
函数原型：

  `push_back(ele);` //尾部插入元素ele
  `pop_back();`  //删除最后一个元素  
  `insert(const_iterator pos,ele) `//迭代器指定位置pos插入元素ele
  `insert(const_iterator pos，int count,ele)` //迭代器指定位置pos插入coun  个元素ele
  `insert(const_iterator pos,const_iterator start,const_iterator end)`
  
  `erase(const_iterator pos);` //删除迭代器指定的元素
  `erase(const_iterator start,const_iterator end);` //删除迭代器从start  end的元素
  `clear（）；`  //删除容器中所有元素

4. 数据存放
函数原型：
  `at(int idx);` //返回索引所指向的数据
  `operator[];` 
  `front();` //返回第一个数据元素
  `back（）；`  //返回最后一个元素


5. `vector` 互换容器
实现两个容器内元素进行互换
函数原型：
  `swap(vec); ` //将vec与本身的元素互换

6. vector预留空间
减少vector在动态扩展次数
函数原型：
  `reserve（int len）; `//容器预留len个元素长度，预留位置不可初始化，元素不可访问
7. `std::vector` 的 `resize` 函数在 C++ 中可以改变 `vector` 的大小，但它的行为取决于新的大小相对于当前大小的变化
    1. 新大小大于当前大小：
    如果新的大小大于当前大小，则 resize 会扩展 vector 并添加新的元素。新元素的值取决于 resize 函数的参数。如果提供了默认值参数，则新元素使用该值初始化，否则使用 T 类型的默认构造函数初始化。
    已有的元素保持不变。
    2. 新大小小于当前大小：
    如果新的大小小于当前大小，则 resize 会缩减 vector 的大小。多余的元素将被移除。
    剩下的元素保持不变。
8. `pop_back`
   提供了 pop_back 方法，用于移除并丢弃向量的最后一个元素。pop_back 方法不返回被移除的元素，仅从向量中删除它。
   `numbers.pop_back();`
   **注意**
   1.调用 pop_back 时，如果向量是空的，会导致未定义行为。因此，在调用 pop_back 之前，应该确保向量不为空。
   2.pop_back 不返回被移除的元素。如果需要访问并移除最后一个元素，可以先访问它，然后再调用 pop_back
9. 初始化
   vector<vector<bool>>flag;
   flag=vector<vector<bool>>(m,vector<bool>(n,false));
   flag.resize(m, vector<bool>(n, false));//要用resize
10. `std::vector::reserve`
`std::vector::reserve` 是 C++ 标准库中的一个成员函数，用于预先分配内存以容纳至少指定数量的元素。它不会改变 `vector` 的大小（即 size 不变），但会改变 vector 的容量（即 capacity 可能增大）。这可以提高性能，特别是当你知道即将向 `vector` 添加大量元素时，通过避免多次重新分配内存。

你可以在向 `vector` 添加大量元素之前调用 `reserve` 函数，以预先分配足够的内存。

**作用与优势：**
减少内存重新分配：当你向 vector 添加元素且当前容量不足时，vector 会自动分配更多内存，这个过程可能需要复制现有元素到新的内存位置。频繁的重新分配和复制会影响性能。通过使用 reserve，可以减少甚至避免这种情况的发生。

提高性能：预先分配足够的内存可以减少重新分配和复制的次数，从而提高性能，特别是在需要添加大量元素时。
11. `push_back` 和 `emplace_back`
都是向 std::vector 等容器添加元素的方法，但它们在处理新元素的方式上有所不同。

`push_back`:向容器的末尾添加一个元素。它要求传递一个对象的**副本或右值引用**，之后将其拷贝或移动到容器中。在向vec添加obj时，需要先构造obj，然后调用拷贝构造函数将其添加到vec中

`emplace_back` 直接在容器末尾构造元素。它避免了不必要的拷贝或移动操作。它会传递构造函数所需的参数，并在容器的存储空间中**直接构造对象**。则直接在vec中构造对象，避免了不必要的拷贝或移动。

**区别**
`push_back` 需要传递一个已经构造好的对象，可能会涉及拷贝构造或移动构造。
`emplace_back` 直接在容器的存储空间中构造对象，通常避免了不必要的拷贝或移动操作，因此在某些情况下性能更好。
`push_back` 需要一个对象作为参数。
`emplace_back` 需要传递给对象构造函数的参数。


# 5.`<string_view>`
string_view是c++17标准库提供的一个类，它提供一个字符串的视图，即可以通过这个类以各种方法“观测”字符串，但**不允许修改字符串**。也就是只读

字符串视图并不真正的创建或者拷贝字符串，而只是拥有一个字符串的查看功能

std::string_view比std::string的性能要高很多，因为每个std::string都独自拥有一份字符串的拷贝，而std::string_view只是记录了自己对应的字符串的指针和偏移位置。当我们在只是查看字符串的函数中可以直接使用std::string_view来代替

std::string在进行操作时会重新分配内存，生成一个对应的std::string副本，大量的new操作。而std::string_view操作，从头到尾其实只有一个字符串数据，其它皆为视图。这也是需要注意的地方，因为std::string_view是原始字符串的视图，如果在查看std::string_view的同时修改了字符串，或者字符串被消毁，那么将是未定义的行为。

```cpp
const std::basic_string<char, std::char_traits<char>, Allocator& str>
noexcept 
    : ptr_(str.data()), length_(CheckLengthInternal(str.size())) {}
```
```
从上面的代码可以看出，string_view的构造函数，仅仅是记录的字符串的指针地址和字符串长度，并未进行拷贝。


tring_view::substr的原理类似，只是指针移动操作，返回一个子string_view

1.string_view并没有尾0 （'\0），所以在输出的时候，要注意边界。
2.因为string_view并不拷贝内存，所以要特别注意它所指向的字符串的生命周期。string_view指向的字符串，不能再string_view死亡之前被回收。

1. 初始化
```cpp
std::string_view s1 { "Hello, world!" }; // initialize with C-style string literal
std::cout << s1 << '\n';

std::string s{ "Hello, world!" };
std::string_view s2 { s };  // initialize with std::string
std::cout << s2 << '\n';

std::string_view s3 { s2 }; // initialize with std::string_view
std::cout << s3 << '\n';
```
2. substr
```cpp
std::string_view substr(size_t pos = 0, size_t count = npos) const;
```
pos：子字符串开始的位置。
count：子字符串的长度。如果 count 超过了剩余字符数，则子字符串将延伸到视图的末尾。
npos：是 std::string_view 的一个常量值，通常表示到字符串末尾

越界处理：如果 pos 大于原始字符串视图的长度，则会抛出 std::out_of_range 异常。如果 count 超过了剩余字符数，返回的子视图将延伸到字符串视图的末尾。
高效性：substr 方法返回的子视图与原始视图共享相同的底层数据，没有进行任何数据拷贝操作，非常高效。


# 6.`<unordered_set>`

std::unordered_set 是 C++ 标准库中的一个容器，它提供了一个无序集合，支持快速的查找、插入和删除操作。其底层实现基于哈希表，因此大多数操作的平均时间复杂度为常数时间。

无序：元素没有特定的顺序。
唯一性：每个元素在集合中都是唯一的。
哈希表：使用哈希表实现，提供 O(1) 平均时间复杂度的查找、插入和删除操作。
自动管理内存：会根据元素数量自动调整内部存储，以维持高效的操作。

>对于 unordered_set 容器不以键值对的形式存储数据，读者也可以这样认为，即 unordered_set 存储的都是键和值相等的键值对，为了节省存储空间，该类容器在实际存储时选择只存储每个键值对的值。

## 基本用法
1. 引入头文件
```cpp
#include <iostream>
#include <unordered_set>
#include <string>
```
2. 定义和初始化
```cpp
int main() {
    // 定义一个空的 unordered_set
    std::unordered_set<std::string> mySet;

    // 使用初始化列表初始化
    std::unordered_set<std::string> fruitSet = {"apple", "banana", "cherry"};
    
    return 0;
}
```
3. 插入元素
`insert()`:向容器中添加新元素。
```cpp
    // 插入单个元素
    mySet.insert("apple");

    // 插入多个元素
    mySet.insert({"banana", "cherry"});

```
3. 查找元素
`find(key)`:查找以值为 key 的元素，如果找到，则返回一个指向该元素的正向迭代器；反之，则返回一个指向容器中最后一个元素之后位置的迭代器（如果 end() 方法返回的迭代器）。
`count(key)`:在容器中查找值为 key 的元素的个数。
```cpp
    // 查找元素
    if (mySet.find("apple") != mySet.end()) {
        std::cout << "Found apple" << std::endl;
    }
    // 另一种查找方式
    if (mySet.count("banana") > 0) {
        std::cout << "Found banana" << std::endl;
    }

```
4. 删除元素
`erase()`:删除指定元素。
```cpp
    // 删除单个元素
    mySet.erase("apple");

    // 删除多个元素
    auto it = mySet.find("banana");
    if (it != mySet.end()) {
        mySet.erase(it);
    }
```
5. 遍历元素
```cpp
    // 遍历所有元素
    for (const auto& fruit : mySet) {
        std::cout << fruit << " ";
    }
    std::cout << std::endl;

```
6. `emplace()`:向容器中添加新元素，效率比 insert() 方法高。
7. `size()`:返回当前容器中存有元素的个数。
8. `empty()`:若容器为空，则返回 true；否则 false。


# 7.`<memory>`
智能指针
使用指针，我们没有释放，就会造成内存泄露。但是我们使用普通对象却不会！
如果我们分配的动态内存都交由有生命周期的对象来处理，那么在对象过期时，让它的析构函数删除指向的内存；
智能指针就是通过这个原理来解决指针自动释放的问题！

智能指针是用对象来管理动态分配的内存，并利用析构函数来自动释放内存。智能指针是一种 RAII（Resource Acquisition Is Initialization）技术，其核心思想是在对象的生命周期内获取资源，并在对象销毁时自动释放资源。

智能指针的种类：std::unique_ptr ;std::shared_ptr ;std::weak_ptr; 
创建智能指针需要包含头文件`#include<memory>` ;智能指针模板位于名称空间`std`中；

1. auto_ptr()；已被弃用
`std::auto_ptr` 的缺陷
   1. 所有权转移机制：
     `std::auto_ptr` 使用所有权转移机制，这意味着当一个 `auto_ptr` 赋值给另一个 `auto_ptr `时，所有权会从源 `auto_ptr` 转移到目标 `auto_ptr`，源 `auto_ptr` 被置为 `nullptr`。这导致了意外的所有权转移，容易造成程序逻辑混乱。
     ```cpp
     std::auto_ptr<int> ptr1(new int(10));
     std::auto_ptr<int> ptr2 = ptr1;  // 所有权转移，ptr1 变成 nullptr
     // 现在 ptr1 是 nullptr，ptr2 拥有该对象
     ```
   2. 拷贝构造和赋值的非直观行为：
     由于所有权转移，`std::auto_ptr` 的拷贝构造和赋值操作的行为不同于普通指针或其他容器。这种行为不直观，容易导致错误。
     ```cpp
     std::auto_ptr<int> ptr1(new int(10));
     std::auto_ptr<int> ptr2(ptr1);  // ptr1 变成 nullptr
     ```
   3. 标准容器的不兼容性：
     `std::auto_ptr` 不能安全地用于标准容器（如 `std::vector`、`std::list` 等），因为容器的拷贝和赋值操作会导致所有权的不确定性，容易引发未定义行为。
     ```cpp
      std::vector<std::auto_ptr<int>> vec;
      vec.push_back(std::auto_ptr<int>(new int(10)));  // 可能会导致未定义行为,智能用std::move修饰右值
      // 必须使用std::move修饰成右值，才可以进行插入容器中
      auto_ptr<string> p3(new string("I'm P3"));
      auto_ptr<string> p4(new string("I'm P4"));
      vec.push_back(std::move(p3));
      vec.push_back(std::move(p4));
      // 风险来了：
      vec[0] = vec[1];	// 如果进行赋值，问题又回到了上面一个问题中。vec为NULL
      cout << "vec.at(0)：" << *vec.at(0) << endl;
     ```

智能指针三个常用的函数：
   1. `.get()` 获取智能指针托管的指针地址
     ```cpp
        // 定义智能指针
        auto_ptr<Test> test(new Test);

        Test *tmp = test.get();		// 获取指针返回
        cout << "tmp->debug：" << tmp->getDebug() << endl;
     ```
     但我们一般不会这样使用，因为都可以直接使用智能指针去操作，除非有一些特殊情况。
     函数原型：
     ```cpp
     _NODISCARD _Ty * get() const noexcept
        {	// return wrapped pointer
            return (_Myptr);
        }
     ```
   2. `.release()`取消智能指针对动态内存的托管
     ```cpp
        // 定义智能指针
        auto_ptr<Test> test(new Test);

        Test *tmp2 = test.release();	// 取消智能指针对动态内存的托管
        delete tmp2;	// 之前分配的内存需要自己手动释放
     ```
     也就是智能指针不再对该指针进行管理，改由管理员进行管理！
     函数原型：
     ```cpp
        _Ty * release() noexcept
        {	// return wrapped pointer and give up ownership
            _Ty * _Tmp = _Myptr;
            _Myptr = nullptr;
            return (_Tmp);
        }
     ```
   3. `.reset()` 重置智能指针托管的内存地址，如果地址不一致，原来的会被析构掉
     ```cpp
        // 定义智能指针
        auto_ptr<Test> test(new Test);

        test.reset();			// 释放掉智能指针托管的指针内存，并将其置NULL

        test.reset(new Test());	// 释放掉智能指针托管的指针内存，并将参数指针取代之
     ```
     reset函数会将参数的指针(不指定则为NULL)，与托管的指针比较，如果地址不一致，那么就会析构掉原来托管的指针，然后使用参数的指针替代之。然后智能指针就会托管参数的那个指针了。
     函数原型：
     ```cpp
      void reset(_Ty * _Ptr = nullptr)
        {	// destroy designated object and store new pointer
            if (_Ptr != _Myptr)
                delete _Myptr;
            _Myptr = _Ptr;
        }
     ```

`std::auto_ptr` 被弃用的主要原因是它的所有权转移机制和拷贝行为不直观，容易导致内存管理错误。`std::unique_ptr` 作为它的替代品，通过禁止拷贝、引入移动语义、支持自定义删除器等方式，提供了更安全、更高效的内存管理机制。因此，在现代 C++ 编程中，应当使用 `std::unique_ptr` 来管理动态分配的内存。

2. std::unique_ptr 的改进
`std::unique_ptr` 是 C++11 引入的智能指针，用于替代 `std::auto_ptr`，它解决了 `auto_ptr` 的缺陷，提供了更安全和直观的内存管理机制。
   1. 独占所有权：
     `std::unique_ptr` 确保某一时刻只有一个指针可以拥有一个资源，不允许拷贝操作，只允许移动操作，从而避免了所有权的不确定性。
     ```cpp
     std::unique_ptr<int> ptr1(new int(10));
     // std::unique_ptr<int> ptr2 = ptr1; // 编译错误，不允许拷贝
     std::unique_ptr<int> ptr2 = std::move(ptr1);  // 允许移动，ptr1 变成 nullptr
     ```
   2. 标准容器的兼容性：
     由于 `std::unique_ptr` 只允许移动语义，标准容器可以安全地使用 `std::unique_ptr`
     ```cpp
     std::vector<std::unique_ptr<int>> vec;
     vec.push_back(std::make_unique<int>(10));  // 安全使用
     ```
   3. 自定义删除器
     `std::unique_ptr` 支持自定义删除器，可以更灵活地管理资源的释放方式。
`std::auto_ptr` 被弃用的主要原因是它的所有权转移机制和拷贝行为不直观，容易导致内存管理错误。`std::unique_ptr` 作为它的替代品，通过禁止拷贝、引入移动语义、支持自定义删除器等方式，提供了更安全、更高效的内存管理机制。因此，在现代 C++ 编程中，应当使用 `std::unique_ptr` 来管理动态分配的内存。

3. std::shared_ptr
`std::shared_ptr` 是一种共享所有权的智能指针。多个 `std::shared_ptr` 可以指向同一个资源，引用计数会自动管理资源的生命周期。当最后一个引用该资源的 `std::shared_ptr` 被销毁时，资源会被自动释放。
记录引用特定内存对象的智能指针数量，当复制或拷贝时，引用计数加1，当智能指针析构时，引用计数减1，如果计数为零，代表已经没有指针指向这块内存，那么我们就释放它！这就是 `shared_ptr` 采用的策略！
   1. 初始化
     1）构造函数
     ```cpp
        shared_ptr<int> up1(new int(10));  // int(10) 的引用计数为1
        shared_ptr<int> up2(up1);  // 使用智能指针up1构造up2, 此时int(10) 引用计数为2
     ```
     2）使用`make-shared`初始化对象，分配内存效率更高(推荐使用)
      make_shared函数的主要功能是在动态内存中分配一个对象并初始化它，返回指向此对象的shared_ptr; 用法：
      make_shared<类型>(构造类型对象需要的参数列表);
     ```cpp
     shared_ptr<int> up3 = make_shared<int>(2); // 多个参数以逗号','隔开，最多接受十个
     shared_ptr<string> up4 = make_shared<string>("字符串");
     shared_ptr<Person> up5 = make_shared<Person>(9);
     ```
   2. 赋值
     ```cpp
     shared_ptrr<int> up1(new int(10));  // int(10) 的引用计数为1
     shared_ptr<int> up2(new int(11));   // int(11) 的引用计数为1
     up1 = up2;	// int(10) 的引用计数减1,计数归零内存释放，up2共享int(11)给up1, int(11)的引用计数为2
     ```
   3. 主动释放
     ```cpp
        shared_ptrr<int> up1(new int(10));
        up1 = nullptr ;	// int(10) 的引用计数减1,计数归零内存释放 
        // 或
        up1 = NULL; // 作用同上 
     ``` 
   4. 重置
     ```cpp
        p.reset() ; 将p重置为空指针，所管理对象引用计数 减1
        p.reset(p1); 将p重置为p1（的值）,p 管控的对象计数减1，p接管对p1指针的管控
        p.reset(p1,d); 将p重置为p1（的值），p 管控的对象计数减1并使用d作为删除器
        p1是一个指针！
     ```
   5. 交换
      p1 和 p2 是智能指针
      ```cpp
        std::swap(p1,p2); // 交换p1 和p2 管理的对象，原对象的引用计数不变
        p1.swap(p2);    // 交换p1 和p2 管理的对象，原对象的引用计数不变
      ```


1. unique_ptr
三种创建方式：
（1）通过已有裸指针创建；
（2）通过new来创建；
（3）通过std::make_unique创建（推荐）；
uinque_ptr可以通过get（）获取地址；
unique_ptr实现了->和*;

（1）通过落裸指针创建
`Person *s1 = new Person("aaa");` 
`std::unique_ptr<Person> u_s1{s1};`
//这样会导致，当s1改变的时候，u_s1也会跟着改变，所以可以先销毁s1，并指向空；
`delete s1;`
`s1 = nullptr;`

（2）通过new创建
`std::unique_ptr<Person> u_s2{new Person("aa")};`
（3）通过make_unique创建(推荐)
`std::unique_ptr<Person> u_s3 = make_unique<Person>("afdsa");`
`u_s2.get()` //获取地址
`u_s3.get()` //获取地址



# 8.`<algorithm>`

1. **std::min()**
  它可以通过以下三种方式使用：
  它比较在其参数中传递的两个数字，并返回两个中较小的一个，如果两个相等，则返回第一个。
  它还可以使用由用户定义的二进制函数比较两个数字，然后将其作为参数传递给std::min。
  如果我们想在给定列表中找到最小的元素，它也很有用；如果列表中存在多个元素，它会返回第一个元素。
2. **max_element // min_element**o(n)
  用于在给定范围内查找最大元素，并返回指向该元素的迭代器。它在 <algorithm> 头文件中声明。
  
  ```cpp
  template< class ForwardIt >
  ForwardIt max_element( ForwardIt first, ForwardIt last );
  
  template< class ForwardIt, class Compare >
  ForwardIt max_element( ForwardIt first, ForwardIt last, Compare comp );
  ```
  first 和 last 是表示要搜索的元素范围的迭代器。搜索范围包括 `[first, last)`，即左闭右开区间。
  comp 是一个可选的二元谓词，用于定义比较规则。如果未提供此参数，函数将使用默认的 < 比较运算符进行  比较。
  max_element 返回一个迭代器，指向范围内的最大元素。如果范围为空，则返回 last
3. **reverse** o(n)
  用于反转容器或数组的元素顺序
  `void reverse(BidirectionalIterator first, BidirectionalIterator last);`
  first：指向需要反转的范围的起始位置。
  last：指向需要反转的范围的结束位置（不包括此位置）。
  ```cpp
  int arr[] = {1, 2, 3, 4, 5};
  int n = sizeof(arr) / sizeof(arr[0]);
  std::reverse(arr, arr + n);
  
  std::reverse(str.begin(), str.end());
  ```
4. **unique**
  std::unique 是 C++ 标准库 <algorithm> 头文件中提供的一个函数模板，用于在容器中移除连续的重复元素。它将相邻的重复元素移动到容器末尾，并返回一个指向新的“逻辑结束位置”的迭代器，指示不重复的元素的范围。
  ```cpp
	template< class ForwardIt >
	ForwardIt unique( ForwardIt first, ForwardIt last );
  ```
  first 和 last 是容器的起始迭代器和结束迭代器，表示要处理的元素范围。[first, last) 表示左闭右开区  间，即包含 first，但不包含 last。
  函数返回一个指向新的“逻辑结束位置”的迭代器，指示不重复的元素的范围。
  
  std::unique 函数只能移除相邻的重复元素，如果容器中的重复元素不相邻，则需要先对容器进行排序，然后  再使用 std::unique。
  
  注意：std::merge() 函数的第四个参数上。在你的代码中，你没有指定目标序列的起始位置，而是直接使用  了 win2.begin()，这会导致未定义的行为，因为 win2 目前是一个空的向量，其 begin() 方法返回的迭代器  并不指向有效的位置。你需要确保目标序列 win2 至少具有足够的容量，或者在调用 std::merge() 函数之前  使用 resize() 方法来为目标序列分配足够的空间。
5. **distance**
  std::distance 是一个标准库函数，用于计算两个迭代器之间的距离（即元素的个数）
  ```cpp
   // 获取最大元素的迭代器
    auto lementIt = std::max_element(vec.begin(), vec.end());
    // 计算下标
    int index = std::distance(vec.begin(), maxElementIt);
  ```
6. for_each()函数
用于对范围内的每个元素执行给定的操作;
```cpp
	template< class InputIt, class UnaryFunction >
	UnaryFunction for_each( InputIt first, InputIt last, UnaryFunction f );
```
first, last: 迭代器表示要操作的范围 [first, last)。
f: 一个可调用对象（如函数、函数对象或 lambda 表达式），它接受一个元素并对其进行操作。

比如：
```cpp
// 定义一个函数对象
struct Print {
    void operator()(int n) const {
        std::cout << n << " ";
    }
};
int main() {
    std::vector<int> numbers = {1, 2, 3, 4, 5};

    // 使用 for_each 打印每个元素
    std::for_each(numbers.begin(), numbers.end(), Print());

    std::cout << std::endl;

    // 使用 for_each 和 lambda 表达式打印每个元素
    std::for_each(numbers.begin(), numbers.end(), [](int n) {
        std::cout << n * n << " "; // 打印每个元素的平方
    });

    std::cout << std::endl;

    return 0;
}
```
7. `std::all_of`
用于检查容器中的所有元素是否满足指定的条件.
```cpp
template< class InputIt, class UnaryPredicate >
bool all_of(InputIt first, InputIt last, UnaryPredicate p);
```
InputIt first 和 InputIt last 是输入迭代器，表示要检查的范围 [first, last)。
UnaryPredicate p 是一个一元谓词，用于检查每个元素是否满足条件。

```cpp
bool isPositive(int n) {
    return n > 0;
}
std::vector<int> numbers = {1, 2, 3, 4, 5};

bool allPositive = std::all_of(numbers.begin(), numbers.end(), isPositive);

bool allPositive = std::all_of(numbers.begin(), numbers.end(), [](int n) { return n > 0; });
```
8. `std::lower_bound`
用于在有序范围内查找不小于给定值的第一个元素的位置。它是二分查找的一种实现，时间复杂度为 O(log n)。

```cpp
template< class ForwardIt, class T >
ForwardIt lower_bound( ForwardIt first, ForwardIt last, const T& value );
```
first: 要搜索的范围的起始迭代器。
last: 要搜索的范围的结束迭代器（不包括此位置）。
value: 要查找的值。

返回一个迭代器，指向范围 [first, last) 中的第一个 不小于 value 的元素。如果范围内所有元素都小于 value，则返回 last。

9. `std::upper_bound`
查找的是在指定范围内 大于 给定值的第一个元素的位置。
```cpp
template< class ForwardIt, class T >
ForwardIt upper_bound( ForwardIt first, ForwardIt last, const T& value );
```
first: 要搜索的范围的起始迭代器。
last: 要搜索的范围的结束迭代器（不包括此位置）。
value: 要查找的值。

返回一个迭代器，指向范围 [first, last) 中第一个 大于 value 的元素。如果没有找到这样的元素，则返回 last。

std::lower_bound：返回不小于给定值的第一个元素的位置。
std::upper_bound：返回大于给定值的第一个元素的位置。


# 9.`<unordered_map>`

哈希表最适合的求解问题是查找与给定值相等的记录；

c++使用哈希表
 使用STL库中的哈希表

`std::unordered_map` 中的元素都是 `std::pair<const Key, T> `类型。每个元素包含一个键（key）和值（value），其中键是常量类型。
所以可以通过`first`和`second`访问
```cpp
// 通过迭代器访问元素
for (const auto& pair : myMap) {
    std::cout << "Key: " << pair.first << ", Value: " << pair.second << std::endl;
}

// 通过键访问值
std::string key = "two";
if (myMap.find(key) != myMap.end()) {
    std::cout << "Value for key '" << key << "' is " << myMap[key] << std::endl;
} else {
    std::cout << "Key '" << key << "' not found." << std::endl;
}

//find结合pair访问
auto it = cnt.find(fruits[left]);
--it->second;
```


(1)导入头文件  
`#include<unordered_map>`  

(2)哈希表声明和初始化

 1）声明
	
	unordered_map<elemType_1, elemType_2> var_name; //声明一个没有任何元素的哈希表，
	//其中elemType_1和elemType_2是模板允许定义的类型，如要定义一个键值对都为Int的哈希表：
	unordered_map<int, int> map;

  2）初始化

    以上在声明哈希表的时候并没有给unordered_map传递任何参数，因此调用的是unordered_map的默认构造函数，生成一个空容器。初始化主要有一下几种方式：

   a）在定义哈希表的时候通过初始化列表中的元素初始化:

	unordered_map<int, int> hmap{ {1,10},{2,12},{3,13} };
	//如果知道要创建的哈希表的元素个数时，也可以在初始化列表中指定元素个数
	unordered_map<int, int> hmap{ {{1,10},{2,12},{3,13}},3 };

   b）通过下标运算来添加元素:

	//当我们想向哈希表中添加元素时也可以直接通过下标运算符添加元素，格式为: mapName[key]=value;
	//如：hmap[4] = 14;
	//但是这样的添加元素的方式会产生覆盖的问题，也就是当hmap中key为4的存储位置有值时，
	//再用hmap[4]=value添加元素，会将原哈希表中key为4存储的元素覆盖
	hmap[4] = 14;
	hmap[4] = 15;
	cout << hmap[4];  //结果为15

 c）通过insert()函数来添加元素:

	/通过insert()函数来添加元素的结果和通过下标来添加元素的结果一样，不同的是insert()可以避免覆盖问题，
	//insert()函数在同一个key中插入两次，第二次插入会失败
	hmap.insert({ 5,15 });
	hmap.insert({ 5,16 });
	cout << hmap[5];  //结果为15
	
 d）复制构造，通过其他已初始化的哈希表来初始新的表:

	unordered_map<int, int> hmap{ {1,10},{2,12},{3,13} };
	unordered_map<int, int> hmap1(hmap);

（3） STL中哈希表的常用函数

(1) begin( )函数：该函数返回一个指向哈希表开始位置的迭代器

	unordered_map<int, int>::iterator iter = hmap.begin(); //申请迭代器，并初始化为哈希表的起始位置
	cout << iter->first << ":" << iter->second;

(2) end( )函数：作用于begin函数相同，返回一个指向哈希表结尾位置的下一个元素的迭代器

	unordered_map<int, int>::iterator iter = hmap.end();

(3) cbegin() 和 cend()：这两个函数的功能和begin()与end()的功能相同，唯一的区别是cbegin()和cend()是面向不可变的哈希表

	const unordered_map<int, int> hmap{ {1,10},{2,12},{3,13} };
	unordered_map<int, int>::const_iterator iter_b = hmap.cbegin(); //注意这里的迭代器也要是不可变的const_iterator迭代器
	unordered_map<int, int>::const_iterator iter_e = hmap.cend();

(4) empty()函数：判断哈希表是否为空，空则返回true，非空返回false

	bool isEmpty = hmap.empty();

(5) size()函数：返回哈希表的大小

	int size = hmap.size();

(6) erase()函数： 删除某个位置的元素，或者删除某个位置开始到某个位置结束这一范围内的元素， 或者传入key值删除键值对

	unordered_map<int, int> hmap{ {1,10},{2,12},{3,13} };
	unordered_map<int, int>::iterator iter_begin = hmap.begin();
	unordered_map<int, int>::iterator iter_end = hmap.end();
	hmap.erase(iter_begin);  //删除开始位置的元素
	hmap.erase(iter_begin, iter_end); //删除开始位置和结束位置之间的元素
	hmap.erase(3); //删除key==3的键值对

(7) at()函数：根据key查找哈希表中的元素

	unordered_map<int, int> hmap{ {1,10},{2,12},{3,13} };
	int elem = hmap.at(3);

(8) clear()函数：清空哈希表中的元素

	hmap.clear()

(9) find()函数：以key作为参数寻找哈希表中的元素，如果哈希表中存在该key值则返回该位置上的迭代器，否则返回哈希表最后一个元素下一位置上的迭代器

	unordered_map<int, int> hmap{ {1,10},{2,12},{3,13} };
	unordered_map<int, int>::iterator iter;
	iter = hmap.find(2); //返回key==2的迭代器，可以通过iter->second访问该key对应的元素
	if(iter != hmap.end())  cout << iter->second;

(10) bucket()函数：以key寻找哈希表中该元素的储存的bucket编号（unordered_map的源码是基于拉链式的哈希表，所以是通过一个个bucket存储元素）

	int pos = hmap.bucket(key);

(11) bucket_count()函数：该函数返回哈希表中存在的存储桶总数（一个存储桶可以用来存放多个元素，也可以不存放元素，并且bucket的个数大于等于元素个数）

	int count = hmap.bucket_count();

(12) count()函数： 统计某个key值对应的元素个数， 因为unordered_map不允许重复元素，所以返回值为0或1

	int count = hmap.count(key);

(13) 哈希表的遍历: 通过迭代器遍历

	unordered_map<int, int> hmap{ {1,10},{2,12},{3,13} };
	unordered_map<int, int>::iterator iter = hmap.begin();
	for( ; iter != hmap.end(); iter++){
	cout << "key: " <<  iter->first  << "value: " <<  iter->second <<endl;
	}

### map和unordered_map的区别

底层实现：

map 使用红黑树（Red-Black Tree）作为底层数据结构，因此键值对是按照键的大小顺序进行排序的。这意味着在 map 中查找、插入和删除操作的时间复杂度为 O(log n)。
unordered_map 使用哈希表（Hash Table）作为底层数据结构，键值对的存储顺序是无序的。哈希表具有良好的平均时间复杂度，通常情况下查找、插入和删除操作的时间复杂度为 O(1)。
元素顺序：

在 map 中，键值对按照键的大小顺序进行排序，并且可以通过比较器自定义排序方式。
在 unordered_map 中，键值对的顺序是不确定的，因为哈希表会根据键的哈希值将键值对分布到不同的桶中。
性能特点：

map 适用于需要按照键的大小顺序进行访问的场景，例如有序存储和查找数据。
unordered_map 适用于需要快速查找的场景，例如查找操作频繁且无序存储的情况。

# 10.`<cassert>`
  1. `assert`:是一个宏，用于在开发过程中检查程序中的假设。它通常用于调试和测试，以确保程序在运行时满足某些条件。如果条件不满足，程序将终止，并打印一条错误消息，指示出错的位置。
   `assert(condition);`:condition：这是一个表达式。如果这个表达式的值为假（即为 0），assert 将导致程序终止，并显示一个包含表达式文本、文件名和行号的诊断消息。


# 11 `<sys/types.h>`

是用于在 C 和 C++ 程序中包含数据类型定义的头文件，通常用于与 POSIX 标准和系统级编程相关的类型定义。

`sys/types.h` 定义了一些基本的数据类型，这些类型主要用于系统调用和其他底层系统操作。以下是一些常见的数据类型：

`pid_t`：进程ID类型。
`uid_t`：用户ID类型。
`gid_t`：组ID类型。
`mode_t`：文件权限模式类型。
`off_t`：文件偏移量类型。
`ssize_t`：签名的大小类型，通常用于表示读写操作的返回值


# 12 `<unistd.h>`

是在 C 和 C++ 编程中用于引入 POSIX 操作系统 API 的头文件。它提供了对许多低级系统调用的访问，这些系统调用涉及文件操作、进程控制、目录操作等

1. 进程控制

`fork()`：创建一个新进程。
`exec()` 系列函数：执行一个新程序。
`_exit()`：立即终止当前进程。
`getpid()`：获取当前进程的ID。
`getppid()`：获取父进程的ID。

2. 文件操作

`open()`：打开一个文件。
`close()`：关闭一个文件描述符。
`read()`：从文件中读取数据。
`write()`：向文件中写入数据。
`lseek()`：移动文件指针。

3. 目录操作

`chdir()`：改变当前工作目录。
`getcwd()`：获取当前工作目录

4. 管道和重定向

`pipe()`：创建一个管道。
`dup()` 和 `dup2()`：复制文件描述符。

5. 用户和组

`getuid()`：获取用户ID。
`getgid()`：获取组ID。

`pid_t getpgrp(void)`:返回调用进程组的ID，即当前进程的进程组 ID
一个进程可以通过使用setpgid函数来改变自己或者其他进程的进程组；
`int setpgid(pid_t pid,pid_t pgid)`:返回，若成功则为0，若错误则为-1

# 13 `<sys/wait.h>`
是一个用于进程管理的头文件，主要提供了等待进程状态变化和获取终止状态的相关函数和宏。它常与 fork 函数一起使用，用于父进程等待其子进程结束，并获取子进程的终止状态。

函数：
`pid_t wait(int *status)`;阻塞等待任何一个子进程结束，返回子进程的PID，并将其终止状态存储在 status 中。相当于`pid_t waitpid(-1, int *status, 0)`；
`pid_t waitpid(pid_t pid, int *status, int options)`;根据 pid 和 options 参数等待特定的子进程，返回子进程的PID，并将其终止状态存储在 status 中。返回：如果成功，则为子进程的PID，如果WNOHANG(任何子进程都没有终止)则为0.如果其他错误则为-1
pid 可以是以下值：
>`-1`：等待任何子进程（与 wait 相同）。
`>0`：等待特定子进程。
`0`：等待任一组ID与调用进程相同的子进程。
`<-1`：等待任一进程组ID等于 -pid 的绝对值的子进程。

宏:
`WIFEXITED(status)`：若子进程正常结束则为真。
`WEXITSTATUS(status)`：获取子进程的返回值。

`WIFSIGNALED(status)`：若子进程因信号终止则为真。
`WTERMSIG(status)`：获取导致子进程终止的信号。

`WIFSTOPPED(status)`：若子进程因信号而暂停则为真。
`WSTOPSIG(status)`：获取导致子进程暂停的信号。

`WIFCONTINUED(status)`：若子进程已恢复执行则为真（仅用于 waitpid）


# 14 `<stdio.h>`

1. fflush 是 C 标准库中的一个函数，用于刷新输出缓冲区。其定义在 <stdio.h> 头文件中。该函数的主要用途是在不关闭文件的情况下强制将缓冲区中的数据写入到文件或标准输出中。
```cpp
#include <stdio.h>

int fflush(FILE *stream);
```
stream：指向 FILE 类型的指针，可以是文件指针或者标准流（如 stdout、stderr）。
如果 stream 是 NULL，则 fflush 刷新所有打开的输出流。
成功时返回 0。
失败时返回 EOF，并设置 errno 以指示错误。

使用场景：
强制输出缓冲区数据：通常用于确保在程序崩溃或意外退出前，所有缓冲区中的数据都已写入文件或输出流。
调试：在调试代码时，可以使用 fflush(stdout) 来确保调试信息立即输出，以便在程序崩溃前看到最新的调试信息。

# 15. `<errno.h>`

1. errno 是一个全局变量，用于表示最近一次系统调用或库函数调用中的错误代码。许多系统调用和库函数在失败时会设置 errno，以指示错误类型。

使用方法：
包含头文件：使用 errno 需要包含 <errno.h> 头文件。
检查函数返回值：通常在系统调用或库函数返回一个错误指示值（例如，-1 或 NULL）时检查 errno。
读取 errno：直接读取 errno 变量以获取错误代码。
使用错误消息：可以使用 perror 或 strerror 函数将错误代码转换为人类可读的错误消息。

# 16. `<queue>`
std::queue是一个容器适配器，它只允许在末尾添加元素（push）和在前端删除元素（pop），遵循先进先出（FIFO）的顺序。

单向操作：只能在末尾添加元素和在前端删除元素。
基础容器：通常使用std::deque或std::list作为底层容器（默认是std::deque）。
std::queue：通常基于std::deque或std::list实现。

队列：是一种先进先出的数据结构，

常用的接口：

构造函数：
```cpp
	queue<T> que;  //默认构造函数
	queue(const queue &que);  //拷贝构造函数

赋值操作：

	queue& operator=(const queue& que);

数据存取

	push(elem); //往队尾添加元素
	pop(); //从对头溢出第一个元素
	back(); //返回最后一个元素
	front(); //返回第一个元素

大小操作：

	empty(); //判断对战是否为空
	size(); //返回栈的大小

```
2. `std::priority_queue` **优先队列**
  `std::priority_queue` 是一种适配器容器，提供类似堆的数据结构，用于实现优先队列。优先队列是一种特殊的队列，其中元素按优先级排序，最高优先级的元素最先出队。`std::priority_queue` 默认是一个最大堆（最大优先级的元素在顶部），但也可以通过自定义比较函数实现最小堆。

  `std::priority_queue` 定义在 `<queue>` 头文件中，并依赖于底层容器（通常是 `std::vector`）来存储元素。

  1. 创建优先队列
   ```cpp
    #include <queue>
    #include <vector>
    #include <functional> // For std::greater

    std::priority_queue<int> maxHeap; // 最大堆
    std::priority_queue<int, std::vector<int>, std::greater<int>> minHeap; // 最小堆
   ```
  2. 插入元素
   `void push(const T& val);`
   `void push(T&& val);`
   ```cpp
    maxHeap.push(10);
    maxHeap.push(20);
    minHeap.push(30);
   ```
  3. 访问最高优先级的元素
   `const T& top() const;`
   ```cpp
    std::cout << "Max element: " << maxHeap.top() << "\n";
    std::cout << "Min element: " << minHeap.top() << "\n";
   ```
   4. 移除最高优先级的元素
  ` void pop();`

  5. 检查优先队列是否为空
   ` bool empty() const;`
   ```cpp
    if (maxHeap.empty()) {
        std::cout << "Max heap is empty\n";
    }
   ```

  6. 获取优先队列的大小
   `size_t size() const;`
   ```cpp
    std::cout << "Max heap size: " << maxHeap.size() << "\n";
   ```


   std::priority_queue 的底层容器默认是 std::vector，但可以通过模板参数指定其他序列容器，如 std::deque。
   插入和删除操作的时间复杂度为 O(log n)。
   访问最高优先级元素的时间复杂度为 O(1)。
  7. 优先队列，参数是pair
    在存储`pair<int, int>`时，优先队列会基于pair的第一个元素（即first）进行比较。
    假如相通过第二个值进行比较：
      1. 你可以使用std::function来包装lambda函数，并将其传递给std::priority_queue：
       ```CPP
        // 定义一个自定义比较器
            auto comp = [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
                return a.second > b.second;
            };

            // 使用 std::function 包装比较器
            std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::function<bool(const std::pair<int, int>&, const std::pair<int, int>&)>> q(comp);
       ```
      2. 定义一个结构体来实现自定义比较器：
        ```CPP
        struct CompareBySecond {
            bool operator()(const std::pair<int, int>& a, const std::pair<int, int>& b) {
                return a.second > b.second;
            }
        };
        // 使用自定义比较器结构体
        std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, CompareBySecond> q;
        ```
    是使用自顶向下（down-heap）调整:
      1. 将堆顶元素移除，并将堆的最后一个元素放到堆顶。
      2. 逐步将该元素与其子节点进行比较，并在必要时进行交换，直到元素达到合适的位置。
    复杂度：
        插入元素 (push)：
        时间复杂度：O(log n)
        解释：插入操作会将新元素添加到堆的末尾，并进行上滤（上升操作）以维护堆的性质。
        
        删除最大或最小元素 (pop)：
        时间复杂度：O(log n)
        解释：删除操作会移除堆顶元素（最大或最小），将堆的最后一个元素移动到堆顶，并进行下滤（下降操作）以维护堆的性质。
        
        访问最大或最小元素 (top)：
        时间复杂度：O(1)
        解释：堆顶元素始终位于容器的顶部，可以在常数时间内访问。
        
        构造优先队列：
        时间复杂度：O(n)（如果批量插入构造），O(n log n)（如果一个一个插入）
        解释：批量插入构造优先队列时，可以使用堆化（heapify）操作，在O(n)时间内完成。
        
        合并两个优先队列：
        时间复杂度：O(n log n)
        解释：将一个优先队列的元素逐一插入到另一个优先队列中。


# 17.<deque>
`std::deque`（双端队列）是一种支持高效插入和删除操作的容器适配器，可以在容器的两端进行插入和删除操作

是一种更通用的序列容器，可以在两端进行快速的插入和删除操作。
双向操作：可以在两端进行插入和删除操作
动态数组：底层实现通常是一个动态数组，可以在两端快速扩展。

1. 创建`std::deque`
```cpp
#include <deque>

std::deque<int> dq;  // 创建一个空的双端队列
```
2. 检查双端队列是否为空
`bool empty() const;`

```cpp
if (dq.empty()) {
    std::cout << "Deque is empty\n";
}
```
3. 获取双端队列的大小
`size_t size() const;`
```cpp
std::cout << "Deque size: " << dq.size() << "\n";
```
4. 访问双端队列的第一个元素
`T& front();`
`const T& front() const;`
```cpp
if (!dq.empty()) {
    std::cout << "Front element: " << dq.front() << "\n";
}
```
5. 访问双端队列的最后一个元素

```cpp
if (!dq.empty()) {
    std::cout << "Back element: " << dq.back() << "\n";
}
```
6. 向双端队列添加元素
```cpp
void push_front(const T& val);
void push_front(T&& val);

void push_back(const T& val);
void push_back(T&& val);

```
```cpp
dq.push_front(10);  // 添加元素到双端队列的前面
dq.push_back(20);   // 添加元素到双端队列的后面

```
7. 从双端队列中移除元素
```cpp
void pop_front();
void pop_back();
```
```cpp
dq.pop_front();  // 移除双端队列的第一个元素
dq.pop_back();   // 移除双端队列的最后一个元素
```
8. 访问和修改双端队列中的元素
```cpp
T& operator[](size_t idx);
const T& operator[](size_t idx) const;
```
```cpp
dq[0] = 30;  // 修改第一个元素
std::cout << "Element at index 0: " << dq[0] << "\n";
```

# 18.`<numeric>`

1. `iota`函数会从指定的值开始，然后递增地为容器中的每个元素赋予连续的值。下面是一个示例代码：
```cpp
    std::vector<int> vec(n);

    // 使用 iota 函数从0开始赋值
    std::iota(vec.begin(), vec.end(), 0);
```
2. accumulate
   用于计算一个范围内所有元素的总和或者通过一个自定义的二元操作累积计算结果。
   ```cpp
    template<class InputIterator, class T>
    T accumulate(InputIterator first, InputIterator last, T init);
   ```
  first 和 last 是要进行累积的元素范围的迭代器。
  init 是初始值，即累积的起始值。
  函数返回值是累积计算的结果。

  ```cpp
   std::vector<int> numbers = {1, 2, 3, 4, 5};
   int sum = std::accumulate(numbers.begin(), numbers.end(), 0);
  ```

# 19. `<functional>`
1. 算术仿函数   

功能：实现四则运算
其中negate是一元运算，其他都是二元运算；

仿函数原型：  
```cpp
template<class T>T plus<T> //加法仿函数
template<class T>T minus<T> //减法仿函数
template<class T>T multiplies<T> //乘法仿函数
template<class T>T dicides<T> //除法仿函数
template<class T>T modulus<T> //取模仿函数
template<class T>T negate<T> //取反仿函数
```

2. 关系仿函数
功能：实现关系对比

原型：
```cpp
template<class T> bool greater<T>  //大于
template<class T>bool equal_to<T> //等于
template<class T>bool not_equal_to<T> //不等于
template<class T>bool greater_equal<T> //大于等于
template<class T>bool less<T> //小于
template<class T>bool less_equal<T> //小于等于
```
降序：用greater

std::sort(s.begin(), s.end(),std::greater<int>());
为什么是这样呢？
sort使用快排：
```cpp
template<typename RandomIt, typename Compare>
void quicksort(RandomIt first, RandomIt last, Compare comp) {
    if (first == last) return;

    // Partitioning step
    auto pivot = *(first + (last - first) / 2);
    auto middle1 = std::partition(first, last, [=](const auto& elem) { return comp(elem, pivot); });
    auto middle2 = std::partition(middle1, last, [=](const auto& elem) { return !comp(pivot, elem); });

    // Recursively sort the partitions
    quicksort(first, middle1, comp);
    quicksort(middle2, last, comp);
}
```
可以发现调用了`partition`,这个函数用于将范围内的元素根据给定的谓词进行分区。它将范围 [first, last) 分成两个部分：满足谓词 的元素，不满足谓词 的元素。即范围内的所有满足谓词 p 的元素位于不满足谓词 p 的元素之前，它返回一个迭代器，该迭代器指向第一个不满足谓词 p 的元素。

所以假如使用`greater`，大于`pivot`的元素放到前面，小于的放到后面，就成降序了


3. 逻辑仿函数

功能：实现逻辑运算

函数原型：
```cpp
template<class T>bool logicai_and<T> //逻辑与
template<class T>bool logicai_or<T> //逻辑或
template<class T>bool logicai_not<T> //逻辑非  
```
  
# 20 `<stack>`
	stack<T> stk;  //stack采用模板类实现，stack对象的默认构造函数
	stack(const stack& stk); //拷贝构造函数
	
	stack& operator=(const stack&stk); //重载等号运算符
	
	push(elem); //添加元素
	pop(); //从栈顶移除第一个元素
	top(); //返回栈顶元素
	
	empty(); //判断堆栈是否为空
	size(); //栈的大小


# 21 `<list>`
   
   STL 列表是一个双向循环链表容器
   
   1. 创建list容器
   
   函数原型：
   
   `list<T> lst;`   //创建一个空链表,List采用模板类实现，对象的默认构造形式
   `list(beg,end);` //构造函数将[begin,end)区间中的元素拷贝给本身
   `list(n,elem); `//构造函数将n个elem拷贝给本身
   `list(const list &lst); `//拷贝构造函数
   
   
   2. list赋值和交换
   
   功能：给list容器进行赋值，以及·交换list容器
   
   函数原型：  
   
   `assign(beg,end);` //将[beg,end)区间的数据拷贝到本身
   `assign(n,elem);` //将n个elem拷贝赋值
   `list& operator=(const list&lst);` //重载等号
   `swap(lst);` //将lst与本身元素交换
   
   3. list大小操作
   功能：对list容器的大小进行操作
   函数原型：
   `size()`; //返回容器中元素的个数
   `empty()`; //判断容器是否为空
   `resize(num)`; //重新指定容器的长度为num，若容器变长，则以默认值填充新位置，如果容器 变短，则末尾超出容器长度的元素被删除
   `resize(num,elem);` //重新指定容器的长度为num，若容器边长，则以elem填充新位置，如果   容器变短，则末尾超出容器长度的元素被删除
   
   4. list 插入和删除
   功能：对list容器进行数据的插入和删除
   函数原型：
   `push_back(elem);` //在容器尾部加入一个元素
   `pop_back();` //删除容器中最后一个元素
   `push_front(elem);` //在容器开头插入一个元素
   `pop_front();` //从容器开始移除第一个元素
   
   `insert(pos,elem);` //在pos位置插入elem元素的拷贝，返回新数据的位置
   `insert(pos,n,elem);` //在pos位置插入n个elem数据，无返回值
   `insert(pos,beg,end);` //在pos位置插入[beg,end)区间的数据，无返回值
   `clear();` //移除容器的所有数据
   `erase(beg,end);` //删除[beg,end)区间的数据，返回下一个数据的位置
   `erase(pos);` //删除pos位置的数据，返回下一个数据的位置
   `remove(elem);` //删除容器中所有与elem值匹配的元素
   
    插入单个元素：
    `iterator insert (const_iterator pos, const T& value);`
    插入多个相同元素:
    `void insert (const_iterator pos, size_type count, const T& value);`
    插入一个范围的元素：
    `void insert (const_iterator pos, InputIterator first, InputIterator last);`
   
    在 `std::list` 中，你不能直接使用 `it + position` 来确定插入位置，因为 `std::list` 是双向链表，它不支持随机访问迭代器。`std::list` 的迭代器不支持 + 或 - 操作来移动位置，像 std::vector 那样的随机访问迭代器只有 std::vector 和 std::deque 支持。

    为了在 std::list 中插入元素，你需要使用迭代器来遍历列表或者使用 std::advance 函数来移动迭代器。
    ```cpp
    for (const auto& person : people) {
        int position = person[1];
        auto it = que.begin();
        std::advance(it, position); // 移动迭代器到正确的位置
        que.insert(it, person);
    }
    ```
    `std::advance` 函数：用于在迭代器上移动。它接受一个迭代器和一个整数值，移动迭代器 n 个位置（正值为向前移动，负值为向后移动）。这个函数对于 `std::list` 是有效的，因为 `std::list` 支持双向迭代器。

    在 std::list 中，虽然不能使用随机访问操作（如 it + position），但是你可以使用迭代器的自增操作（it++ 或 ++it）来移动到正确的位置。这是因为 std::list 支持双向迭代器，可以前向或后向遍历。
    
    双向迭代器：std::list 提供的迭代器是双向迭代器，支持前向（++it）和后向（--it）移动，但不支持随机访问操作（如 it + n）。因此，你可以通过循环增加迭代器来达到目标位置。

   逐步遍历：当你使用 it++ 或 ++it 来移动迭代器时，你实际上是逐步遍历链表的元素。尽管这个操作比随机访问慢，但它在链表的设计中是有效的。 



   5. list访问元素
   
   功能：对list容器中数据进行存取
   
   函数原型：
   
   `front();` //返回第一个元素
   `back();` //返回最后一个元素
   
   注意：list没有[]和at()，不支持随机访问
   
   6. list反转和排序
   
   功能：将容器中的元素反转，以及将容器中的数据进行排序
   
   函数原型：
   	
   `reverse();` //反转链表
   `sort();` //链表排序
   `merge(lst)：`将另一个链表lst合并到当前链表。
   `splice(pos, lst)：`将lst链表的内容插入到当前链表的pos位置。
   `remove(value)：`移除链表中所有值为value的元素。
   `unique()：`移除链表中连续相同的元素。
# 22 `<pthread.h>`
## 1. `pthread_create`
用于创建新的线程

函数原型
```cpp
#include <pthread.h>

int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void*), void *arg);
```
参数
  thread：指向 pthread_t 类型的指针，用于接收新线程的标识符。

  attr：线程属性对象的指针。如果为 NULL，则使用默认的线程属性。

  start_routine：指向线程函数的指针。**线程开始时将调用这个函数**。该函数必须接受一个 void* 类型的参数，并返回一个 void* 类型的结果。

  arg：传递给 start_routine 函数的参数。它是一个 void* 类型的指针，可以指向任何数据类型。

返回值
  成功：返回 0。
  失败：返回一个错误码，表示创建线程失败。

```cpp
#include <iostream>
#include <pthread.h>

// 线程函数
void* thread_function(void* arg) {
    int* num = static_cast<int*>(arg);
    std::cout << "Thread received number: " << *num << std::endl;
    return nullptr;
}

int main() {
    pthread_t thread;
    int number = 42;

    // 创建线程
    if (pthread_create(&thread, nullptr, thread_function, &number) != 0) {
        std::cerr << "Failed to create thread" << std::endl;
        return 1;
    }

    // 等待线程结束
    pthread_join(thread, nullptr);

    std::cout << "Thread has finished execution" << std::endl;
    return 0;
}
```
线程函数：
  线程函数 thread_function 必须符合 void*(* )(void*) 的签名。

参数传递：
  使用 void* 作为参数传递方式，可以通过类型转换将其还原为实际的数据类型。

线程同步： 
  pthread_join 用于等待线程结束，确保主线程在子线程结束之前不会退出。

编译和链接
  为了使用 POSIX 线程库，你需要在编译时链接 pthread 库：
  `g++ -o my_program my_program.cpp -pthread`

## 2. `pthread_join`

pthread_join 是 POSIX 线程库中的一个函数，用于等待指定线程结束并获取线程的退出状态。这个函数在多线程编程中用于确保线程执行完成，处理线程的返回值，并进行必要的清理工作。

函数原型
```cpp
#include <pthread.h>

int pthread_join(pthread_t thread, void **retval);
```
参数
thread：
  需要等待的线程标识符。这个标识符是由 pthread_create 创建线程时返回的 pthread_t 类型的值。
retval：
  指向 void* 类型的指针，用于接收线程函数返回的值。如果线程函数没有返回值或你不需要线程返回值，可以传递 NULL。

返回值
  成功：返回 0。
  失败：返回一个错误码，表示线程等待失败。

```cpp
#include <iostream>
#include <pthread.h>

// 线程函数
void* thread_function(void* arg) {
    int* num = static_cast<int*>(arg);
    std::cout << "Thread received number: " << *num << std::endl;
    
    int* result = new int(*num * 2); // 动态分配返回值
    pthread_exit(result); // 线程退出并返回值
}

int main() {
    pthread_t thread;
    int number = 42;
    void* retval;

    // 创建线程
    if (pthread_create(&thread, nullptr, thread_function, &number) != 0) {
        std::cerr << "Failed to create thread" << std::endl;
        return 1;
    }

    // 等待线程结束并获取返回值
    if (pthread_join(thread, &retval) != 0) {
        std::cerr << "Failed to join thread" << std::endl;
        return 1;
    }

    if (retval != nullptr) {
        int result = *static_cast<int*>(retval);
        std::cout << "Thread returned: " << result << std::endl;
        delete static_cast<int*>(retval); // 释放动态分配的内存
    }

    std::cout << "Thread has finished execution" << std::endl;
    return 0;
}

```
线程函数返回值：
  线程函数可以通过 pthread_exit 返回一个 void* 类型的值。这个值可以通过 pthread_join 获取。

多次等待：
  每个线程只能被 pthread_join 调用一次。如果你试图多次 pthread_join 同一个线程，第二次及以后的调用将失败。

线程退出：
  pthread_join 确保线程资源被回收。如果线程结束后没有调用 pthread_join，线程会进入“僵尸”状态，直到所有线程资源被回收。

线程状态：
  线程在被 pthread_join 等待之前必须已经结束。否则，pthread_join 将阻塞直到线程结束。

## 3. `pthread_detach`
用于将一个线程的状态设置为 "detached" (分离状态)。分离状态的线程在终止时会**自动释放其占用的资源**，而不需要其他线程调用 pthread_join 来回收它的资源。

使用场景
  当你确定不需要获取某个线程的返回值或等待其结束时，可以将该线程设置为分离状态，这样可以避免资源泄漏，同时减少线程管理的复杂性。

`int pthread_detach(pthread_t thread);`
参数:
  thread: 需要分离的线程的线程标识符。
返回值:
  成功时返回 0。
  失败时返回错误码。

不可与 pthread_join 一起使用:
  一旦线程被分离，它的资源在终止时将自动释放，不能再对其调用 pthread_join，否则会返回错误。

避免资源泄漏:
  如果不分离线程，也不调用 pthread_join，线程终止后其资源不会被释放，从而导致内存泄漏。因此，在不关心线程结束状态时，应该使用 pthread_detach。

时机:
  pthread_detach 通常在创建线程后立即调用，或者在明确不再需要等待线程完成的地方调用。

```cpp
#include <pthread.h>
#include <iostream>

void* thread_function(void* arg) {
    std::cout << "Thread is running." << std::endl;
    // 模拟线程工作
    pthread_exit(nullptr);
}

int main() {
    pthread_t thread;
    
    // 创建线程
    if (pthread_create(&thread, nullptr, thread_function, nullptr) != 0) {
        std::cerr << "Failed to create thread." << std::endl;
        return 1;
    }
    
    // 将线程设置为分离状态
    if (pthread_detach(thread) != 0) {
        std::cerr << "Failed to detach thread." << std::endl;
        return 1;
    }
    
    std::cout << "Thread detached. Main thread continues." << std::endl;
    
    // 主线程继续运行，而不等待新线程
    pthread_exit(nullptr); // 或使用 return 0; 以防主线程结束太快
    return 0;
}
```


## 4. `pthread_exit`
用于终止调用线程的执行，并返回一个指向任意数据类型的指针作为退出状态。它不会终止整个进程，只会终止调用的线程，同时释放线程占用的资源。

`void pthread_exit(void *retval);`

retval: 一个指针，指向线程的退出状态（可以是任意类型的指针）。如果线程的退出状态需要在 pthread_join 中使用，可以传递给 retval。

返回值:
  该函数不返回值，因为调用它的线程会立即终止。

正常退出:
  线程可以通过 pthread_exit 正常退出并返回退出状态。
主线程退出:
  当主线程调用 pthread_exit 时，它会等待所有未分离的线程（即没有调用 pthread_detach 的线程）终止。
返回线程函数:
  在线程函数中使用 return，也可以让线程正常退出，此时返回值相当于调用 pthread_exit 时的参数。

```cpp
#include <pthread.h>
#include <iostream>

void* thread_function(void* arg) {
    int* num = static_cast<int*>(arg);
    std::cout << "Thread received value: " << *num << std::endl;
    
    int result = (*num) * 2;  // 假设处理并生成结果
    pthread_exit(static_cast<void*>(&result));  // 返回处理结果
}

int main() {
    pthread_t thread;
    int arg = 10;
    int* thread_result;

    // 创建线程
    if (pthread_create(&thread, nullptr, thread_function, static_cast<void*>(&arg)) != 0) {
        std::cerr << "Failed to create thread." << std::endl;
        return 1;
    }

    // 等待线程结束，并获取其返回值
    if (pthread_join(thread, reinterpret_cast<void**>(&thread_result)) != 0) {
        std::cerr << "Failed to join thread." << std::endl;
        return 1;
    }

    std::cout << "Thread returned value: " << *thread_result << std::endl;
    return 0;
}

```








