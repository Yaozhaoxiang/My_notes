### 1. c++有没有Gc垃圾回收

C++11 引入了智能指针，包括 std::unique_ptr、std::shared_ptr 和 std::weak_ptr，它们可以自动管理动态分配的内存，避免手动管理内存带来的问题，在一定程度上起到了类似垃圾回收的作用。

第三方垃圾回收库
除了智能指针，也有一些第三方库可以为 C++ 提供垃圾回收功能，例如 Boehm-Demers-Weiser 保守式垃圾回收器（Boehm GC）。

Boehm GC (“贝姆吉西”)
Boehm GC 是一个广泛使用的 C 和 C++ 垃圾回收库，它可以自动检测和回收不再使用的内存。使用 Boehm GC 时，开发者不需要手动调用 new 和 delete，而是使用 GC 提供的内存分配函数。

```cpp
#include <iostream>
#include <gc/gc.h>

int main() {
    // 使用 Boehm GC 分配内存
    int* ptr = (int*)GC_MALLOC(sizeof(int));
    *ptr = 40;
    std::cout << *ptr << std::endl;
    // 不需要手动释放内存，Boehm GC 会自动回收
    return 0;
}
```

底层原理：

Boehm GC 采用的是保守式垃圾回收策略，这意味着它并不精确地知道哪些内存区域是指针，而是通过扫描内存中的值，猜测哪些可能是指针。与之相对的是精确式垃圾回收，精确式垃圾回收器明确知道哪些内存位置是指针。


1. 标记阶段

+ 根集合扫描：垃圾回收的第一步是确定根集合（Root Set），根集合包含了程序中直接可访问的对象，例如全局变量、栈上的变量等。Boehm GC 会从根集合开始，扫描这些根对象所指向的内存地址。
+ 指针猜测与标记：在扫描过程中，Boehm GC 会检查内存中的每个值，猜测它是否是一个指针。由于采用保守式策略，它会把所有可能是指针的值都当作指针处理。对于被认为是指针的值，它会标记该指针所指向的对象为 “存活” 状态。然后，递归地对这些存活对象所指向的其他对象进行同样的标记操作，直到所有可达对象都被标记。

2. 清除阶段
+ 遍历堆内存：标记阶段完成后，Boehm GC 会遍历整个堆内存，检查每个对象的标记状态。
+ 回收未标记对象：那些未被标记为 “存活” 的对象被认为是不可达对象，也就是垃圾对象。Boehm GC 会回收这些垃圾对象所占用的内存空间，将其标记为可用，以便后续的内存分配使用。

关键技术与机制
1. 内存分配
Boehm GC 有自己的内存分配器，当程序需要分配内存时，会调用 Boehm GC 的内存分配函数。它会从预先分配的内存池中分配内存给程序使用。如果内存池中的内存不足，它会向操作系统请求更多的内存。
2. 内存对齐与块管理
为了提高内存访问效率和便于管理，Boehm GC 会对内存进行对齐处理。它将堆内存划分为不同大小的块，每个块可以包含一个或多个对象。在标记和清除过程中，以块为单位进行操作，这样可以减少扫描的粒度，提高回收效率。

### 2. 内存泄漏？怎么检测？如果开发阶段没有问题，但是运行的时候内存泄漏怎么排查定位？

内存泄漏指的是程序在运行过程中，由于某些原因导致已经不再使用的内存没有被及时释放，从而使得可用内存逐渐减少。随着程序的持续运行，内存泄漏可能会导致系统性能下降，甚至引发程序崩溃。在像 C 和 C++ 这类需要手动管理内存的语言中，内存泄漏是比较常见的问题。

使用静态代码分析工具
+ 原理：静态代码分析工具会在不运行代码的情况下，对代码进行语法和语义分析，检查代码中可能存在的内存泄漏问题。
+ 工具示例：
    + Cppcheck：是一个开源的 C/C++ 静态代码分析工具，它可以检查代码中的内存泄漏、未初始化变量等问题。

运行阶段排查定位内存泄漏的方法：

1. 使用内存分析工具
+ Valgrind（适用于 Linux 系统）
    + 原理：Valgrind 是一个强大的内存调试和分析工具，它会在程序运行时对内存的使用情况进行监控。当程序进行内存分配和释放操作时，Valgrind 会记录相关信息，并在程序结束后生成详细的报告，指出可能存在的内存泄漏问题。
    + 使用示例：假设你有一个 C++ 程序 test.cpp，编译生成可执行文件 test 后，可以使用以下命令运行程序并进行内存分析：

```bash
valgrind --leak-check=full ./test
```

报告解读：Valgrind 的报告中会显示哪些内存块没有被正确释放，以及这些内存块是在哪个函数中分配的，帮助你定位问题代码。

> Cppcheck 的使用方法
> cppcheck file1.cpp file2.cpp file3.cpp

#### 制造内存泄漏工具
https://www.zhihu.com/question/29859828/answer/1798470821

众所周知C++中申请和释放内存使用的是new和delete关键字：

再明确下需求：如果程序中存在内存泄漏，我们的目的是找到这些内存是在哪里分配的，如果能够具体对应到代码中哪一个文件的那一行代码最好。

内存在哪里释放的我们没必要监测，只需要检测出内存是在哪里申请的即可，如何检测呢？

整体思路很简单：在申请内存时记录下该内存的地址和在代码中申请内存的位置，在内存销毁时删除该地址对应的记录，程序最后统计下还有哪条记录没有被删除，如果还有没被删除的记录就代表有内存泄漏。

很多人应该都知道new关键字更底层是通过operator new来申请内存的：

> void* operator new(std::size_t sz)

也就是正常情况下C++都是通过operator new(std::size_t sz)来申请内存，而这个操作符我们可以重载：

> void* operator new(std::size_t size, const char* file, int line);
> void* operator new[](std::size_t size, const char* file, int line);

怎么能够让底层程序申请内存时调用重载的这个函数呢？这里可以对new使用宏定义：

> #define new new (__FILE__, __LINE__)

有了这个宏定义后，在new A的时候底层就会自动调用operator new(std::size_t size, const char* file, int line)函数，至此达到了我们记录内存申请位置的目的。

这里有两个问题：
1. 在哪里记录内存申请的位置等信息呢？如果在operator new内部又申请了一块内存，用于记录位置，那新申请的这块内存需要记录不？这岂不是递归调用了？
2. 只有在new宏定义包裹范围内申请了内存才会被记录，然而某些第三方库或者某些地方没有被new宏定义包裹，可能就无法被监测是否申请了内存吧？

哪里存储具体信息？
我们肯定不能让它递归调用啊，那这些信息存储在哪里呢？这里可以在每次申请内存时，一次性申请一块稍微大点的内存，具体信息存储在多余的那块内存里，像这样：

```cpp
链接：https://www.zhihu.com/question/29859828/answer/1798470821

static void* alloc_mem(std::size_t size, const char* file, int line, bool is_array) {
    assert(line >= 0);

    std::size_t s = size + ALIGNED_LIST_ITEM_SIZE;
    new_ptr_list_t* ptr = (new_ptr_list_t*)malloc(s);
    if (ptr == nullptr) {
        std::unique_lock<std::mutex> lock(new_output_lock);
        printf("Out of memory when allocating %lu bytes\n", (unsigned long)size);
        abort();
    }
    void* usr_ptr = (char*)ptr + ALIGNED_LIST_ITEM_SIZE;

    if (line) {
        strncpy(ptr->file, file, _DEBUG_NEW_FILENAME_LEN - 1)[_DEBUG_NEW_FILENAME_LEN - 1] = '\0';
    } else {
        ptr->addr = (void*)file;
    }

    ptr->line = line;
    ptr->is_array = is_array;
    ptr->size = size;
    ptr->magic = DEBUG_NEW_MAGIC;
    {
        std::unique_lock<std::mutex> lock(new_ptr_lock);
        ptr->prev = new_ptr_list.prev;
        ptr->next = &new_ptr_list;
        new_ptr_list.prev->next = ptr;
        new_ptr_list.prev = ptr;
    }
    total_mem_alloc += size;
    return usr_ptr;
}
```

new_ptr_list_t结构体定义如下：
```cpp
struct new_ptr_list_t {
    new_ptr_list_t* next;
    new_ptr_list_t* prev;
    std::size_t size;
    union {
        char file[200];

        void* addr;
    };
    unsigned line;
};
```

没有被new宏包裹的地方可以检测的到吗？

没有被new宏包裹的地方是会调用operator new(std::size_t sz)函数来申请内存的。这里operator new函数不只可以重载，还可以重新定义它的实现，而且不会报multi definition的错误哦。因为它是一个weak symbol

既然可以重定义，那就可以这样：
```cpp
void* operator new(std::size_t size) { 
    return operator new(size, nullptr, 0); 
}
```

这样有个缺点，就是不能记录内存申请的具体代码位置，只能记录下来是否申请过内存，不过这也挺好，怎么也比没有任何感知强的多。

其实这里不是没有办法，尽管没有了new宏，获取不到具体申请内存的代码位置，但是可以获取到调用栈信息，把调用栈信息存储起来，还是可以定位大体位置。关于如何获取调用栈信息，大家可以研究下libunwind库看看。

释放内存时怎么办？
这里需要重定义operator delete(void* ptr)函数：

```cpp
void operator delete(void* ptr) noexcept { 
    free_pointer(ptr, nullptr, false); 
}
```

free_pointer函数的大体思路就是在链表中找到要对应节点，删除掉，具体定义如下：

```cpp
static void free_pointer(void* usr_ptr, void* addr, bool is_array) {
    if (usr_ptr == nullptr) {
        return;
    }
    new_ptr_list_t* ptr = (new_ptr_list_t*)((char*)usr_ptr - ALIGNED_LIST_ITEM_SIZE);
    {
        std::unique_lock<std::mutex> lock(new_ptr_lock);
        total_mem_alloc -= ptr->size;
        ptr->magic = 0;
        ptr->prev->next = ptr->next;
        ptr->next->prev = ptr->prev;
    }
    free(ptr);
}

```

如何检测是否有内存泄漏？

遍历链表即可，每次new时候会把这段内存插入链表，delete时候会把这段内存从链表中移出，如果程序最后链表长度不为0，即为有内存泄漏，代码如下：

