# 一、语法






# 二、库
## 1. map和unordered_map的区别

    底层实现：

    map 使用红黑树（Red-Black Tree）作为底层数据结构，因此键值对是按照键的大小顺序进行排序的。这意味着在 map 中查找、插入和删除操作的时间复杂度为 O(log n)。
    unordered_map 使用哈希表（Hash Table）作为底层数据结构，键值对的存储顺序是无序的。哈希表具有良好的平均时间复杂度，通常情况下查找、插入和删除操作的时间复杂度为 O(1)。
    元素顺序：

    在 map 中，键值对按照键的大小顺序进行排序，并且可以通过比较器自定义排序方式。
    在 unordered_map 中，键值对的顺序是不确定的，因为哈希表会根据键的哈希值将键值对分布到不同的桶中。
    性能特点：

    map 适用于需要按照键的大小顺序进行访问的场景，例如有序存储和查找数据。
    unordered_map 适用于需要快速查找的场景，例如查找操作频繁且无序存储的情况。

## 2. 字符串.size()返回的是无符号类型

    #####注意：字符串.size()返回的是无符号类型；

    当无符号类型整形和有符号类型进行比较的时候，会发生隐式类型转换。具体地说，有符号整数类型会被转换为无符号整数类型。

    在这种情况下，如果有符号整数是负数，那么它会被转换为大于等于零的无符号整数。因此，如果你将一个负数与无符号整数进行比较，该负数会被转换为一个很大的正数。这通常会导致意外的行为，因为这个正数比较大，可能会大于你期望的值。

    例如，如果你将 -1 与一个 size_t 或 std::string::size_type 类型的变量进行比较，-1 会被转换为一个很大的正数，然后与该变量进行比较。通常情况下，这个正数的值会比较大，因此结果可能不是你期望的。

    为了避免这种情况，你应该尽量避免将有符号整数与无符号整数进行比较，或者确保你的有符号整数永远不会为负数。


    所以不要直接使用s.size()；而是使用int n = s.size();进行转换


# 三、类设计
## 1. 成员函数加上`const`关键字
为成员函数加上`const`关键字表示该函数不会修改类的成员变量。具体来说，加上`const`后，该成员函数只能读取成员变量，不能修改它   们。这是一种对类的接口进行约束的方式，确保特定的成员函数不会改变对象的状态，从而提高代码的可靠性和可维护性。

1. 作用域和约束
加上const关键字后，编译器会强制约束该成员函数不能修改任何成员变量（除非它们被声明为mutable）。
同时，这个函数也不能调用任何非const的成员函数，因为那些函数可能会修改对象的状态。

2. 接口设计
这种设计明确了函数的意图，使代码的行为更加透明。调用者可以放心地调用这些const成员函数，而不必担心对象的状态会被改变

3. 编译时检查
编译器会在编译时检查这些约束条件，确保const成员函数不会尝试修改对象的状态。如果尝试修改，会导致编译错误。

```cpp
template <typename T>
class BSTNode {
public:
    T data;
    BSTNode* left;
    BSTNode* right;

    // 前序遍历的方法声明，带有const关键字
    void preOrder(BSTNode<T>* tree) const;
    
    // 构造函数和其他成员函数
    BSTNode(T val) : data(val), left(nullptr), right(nullptr) {}
};
```
## 2. 将接口分为public和private两个版本

在设计类时，将接口分为public和private两个版本是常见的做法，主要目的是为了控制接口的访问权限和实现细节的封装。

1. 接口的简化和封装：
+ Public接口：提供给类的使用者，通常是简化和友好的接口。它隐藏了实现的细节，使得用户更容易使用。
+ Private接口：用于类的内部实现细节，不对外公开。它可以包含更复杂的逻辑和实现细节，只供类的其他成员函数调用

2. 提高代码的可维护性：
+ 将实现细节封装在private接口中，public接口只做必要的调用。这样，如果实现细节需要更改，只需要修改private接口的实现，而public接口和用户代码可以保持不变。

3. 保护类的内部状态：
+ Private接口可以对类的内部状态进行直接操作，而public接口通常会有额外的检查或限制，避免用户对类的内部状态进行非法操作。

4. 提高代码的可读性和逻辑性：
+ 通过将复杂的实现细节放在private接口中，可以使public接口更清晰，更符合用户的使用习惯。这样有助于代码的可读性和逻辑性。

```cpp
template <typename T>
class BSTNode {
public:
    T data;
    BSTNode* left;
    BSTNode* right;

    BSTNode(T val) : data(val), left(nullptr), right(nullptr) {}
};

template <typename T>
class BSTree {
public:
    BSTree() : root(nullptr) {}
    ~BSTree() { deleteTree(root); }

    // Public接口：前序遍历
    void preOrder() {
        preOrder(root);
    }

private:
    BSTNode<T>* root;

    // Private接口：前序遍历具体实现
    void preOrder(BSTNode<T>* tree) const {
        if (tree) {
            std::cout << tree->data << " ";
            preOrder(tree->left);
            preOrder(tree->right);
        }
    }

    // 递归删除树的节点
    void deleteTree(BSTNode<T>* tree) {
        if (tree) {
            deleteTree(tree->left);
            deleteTree(tree->right);
            delete tree;
        }
    }
};
```
**Public接口：**`void preOrder()`：这个接口是公开的，用户可以直接调用它来进行前序遍历。它不需要知道树的内部结构和根节点，只需要调用这个函数即可。

**Private接口：**`void preOrder(BSTNode<T>* tree) const：`这个接口是私有的，只在类的内部使用。它接受一个节点指针作为参数，进行递归遍历。用户无法直接访问这个接口，这样可以保护类的内部实现细节。








