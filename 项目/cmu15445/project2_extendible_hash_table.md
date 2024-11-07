链接： https://zhuanlan.zhihu.com/p/622221722#:~:text=Overview%20o#:~:text=Overview%20o

Overview of Extendible Hash Table

+ Directory：是存放bucket指针的容器，可动态生长（以原大小的倍数作为增长率），容器的每个元素可用哈希值来索引。
+ Bucket：桶。存放Key/value pair的桶，数据结构层面是一个线性表。

+ Global Depth：假设global depth为n，那么当前的directory必定有 2^n
 个entry。例如，当前n=2，那么就有4个entry，n=3就有8个entry。同时，给定一个key，需要用global depth取出这个key的低n位的二进制值。例如，一个key的二进制是10111，如果global depth是3，通过IndexOf(key)函数，得到返回值的二进制值是111，即为7。这个值用来索引directory[111]位置的bucket。
+ Local Depth：local depth指的是（假设local depth为n），在当前的bucket之下，每个元素的key的低n位都是相同的。

两者之间有什么关系呢？
global depth一定大于等于local depth
+ 对于一个bucket来说，如果当前的global depth等于local depth，那说明这个bucket只有一个指针指向它。
+ 如果当前的global depth大于local depth，必定不止一个指针指向它。
+ 计算当前bucket有几个指针指向它的公示是 2^(globalDepth-localDepth)
