```cpp
int checkLeaks() {
    int leak_cnt = 0;
    int whitelisted_leak_cnt = 0;
    new_ptr_list_t* ptr = new_ptr_list.next;

    while (ptr != &new_ptr_list) {
        const char* const usr_ptr = (char*)ptr + ALIGNED_LIST_ITEM_SIZE;
        printf("Leaked object at %p (size %lu, ", usr_ptr, (unsigned long)ptr->size);
        if (ptr->line != 0) {
            print_position(ptr->file, ptr->line);
        } else {
            print_position(ptr->addr, ptr->line);
        }
        printf(")\n");
        ptr = ptr->next;
        ++leak_cnt;
    }
    return leak_cnt;
}
```

### 3. std::unordered_map 和 std::map,怎么处理哈希冲突? hash的实现？实现线程安全的写法？哈希冲突解决？unordered_map的扩容原理和缩容？手撕一个mst

> 常见的散列函数

1）线性定址法：直接取关键字的某个线性函数作为存储地址，散列函数为：

Hash(key)=a×key+b


2）除留余数法：将关键字对某一小于散列表长度的数p取余的结果作为存储地址，散列函数为：

Hash(key)=key mod p

3）平方取中法：对关键字取平方，然后将得到结果的中间几位作为存储地址；

4）折叠法：将关键字分割为几部分，然后将这几部分的叠加和作为存储地址。

> 地址冲突解决方法

 通过以上方法构建的哈希表在理想的情况下能够做到一个关键字对应一个地址，但是实际情况是会有冲突发生，也就是散列函数会将多个关键字映射到同一个地址上。以下是一些解决冲突的方法：

 1）开放地址法：
    ①线性探测法：当发生冲突时，就顺序查看下一个存储位置，如果位置为空闲状态，就将该关键字存储在该位置上，如果还是发生冲突，就依次往后查看，当查看到存储空间的末尾时还是找不到空位置，就返回从头开始查看；

2）拉链法
    将具有相同存储地址的关键字链成一单链表， m个存储地址就设m个单链表，然后用一个数组将m个单链表的表头指针存储起来，形成一个动态的结构，假设散列函数为 Hash(key) = key %13，其拉链存储结构为：

```cpp
#include <iostream>
#include <list>
#include <vector>

template <typename K, typename V>
class HashTable {
private:
    // 哈希表的大小
    int tableSize;
    // 存储链表的向量
    std::vector<std::list<std::pair<K, V>>> table;

    // 简单的哈希函数
    int hashFunction(const K& key) const {
        return std::hash<K>()(key) % tableSize;
    }

public:
    // 构造函数
    HashTable(int size) : tableSize(size) {
        table.resize(tableSize);
    }

    // 插入键值对
    void insert(const K& key, const V& value) {
        int index = hashFunction(key);
        for (auto& pair : table[index]) {
            if (pair.first == key) {
                // 如果键已存在，更新值
                pair.second = value;
                return;
            }
        }
        // 若键不存在，插入新的键值对
        table[index].emplace_back(key, value);
    }

    // 查找键对应的值
    bool find(const K& key, V& value) const {
        int index = hashFunction(key);
        for (const auto& pair : table[index]) {
            if (pair.first == key) {
                value = pair.second;
                return true;
            }
        }
        return false;
    }

    // 删除键值对
    bool remove(const K& key) {
        int index = hashFunction(key);
        for (auto it = table[index].begin(); it != table[index].end(); ++it) {
            if (it->first == key) {
                table[index].erase(it);
                return true;
            }
        }
        return false;
    }
};

#include <iostream>

int main() {
    HashTable<std::string, int> hashTable(10);

    // 插入键值对
    hashTable.insert("apple", 10);
    hashTable.insert("banana", 20);
    hashTable.insert("cherry", 30);

    // 查找键值对
    int value;
    if (hashTable.find("banana", value)) {
        std::cout << "Value of banana: " << value << std::endl;
    } else {
        std::cout << "Key not found." << std::endl;
    }

    // 删除键值对
    if (hashTable.remove("cherry")) {
        std::cout << "Key cherry removed." << std::endl;
    } else {
        std::cout << "Key cherry not found." << std::endl;
    }

    return 0;
}    
```


std::unordered_map 的实现（基于哈希表）

基本原理
std::unordered_map 使用哈希表来存储键值对。哈希表是一种根据键（key）直接访问内存存储位置的数据结构，它通过哈希函数将键映射到一个固定大小的数组中的某个位置，这个数组被称为哈希桶（bucket）。

实现步骤
+ 哈希函数：对于插入的每个键，哈希函数会计算出一个哈希值，这个哈希值通常是一个整数。
+ 桶索引计算：通过哈希值和桶的数量，计算出该键应该存储在哪个桶中。
+ 冲突处理：由于不同的键可能会产生相同的哈希值，即哈希冲突，需要使用某种方法来处理冲突。常见的冲突处理方法有链地址法（separate chaining）和开放地址法（open addressing），std::unordered_map 通常使用链地址法，即每个桶中存储一个链表或其他容器，当发生冲突时，将新的键值对插入到对应的链表中。

链地址法:实现
具体来说，哈希表是一个数组，每个数组元素（即哈希桶）指向一个链表，当插入一个键值对时，先通过哈希函数计算出对应的桶索引，然后将该键值对插入到该桶对应的链表中。扩容和缩容操作主要是为了保持哈希表的**负载因子**在一个合理的范围内，从而保证哈希表的性能。负载因子（Load Factor）定义为**哈希表中元素的数量与桶的数量之比**。


扩容原理:
1. 计算新容量：通常新容量是当前容量的两倍，但不同的实现可能会有所差异。
2. 创建新的哈希表：根据新容量创建一个新的哈希表。
3. 重新哈希：将原哈希表中的所有元素重新计算哈希值，并插入到新的哈希表中。
4. 替换原哈希表：将新的哈希表替换原有的哈希表，释放原哈希表的内存。

扩容的触发条件通常是负载因子（元素数量与桶数量的比值）超过了某个阈值（默认情况下，std::unordered_map 的负载因子阈值为 1.0）。

缩容
std::unordered_map 标准库实现通常没有自动缩容机制。这是因为缩容操作同样需要重新哈希，会带来较大的性能开销。不过，在某些情况下，如果开发者明确知道元素数量会大幅减少，可以手动调用 reserve 方法来减少桶的数量，从而达到类似缩容的效果。




```cpp
#include <iostream>
#include <vector>
#include <list>
#include <functional>

template <typename K, typename V>
class UnorderedMap {
private:
    // 哈希表的桶数组
    std::vector<std::list<std::pair<K, V>>> buckets;
    // 元素数量
    size_t size_;
    // 负载因子阈值
    static constexpr double loadFactorThreshold = 1.0;

    // 哈希函数
    size_t hash(const K& key) const {
        return std::hash<K>()(key) % buckets.size();
    }

    // 扩容函数
    void rehash() {
        std::vector<std::list<std::pair<K, V>>> oldBuckets = buckets;
        // 新容量为原容量的两倍
        buckets.resize(buckets.size() * 2);
        size_ = 0;

        // 重新哈希所有元素
        for (const auto& bucket : oldBuckets) {
            for (const auto& pair : bucket) {
                insert(pair.first, pair.second);
            }
        }
    }

public:
    // 构造函数，初始化桶的数量
    UnorderedMap(size_t initialCapacity = 16) : buckets(initialCapacity), size_(0) {}

    // 插入键值对
    void insert(const K& key, const V& value) {
        if ((double)size_ / buckets.size() >= loadFactorThreshold) {
            rehash();
        }

        size_t index = hash(key);
        for (auto& pair : buckets[index]) {
            if (pair.first == key) {
                pair.second = value;
                return;
            }
        }

        buckets[index].emplace_back(key, value);
        ++size_;
    }

    // 查找键对应的值
    bool find(const K& key, V& value) const {
        size_t index = hash(key);
        for (const auto& pair : buckets[index]) {
            if (pair.first == key) {
                value = pair.second;
                return true;
            }
        }
        return false;
    }

    // 获取元素数量
    size_t size() const {
        return size_;
    }
};

#include <iostream>

int main() {
    UnorderedMap<std::string, int> map;

    // 插入键值对
    map.insert("apple", 10);
    map.insert("banana", 20);
    map.insert("cherry", 30);

    // 查找键值对
    int value;
    if (map.find("banana", value)) {
        std::cout << "Value of banana: " << value << std::endl;
    } else {
        std::cout << "Key not found." << std::endl;
    }

    return 0;
}    

```


开放寻址法
原理
开放寻址法不使用额外的链表结构，当发生哈希冲突时，会在哈希表中寻找下一个可用的位置来存储冲突的键值对。寻找下一个位置的方式有多种，常见的有线性探测、二次探测和双重哈希等。

+ 线性探测：如果当前位置已经被占用，就依次检查下一个位置，直到找到一个空位置。例如，哈希函数计算出的位置为 i，如果该位置被占用，就检查 i + 1、i + 2 等位置。
+ 二次探测：探测的步长是二次方关系，如检查 i + 1²、i + 2²、i + 3² 等位置。
+ 双重哈希：使用两个不同的哈希函数，当发生冲突时，使用第二个哈希函数来计算下一个探测位置。




### 4. 红黑树有什么优势?红和黑代表什么?

https://juejin.cn/post/6844904020549730318
https://juejin.cn/post/6844904097141768199

我们知道ALV树是一种严格按照定义来实现的平衡二叉查找树，所以它查找的效率非常稳定，为O(log n),由于其严格按照左右子树高度差不大于1的规则，插入和删除操作中需要大量且复杂的操作来保持ALV树的平衡（左旋和右旋），因此ALV树适用于大量查询，少量插入和删除的场景中

那么假设现在假设有这样一种场景：大量查询，大量插入和删除，现在使用ALV树就不太合适了，因为ALV树大量的插入和删除会非常耗时间，那么我们是否可以降低ALV树对平衡性的要求从而达到快速的插入和删除呢？

答案肯定是有的，红黑树这种数据结构就应运而生了（因为ALV树是高度平衡的，所以查找起来肯定比红黑树快，但是红黑树在插入和删除方面的性能就远远不是ALV树所能比的了）

红色和黑色是为了更快的判断此树是不是满足平衡的条件。

说明为啥要标颜色，其实只用4和5两个性质，再结合一点点代码。这个算法实现的时候，每次假定插入的新节点是红色的，因为红色的不会影响路径上黑色节点的数量，也就不会改变黑色路径的长度。这时候就会出现两种情况，它的父亲节点是黑色的话，皆大欢喜啊，完美的满足了插入一个节点，又不影响黑色路径的长度这个条件，插入结束。但如果它的父亲是红色的呢？条件4说不能有两个连续的红色节点啊，那好吧，我把它父亲改成黑色的就可以了，但是，这一改，路径上凭空多了一个黑色的节点，那么黑色路径也就加长了，可能会引起失衡啊。为了判断是否失衡，就要扩大范围多看一下了。

假设，我们现在真的要改父亲节点的颜色了，去看一下它父亲节点的兄弟节点（我叫他叔叔节点）。如果叔叔节点和父亲节点同为红色，那就把这哥俩都变成黑色，左右子树同时增加黑色路径长度，那么新增节点的爷爷节点就是平衡的，这时候就可以放心的去看爷爷节点的父亲（祖爷爷节点）的两个儿子是不是平衡就可以了，这是一个递归的过程。但如果，叔叔节点为黑色呢？这个时候只是单方面的把父亲节点变成了黑色，那么父亲节点这边的子树就比叔叔节点那边高了一层，这个时候就失衡了，会让爷爷节点无法接受，并且能肯定的是，此时是无法通过改变颜色来实现平衡的，所以这个时候就要进行旋转了。

总结成颜色的语言就是，爸爸是黑色的，皆大欢喜。爸爸和叔叔是红色的，只用改成黑色。爸爸是红色的，叔叔是黑色的，需要进行旋转。以上，就是把树标成红色和黑色的意义，只用简单的判断颜色，就能确定应该怎么调整。

扩展来看，红色和黑色只有两种颜色，其最糟糕的情况就是一边子树全是黑色，而另一边子树是红黑相间的，由性质1和性质3可知，这棵树的两头都是黑色的，所以红黑相间的子树只可能比全黑的子树高一倍（黑色路径是另一边的两倍），这也是红黑树所追求的近似平衡。


### 5. vector的resize和reshape有什么区别

resize 函数的主要作用是改变 std::vector 容器中元素的数量。它会根据传入的参数对容器进行调整，可能会增加新元素，也可能会删除现有元素。

reserve 函数用于预先分配一定大小的内存空间，以避免在后续插入元素时频繁进行内存重新分配，从而提高插入操作的效率。它只影响容器的容量（即容器可以容纳的元素数量），而不改变容器中实际元素的数量。

### 6. vector是分配在堆还是栈,sizeof(vector)返回什么值?

std::vector 对象本身的分配位置取决于其定义方式，既可以分配在栈上，也可以分配在堆上。

```cpp
#include <iostream>
#include <vector>

int main() {
    std::vector<int> vec;  // vec 对象分配在栈上
    return 0;
}
```

vec 是一个局部变量，其内存是在栈上分配的。栈上的内存由编译器自动管理，当函数执行结束时，vec 对象会自动销毁。

```cpp
#include <iostream>
#include <vector>

int main() {
    std::vector<int>* vecPtr = new std::vector<int>();  // vecPtr 指向的对象分配在堆上
    delete vecPtr;  // 需要手动释放堆上的内存
    return 0;
}
```
当使用 new 操作符动态创建 std::vector 对象时，它会被分配在堆上

vector<int> v={1,2,3};v是存放在堆中还是栈中？
v本身是个栈对象，但里面allocator分配的空间是在堆中;std::vector 使用 allocator（默认是 std::allocator）来管理内存的分配和释放，而 allocator 分配的内存是在堆上进行的。

> + 都用于内存分配：allocator 和 malloc 都可以用于在程序运行时分配内存。malloc 是 C 语言标> 准库中的函数，用于分配指定大小的内存块；allocator 是 C++ 标准库中的类模板，也能实现内存的> 分配功能。
> + 底层可能依赖 malloc：在某些实现中，std::allocator 的 allocate 成员函数底层可能会调用 > malloc 来完成实际的内存分配操作。这是因为 malloc 是一种成熟且高效的内存分配方式，很多 C++ > 标准库的实现会借助它来实现内存分配功能。
> 
> 区别
> + 类型安全：
>   + allocator 是类型安全的，它在分配内存时会考虑对象的类型信息。例如，std::allocator<int> 只> 能分配 int 类型的内存，并且在构造对象时会调用相应的构造函数。
>   + malloc 是无类型的，它返回的是 void* 类型的指针，需要手动进行类型转换。而且 malloc 只是分配> 内存，不会调用对象的构造函数。
> + 对象构造和销毁：
>   + allocator 提供了 construct 和 destroy 成员函数，用于在已分配的内存上构造和销毁对象。这使> 得内存分配和对象构造可以分离，符合 C++ 的面向对象编程思想。
>   + malloc 只负责分配内存，不涉及对象的构造和销毁。如果需要构造对象，需要使用 placement new 来> 手动调用构造函数；销毁对象时，需要手动调用析构函数。
> + 异常处理：
>   + allocator 的 allocate 成员函数在内存分配失败时会抛出 std::bad_alloc 异常，符合 C++ 的异> 常处理机制。
>   + malloc 在内存分配失败时返回 NULL 指针，需要手动检查返回值来处理内存分配失败的情况。
> + 内存管理粒度：
>   + allocator 可以与 C++ 标准库容器紧密集成，为容器提供定制化的内存管理策略。不同的容器可以根> 据自身的需求选择不同的分配器。
>   + malloc 是一个通用的内存分配函数，它的内存管理粒度相对较粗，不能很好地与 C++ 容器集成。



sizeof(std::vector) 返回的值
+ sizeof(std::vector) 返回的是 std::vector 对象本身的大小，而不是它所管理的元素占用的内存大小。std::vector 对象内部通常包含几个指针和一些其他的成员变量，用于管理元素的存储和状态。这些成员变量的具体内容和数量可能因编译器和实现而异，但一般来说，std::vector 对象包含指向元素存储区域的指针、当前元素数量和容量等信息。

在常见的 64 位系统上，sizeof(std::vector<int>) 通常返回 24 字节。这是因为 std::vector 对象可能包含三个指针（指向元素存储区域的起始位置、当前元素的末尾位置和容量的末尾位置），每个指针在 64 位系统上占用 8 字节。

需要注意的是，sizeof(std::vector) 不包括 std::vector 所管理的元素占用的内存大小。要获取 std::vector 中元素占用的总内存大小，可以使用 vec.size() * sizeof(T)，其中 vec 是 std::vector 对象，T 是 std::vector 存储的元素类型。

```cpp
    std::vector<int> vec = {1, 2, 3, 4, 5};
    std::cout << "Size of vector elements: " << vec.size() * sizeof(int) << std::endl;
```

### 7. unordered_map中插入一个元素,它原来的iterator还有效吗?

在 std::unordered_map 中插入一个元素后，原来的迭代器（iterator）是否有效取决于插入操作是否引发了哈希表的重新哈希（rehashing）。

当插入元素时，如果 std::unordered_map 的负载因子（load factor）没有超过其最大负载因子（max load factor），那么不会发生重新哈希。在这种情况下，原来的迭代器仍然有效。
负载因子的计算公式为：负载因子 = 元素数量 / 桶数量。当插入元素后负载因子超过最大负载因子时，std::unordered_map 会自动进行重新哈希，增加桶的数量以保持较低的负载因子，从而维持较好的查找性能。

插入操作引发重新哈希
当插入元素导致 std::unordered_map 的负载因子超过最大负载因子时，会发生重新哈希。重新哈希会重新分配桶的数量，并将所有元素重新插入到新的桶中。在这种情况下，原来的迭代器会失效。

+ 若插入元素未引发重新哈希，std::unordered_map 原来的迭代器仍然有效。
+ 若插入元素引发了重新哈希，std::unordered_map 原来的迭代器会失效。在进行插入操作后，如果不确定是否发生了重新哈希，不应该继续使用原来的迭代器，而应该重新获取有效的迭代器。


#### 迭代器失效的情况

+ 对于序列容器 vector，deque 来说，使用 erase 后，后边的每个元素的迭代器都会失效，后边每个元素都往前移动一位，erase 返回下一个有效的迭代器。
+ 对于关联容器 map，set 来说，使用了 erase 后，当前元素的迭代器失效，但是其结构是红黑树，删除当前元素，不会影响下一个元素的迭代器，所以在调用 erase 之前，记录下一个元素的迭代器即可。
+ 对于 list 来说，它使用了不连续分配的内存，并且它的 erase 方法也会返回下一个有效的迭代器，因此上面两种方法都可以使用

##### std::vector
std::vector 是一个动态数组，其迭代器失效主要与内存重新分配和元素删除插入操作有关。

1. 插入元素导致迭代器失效
+ 在中间或开头插入元素：当在 std::vector 的中间或开头插入元素时，如果当前容量不足以容纳新元素，vector 会重新分配一块更大的内存空间，并将原有的元素复制（或移动）到新的内存空间中，然后释放原来的内存。此时，所有指向原内存空间的迭代器都会失效。

+ 在末尾插入元素且容量足够：如果在末尾插入元素且当前容量足够，不会发生内存重新分配，指向插入位置之前的元素的迭代器仍然有效，但指向插入位置之后的元素的迭代器会失效。

2. 删除元素导致迭代器失效
+ 删除元素：当删除 std::vector 中的元素时，被删除元素及其后面的所有元素会向前移动，因此指向被删除元素及其后面元素的迭代器都会失效。

##### std::list

std::list 是一个双向链表，其迭代器失效主要与元素删除操作有关。

1. 插入元素不会导致迭代器失效
在 std::list 中插入元素时，只会在插入位置创建新的节点，不会影响其他节点的内存地址，因此除了指向插入位置的迭代器可能会因为插入操作而改变指向外，其他迭代器仍然有效。

2. 删除元素导致迭代器失效
当删除 std::list 中的元素时，被删除元素的迭代器会失效，而其他迭代器仍然有效。

##### std::map 和 std::set
std::map 和 std::set 是基于红黑树实现的关联容器，其迭代器失效主要与元素删除操作有关。

1. 插入元素不会导致迭代器失效
在 std::map 或 std::set 中插入元素时，只会在树中插入新的节点，不会影响其他节点的内存地址，因此所有迭代器仍然有效。

2. 删除元素导致迭代器失效
当删除 std::map 或 std::set 中的元素时，被删除元素的迭代器会失效，而其他迭代器仍然有效。


### 8. map中插入一个元素,它原来的iterater还有效吗?

在 C++ 的 std::map 中插入一个元素后，原来的迭代器（iterator）仍然有效.

底层实现原理
std::map 是基于红黑树（一种自平衡的二叉搜索树）实现的。红黑树具有以下特点：
+ 节点的有序性：红黑树中的节点按照键的大小进行排序，每个节点包含一个键值对，并且左子树中的所有节点的键都小于当前节点的键，右子树中的所有节点的键都大于当前节点的键。
+ 插入操作的特性：当在 std::map 中插入一个新元素时，红黑树会根据新元素的键的大小找到合适的位置插入新节点，并且在插入后会通过旋转和变色等操作来保持树的平衡。在这个过程中，除了新插入的节点和可能的一些节点的颜色或指针调整外，其他已存在的节点的内存地址不会发生改变。

由于迭代器本质上是指向红黑树中节点的指针或封装了指向节点的指针，而插入操作不会改变已存在节点的内存地址，所以原来的迭代器仍然指向有效的节点，即原来的迭代器仍然有效。

### 9 . share_ptr是不是线程安全的,为什么?

std::shared_ptr 部分操作是线程安全的，但并非所有操作都具备线程安全性:

1. 引用计数操作的线程安全性
std::shared_ptr 的引用计数操作是线程安全的，这主要得益于标准库的实现采用了原子操作来管理引用计数。引用计数用于记录有多少个 std::shared_ptr 实例共享同一个对象，当引用计数变为 0 时，所管理的对象会被自动释放。

2. 非线程安全的操作
虽然引用计数操作是线程安全的，但 std::shared_ptr 的其他一些操作并非线程安全。

+ 对管理对象的访问
如果多个线程同时访问 std::shared_ptr 所管理的对象，并且其中至少有一个线程进行写操作，那么就会出现数据竞争问题，因为 std::shared_ptr 本身并不提供对所管理对象的并发访问保护。

```cpp
#include <iostream>
#include <memory>
#include <thread>

std::shared_ptr<int> sharedData = std::make_shared<int>(0);

void incrementData() {
    for (int i = 0; i < 10000; ++i) {
        ++(*sharedData); // 非线程安全的写操作
    }
}

int main() {
    std::thread t1(incrementData);
    std::thread t2(incrementData);

    t1.join();
    t2.join();

    std::cout << "Final value: " << *sharedData << std::endl;
    return 0;
}
```


### 10. 把unique_ptr move到share_ptr会发送什么?

std::unique_ptr 用于独占式地管理对象的生命周期，同一时间只能有一个 std::unique_ptr 指向某个对象；而 std::shared_ptr 则允许多个 std::shared_ptr 共享同一个对象的所有权，通过引用计数来管理对象的生命周期。当把 std::unique_ptr 移动（std::move）到 std::shared_ptr 时，会发生以下事情：

1. 所有权转移
+ std::unique_ptr 是独占所有权的智能指针，一旦使用 std::move 将其转移到 std::shared_ptr，std::unique_ptr 会失去对原对象的所有权，变为空指针（即 get() 方法返回 nullptr）。
+ std::shared_ptr 会接管该对象的所有权，并开始管理其生命周期。std::shared_ptr 会为这个对象维护一个引用计数，初始值为 1。


2. 引用计数机制启动
+ std::shared_ptr 内部使用引用计数来跟踪有多少个 std::shared_ptr 实例共享同一个对象。当 std::unique_ptr 转移到 std::shared_ptr 后，引用计数被初始化为 1。后续如果有其他 std::shared_ptr 拷贝或赋值这个 std::shared_ptr，引用计数会相应增加；当这些 std::shared_ptr 被销毁或重置时，引用计数会减少。当引用计数变为 0 时，对象会被自动删除

### 11. 堆内存泄露和栈内存泄漏?

栈内存是由操作系统自动管理的内存区域，用于存储函数的局部变量、参数和返回地址等。栈内存泄漏通常指的是由于栈溢出（Stack Overflow）导致的内存问题。栈溢出是指程序在使用栈内存时，超过了栈的最大容量，导致栈内存被耗尽。

产生原因
+ 递归调用过深：递归函数在每次调用时都会在栈上分配一定的内存空间，如果递归调用没有正确的终止条件或者终止条件设置不当，会导致递归调用过深，栈空间不断被占用，最终导致栈溢出。

+ 局部变量占用空间过大：在函数中定义了非常大的局部数组或结构体，可能会导致栈空间被迅速耗尽。例如：

调试器：如 GDB，可以在程序崩溃时查看栈信息，找出栈溢出的位置

### 12. static变量的初始化和线程安全，const

1.静态局部变量 ：

在局部变量之前加上关键字static，局部变量就被定义成为一个局部静态变量。
内存中的位置：data段
初始化：局部的静态变量只能被初始化一次
作用域：作用域仍为局部作用域，当定义它的函数或者语句块结束的时候，作用域随之结束。
> 当static用来修饰局部变量的时候，它就改变了局部变量的存储位置（从原来的栈中存放改为静态存储区）及其生命周期（局部静态变量在离开作用域之后，并没有被销毁，而是仍然驻留在内存当中，直到程序结束，只不过我们不能再对他进行访问），但未改变其作用域。

2.静态全局变量：

在全局变量之前加上关键字static，全局变量就被定义成为一个全局静态变量。
内存中的位置：静态存储区（静态存储区在整个程序运行期间都存在）
初始化：未经初始化的全局静态变量会被程序自动初始化为0
作用域：全局静态变量在声明他的文件之外是不可见的。准确地讲从定义之处开始到文件结尾。(只能在本文件中存在和使用)
>全局变量本身就是静态存储方式， 静态全局变量当然也是静态存储方式。两者的区别在于非静态全局变量的作用域是整个源程序， 当一个源程序由多个源文件组成时，非静态的全局变量在各个源文件中都是有效的（在其他源文件中使用时加上extern关键字重新声明即可）。 而静态全局变量则限制了其作用域， 即只在定义该变量的源文件内有效， 在同一源程序的其它源文件中不能使用它。

3.静态函数：

仅在定义该函数的文件内才能使用。在多人开发项目时，为了防止与他人命名空间里的函数重名，可以将函数定位为 static。（和全局变量一样限制了作用域而已）

4.静态类成员变量：

用static修饰类的数据成员实际使其成为类的全局变量，会被类的所有对象共享，包括派生类的对象。因此，static成员必须在类外进行初始化，而不能在构造函数内进行初始化。不过也可以用const 修饰static数据成员在类内初始化 。

5.静态类成员函数：

用static修饰成员函数，使这个类只存在这一份函数，所有对象共享该函数，不含this指针。
静态成员是可以独立访问的，也就是说，无须创建任何对象实例就可以访问。
不可以同时用const和static修饰成员函数。



+ const
1 const修饰变量：限定变量为不可修改。编译器优化，嵌入到代码中
2 const修饰指针：指针常量和指向常量的指针
3 const和函数：有以下几种形式
```cpp
const int& fun(int& a); //修饰返回值
int& fun(const int& a); //修饰形参，可根据参数是不是const支持重载
int& fun(int& a) const{} //const成员函数
```
const和类：
①const修饰成员变量，在某个对象的声明周期内是常量，但是对于整个类而言是可以改变的。因为类可以创建多个对象，不同的对象其const成员变量的值是不同的。切记，不能在类内初始化const成员变量，因为类的对象没创建前，编译器并不知道const成员变量是什么，因此const数据成员**只能在初始化列表中初始化**。类中mutable可以修改
②const修饰成员函数，主要目的是防止成员函数修改成员变量的值，即该成员函数并不能修改成员变量。
③const对象，常对象，常对象只能调用常函数。


+ static和const可以同时修饰成员函数吗?

不可以。C++编译器在实现const的成员函数的时候为了确保该函数不能修改类的实例的状态，会在函数中添加一个隐式的参数const this*。但当一个成员为static的时候，该函数是没有this指针的。也就是说此时const的用法和static是冲突的。两者的语意是矛盾的。static的作用是表示该函数只作用在类型的静态变量上，与类的实例没有关系；而const的作用是确保函数不能修改类的实例的状态，与类型的静态变量没有关系。因此不能同时用它们。

#### static初始化时机和线程安全问题

c语言：静态局部变量和全局变量一样，数据都存放在全局区域，所以在主程序之前，编译器已经为其分配好了内存，在编译阶段分配好了内存之后就进行初始化，在程序运行结束时变量所处的全局内存会被回收。所以在c语言中无法使用变量对静态局部变量进行初始化。

再说C++和C语言的区别：
c++主要引入了类这种东西，要进行初始化必须考虑到相应的构造函数和析构函数，而且很多时候构造或者析构函数中会指定我们定义的操作，并非简单的分配内存。因此为了造成不必要的影响（一些我不需要的东西被提前构造出来）所以c++规定**全局或者静态对象在首次用到的时候才会初始化**。

所以c++整了两种初始化的情况，我理解就是编译初始化和运行初始化。

编译初始化也叫静态初始化。对全局变量和const类型的初始化主要是，叫做zero initialization 和 const initialization，静态初始化在程序加载的过程中完成。从具体实现上看，zero initialization 的变量会被保存在 bss 段，const initialization 的变量则放在 data 段内，程序加载即可完成初始化，这和 c 语言里的全局变量静态变量初始化基本是一致的。其次全局类对象也是在编译器初始化。

动态初始化也叫运行时初始化。主要是指需要经过函数调用才能完成的初始化或者是类的初始化，一般来说是局部静态类对象的初始化和局部静态变量的初始化。


> 线程安全问题

C++11标准针规定了局部静态变量初始化是线程安全的。这里的线程安全指的是：一个线程在初始化 m 的时候，其他线程执行到 m 的初始化这一行的时候，就会挂起而不是跳过。

具体实现如下：局部静态变量在编译时，编译器的实现是和全局变量类似的，均存储在bss段中。然后编译器会生成一个guard_for_bar 用来保证线程安全和一次性初始化的整型变量，是编译器生成的，存储在 bss 段。它的最低的一个字节被用作相应静态变量是否已被初始化的标志， 若为 0 表示还未被初始化，否则表示已被初始化( if ((guard_for_bar & 0xff) == 0)判断)。 __cxa_guard_acquire 实际上是一个加锁的过程， 相应的 __cxa_guard_abort 和 __cxa_guard_release 释放锁。


### 13. C和C++函数调用方式,CPP相比c的调用有哪些?

函数重载：1. 参数个数不同，2. 参数类型不同3. 参数顺序不同;返回值类型不参与函数重载判断

C与C++之间的相互调用及函数区别?

c与c++的函数区别
+ c++支持函数重载，而c语言不支持。为了使函数支持重载，c++在c语言的基础上，将函数名添加上返回值和参数的类型信息。
    + 例如，int add(int, int)这个函数，通过c++编译器编译后，可能呈现的函数名为int int_add_int_int(int, int)（注：此处为大概地说明c++是如何将返回值和参数信息添加到函数名中的，实际中编译器不一定是这样实现的）。

extern "C"的作用
那么，c与c++是不能相互调用了吗？答案是否定的，因为存在着extern "C"这个关键字可以使语句可以按照类C的编译和连接规约来编译和连接，而不是C++的编译的连接规约。这样在类C的代码中就可以调用C++的函数or变量等。

注意：extern "C"指令中的"C"，表示的一种编译和连接规约，而不是一种语言。"C"表示符合C语言的编译和连接规约的任何语言，如Fortran、assembler等。

还有要说明的是，extern "C"指令仅指定编译和连接规约，但不影响语义。例如在函数声明中，指定了extern "C"，仍然要遵守C++的类型检测、参数转换规则。

c语言调用c++代码:
c语言调用c++代码却并不容易，原因是c语言并不兼容c++。就算c语言可以调用c++，也会因为无法识别c++新定义的符号而编译报错。因此，为了实现c语言调用c++函数，必须实现以下两个步骤：
1. 将c++相关函数封装为静态库或动态库（因为调用库函数时编译器并不知道里面执行的是什么语言）；在g++编译的时候去加入extern "C"
2. 对外提供遵循类c语言规约的接口函数。

### 14. c++内存分区,堆区能否多线程共享

堆区可以被多个线程共享，不过在多线程环境下访问堆区需要特别注意以下几个方面

线程安全问题：多个线程同时对堆区进行读写操作时，可能会引发数据竞争（Data Race）问题。数据竞争是指多个线程同时访问共享资源，并且至少有一个线程进行写操作，而没有适当的同步机制来保证操作的原子性，这会导致程序的行为变得不可预测。例如，多个线程同时对堆上的同一个变量进行写操作，可能会导致数据的不一致。


### 15 c++中的内存分区。bss段是什么？未初始化的全局变量和初始化的全局变量放在哪里

内存从上到下分别是：

栈stack |高地址|
堆heap
bss段
data段
代码段text |低地址|



+ 栈（Stack）：由操作系统自动分配和释放，主要用于存储函数的局部变量、函数调用时的上下文信息（如返回地址、寄存器值等）。栈的访问速度较快，但是空间有限。当函数被调用时，其局部变量会被压入栈中；函数执行完毕后，这些变量会被自动从栈中弹出。
+ 堆（Heap）：由程序员手动分配和释放（使用new和delete，或者malloc和free），用于动态分配内存。堆的空间相对较大，但访问速度比栈慢，并且需要程序员自己管理内存，否则容易出现内存泄漏问题。
+ bss段：（Block Started by Symbol）存放程序中未初始化的全局变量的一块内存区域，在程序载入时由内核置为0。
+ data段：static变量和所有初始化的全局变量都在data段中。
+ 代码段（Code Segment）：也称为文本段（Text Segment），用于存储程序的可执行代码，通常是只读的，以防止程序代码被意外修改。


BSS 段（未初始化数据段）
BSS 是英文 Block Started by Symbol 的缩写。BSS 段用于存放未初始化的全局变量和未初始化的静态变量。这些变量在程序开始执行前会被自动初始化为 0（对于数值类型）或空指针（对于指针类型）。

BSS 段的优点是在可执行文件中不占用实际的磁盘空间，因为这些变量的值在程序加载时才会被初始化为 0，可执行文件中只需要记录这些变量的大小和位置信息。

>ss段和data段都是静态内存分配，也就是说在编译的时候自动分配的。
bss和data段也有一种说法合起来叫数据段，有三种类型：
+ 只读数据段，常量与const修饰的全局变量
+ 可读可写数据段，存放初始化的全局变量和static变量
+ bss段，存放未初始化的全局变量


### 16. 如果一个类内存很大,给别人使用这个类时,怎么要求只能把内存分配到堆区

限制栈上对象的创建


方法一：将析构函数设为私有
+ 原理：在栈上创建的对象，其生命周期结束时，编译器会自动调用析构函数来释放对象。若把析构函数设为私有，栈上对象在生命周期结束时无法自动调用析构函数，从而禁止在栈上创建对象。而在堆上创建的对象，需要手动调用 delete 来释放内存，可通过提供一个公有静态成员函数来完成对象的释放操作。

```cpp
#include <iostream>

class LargeClass {
private:
    // 私有析构函数
    ~LargeClass() {
        std::cout << "LargeClass destroyed." << std::endl;
    }

public:
    // 静态成员函数，用于在堆上创建对象
    static LargeClass* create() {
        return new LargeClass();
    }

    // 静态成员函数，用于释放堆上的对象
    static void destroy(LargeClass* obj) {
        delete obj;
    }

    void doSomething() {
        std::cout << "Doing something..." << std::endl;
    }
};

int main() {
    // 只能在堆上创建对象
    LargeClass* obj = LargeClass::create();
    obj->doSomething();
    // 手动释放对象
    LargeClass::destroy(obj);

    // 以下代码会编译错误，因为析构函数是私有的，不能在栈上创建对象
    // LargeClass stackObj;

    return 0;
}
```

### 17. 构造函数 析构函数

构造函数可以设置为私有：单例模式
不能设置为虚函数：构造函数不是不能是虚函数，而是完全没意义。
+ c++在编译期间就能确定你要创建的对象的具体类型，而这个具体类型包含了什么，继承了什么在编译期间也是明确的，所以要构造什么也都是明确的，根本没必要存在虚构造函数。
+ 虚函数的存在是因为编译期间没法确定具体调用对象，才会有虚函数，虚函数表这么个东西。

一、为什么构造函数不能为虚函数？
1、 从存储空间角度，虚函数相应一个指向vtable虚函数表的指针，这个指向vtable的指针事实上是存储在对象的内存空间的。问题出来了，假设构造函数是虚的，就须要通过 vtable来调用，但是对象还没有实例化，也就是内存空间还没有，怎么找vtable呢？所以构造函数不能是虚函数。
2、 虚函数的作用在于通过父类的指针或者引用来调用它的时候可以变成调用子类的那个成员函数。而构造函数是在创建对象时自己主动调用的，不可能通过父类的指针或者引用去调用，因此也就规定构造函数不能是虚函数。
3、创造一个对象时需要确定对象的类型，而虚函数是在运行时动态确定其类型的。在构造一个对象时，由于对象还未创建成功，编译器无法知道对象的实际类型。

二、为什么析构函数可以是虚函数？如果不设为虚函数可能会存在什么问题？
+ 析构函数的作用： 析构函数用于在对象生命周期结束时释放资源。当一个派生类对象通过基类指针被删除时，如果析构函数不是虚函数，编译器只会调用基类的析构函数，而不会调用派生类的析构函数，这会导致派生类中资源没有被正确释放，产生内存泄漏或其他未定义行为。
+ 通过将析构函数设为虚函数，编译器在运行时通过虚函数表确定正确的析构函数顺序，先调用派生类的析构函数，再调用基类的析构函数，确保资源被正确释放。

### 18 atomic 是怎么保证原子性的

1. 屏蔽中断
2. 底层硬件自旋锁
// 避免其他核心操作相关的内存区域
3. 锁总线：lock，避免所有的内存访问；
4. 现在 lock 指令，指挥阻止其他核心对相关内存空间的访问


而对于atomic的实现最基础的解释：原子操作是由底层硬件支持的一种特性。

我们可以看到在执行自增操作的时候，在xaddl 指令前多了一个lock前缀，而cpu对这个lock指令的支持就是所谓的底层硬件支持。

增加这个前缀后，保证了 load-add-store 步骤的不可分割性。

lock 指令的实现
众所周知，cpu在执行任务的时候并不是直接从内存中加载数据，而是会先将数据加载到L1和L2 cache中（典型的是两层缓存，甚至可能更多），然后再从cache中读取数据进行运算。

而现在的计算机通常都是多核处理器，每一个内核都对应一个独立的L1层缓存，多核之间的缓存数据同步是cpu框架设计的重要部分，MESI是比较常用的多核缓存同步方案。

当我们在单线程内执行 atomic++操作，自然是不会发生多核之间数据不同步的问题，但是我们在多线程多核的情况下，cpu是如何保证Lock特性的？

上面说明了lock前缀实现原子性的两种方式：
+ 锁bus：性能消耗大，在intel 486处理器上用此种方式实现
+ 锁cache：在现代处理器上使用此种方式，但是在无法锁定cache的时候（如果锁驻留在不可缓存的内存中，或者如果锁超出了划分cache line 的cache boundy），仍然会去锁定总线。

### 19. 线程的独立的寄存器是怎么理解的



### 20. QUIC 了解过吗

是一种基于 UDP 协议的传输层网络协议；

1. 快速连接建立
+ 减少握手延迟：TCP 连接建立通常需要经过三次握手，而 QUIC 协议在首次连接时可以在一次往返（RTT）内完成密钥交换和连接建立，后续连接甚至可以实现 0-RTT 恢复，大大减少了连接建立的时间。这对于需要频繁建立连接的应用（如移动应用、网页浏览等）来说，可以显著提升用户体验。
2. 多路复用
+ 避免队头阻塞：在 TCP 中，同一连接上的多个流是按顺序传输的，如果一个数据包丢失，后续的数据包都需要等待该数据包重传，这会导致队头阻塞问题。而 QUIC 协议支持多路复用，允许在同一连接上同时传输多个独立的数据流，每个数据流都有自己的序列号和确认机制，一个数据流的数据包丢失不会影响其他数据流的传输，从而提高了带宽利用率和传输效率。
3. 可靠传输
+ 内置拥塞控制：QUIC 协议在 UDP 的基础上实现了可靠传输机制，包括数据包的重传、确认和拥塞控制。它采用了类似 TCP 的拥塞控制算法（如 Cubic、BBR 等），能够根据网络状况动态调整发送速率，避免网络拥塞，保证数据的可靠传输。
4. 安全性
+ 加密传输：QUIC 协议集成了 TLS 1.3 加密协议，所有数据在传输过程中都进行加密，确保了数据的机密性和完整性。同时，加密过程与连接建立过程紧密结合，进一步提高了安全性和性能。
5. 连接迁移
+  无缝切换网络：在移动设备上，当网络环境发生变化（如从 Wi-Fi 切换到移动数据网络）时，TCP 连接通常会中断，需要重新建立连接。而 QUIC 协议通过使用连接 ID 来标识连接，而不是依赖于 IP 地址和端口号，因此在网络切换时可以实现无缝迁移，保持连接的连续性。

在实时通信应用（如视频会议、在线游戏等）中，QUIC 协议的低延迟和可靠传输特性能够保证数据的及时传输和高质量的通信体验。其多路复用功能可以同时处理音频、视频和控制信息等多个数据流，互不干扰。
### 21. new和malloc

1. 语法和来源
+ malloc：malloc 是 C 语言标准库中的函数，在 C++ 中仍然可以使用。
malloc 函数接受一个 size_t 类型的参数，表示需要分配的内存字节数，返回一个 void* 类型的指针，指向分配的内存块的起始地址。如果分配失败，返回 NULL。
+ new：new 是 C++ 中的运算符。new 运算符会根据类型 T 自动计算所需的内存大小，并返回一个指向该类型对象的指针。

2. 内存分配和类型处理
+ malloc：malloc 只是简单地分配指定大小的内存块，不会对内存进行初始化，并且返回的是 void* 类型的指针，需要手动将其转换为所需的指针类型。
+ new：new 会根据对象的类型自动分配所需的内存，并在分配内存后调用对象的构造函数进行初始化。

3. 构造函数和析构函数的调用
+ malloc：malloc 只是分配内存，不会调用对象的构造函数。同样，使用 free 释放内存时，也不会调用对象的析构函数。这意味着如果使用 malloc 分配类对象的内存，对象的初始化和清理工作需要手动完成。
+ new：new 会在分配内存后自动调用对象的构造函数进行初始化。而 delete 运算符会在释放内存之前自动调用对象的析构函数，确保对象的资源得到正确清理。

4. 内存分配失败的处理
+ malloc：当 malloc 无法分配所需的内存时，会返回 NULL。因此，在使用 malloc 分配内存后，通常需要检查返回值是否为 NULL，以确保内存分配成功。
+ new：当 new 无法分配所需的内存时，默认情况下会抛出 std::bad_alloc 异常。可以使用 try-catch 块来捕获该异常进行处理。

5. 数组分配
+ malloc：使用 malloc 分配数组时，需要手动计算数组所需的总内存大小。例如，分配一个包含 10 个 int 类型元素的数组：
+ new：使用 new 分配数组时，只需指定数组的元素个数，编译器会自动计算所需的内存大小。并且，使用 delete[] 来释放数组内存。

### 22. 定位 new 了解过吗

定位 new（Placement new）是 C++ 中的一种特殊的 new 表达式，它允许在已分配的内存块上构造对象，而不是像普通 new 那样从堆上分配新的内存。

> new (memory_address) Type(arguments);

+ memory_address：这是一个指向已分配内存块的指针，对象将在这个内存块上构造。
+ Type：要构造的对象的类型。
+ arguments：传递给对象构造函数的参数（可选）。

在使用内存池时，预先分配了一大块内存，当需要创建对象时，使用定位 new 可以直接在内存池的空闲内存块上构造对象，避免了频繁的系统调用（如 malloc 和 free），提高了内存分配和释放的效率，同时也有助于减少内存碎片。

```cpp
#include <iostream>
#include <new>

// 定义一个简单的类
class MyClass {
public:
    MyClass() {
        std::cout << "MyClass constructor" << std::endl;
    }
    ~MyClass() {
        std::cout << "MyClass destructor" << std::endl;
    }
    void doSomething() {
        std::cout << "Doing something..." << std::endl;
    }
};

int main() {
    // 分配一块内存
    char* buffer = new char[sizeof(MyClass)];

    // 使用定位 new 在已分配的内存上构造对象
    MyClass* obj = new (buffer) MyClass();

    // 调用对象的成员函数
    obj->doSomething();

    // 手动调用析构函数
    obj->~MyClass();

    // 释放之前分配的内存
    delete[] buffer;

    return 0;
}
```

注意事项
+ 手动调用析构函数：使用定位 new 构造的对象不会自动调用析构函数，需要手动调用析构函数来清理对象的资源。
+ 内存管理：定位 new 不会分配新的内存，它只是在已有的内存位置上构造对象。因此，你需要负责管理这块内存的分配和释放。
+ 对齐要求：指定的内存地址必须满足对象类型的对齐要求，否则可能会导致未定义行为。在大多数情况下，使用 operator new 分配的内存可以满足对齐要求。


1. buffer 大小大于 MyClass 的大小
当 buffer 的大小大于 MyClass 对象所需的大小，定位 new 仍然可以正常工作。这是因为定位 new 的主要作用是在指定的内存地址上构造对象，它并不会检查该地址处的内存块整体大小，只要这块内存能够容纳要构造的对象即可。

2. buffer 大小小于 MyClass 的大小
当 buffer 的大小小于 MyClass 对象所需的大小，这会导致未定义行为。因为定位 new 会尝试在这块不足的内存上构造对象，对象可能无法完整地存储在该内存区域中。

### 23. new 内部调用的是什么系统调用

在 Linux 系统中，operator new 通常会调用 malloc 函数来分配内存，而 malloc 函数在底层又会根据具体情况调用不同的系统调用，主要涉及以下两个系统调用：

+ brk 系统调用：brk 系统调用用于调整进程数据段的结束地址。当需要分配的内存较小时，malloc 会使用 brk 来扩展进程的堆空间。它通过移动进程的堆顶指针来分配内存，操作相对简单，但分配的内存是连续的，并且在释放时可能会导致内存碎片问题。

+ mmap 系统调用：当需要分配的内存较大时，malloc 会使用 mmap 系统调用。mmap 可以将一个文件或者设备映射到进程的地址空间，也可以用于分配匿名内存。与 brk 不同，mmap 分配的内存是独立的，不依赖于进程的堆空间，释放时可以直接解除映射，不会产生内存碎片。

#### 24. shared_ptr 计数是怎么共享的，怎么解决并发问题的

共享的控制块（control block）来实现的。当使用 std::shared_ptr 管理一个对象时，会创建一个控制块，该控制块包含以下主要信息：

+ 引用计数：记录当前有多少个 std::shared_ptr 实例指向同一个对象。
+ 弱引用计数：用于 std::weak_ptr，记录有多少个 std::weak_ptr 实例观察同一个对象。
+ 指向被管理对象的指针：指向实际被管理的动态分配的对象。

解决并发问题
在多线程环境下，多个线程可能同时对 std::shared_ptr 的引用计数进行操作，这会导致数据竞争问题。为了解决并发问题，std::shared_ptr 的引用计数操作使用了原子操作。

### 25. 介绍下左值和右值

左值是一个表示对象的表达式，它具有一个确定的内存地址，并且可以出现在赋值运算符的左边。通俗来讲，左值是可以被引用的表达式，通常代表一个持久的对象。

右值是一个不表示对象的表达式，它没有确定的内存地址，通常是临时对象或者字面量。右值不能出现在赋值运算符的左边，但可以出现在右边。右值又可分为纯右值和将亡值。

纯右值是最常见的右值类型，它通常是字面量、临时对象或者没有名字的表达式结果。
```cpp
#include <iostream>

int main() {
    int a = 5 + 3;  // 5 + 3 是一个纯右值，它是一个临时的计算结果
    int b = 10;
    int c = b + 2;  // b + 2 也是一个纯右值

    return 0;
}
```

将亡值通常是通过 std::move() 转换得到的结果，或者是某些返回右值引用的函数的返回值。将亡值表示该对象的资源即将被转移，它暗示了对象的生命周期即将结束。

```cpp
#include <iostream>
#include <utility>

class MyClass {
public:
    MyClass() { std::cout << "Constructor" << std::endl; }
    MyClass(const MyClass& other) { std::cout << "Copy constructor" << std::endl; }
    MyClass(MyClass&& other) noexcept { std::cout << "Move constructor" << std::endl; }
    ~MyClass() { std::cout << "Destructor" << std::endl; }
};

MyClass getTempObject() {
    return MyClass();
}

int main() {
    MyClass obj1 = getTempObject();  // getTempObject() 的返回值是一个将亡值
    MyClass obj2 = std::move(obj1);  // std::move(obj1) 产生一个将亡值

    return 0;
}
```

泛左值（glvalue，Generalized lvalue）
泛左值是左值和将亡值的统称。它是一个表示对象、位域或者函数的表达式，其特点是具有身份（即可以确定该表达式是否和其他表达式指代同一个实体）。简单来说，泛左值能够确定一个对象或者函数的存在，并且可以判断不同的表达式是否引用了同一个对象或函数。


### 26. 设计内存池，用来解决内存碎片问题

设计思路
内存池的基本思想是预先分配一大块连续的内存，然后将这块内存划分为多个固定大小的内存块。当需要分配内存时，直接从内存池中取出一个空闲的内存块；当释放内存时，将该内存块标记为空闲，以便后续再次使用。这样可以避免频繁的系统调用（如 malloc 和 free），减少内存碎片的产生。

```cpp
#include <iostream>
#include <vector>
#include <stdexcept>

// 内存块结构体
struct MemoryBlock {
    MemoryBlock* next;  // 指向下一个空闲内存块的指针
    bool isFree;        // 标记该内存块是否空闲
};

// 内存池类
class MemoryPool {
private:
    char* pool_;            // 内存池的起始地址
    size_t blockSize_;      // 每个内存块的大小
    size_t blockCount_;     // 内存块的数量
    MemoryBlock* freeList_; // 空闲内存块链表的头指针

public:
    // 构造函数，初始化内存池
    MemoryPool(size_t blockSize, size_t blockCount)
        : blockSize_(blockSize), blockCount_(blockCount) {
        // 分配一大块连续的内存
        pool_ = static_cast<char*>(std::malloc(blockSize * blockCount));
        if (!pool_) {
            throw std::bad_alloc();
        }

        // 初始化内存块链表
        freeList_ = nullptr;
        for (size_t i = 0; i < blockCount; ++i) {
            MemoryBlock* block = reinterpret_cast<MemoryBlock*>(pool_ + i * blockSize);
            block->next = freeList_;
            block->isFree = true;
            freeList_ = block;
        }
    }

    // 析构函数，释放内存池
    ~MemoryPool() {
        std::free(pool_);
    }

    // 分配内存
    void* allocate() {
        if (!freeList_) {
            throw std::bad_alloc();
        }

        // 从空闲链表中取出一个内存块
        MemoryBlock* block = freeList_;
        freeList_ = block->next;
        block->isFree = false;
        return block;
    }

    // 释放内存
    void deallocate(void* ptr) {
        if (!ptr) {
            return;
        }

        MemoryBlock* block = reinterpret_cast<MemoryBlock*>(ptr);
        if (block->isFree) {
            throw std::invalid_argument("Double free detected");
        }

        // 将内存块插入到空闲链表头部
        block->isFree = true;
        block->next = freeList_;
        freeList_ = block;
    }

};

// 测试代码
int main() {
    try {
        // 创建一个内存池，每个内存块大小为 128 字节，共有 10 个内存块
        MemoryPool pool(128, 10);

        // 分配内存
        void* ptr1 = pool.allocate();
        void* ptr2 = pool.allocate();

        // 使用内存
        // ...

        // 释放内存
        pool.deallocate(ptr1);
        pool.deallocate(ptr2);
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
```

## 27 内联函数

内联函数（Inline Function）是 C++ 中一种特殊的函数，它通过在调用处直接展开函数体代码来避免函数调用的开销，从而提高程序的执行效率。


代码膨胀
+ 原理：内联函数会在每个调用点将函数体代码展开。如果一个内联函数被频繁调用，或者函数体本身比较大，那么代码量会显著增加。因为每次调用都会复制一份函数体的代码，这就导致最终生成的可执行文件体积增大。
+ 影响：代码膨胀可能会降低程序的缓存命中率。由于可执行文件变大，更多的代码需要加载到内存中，这可能导致缓存中无法容纳所有常用的代码，从而增加了缓存缺失的概率，使得 CPU 需要从主存中读取代码，降低了程序的执行速度。

## 28. algrind的memcheck具体怎么实现的？

1. 二进制重写技术
Memcheck 采用二进制重写（Binary Rewriting）技术，这是其核心实现方式。具体步骤如下：

+ 加载程序：当使用 Memcheck 运行一个程序时，Valgrind 首先会加载该程序的可执行文件和相关的动态链接库。

+ 重写代码：Valgrind 会对程序的机器码进行逐行分析和修改。它会在程序的每条内存访问指令（如读取或写入内存）前后插入额外的代码，这些额外代码用于记录和检查内存访问的情况。例如，对于一个内存读取指令，Memcheck 会插入代码来检查读取的内存地址是否有效，是否越界等。

+ 执行重写后的程序：程序在 Valgrind 的控制下运行重写后的代码，而不是原始的机器码。这样，Memcheck 就可以在程序执行过程中实时监控内存的使用情况。


2. 阴影内存（Shadow Memory）机制

+ 概念：阴影内存是 Memcheck 实现内存检查的关键机制。它是一个与程序实际使用的内存空间一一对应的虚拟内存区域，用于记录实际内存的状态信息。

+ 状态记录：对于实际内存中的每个字节，阴影内存中都有一个或多个字节与之对应，用于记录该字节的状态。例如，阴影内存可以记录某个字节是否已经被初始化、是否已经被释放等信息。当程序进行内存访问时，Memcheck 会先检查阴影内存中对应字节的状态，根据状态来判断该内存访问是否合法。

+ 示例：如果程序试图读取一个未初始化的内存区域，Memcheck 通过检查阴影内存可以发现该区域的状态标记为未初始化，从而报告错误。

3. 内存管理跟踪
+ 内存分配和释放跟踪：Memcheck 会拦截程序中所有的内存分配和释放函数，如 malloc、free、new 和 delete。当程序调用这些函数时，Memcheck 会记录相关的信息，如分配的内存块的大小、地址和释放情况。

+ 内存泄漏检测：在程序结束时，Memcheck 会检查所有已经分配但未释放的内存块。如果发现有这样的内存块，就会报告内存泄漏错误。同时，它还会提供详细的信息，如泄漏内存块的分配位置（调用栈信息），帮助开发者定位问题。

4. 边界检查
+ 越界访问检测：Memcheck 会对程序的内存访问进行边界检查。当程序进行内存读写操作时，它会检查访问的地址是否在合法的内存范围内。例如，如果程序试图访问一个数组越界的位置，Memcheck 会通过检查阴影内存和记录的内存块边界信息，发现这种越界访问并报告错误。

5. 堆和栈的管理
+ 堆内存管理：对于堆内存（通过 malloc、new 等分配的内存），Memcheck 会跟踪其分配和释放过程，确保堆内存的正确使用。同时，它会检查堆内存的越界访问、重复释放等问题。
+ 栈内存管理：对于栈内存（函数调用时自动分配和释放的内存），Memcheck 也会进行监控。它会检查栈溢出、栈上变量的未初始化使用等问题。

## 29. 有什么方法能在main函数执行前调用某个函数？

1. 使用全局对象的构造函数

在 C++ 中，全局对象的构造函数会在 main 函数开始执行之前被调用。这是因为全局对象的生命周期从程序启动就开始，其构造函数会在程序初始化阶段自动执行。

```cpp
#include <iostream>

// 定义一个类
class BeforeMain {
public:
    // 构造函数
    BeforeMain() {
        std::cout << "Function in BeforeMain's constructor is called before main." << std::endl;
    }
};

// 创建全局对象
BeforeMain beforeMain;

int main() {
    std::cout << "This is the main function." << std::endl;
    return 0;
}
```


## 30 c++的有栈协程和无栈协程

在 C++ 中，协程是一种比线程更加轻量级的并发编程机制，它允许程序在执行过程中暂停和恢复，从而实现高效的异步操作。协程可分为有栈协程和无栈协程

有栈协程

特点
有栈协程拥有自己独立的栈空间，这与线程类似。当协程暂停时，其当前的执行上下文（包括栈指针、寄存器等信息）会被保存，当协程恢复执行时，之前保存的上下文会被恢复，使得协程能够从暂停的位置继续执行。

优缺点

+ 优点：
    + 编程模型简单：有栈协程的编程方式与传统的函数调用和返回类似，开发者可以像编写普通函数一样编写协程函数，易于理解和使用。
    + 支持嵌套调用：由于有独立的栈空间，有栈协程可以像普通函数一样进行嵌套调用，方便实现复杂的逻辑。

+ 缺点：
    + 栈空间开销大：每个有栈协程都需要分配一定的栈空间，当协程数量较多时，会占用大量的内存资源。
    + 上下文切换开销大：保存和恢复协程的上下文（包括栈指针、寄存器等）需要一定的时间和资源，会影响程序的性能。


无栈协程

特点
无栈协程没有独立的栈空间，它的状态是通过状态机来实现的。当协程暂停时，不会保存整个栈的状态，而是保存一些必要的状态信息，如局部变量的值、当前执行的位置等。当协程恢复执行时，根据保存的状态信息从暂停的位置继续执行。


优缺点
+ 优点：
    + 内存开销小：无栈协程不需要分配独立的栈空间，只需要保存必要的状态信息，因此内存开销较小。
    + 上下文切换开销小：由于不需要保存和恢复整个栈的状态，无栈协程的上下文切换开销相对较小，性能较高。
+ 缺点：
    + 编程复杂度高：无栈协程需要使用状态机来实现，编程模型相对复杂，对开发者的要求较高。    
    + 不支持嵌套调用：无栈协程的状态机实现方式使得它在嵌套调用时会变得复杂，不如有栈协程方便。


## 31. c++的迭代器是怎么获取原本的类型的？原生指针怎么获取？


迭代器获取原本的类型:

1. 使用 std::iterator_traits

std::iterator_traits 是一个模板类，定义在 <iterator> 头文件中，它提供了一种标准化的方式来获取迭代器的各种属性，包括所指向元素的类型。

## 32.在循环引用中，两个节点，如果一个用shared_ptr指向另一个，另一个用weak_ptr回指，请问如果判断哪个用shared_ptr哪个用weak_ptr？根据什么判断？

1. 基本原则：拥有较长生命周期，需要负责管理另一个对象生命周期的节点使用shared_ptr；而依赖于另一个对象，但不负责其生命周期的节点使用weak_ptr。
```cpp
#include <iostream>
#include <memory>

class Child;

class Parent {
public:
    std::shared_ptr<Child> child;
    ~Parent() {
        std::cout << "Parent destroyed" << std::endl;
    }
};

class Child {
public:
    std::weak_ptr<Parent> parent;
    ~Child() {
        std::cout << "Child destroyed" << std::endl;
    }
};

int main() {
    auto parent = std::make_shared<Parent>();
    auto child = std::make_shared<Child>();

    parent->child = child;
    child->parent = parent;

    return 0;
}
```

Parent对象通常被认为拥有Child对象的生命周期，所以Parent使用shared_ptr指向Child。而Child对象依赖于Parent对象，但不负责其生命周期，所以Child使用weak_ptr指向Parent。当Parent对象被销毁时，Child对象的引用计数减 1，如果没有其他shared_ptr指向Child，Child对象也会被销毁。

2. 所有权关系

基本原则：具有所有权的节点使用shared_ptr，没有所有权的节点使用weak_ptr。所有权意味着一个对象负责另一个对象的存在和销毁。


## 33. go和C++的区别是什么？

go和C++的区别是什么？
go的“轻量”体现在什么地方？
go的劣势在哪？

并发编程
+ Go：内置了轻量级线程（goroutine）和通道（channel），使得并发编程变得简单高效。开发者可以轻松地创建大量的 goroutine 来处理并发任务。
+ C++：在 C++11 之前，并发编程需要依赖第三方库。C++11 之后引入了标准库中的线程库，但相比 Go，并发编程的代码复杂度较高。

内存管理
+ Go：拥有自动垃圾回收机制（GC），开发者无需手动管理内存，降低了内存泄漏的风险，但 GC 可能会带来一定的性能开销。
+ C++：需要开发者手动管理内存，使用new和delete（或malloc和free）进行内存的分配和释放。虽然可以更精细地控制内存使用，但容易出现内存泄漏和悬空指针等问题。

语法风格
+ Go：语法简洁，代码风格统一，去除了一些复杂的语法特性，如头文件、显式指针运算等，易于学习和阅读。例如，Go 的变量声明和函数定义更加简洁直观。
+ C++：语法复杂，支持多种编程范式，如面向对象、泛型编程等。提供了丰富的特性，如模板、继承、多态等，但也增加了学习和使用的难度。


Go 的 “轻量” 体现

轻量级线程（goroutine）
+ 占用资源少：一个 goroutine 只占用几 KB 的栈内存，相比传统的线程（通常占用几 MB 的栈内存），可以在相同的内存资源下创建大量的 goroutine。
+ 调度开销小：Go 的运行时系统会自动管理 goroutine 的调度，调度开销远小于操作系统对线程的调度开销。

快速编译和部署
+ 编译速度快：Go 的编译速度非常快，即使是大型项目也能在短时间内完成编译。
+ 部署方便：Go 编译后的二进制文件可以直接在目标机器上运行，无需依赖复杂的运行环境。


Go 的劣势

缺乏某些高级特性
+ 模板编程：Go 语言没有像 C++ 那样的模板机制，在实现泛型编程时相对受限。虽然 Go 1.18 引入了泛型，但相比 C++ 的模板，功能还不够强大。
+ 多重继承：Go 不支持多重继承，在一些需要多重继承的场景下，实现起来会比较复杂。


性能优化难度
+ GC 开销：Go 的垃圾回收机制会带来一定的性能开销，在对性能要求极高的场景下，可能会成为瓶颈。虽然 Go 的开发者一直在优化 GC 算法，但仍然无法完全避免 GC 带来的影响。
+ 底层控制能力弱：相比 C++，Go 对底层硬件资源的控制能力较弱，在一些需要直接操作硬件的场景下，Go 可能无法满足需求。

## 34. 指针和引用

### 1. 引用的本质

从高级语言层面的概念来说：引用是变量的别名，它不能脱离被引用对象独立存在。

底层，引用就是一个常量指针。

高级语言层面引用与指针常量的关系

+ 相同点：指针和引用在内存中都占用4个或者8个字节的存储空间，都必须在定义的时候给初始化。

+ 指针常量本身（以p为例）允许寻址，即&p返回指针常量本身的地址，*p表示被指向的对象

+ 引用变量本身（以r为例）不允许寻址，&r返回的是被引用对象的地址，而不是变量r的地址(r的地址由编译器掌握，程序员无法直接对它进行存取)

+ 引用不能为空，指针可以为空；

+ 指针数组这一块。数组元素允许是指针但不允许是引用，主要是为了避免二义性。假如定义一个“引用的数组”，那么array[0]=8;这条语句该如何理解？是将数组元素array[0]本身的值变成8呢，还是将array[0]所引用的对象的值变成8呢?

+ 在C++中，指针和引用经常用于函数的参数传递，然而，指针传递参数和引用传递参数是有本质上的不同的：

    + 指针参数传递本质上是值传递，它所传递的是一个地址值。值传递过程中，被调函数的形式参数作为被调函数的局部变量处理，会在栈中开辟内存空间以存放由主调函数传递进来的实参值，从而形成了实参的一个副本（替身）。值传递的特点是，被调函数对形式参数的任何操作都是作为局部变量进行的，不会影响主调函数的实参变量的值（形参指针变了，实参指针不会变）。

    + 而在引用传递过程中，被调函数的形参虽然也作为局部变量在栈中开辟了内存空间，但是这时存放的是由主调函数放进来的实参变量的地址（指针放的是实参变量地址的副本）。被调函数对形参（本体）的任何操作都被处理成间接寻址，即通过栈中存放的地址访问主调函数中的实参变量（根据别名找到主调函数中的本体）。因此，被调函数对形参的任何操作都会影响主调函数中的实参变量。

    + 引用传递和指针传递是不同的，虽然他们都是在被调函数栈空间上的一个局部变量，但是任何对于引用参数的处理都会通过一个间接寻址的方式操作到主调函数中的相关变量。而对于指针传递的参数，如果改变被调函数中的指针地址，它将应用不到主调函数的相关变量。如果想通过指针参数传递来改变主调函数中的相关变量（地址），那就得使用指向指针的指针或者指针引用。

    + 从编译的角度来讲，程序在编译时分别将指针和引用添加到符号表上，符号表中记录的是变量名及变量所对应地址。指针变量在符号表上对应的地址值为指针变量的地址值，而引用在符号表上对应的地址值为引用对象的地址值（与实参名字不同，地址相同）。符号表生成之后就不会再改，因此指针可以改变其指向的对象（指针变量中的值可以改），而引用对象则不能修改。


+ "sizeof引用"得到的是所指向的变量(对象)的大小，而"sizeof指针"得到的是指针本身的大小；

## 35 段错误（Segmentation fault）

段错误是一种内存访问错误，它表示程序试图访问未被分配给它的内存区域，或者以不允许的方式访问已分配的内存。

在操作系统中，每个进程都有自己独立的虚拟地址空间，操作系统会为进程分配一定的内存区域供其使用。当程序试图越界访问内存、访问未初始化的指针或者释放已经释放的内存时，就会触发段错误。

常见原因：
+ 1. 访问空指针
+ 2. 数组越界访问
+ 3. 释放已释放的内存
+ 4. 栈溢出


调试方法
1. 使用调试器
调试器（如 GDB）可以帮助定位段错误的具体位置。通过在调试器中运行程序，当程序出现段错误时，调试器会停止程序的执行，并显示出错的代码行和相关的调用栈信息。

```cpp
# 编译程序时加上 -g 选项，以包含调试信息
g++ -g your_program.cpp -o your_program
# 启动 GDB 调试程序
gdb your_program
# 在 GDB 中运行程序
(gdb) run
# 当程序出现段错误时，使用 backtrace 命令查看调用栈信息
(gdb) backtrace
```

2. 日志输出
在代码中添加适当的日志输出语句，记录程序的执行流程和关键变量的值。通过查看日志信息，可以逐步缩小段错误可能出现的范围。

3. 代码审查
仔细检查代码，特别是涉及指针操作、数组访问和动态内存管理的部分，确保没有逻辑错误。

## 36. 线程池

> 1. 什么是线程池？线程池有什么好处（被问概率很小）？

所谓线程池，通俗来讲，就是一个管理线程的池子。它可以容纳多个线程，其中的线程可以反复利用，省去了频繁创建线程对象的操作。

好处：
1）降低资源消耗。通过重复利用已创建的线程降低线程创建和销毁造成的消耗。
2）提高响应速度。当任务到达时，任务可以不需要等到线程创建就能立即执行。
3）提高线程的可管理性。线程是稀缺资源，如果无限制的创建，不仅会消耗系统资源，还会降低系统的稳定性，使用线程池可以进行统一的分配，调优和监控。

> 2、有几种常见的线程池（必知必会）?
1）定长线程池（FixedThreadPool）
2）定时线程池（ScheduledThreadPool）
3）可缓存线程池（CachedThreadPool）
4）单线程化线程池（SingleThreadExecutor）

核心概念：这四个线程池的本质都是ThreadPoolExecutor对象（自己看源码）
不同点在于：
1）FixedThreadPool：只有核心线程，线程数量固定，执行完立即回收，任务队列为链表结构的有界队列。
2）ScheduledThreadPool：核心线程数量固定，非核心线程数量无限，执行完闲置 10ms 后回收，任务队列为延时阻塞队列。
3）CachedThreadPool：无核心线程，非核心线程数量无限，执行完闲置 60s 后回收，任务队列为不存储元素的阻塞队列。
4）SingleThreadExecutor：只有 1 个核心线程，无非核心线程，执行完立即回收，任务队列为链表结构的有界队列

> 3. 线程池的主要参数有哪些（必知必会）？
1）corePoolSize（必需）：核心线程数。默认情况下，核心线程会一直存活，但是当将 allowCoreThreadTimeout 设置为 true 时，核心线程也会超时回收。
2）maximumPoolSize（必需）：线程池所能容纳的最大线程数。当活跃线程数达到该数值后，后续的新任务将会阻塞。
3）keepAliveTime（必需）：线程闲置超时时长。如果超过该时长，非核心线程就会被回收。如果将 allowCoreThreadTimeout 设置为 true 时，核心线程也会超时回收。
4）unit（必需）：指定 keepAliveTime 参数的时间单位。常用的有：TimeUnit.MILLISECONDS（毫秒）、TimeUnit.SECONDS（秒）、TimeUnit.MINUTES（分）。
5）workQueue（必需）：任务队列。通过线程池的 execute() 方法提交的 Runnable 对象将存储在该参数中。其采用阻塞队列实现。
6）threadFactory（可选）：线程工厂。用于指定为线程池创建新线程的方式。
7）handler（可选）：拒绝策略。当达到最大线程数时需要执行的饱和策略。

> 4、线程池的工作流程（必知必会）？

假如核心线程数是5，最大线程数是10，阻塞队列也是10
1）有新任务来的时候，将先使用核心线程执行；
2）当任务数达到5个的时候，第6个任务开始排队；
3）当任务数达到15个的时候，第16个任务将开启新的线程执行，也就是第6个线程
4）当任务数达到20个的时候，线程池满了，如果有第21个任务，将执行拒绝策略（见下一个问题）





8.什么是线程池的预热机制？
线程池的预热机制是指在应用程序启动时预先创建一定数量的核心线程，以减少任务提交后的线程创建延迟。这可以通过设置核心线程数为一个较小的值，并使用任务队列来实现。随着应用程序的运行，线程池可以根据实际需求动态增加线程数。


线程池的核心参数有七个：

corePoolSize：核心线程数

maximumPoolSize：最大线程数量，核心线程+救急线程的最大数量

keepAliveTime：救急线程的存活时间，存活时间内没有新任务，该线程资源会释放

unit：救济线程的存活时间的单位

workQueue：工作队列，当没有空闲核心线程时，新来的任务会在此队列排队，当该队列已满时，会创建应急线程来处理该队列的任务

treadFactory：线程工厂，可以定制线程的创建，线程名称，是否是守护线程等

handler：拒绝策略，在线程数量达到最大线程数量时，实行拒绝策略 拒绝策略：


6、线程池核心线程数如何确定？
没有固定答案，先设定预期，比如我期望的CPU利用率在多少，负载在多少，GC频率多少之类的指标后，再通过测试不断的调整到一个合理的线程数



