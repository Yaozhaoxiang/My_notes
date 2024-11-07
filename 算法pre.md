#算法

**众所周知，刷题有一个很重要的技巧就是按照标签来刷，在某个时间段内只刷相对应的题目，这种做法可以极大的提高刷题的速度，而且能带来更好的效果。**

**1. 二分查找**

    {leetcode:
		33（中等），（2024/4/30 9:15:42 ）
		34（中等），(2024/4/30 15:54:22 )
		35（简单）,（2024/4/28 20:19:11 ）
		69（简单），（2024/5/3 16:56:12 ）
		74 (中等)，（2024/5/3 16:55:58 ）
		81（中等），（）
		153（中等）,（2024/4/29 9:06:22 ）
		162（中等）,(2024/5/7 20:31:58 )
		275，
		278（简单）,（2024/4/28 20:47:14 ）
		287，
		658（中等）,
		704（简单）,(2024/4/28 19:54:40 );
		977，
	}
153：若数组中一定有答案，则可以使while判断条件为while(left < right)，当left=right时，出while。此时最后一个结果就是想要的结果。


最常用的二分查找场景：寻找一个数、寻找左侧边界、寻找右侧边界


**1）. 寻找一个数**


```cpp
int binarySearch(int[] nums, int target) {
    int left = 0; 
    int right = nums.length - 1; // 注意
    while(left <= right) { // 注意
        int mid = (right + left) / 2;
        if(nums[mid] == target)
            return mid; 
        else if (nums[mid] < target)
            left = mid + 1; // 注意
        else if (nums[mid] > target)
            right = mid - 1; // 注意
        }
    return -1;
}
```

**2）. 寻找左侧边界的二分搜索**

```cpp
int left_bound(int[] nums, int target) {
    if (nums.length == 0) return -1;
    int left = 0;
    int right = nums.length; // 注意

    while (left < right) { // 注意
        int mid = (left + right) / 2;
        if (nums[mid] == target) {
            right = mid;//注意
        } else if (nums[mid] < target) {
            left = mid + 1;
        } else if (nums[mid] > target) {
            right = mid; // 注意
        }
    }
    return left;
}

```

1  为什么 while(left < right) 而不是 <= ?

答：用相同的方法分析，因为 right = nums.length 而不是 nums.length - 1 。因此每次循环的「搜索区间」是 [left, right) 左闭右开。while(left < right) 终止的条件是 left == right，此时搜索区间 [left, left) 为空，所以可以正确终止


2 为什么没有返回 -1 的操作？如果 nums 中不存在 target 这个值，怎么办？

答：因为要一步一步来，先理解一下这个「左侧边界」有什么特殊含义：
数组 {1，2，2，2，3}（target=2）
对于这个数组，算法会返回 1。
这个 1 的含义可以这样解读：nums 中小于 2 的元素有 1 个。
比如对于有序数组 nums = [2,3,5,7], target = 1，算法会返回 0，含义是：nums 中小于 1 的元素有 0 个。
再比如说 nums 不变，target = 8，算法会返回 4，含义是：nums 中小于 8 的元素有 4 个。
综上可以看出，函数的返回值（即 left 变量的值）取值区间是闭区间 [0, nums.length]，所以我们简单添加两行代码就能在正确的时候 return -1：

```cpp
while (left < right) {
    //...
}
// target 比所有数都大
if (left == nums.length) return -1;
// 类似之前算法的处理方式
return nums[left] == target ? left : -1;
```
这里注意什么条件会退出while，当left==right时，退出while。而此时的left对应的数组值为第一个target，又因为left是下标，所以left前面有left个小于target的值。


3 为什么 left = mid + 1，right = mid ？和之前的算法不一样？

答：这个很好解释，因为我们的「搜索区间」是 [left, right) 左闭右开，所以当 nums[mid] 被检测之后，下一步的搜索区间应该去掉 mid 分割成两个区间，即 [left, mid) 或 [mid + 1, right)。

4 为什么该算法能够搜索左侧边界？

答：关键在于对于 nums[mid] == target 这种情况的处理：


	if (nums[mid] == target)
	    right = mid;

可见，找到 target 时不要立即返回，而是缩小「搜索区间」的上界 right，在区间 [left, mid) 中继续搜索，即不断向左收缩，达到锁定左侧边界的目的。5. 为什么返回 left 而不是 right？
答：都是一样的，因为 while 终止的条件是 left == right。

**3). 寻找右侧边界的二分查找**

```cpp
int right_bound(int[] nums, int target) {
    if (nums.length == 0) return -1;
    int left = 0, right = nums.length;

    while (left < right) {
        int mid = (left + right) / 2;
        if (nums[mid] == target) {
            left = mid + 1; // 注意
        } else if (nums[mid] < target) {
            left = mid + 1;
        } else if (nums[mid] > target) {
            right = mid;
        }
    }
    return left - 1; // 注意
}

```
2 . 为什么最后返回 left - 1 而不像左侧边界的函数，返回 left？而且我觉得这里既然是搜索右侧边界，应该返回 right 才对。

答：首先，while 循环的终止条件是 left == right，所以 left 和 right 是一样的，你非要体现右侧的特点，返回 right - 1 好了。至于为什么要减一，这是搜索右侧边界的一个特殊点，关键在这个条件判断：
	
	 	if (nums[mid] == target) {
	    left = mid + 1;
	    // 这样想: mid = left - 1
		}

因为我们对 left 的更新必须是 left = mid + 1，就是说 while 循环结束时，nums[left] 一定不等于 target 了，而 nums[left-1] 可能是 target。

至于为什么 left 的更新必须是 left = mid + 1，同左侧边界搜索，就不再赘述。


3.为什么没有返回 -1 的操作？如果 nums 中不存在 target 这个值，怎么办？

答：类似之前的左侧边界搜索，因为 while 的终止条件是 left == right，就是说 left 的取值范围是 [0, nums.length]，所以可以添加两行代码，正确地返回 -1：
	
```cpp
	while (left < right) {
	    // ...
	}
	if (left == 0) return -1;
	return nums[left-1] == target ? (left-1) : -1;
```

**计算mid时，要防止溢出**

int 是一种数据类型，用于定义整数类型变量的标识符，是带符号整数。

在现在对计算机中，int 占用 4 字节，32 比特，数据范围为：-2147483648 ~ 2147483647 [-2^31 ~ 2^31-1]。

那么对于两个都接近 2147483647 的数字而言，它们相加的结果将会溢出，变成负数。

所以，为了避免溢出情况的发生，我们不能使用 mid = (left + right) // 2 来计算 mid 值，而是使用 mid = left + (right - left) // 2 来作为替代。



**2. 双指针法**

本质上就是定义两个指针，来操作！


**3.滑动窗口**

模板：

初始化：left,right,result,bestresult
```cpp
//最长模板
while(右指针没有到结尾)
{
	窗口扩大，加入right对应元素，更新当前result
	while(result不满足要求)
	{
		窗口缩小，溢出left对应元素，left右移
	}
	更新最优结果bestresult
	right++；
}
返回bestresult;


//最短模板
while(右指针没有到结尾)
{
	窗口扩大，加入right对应元素，更新当前result
	while(result不满足要求)
	{
		更新最优结果bestresult
		窗口缩小，溢出left对应元素，left右移
	}
	right++；
}
返回bestresult;
```

**4.kmp算法**

#####注意：字符串.size()返回的是无符号类型；

当无符号类型整形和有符号类型进行比较的时候，会发生隐式类型转换。具体地说，有符号整数类型会被转换为无符号整数类型。

在这种情况下，如果有符号整数是负数，那么它会被转换为大于等于零的无符号整数。因此，如果你将一个负数与无符号整数进行比较，该负数会被转换为一个很大的正数。这通常会导致意外的行为，因为这个正数比较大，可能会大于你期望的值。

例如，如果你将 -1 与一个 size_t 或 std::string::size_type 类型的变量进行比较，-1 会被转换为一个很大的正数，然后与该变量进行比较。通常情况下，这个正数的值会比较大，因此结果可能不是你期望的。

为了避免这种情况，你应该尽量避免将有符号整数与无符号整数进行比较，或者确保你的有符号整数永远不会为负数。


所以不要直接使用s.size()；而是使用int n = s.size();进行转换

1. KMP算法：

（1）字符串第一位保存长度，从第二位开始

```cpp
int index(string s, string t, int pos)
{
	int i = pos;
	int j = 1;
	int next[50];
	getnext(t, next);
	while (i<s.size()&&j<t.size()) 
	{
		if (j == 0||s[i] == t[j])
		{
			++i;
			++j;
		}
		else
			j = next[j];
	}

	if (j >= t.size())
	{
		return i - j;
	}
	else 
		return -1;
}
void getnext(string t, int* next)
{
	next[1] = 0;
	int i = 1;
	int k = 0; //当前前后缀相等的个数
	while (i<t.size()-1) //这里减1，是因为下一时刻的next是由当前时刻确定的，所以最后一位的next是next-1确定的。
	{
		if (t[k] == t[i])
		{
			++i;
			++k;
			next[i] = k;
		}
		else if (k == 0)
		{
			++i;
			++k;
			next[i] = 1;
		}
		else
			k = next[k];
	}
}
```

（2） 字符串从第一位开始检测

```cpp
int index0(string s, string t, int pos)
{
	//pos是第几个位置，而不是下标
	int i = pos - 1;
	int j = 0;
	int next[50];
	getnext0(t, next);
	for (int i = 0; i < t.size(); i++)
	{
		cout << next[i]<<" ";
	}
	cout << endl;
	while (i<s.size()&&j<t.size())//注意：这里不要使用s.size()和t.size(),因为当j=-1时，j会发生隐式转换，编程很大的整数，从而出while循环。应该在前面使用int n=s.size();
	{
		if (j==-1||s[i] == t[j]) 
		{
			++i;
			++j;
		}
		else
		{
			j = next[j];
		}
	}
	if (j >= t.size())
		return i - j;
	else
	{
		return -1;
	}
}
void getnext0(string t, int* next)
{
	next[0] = -1;
	int i = 0;
	int k = -1;
	while (i<t.size()-1)
	{
		if (k == -1) 
		{
			++k;
			++i;
			next[i] = k;
		}
		else if (t[i] == t[k])
		{
			++k;
			++i;
			next[i] = k;
		}
		else
		{
			k = next[k];
		}
	}

}
```

2. 对KMP算法改进

对于t=aaaaax这种情况，
其中next值为：012345；
假如x与s串不匹配时，然后和next[6]=4比较；
由于t串的第二，三，四，五位置的字符都与首位的a相等，那么可用首位的next[1]的值取代与他们相等的自古后续next[j]。也就是对next函数进行改进。


（1）字符串第一位保存长度，从第二位开始

```cpp
void getnextval(string t, int* nextval)
{
	nextval[1] = 0;
	int i = 1;
	int k = 0; //当前前后缀相等的个数
	while (i < t.size() - 1)
	{
		if (k == 0||t[k] == t[i])
		{
			++i;
			++k;
			if (t[i] != t[k])
				nextval[i] = k;
			else
				nextval[i] = nextval[k];
		}
		else
			k = nextval[k];
	}
}

```


（2） 字符串从第一位开始检测

```cpp
void getnext0(string t, int* nextval)
{
	nextval[0] = -1;
	int i = 0;
	int k = -1;
	while (i<t.size()-1)
	{
		if (k == -1) //注意这里必须k==-1写在前面，因为当k==-1时，t[-1]是错的。
		{
			++k;
			++i;
			if (t[i] != t[k])
				nextval[i] = k;
			else
				nextval[i] = nextval[k];
		}
		else if (t[i] == t[k])
		{
			++k;
			++i;
			if (t[i] != t[k])
				nextval[i] = k;
			else
				nextval[i] = nextval[k];
		}
		else
		{
			k = nextval[k];
		}
	}
}
```

其实就是加个判断条件，看下一个元素和现在的元素是否相等

原来的：
			next[i] = k;

现在的：

			if (t[i] != t[k])
				nextval[i] = k;
			else
				nextval[i] = nextval[k];

**5. 二叉树遍历**

	{leetcode:
		94,（简单） (2024/5/4 15:47:30 )
		95,
		96,
		102,（中等）（2024/5/4 17:07:20 ）
		116（中等）（2024/5/9 13:09:19 ）
	}

（1）前序递归遍历

```cpp
template<class T>
void ShowBiTree(TreeNode<T>*& t)
{
	if (t == nullptr)
	{
		return;
	}
	else
	{
		cout << t->data<<" "; //注意，在这里改其他操作
		ShowBiTree(t->lchild);
		ShowBiTree(t->rchild);
	}
}
```

若需要返回值可直接用参数返回，仍然用void：

```cpp
void ShowBiTree(TreeNode<T>*& t, vector<int>& a);
{
	if (t == nullptr)
	{
		return;
	}
	else
	{
		a.push_back(t-data);
		ShowBiTree(t->lchild);
		ShowBiTree(t->rchild);
	}
}
```

(2)前序非递归遍历（中左右）

前序和中序都是用深度优先的方式实现的；
所以对一颗树或者子树进行先序和中序遍历时入栈顺序都是相同的；
都是根->左->右的顺序入栈；
**前序访问后入栈；
中序出栈后访问；**
所以：先序是入栈顺序；
中序是出栈顺序

```cpp
template<class T>
void ShowBiTree_2_(TreeNode<T>* t)
{
	stack<TreeNode<T>*> stk; //用栈存储
	TreeNode<T>* p = t;
	while (p||!stk.empty())
	{
		if (p) 
		{
			cout << p->data << " "; //注意在这里改操作
			stk.push(p);
			p = p->lchild;
		}
		else
		{
			auto q = stk.top();
			stk.pop();
			p = q->rchild;
		}
	}
}
```

（3）中序递归遍历

```cpp
template<class T>
void ShowBiTree_2(TreeNode<T>*& t)
{
	if (t == nullptr)
	{
		return;
	}
	else
	{
		ShowBiTree(t->lchild);
		cout << t->data << " ";
		ShowBiTree(t->rchild);
	}
}
```

（4）中序非递归遍历

```cpp
//中序非递归
template<class T>
void ShowBiTree_2_(TreeNode<T>* t)
{
	stack<TreeNode<T>*> stk; //用栈存储
	TreeNode<T>* p = t;
	while (p||!stk.empty())
	{
		if (p) 
		{
			stk.push(p);
			p = p->lchild;
		}
		else
		{
			auto q = stk.top();
			stk.pop();
			cout << q->data << " ";//注意，这里改其他操作
			p = q->rchild;
		}
	}
}
```
（5）后序递归遍历

```cpp
template<class T>
void ShowBiTree_2(TreeNode<T>*& t)
{
	if (t == nullptr)
	{
		return;
	}
	else
	{
		ShowBiTree(t->lchild);
		ShowBiTree(t->rchild);
		cout << t->data << " ";
	}
}
```
（6）后序非递归遍历
中序遍历：左中右
后序遍历：左右中
在左序遍历的时候，左节点访问后，直接就返回根节点了，然后`·p = q->rchild;`访问右节点；

而在后序遍历是左右中，所以在后序遍历的时候我们应先访问右再访问中。也就是说，当根节点是右节点返回的时候，我们才输出根节点。
也就是说，当走到最左边的时候，先判断这个节点有没有右节点，若有且右节点没有被访问，则访问右节点，若没有则输出当前根节点。

```cpp
void PreOrderTraversal(BinTree *BT)
{
    if (!BT)
        return;
    BinTree *P = BT;
	BinTree *Flag = root;//保存上次访问的节点（用于判断上次的节点是否是右节点，若是右节点则输出根节点，若不是右节点则访问），初始化为root是利用二叉树是无环图
    stack<BinTree*>st;
    while (P != NULL || !st.empty())
    {
        if(P)//走到最左边
        {
            st.push(P);
            P = P->Left;
        }
        else
        {
            P = st.top();
            if(P->Right != NULL && P->Right != Flag)//右子树存在，并且未被访问
            {
                P = P->Right;
            }
            else
            {
                st.pop();
                cout << P->value << endl;
                Flag = P;//记录当前访问的节点
                P = NULL;//当前节点设置为null，防止重复访问左子树
            }
        }
​
    }
}

```

（7）层序遍历

```cpp
//层序遍历输出
template<class T>
void ShowBiTree_3(TreeNode<T>* t)
{
	queue<TreeNode<T>*> que;//创建一个队列
	TreeNode<T>* p;
	que.push(t);
	while (!que.empty())
	{
		p = que.front();
		que.pop();
		cout << p->data<<" "; //注意
		if (p->lchild!=nullptr)
		{
			que.push(p->lchild);
		}
		if (p->rchild != nullptr)
		{
			que.push(p->rchild);
		}
	}
}
```

若要记录每一层的结果，则可以定义一个二维数组保存，和用size来控制每一层的个数：

```cpp
vector<vector<int>> levelOrder(TreeNode* root) {
        queue<TreeNode*>que;
        vector<vector<int>> res;
        int size;
        TreeNode* p;
        if(root!=nullptr) //注意这里防止假如空指针
            que.push(root);
        while(!que.empty())
        {
            size=que.size();
            vector<int> ress;
            while(size--)
            {
                p=que.front();
                que.pop();
                ress.push_back(p->val);
                if(p->left)
                {
                    que.push(p->left);
                }
                if(p->right)
                {
                    que.push(p->right);
                }
            }
            res.push_back(ress);
        }
        return res;
    }
```

DFS:连通性问题（适合），方案数问题（适合），最小步数（不适合）
BFS：连通性问题（适合），方案数问题（不适合），最小步数（适合）

（8）morris前序遍历(leetcode 144)
Morris 遍历的核心思想是利用树的大量空闲指针，实现空间开销的极限缩减。其前序遍历规则总结如下：

新建临时节点，令该节点为 root；

如果当前节点的左子节点为空，将当前节点加入答案，并遍历当前节点的右子节点；

如果当前节点的左子节点不为空，在当前节点的左子树中找到当前节点在中序遍历下的前驱节点：

如果前驱节点的右子节点为空，将前驱节点的右子节点设置为当前节点。然后将当前节点加入答案，并将前驱节点的右子节点更新为当前节点。当前节点更新为当前节点的左子节点。

如果前驱节点的右子节点为当前节点，将它的右子节点重新设为空。当前节点更新为当前节点的右子节点。

重复步骤 2 和步骤 3，直到遍历结束。

这样我们利用 Morris 遍历的方法，前序遍历该二叉树，即可实现线性时间与常数空间的遍历。


```cpp
class Solution {
public:
    vector<int> preorderTraversal(TreeNode *root) {
        vector<int> res;
        if (root == nullptr) {
            return res;
        }

        TreeNode *p1 = root, *p2 = nullptr;

        while (p1 != nullptr) {
            p2 = p1->left;
            if (p2 != nullptr) {
                while (p2->right != nullptr && p2->right != p1) {
                    p2 = p2->right;
                }
                if (p2->right == nullptr) {
                    res.emplace_back(p1->val);
                    p2->right = p1;
                    p1 = p1->left;
                    continue;
                } else {
                    p2->right = nullptr;
                }
            } else {
                res.emplace_back(p1->val);
            }
            p1 = p1->right;
        }
        return res;
    }
};

```
（9）morris后序遍历(leetcode 145)
```cpp
class Solution {
public:
    void addPath(vector<int> &vec, TreeNode *node) {
        int count = 0;
        while (node != nullptr) {
            ++count;
            vec.emplace_back(node->val);
            node = node->right;
        }
        reverse(vec.end() - count, vec.end());
    }

    vector<int> postorderTraversal(TreeNode *root) {
        vector<int> res;
        if (root == nullptr) {
            return res;
        }

        TreeNode *p1 = root, *p2 = nullptr;

        while (p1 != nullptr) {
            p2 = p1->left;
            if (p2 != nullptr) {
                while (p2->right != nullptr && p2->right != p1) {
                    p2 = p2->right;
                }
                if (p2->right == nullptr) {
                    p2->right = p1;
                    p1 = p1->left;
                    continue;
                } else {
                    p2->right = nullptr;
                    addPath(res, p1->left);
                }
            }
            p1 = p1->right;
        }
        addPath(res, root);
        return res;
    }
};

作者：力扣官方题解
链接：https://leetcode.cn/problems/binary-tree-postorder-traversal/solutions/431066/er-cha-shu-de-hou-xu-bian-li-by-leetcode-solution/
来源：力扣（LeetCode）
著作权归作者所有。商业转载请联系作者获得授权，非商业转载请注明出处。
```

**6. 二叉树计算叶子节点**

```cpp
template<class T>
int Leadcount(TreeNode<T>*& t)
{
	if (t == nullptr)
		return 0;
	if (t->lchild == nullptr && t->rchild == nullptr)
		return 1;
	else
	{
		return Leadcount(t->lchild) + Leadcount(t->rchild);
	}

}
```
**7. 二叉树计算总结点数**
```cpp
template<class T>
int Nodecount(TreeNode<T>*& t)
{
	if (t == nullptr)
		return 0;
	else
	{
		return Nodecount(t->lchild) + Nodecount(t->rchild) + 1;
	}
}
```
**8. 二叉树计算深度**
```cpp
int m;
int n;
template<class T>
int Depth(TreeNode<T>*& t)
{
	if (t == nullptr)
		return 0;
	else
	{
		m = Depth(t->lchild);
		n = Depth(t->rchild);
		if (m > n)
			return m + 1;
		else
		{
			return n + 1;
		}
	}
}
```

**9. DFS和BFS**

	{leetcode;
	127,
	200,（2024/5/11）
	473,
	103,
	130,
	994,
	695,
	542,
	547,
	417,
	
	}

深搜和广搜对应二叉树中的递归和层序遍历

**9.1.. DFS:深度优先算法**

DFS（深度优先搜索）算法用于遍历或搜索图或树的节点。它从图或树的一个节点开始，沿着路径尽可能深地访问节点，直到达到不能访问或已经访问过的节点为止，然后回溯到上一个未访问过的节点，继续深度优先搜索。

以下是 DFS 算法的基本思想：

(1) 从图或树的某个起始节点开始，标记该节点为已访问。

(2)对于当前节点的每个相邻节点（未访问过的节点），递归地对该相邻节点进行深度优先搜索。

(3) 重复步骤 2，直到达到不能继续深入搜索的条件（例如，当前节点没有未访问的相邻节点，或者已经访问过所有相邻节点）。

(4) 回溯到上一个未访问过的节点，继续深度优先搜索。

DFS 算法通常使用递归或栈来实现。递归实现更简洁，但由于递归调用的深度可能导致堆栈溢出，因此对于大型图或树来说，使用栈实现更为安全。

以下是一个使用递归实现的简单 DFS 算法的伪代码示例：
```cpp
DFS(graph G, node v):
    mark v as visited
    print v
    for each neighbor u of v:
        if u is not visited:
            DFS(G, u)

```
在这个伪代码中，G 是图，v 是当前节点。mark v as visited 表示将节点 v 标记为已访问。然后，我们打印节点 v，并对 v 的每个未访问的相邻节点 u 递归调用 DFS 函数。


在伪代码中，DFS(G, u) 表示对节点 u 进行深度优先搜索。具体来说，它是一个递归调用，用于在图 G 中从节点 u 开始进行深度优先搜索。
```cpp
//邻接矩阵
void DFSTraverse(const Mgraph& G)
{
	for (int i = 0; i < G.numNodes; ++i)//注意1：标签初始化
		visited[i] = false;
	for (int i = 0; i < G.numNodes; ++i)//注意2：遍历所有的点
	{
		if (!visited[i]) //防止有非连通图，遍历每个节点（若是连通图则只会执行一次）
			DFS(G, i);
	}
}

void DFS(const Mgraph& G, int i) {
    visited[i] = true; //注意3：标记访问过
    std::cout << G.vexs[i] << " ";
    for (int j = 0; j < G.numNodes; ++j) { //注意4：遍历i点邻接的所有点
        if (G.arc[i][j] == 1 && !visited[j]) { //注意5：如果满足条件，则深搜
            DFS(G, j); // 递归调用 DFS 函数
        }
    }
}
```
深搜，大致流程就是上述5个注意
```CPP
//邻接表
void DFS2(const GraphAdjList& G, int i)
{
	EdgeNode* p;
	visited[i] = true;
	cout << G.adjlist[i].data << " ";
	p = G.adjlist[i].fristedge;
	while (p)
	{
		if (!visited[p->adjvex])
			DFS2(G, p->adjvex);
		p = p->next;
	}
}
void DFSTraverse2(const GraphAdjList& G)
{
	int i;
	for (i = 0; i < G.numNodes; i++)
		visited[i] = false;
	for (i = 0; i < G.numNodes; i++)
	{
		if (!visited[i])
			DFS2(G, i);
	}
}
```

### 补充

1. 树深搜

![深搜树](https://github.com/Yaozhaoxiang/MyLeetcode/blob/main/MarkdownPhotos/dfs2.png?raw=true)

根据二叉树触碰点的时机：先中后。可以写出，二叉树前序，中序，后序

```cpp
void dfs(int n)
{
	cout<<u; //前序 前面的1
	dfs(2*u);
	cout<<u;//中序  2
	dfs(2*n+1);
	cout<<u;//后序  4
}
```

2. 图深搜

![图dfs](https://github.com/Yaozhaoxiang/MyLeetcode/blob/main/MarkdownPhotos/dfs1.png?raw=true)

对树深搜，由于会出现环，所以需要用标记，来确定是否遍历过；


**9.2.. BFS:广搜优先算法**
{leetcode:
		200
		695
		979
}
本质和二叉树的层序遍历一样，用队列存放。

1. 广搜的过程
   从根开始，向下逐层扩展，逐层访问
2. 宽搜实现
   宽搜是通过队列实现的，用queue创建一个队列。宽搜的过程，通过队列来维护序列的状态空间，入队就是排队等待，出队就扩展儿子们入队

注意图论都要用visited标签
```cpp
//邻接矩阵
void BFSTraverse(const Mgraph& G)
{
	int size=0
	int i, j;
	queue<int> Q;
	for (i = 0; i < G.numNodes; i++)//注意1：标签初始化
		visited[i] = false;
	for (i = 0; i < G.numNodes; i++)//注意2;遍历所有点点
	{	//注意3：如果没被访问，则进入
		if (!visited[i]) //遍历非连通图节点（若是连通图则只会执行一次）//这一步已经把第一个连通图全部遍历完了，若是相求，非联通集个数，可在下面加size++
		{
			size++;//计算非联通集合的个数
			visited[i] = true;//注意4：标记标签
			cout << G.vexs[i] << " ";
			Q.push(i);//注意5：放入队列
			while (!Q.empty()) //注意6，遍历联通集合
			{
				i = Q.front();
				Q.pop();
				for (j = 0; j < G.numNodes; j++)
				{
					if (G.arc[i][j] == 1 && !visited[j])
					{
						visited[j] = true;
						cout << G.vexs[j] << " ";
						Q.push(j);
					}
				}
			}
		}
	}
}
```
```cpp
//邻接表
void BFSTraverse2(const GraphAdjList& G)
{
	int i;
	EdgeNode* p;
	queue<int>Q;
	for (i = 0; i < G.numNodes; i++)
		visited[i] = false;
	for (i = 0; i < G.numNodes; i++)
	{
		if (!visited[i]) //连通图直走一次
		{
			visited[i] = true;
			cout << G.adjlist[i].data << " ";
			Q.push(i);
			while (!Q.empty()) //遍历每个头节点
			{
				int j = Q.front();
				Q.pop();
				p = G.adjlist[j].fristedge;
				while (p) //每个头节点的边
				{
					if (!visited[p->adjvex])
					{
						visited[p->adjvex] = true;
						cout << G.adjlist[p->adjvex].data << " ";
						Q.push(p->adjvex);
					}
					p = p->next;
				}
			}
		}
	}
}
```





**BFS的重新理解**
（111，752）



**10.最小生成树-Prim（普里姆）算法**

	{leetcode:
			1097,
			1098
			1584（中等）,（2024/5/6 18:09:14 ）
			1135,
	}
对于一个无向树的邻接矩阵，比如MA,有9个点
```cpp
void MiniSpanTree(const int MA[][9])
{
	int num = 9;
	int i, j, min,k;
	int adjvex[20]; //保存相关顶点边的权重点下标
	int lowcost[20]; //保存相关顶点边的权重
	lowcost[0] = 0;
	adjvex[0] = 0;
	for (i = 1; i < num; i++)
	{
		lowcost[i] = MA[0][i]; //0点到其他点的距离
		adjvex[i] = 0; //i代表各点，0代表：0点到i点
	}
	for (i = 1; i < num; i++)
	{
		min = 100;
		j = 1;
		k = 0;
		while (j< num)
		{
			if (lowcost[j] != 0 && lowcost[j] < min)
			{
				min = lowcost[j];
				k = j;
			}
			j++;
		}
		cout << adjvex[k] << " " << k << endl;
		//更新lowcost和adjvex
		lowcost[k] = 0;
		for (j = 1; j < num; j++)
		{
			if (lowcost[j] != 0 && MA[k][j] < lowcost[j])
			{
				lowcost[j] = MA[k][j];
				adjvex[j] = k;
			}
		}
	}

}
```

**11.最小生成树-Kruskal（克鲁斯卡尔）算法**
利用并查法.
这里仍然使用：对于一个无向树的邻接矩阵，比如MA,有9个点
```cpp
//克鲁斯卡尔(Kruskal)算法
void MiniSpanTree_Kruskal(const int MA[][9])
{
	int num = 9;
	int n, m;
	Edge edges[20];
	int fa[20]; 
	int cnt=0;
	int ans=0;
	int idx = 0;
	//取出每条边
	for (int i = 0; i < num; i++)
	{
		for (int j = i+1; j < num; j++)
		{
			if (MA[i][j] != 100 && MA[i][j] != 0)
			{
				edges[idx].begin = i;
				edges[idx].end = j;
				edges[idx].w = MA[i][j];
				idx++;
			}
		}
	}
	//边进行排序，从小到大（注意用到lambda表达式）
	sort(edges, edges + 15, [](const Edge& a, const Edge& b) {return a.w < b.w; });
	//初始化
	for (int i = 0; i < num; i++)
	{
		fa[i] = i;
	}
	//合并
	for (int i = 0; i < 15; i++)
	{
		n = find_edges(fa, edges[i].begin);
		m = find_edges(fa, edges[i].end);
		if (n != m)
		{
			fa[n] = m;
			ans += edges[i].w;
			cnt++;
			cout << edges[i].begin << " "<< edges[i].end << " " << edges[i].w << endl;
		}
	}
	cout << "cnt:" << cnt << ";ans:" << ans << endl;
}
//查找（路径合并）
int find_edges(int* fa,int i)
{
	if (fa[i] == i)
		return i;
	else
		return fa[i] = find_edges(fa,fa[i]);
}
```

	lambda 表达式 [&](const Edge& a, const Edge& b) {return a.w < b.w; }
用于在排序算法中作为比较函数。它捕获了外部的所有变量（使用[&]），并以按值传递的方式获取所有的参数（const Edge& a 和 const Edge& b）。lambda表达式的主体是一个简单的比较操作，它比较两个Edge对象的w成员变量的大小，并返回比较结果。

	sort(edges, edges + 15, [](const Edge& a, const Edge& b) {return a.w < b.w; });


**12.并查集**
	{leetcode:
			116（中等）；(2024/5/10 16:43:37 )
			200
			695
			128	
			547
			765
			947
	}


##背景

并查集常用来解决连通性问题。

大白话就是当我们需要判断两个元素是否在同一个集合里的时候，我们就要想到用并查集。

并查集主要有两个功能：

将两个元素添加到一个集合中。
判断两个元素在不在同一个集合

##实现

用集合中的一个元素代表集合；
(数组尽量都用vector，好控制长度)；
1.初始化

假如有编号为1, 2, 3, ..., n的n个元素，我们用一个数组fa[]来存储每个元素的父节点（因为每个元素有且只有一个父节点，所以这是可行的）。一开始，我们先将它们的父节点设为自己。
```cpp
int fa[MAXN];
inline void init(int n)
{
    for (int i = 1; i <= n; ++i)
        fa[i] = i;
}
```
2.查询
用递归的写法实现对代表元素的查询：一层一层访问父节点，直至根节点（根节点的标志就是父节点是本身）。要判断两个元素是否属于同一个集合，只需要看它们的根节点是否相同即可。
```cpp
int find(int x)
{
    if(fa[x] == x)
        return x;
    else
        return find(fa[x]);
}
```
```cpp
int find(int x)
{
	return fa[x]==x?x:fa[x]=find(fa[x]);
}
```
3.合并
合并操作也是很简单的，先找到两个集合的代表元素，然后将前者的父节点设为后者即可。当然也可以将后者的父节点设为前者
```cpp
inline void merge(int i, int j)
{
    fa[find(i)] = find(j);
}
```
3.1合并优化（路径压缩,针对find）
```cpp
int find(int x)
{
    if(x == fa[x])
        return x;
    else{
        fa[x] = find(fa[x]);  //父节点设为根节点
        return fa[x];         //返回父节点
    }
}
```

3.2按秩合并（针对merge）
初始化（按秩合并）
```cpp
void init(int n)
{
    for (int i = 1; i <= n; ++i)
    {
        fa[i] = i;
        rank[i] = 1;
    }
}
```
合并（按秩合并）
```cpp
void merge(int i, int j)
{
    int x = find(i), y = find(j);    //先找到两个根节点
    if (rank[x] <= rank[y])
        fa[x] = y;
    else
        fa[y] = x;
    if (rank[x] == rank[y] && x != y)
        rank[y]++;                   //如果深度相同且根节点不同，则新的根节点的深度+1
}
```
3.3按秩合并+路径压缩

秩不是准确的子树高，而是子树高的上界，因为路径压缩可能改变子树高。还可以将秩定义成子树节点数，因为节点多的树倾向更高。无论将秩定义成子树高上界，还是子树节点数，按秩合并都是尝试合出最矮的树，并不保证一定最矮。


路径压缩会改变树的深度，但这并不影响rank的使用。rank主要用于在合并操作中决定哪个集合合并到哪个集合，而路径压缩主要用于优化查找操作的性能。两者可以结合使用，以达到更好的性能。
```cpp
// 注意：使用该代码，并不能使得所有的元素都直接指向根节点，仍然存在间接的指向
class Djset {
public:
    vector<int> parent;  // 记录节点的根
    vector<int> rank;  // 记录根节点的深度（用于优化）
    Djset(int n): parent(vector<int>(n)), rank(vector<int>(n)) {
		//rank(vector<int>(n),初始化n个0
        for (int i = 0; i < n; i++) {
            parent[i] = i;
        }
    }
    
    int find(int x) {
        // 压缩方式：直接指向根节点
        if (x != parent[x]) {
            parent[x] = find(parent[x]);
        }
        return parent[x];
    }
    
    void merge(int x, int y) {
		//注意：若要计算集合的个数，merge一次，集合就减少1
        int rootx = find(x);
        int rooty = find(y);
		if（rootx==rooty）
			return false;
        if (rank[rootx] < rank[rooty]) {
            swap(rootx, rooty);
        }
        parent[rooty] = rootx;
        if (rank[rootx] == rank[rooty]) rank[rootx] += 1;
    }
};
```
假如我们要求集合的个数，可以定义一个全局遍历，先初始化为n（节点数），每当节点合并的时候减一，即可得到答案。

**13.最短路径-迪杰斯特拉(Dijkstra)算法**(可优化，用堆，未学后补)
  

	{
	leetcode:
		743(中等)，（2024/5/7 19:38:03 ）
	}

```cpp
//ma是邻接矩阵
void ShortestPath_Dijkstra(const vector<vector<int>>& ma, int v0, vector<int>& P, vector<int>& D)
{
	int n = ma[0].size();
	vector<int> fa(n);
	P = vector<int>(n,-1);//-1表示起始点
	D = ma[0];
	//从v0开始,初始化
	D[v0] = 0;
	fa[v0] = 1;
	//v0开始找路径，从v1开始
	int min;
	int k=-1;
	for (int i = 1; i < n; i++)//遍历所有点
	{
		min = INT_MAX;
		for (int j = 0; j < n; j++)//找到当前D中路径最短的那条边
		{
			if (!fa[j] && D[j] < min)
			{
				min = D[j];
				k = j;
			}
		}
		fa[k] = 1;
		//更新P和D
		for (int j = 0; j < n; j++)
		{
			if (!fa[j] && min + ma[k][j] < D[j])
			{
				D[j] = min + ma[k][j];
				P[j] = k;
			}
		}
	}
}
```
输出路径信息，注意P是保存i点的前驱，而不是经过第i个点（和Floyd不一样）
```cpp
    //起始点v0
    for (int i = 1; i < n; i++)
    {
        cout << "start:" << v0<<"; end:"<<i << "; distance:" << D[i]<<endl;
        cout << "path: " << i;
        int j = i;
        while (P[j]!=-1)
        {
            cout << "<-" << P[j];
            j=P[j];
        }
        cout << "<-" << v0<<endl;
    }
```
**14.最短路径-Floyd（弗洛伊德）算法**

```cpp
//Floyd（弗洛伊德）算法
void ShortestPath_Floyd(const vector<vector<int>>& ma,vector<vector<int>>& P, vector<vector<int>>& D)
{
	int n = ma[0].size();
	vector<int> P_(n);
	std::iota(P_.begin(), P_.end(), 0);
	D = ma;
	P = vector<vector<int>>(n, P_);//P矩阵保存着需要经过的点P[1][2]=3;说明1->2要先经过3即1->3->2;
	for (int k = 0; k < n; k++)
	{
		for (int v = 0; v < n; v++)
		{
			for (int w = 0; w < n; w++)
			{
				if (D[v][w] > D[v][k] + D[k][w])
				{
					D[v][w] = D[v][k] + D[k][w];
					P[v][w] = P[v][k];//这里表示，从v到w先从v走到k，然后再从k走到w；但是从k到w=P[k][w]
				}
			}
		}
	}

}
```
输出路径：
```cpp
    cout << "各路径如下:" << endl;
    for (int i = 0; i < n; i++)
    {
        for (int j = i+1; j < n; j++)
        {
            cout << "start:" << i << "; end:" << j << "; distance:" << D2[i][j] << endl;
            int k = P2[i][j];
            cout << "path: " << i;
            while (k!=j)
            {
                cout << "->"<<k;
                k = P2[k][j];
            }
            cout << "->" << j << endl;
        }
        cout << endl;
    }
```

**15.拓扑排序**
{leetcode:
		207(中等);（2024/5/8 17:22:40）
		210（中等）;（2024/5/8 20:39:49 ）

}
（1）广度优先搜索
1.用邻接表保存结构
```cpp
//边
struct EdgeNode
{
	int adjvex;
	arc_type w;
	EdgeNode* next;
};
//头节点
struct VertexNode
{
	int in;
	vexs_type data;
	EdgeNode* fristedge;
};
struct GraphAdjList
{
	VertexNode adjlist[20];
	int numNodes, numEdges;
};
```
//拓扑排序
```cpp
bool TopologicalSort(GraphAdjList G)
{
	vector<int>tp;
	EdgeNode* p;
	queue<int>que;
	int gettop;
	for (int i = 0; i < G.numNodes; i++) //遍历每个点，入度为0的进入队列
	{
		if (G.adjlist[i].in == 0)
			que.push(i);
	}
	while (!que.empty())
	{
		gettop = que.front();
		que.pop();
		tp.push_back(gettop);
		for (p = G.adjlist[gettop].fristedge; p; p = p->next) //与节点相连接的点，入度减一
		{
			int k = p->adjvex;
			if (!(--G.adjlist[k].in)) //若入度为0，则入队
				que.push(k);
		}
	}
	for (auto x : tp) { cout <<G.adjlist[x].data<<" "; }
	if (tp.size() == G.numNodes)
		return true;
	else
		return false;
}
```
2.或者用din[N]来存入度；
```cpp
int din[N];
vector<int> e[N],tp;//e[N]保存相邻边，tp保存拓扑序列
bool toposort()
{
	queue<int>que;
	for(int i=0;i<n;i++)
		if（din[i]==0）
			que.push(i);
	while(q.size())
	{
		int x=que.front();
		que.pop();
		tp.push_back(x);
		for(auto y:e[x])
		{
			if(--din[y]==0)
				que.push(y);
		}	
	}
	return tp.size()==n;
}
```
（2）深度优先算法
e[x]存点x的领点，tp存拓扑排序，c[x]存点x的颜色
算法的核心是变色法，一路搜一路给点变色，如果有拓扑序，每个点的研三会是从0->-1->1，经历三种变色
0：「未搜索」：我们还没有搜索到这个节点；
-1：「搜索中」：我们搜索过这个节点，但还没有回溯到该节点，即该节点还没有入栈，还有相邻的节点没有搜索完成）；
1：「已完成」：我们搜索过并且回溯过这个节点，即该节点已经入栈，并且所有该节点的相邻节点都出现在栈的更底部的位置，满足拓扑排序的要求。
步骤：
1. 初始状态，所有点染色为0
2. 枚举每个点，进入x点，把x点染色为-1.然后，枚举x的儿子y，如果y的颜色为0，说明没有走过该点，进入y继续走
3. 如果枚举完x的儿子，没有发现环，则x染色为1，并把x压入tp数组
4. 如果发现有个儿子为-1，说明会到了祖先节点，发现了环，则一路返回false，退出程序

```cpp
vector<int> e[N],tp;
int c[N];//染色数组
bool dfs(int x)
{
	c[x]=-1;
	for(int y:e[x])
	{
		if(c[y]<0)return 0;//有环
		else if(c[y]==0)
			if(!dfs(y))return 0;
	}
	c[x]=1;
	tp.push_back(x);
	return 1;
}
bool toposort()
{
	memset(c,0,sizeof(c));
	for(itn x=1;x<n;x++)
	{
		if(!c[x])
			if(!dfs(x))return 0;
	}
	reverse(tp.begin(),tp.end()); //先压入最底层，所以需要反转
	return 1;
}
```

**16.关键路径算法**

```cpp
//关键路径算法
class CritialPath {
public:
	vector<int> etv; //存放节点最早时间
	vector<int> ltv; //存放节点最晚时间
	vector<int> ete; //存放边最早时间
	vector<int> lte; //存放边最晚时间
	stack<int> stk; //存放拓扑序列
	bool TopologicalSort(GraphAdjList G);
	void critial_path(const GraphAdjList& G);
};
//更新etv
bool CritialPath::TopologicalSort(GraphAdjList G)
{
	int gettop;
	EdgeNode* p;
	//压入入度为0的点
	stack<int> stk_inner;
	for (int i = 0; i < G.numNodes; i++)
	{
		if (!G.adjlist[i].in)
			stk_inner.push(i);
	}
	this->etv = vector<int>(G.numNodes, 0);//etv初始化
	while (!stk_inner.empty())
	{
		gettop = stk_inner.top();
		stk_inner.pop();
		this->stk.push(gettop); //存入拓扑序列到stk
		p = G.adjlist[gettop].fristedge;
		while (p)
		{
			if (!(--(G.adjlist[p->adjvex].in)))
				stk_inner.push(p->adjvex);
			if (etv[gettop] + p->w > this->etv[p->adjvex]) //更新etv
				this->etv[p->adjvex] = this->etv[gettop] + p->w;
			p = p->next;
		}
	}
	if (this->stk.size() == G.numNodes)
		return true;
	else
		return false;

}
void CritialPath::critial_path(const GraphAdjList& G)
{
	int gettop;
	EdgeNode* p;
	int k;
	int idx = 0;
	TopologicalSort(G);//更新etv和stk;
	this->ltv = vector<int>(G.numNodes, this->etv[G.numNodes - 1]);//初始化ltv，值全部为终点的etv（重点最早时间和最晚时间相等）
	while (!stk.empty())
	{
		gettop = stk.top();
		stk.pop();
		p = G.adjlist[gettop].fristedge;
		while (p)
		{
			k = p->adjvex;
			if (ltv[k] - p->w < ltv[gettop]) //更新最晚时间
				ltv[gettop] = ltv[k] - p->w;
			p = p->next;
		}
	}
	for (int i = 0; i < G.numNodes; i++) //更新边的最晚时间和最早时间
	{
		for (p = G.adjlist[i].fristedge; p; p = p->next)
		{
			k = p->adjvex;
			ete.push_back(etv[i]);
			lte.push_back(ltv[k] - p->w);
			if (ete[idx] == lte[idx])
				cout << "<" << G.adjlist[i].data << "-" << G.adjlist[k].data << ">" << p->w << " ;";
			idx++;
		}
	}
}
```

**17.平衡二叉树排序**


**18.哈希表**
哈希表最适合的求解问题是查找与给定值相等的记录；

c++使用哈希表
 使用STL库中的哈希表

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


**19.排序**

排序的结构和函数：
```cpp
struct SqList{
	int r[MAXSIZE+1];//由于存储要排序的数组，r[0]用作哨兵或临时变量
	int length;
};
void swap(SqList*L,int i,int j)
{
	int temp=L->r[i];
	L->r[i]=L->r[j];
	L->r[j]=temp;
}
```

1. 冒泡排序 O(n^2)
	```cpp
	void BubbleSort(SQqList* L)
	{
		int i,j;
		for(int i=1;i<L->length;i++)
		{
			for(j=L->length-1;j>=i;j++) //注意j是从后往前循环
			{
				if(L->r[j]>L->r[j+1])
				{
					swap(L,j,j+1);
				}
			}
		}

	}
	```
	**排序算法优化**：假如待排序的序列中有大部分数据是有序的，上述算法i仍然要遍历完，所以增加一个标记变量flag来进行改进；如果有数据交换则flag=TRUE，否则为FALSE
	```cpp
	void BubbleSort(SQqList* L)
	{
		int i,j;、
		bool flag=TRUE;
		for(int i=1;i<L->length&&flag;i++)//注意
		{
			flag=FALSE;//注意
			for(j=L->length-1;j>=i;j++) //注意j是从后往前循环
			{
				if(L->r[j]>L->r[j+1])
				{
					swap(L,j,j+1);
					flag=TRUE; //注意
				}
			}
		}

	}
	```
2. 简单选择排序O(n^2)
	相较于冒泡排序，此方法主要减少交换次序，先找到最小数据的下标，然后再交换元素。虽
	```cpp
	void SelectSort(SqList* L)
	{
		int i,j,min;
		for(i=1;i<L->length;i++)
		{
			min=i;
			for(j=i+1;j<=L->length;j++)
			{
				if(L->r[min]>L->r[j])
					min=j;
			}
			if(i!=min)
				swap(L.i,min);
		}
	}
	```
3. 直接插入排序O(n^2)
   本质就是将一个记录插入到已经拍好的有序表中，从而得到一个新的有序表
   ```cpp
	void InsertSort(SqList* L)
	{
		int i,j;
		for(i=2;i<=L->length;i++)//从2开始，因为第一个元素作为已经排序好的序列
		{
			if(L->r[i]<L->r[i-1])//如果当前元素小于左边的元素，则插入位置在该元素的左边
			{
				// 和哨兵比较，也就是当前要插入的元素
				//如果哨兵小于左边的元素，左边的元素右移，直到不小于左边的元素
				//由于当前元素位置在i，而且已经把值赋值给哨兵了，当前的位置可以直接被左边元素覆盖
				L->r[0]=L->r[i];
				for(j=i-1;L->r[j]>L->r[0];j--)
				{
					L->r[j+1]=L->r[j];
				}
				L->r[j+1]=L->r[0];
			}
            //如果当前元素已经大于r[i-1],则r[i]
		}
	}
	```
	数组形式：
	```cpp
	void InsertSort(vector<int>& a)
	{
		int i, j;
		int len = a.size();
		for (i = 1; i < len; i++)
		{
			if (a[i] < a[i - 1])
			{
				int key = a[i];
				for (j = i - 1; j >= 0&&a[j]>key; j--)
				{
					a[j + 1] = a[j];
				}
				a[j + 1] = key;
			}
		}
	}
	```
4. 希尔排序
	是一种插入排序，是直接插入排序算法的一种加强版；
	把记录按步长 gap 分组，对每组记录采用直接插入排序方法进行排序。
	随着步长逐渐减小，所分成的组包含的记录越来越多，当步长的值减小到 1 时，整个数据合成为一组，构成一组有序记录，则完成排序。

	![](https://images2015.cnblogs.com/blog/318837/201604/318837-20160422102024757-37862627.png)

	```CPP
	void ShellSort(SqList* L)
	{
		int i,j,k=0;
		int increment=L->length;
		do
		{
			increment=increment/3+1;
			for(i=increment+1;i<=L->length;i++)
			{
				if(L->r[i]<L->[i-increment]) //插入排序
				{
					L->r[0]=L->r[i];
					for(j=i-increment;j>0&&L->r[j]>L->r[0];j-=increment)
					{
						L->r[j+increment]=L->r[j];
					}
					L->r[j+increment]=L->r[0];
				}
			}
		}
		while(increment>1);
	}
	```
	数组形式：
	```cpp
	void ShellSort(vector<int>& a)
	{
		int i, j;
		int len = a.size();
		int increment = len;
		while (increment>1)
		{
			increment = increment / 3 + 1;
			for (i = increment; i < len; i++)
			{
				if (a[i] < a[i - increment])
				{
					int key = a[i];
					for (j = i - increment; j >= 0 && a[j] > key; j -= increment)
					{
						a[j + increment] = a[j];
					}
					a[j + increment] = key;
				}
			}
		}
	}
	```

5. 堆排序
	**堆**
	将任意节点不大于其子节点的堆叫做最小堆或小根堆，而将任意节点不小于其子节点的堆叫做最大堆或大根堆。常见的堆有二叉堆、左倾堆、斜堆、二项堆、斐波那契堆等等。

	二叉堆：
	二叉堆是完全二元树或者是近似完全二元树，它分为两种：最大堆和最小堆。
	最大堆：父结点的键值总是大于或等于任何一个子节点的键值；最小堆：父结点的键值总是小于或等于任何一个子节点的键值。示意图如下：

	![](https://images0.cnblogs.com/i/497634/201403/182339209436216.jpg)


	二叉堆一般都通过"数组"来实现。数组实现的二叉堆，父节点和子节点的位置存在一定的关系。有时候，我们将"二叉堆的第一个元素"放在数组索引0的位置，有时候放在1的位置。当然，它们的本质一样(都是二叉堆)，只是实现上稍微有一丁点区别。
	假设"第一个元素"在数组中的索引为 0 的话，则父节点和子节点的位置关系如下：
	(01) 索引为i的左孩子的索引是 (2*i+1);
	(02) 索引为i的左孩子的索引是 (2*i+2);
	(03) 索引为i的父结点的索引是 floor((i-1)/2);

	![](https://images0.cnblogs.com/i/497634/201403/182342224903953.jpg)

	假设"第一个元素"在数组中的索引为 1 的话，则父节点和子节点的位置关系如下：
	(01) 索引为i的左孩子的索引是 (2*i);
	(02) 索引为i的左孩子的索引是 (2*i+1);
	(03) 索引为i的父结点的索引是 floor(i/2);

	![](https://images0.cnblogs.com/i/497634/201403/182343402241540.jpg)

	1. 实现：最大堆排序，这里使用的是从上向下调整

	```cpp
	#include<iostream>
	#include<vector>
	using namespace std;
	void HeapSort(vector<int>& a);
	void HeapAdjust(vector<int>& a, int i, int len);
	void swap(vector<int>& a,int i,int j);

	int main()
	{
		vector<int>a = { 9,1,5,8,3,7,4,6,2 };
		for (auto x : a) { cout << x << " "; };
		HeapSort(a);
		cout << endl;
		for (auto x : a) { cout << x << " "; };
		return 0;
	}
	// 最大堆排序
	void HeapSort(vector<int>& a)
	{
		int i;
		int len = a.size();
		//1.构建大顶堆，从(n/2-1) --> 0逐次遍历。遍历之后，得到的数组实际上是一个(最大)二叉堆。
		for (i = len / 2 - 1; i >= 0; i--)
			//从第一个非叶子结点从下至上，从右至左调整结构
			HeapAdjust(a, i,len);
		//2.调整堆结构+交换堆顶元素与末尾元素，从最后一个元素开始对序列进行调整，不断的缩小调整的范围直到第一个元素
		for (int j = len - 1; j > 0; j--)
		{
			// 交换a[0]和a[i]。交换后，a[i]是a[0...i]中最大的。
			swap(a, 0, j);//将堆顶元素与末尾元素进行交换
			// 调整a[0...i-1]，使得a[0...i-1]仍然是一个最大堆。
        	// 即，保证a[i-1]是a[0...i-1]中的最大值。
			HeapAdjust(a, 0,j);//重新对堆进行调整

		}

	}
	//这里的调整是从0开始，调整到0+len。这是因为，最大的值已经被放到堆末尾了，不能包含
	void HeapAdjust(vector<int>& a, int i, int len)
	{
		int temp, j;
		temp = a[i];//先取出当前元素i
		//从i结点的左子结点开始，也就是2i+1处开始
		for (j = 2 * i + 1; j < len; j = 2 * j + 1)
		{
			if (j + 1 < len && a[j] < a[j + 1]) //如果左子结点小于右子结点，j指向右子结点
				j++;
			if (a[j] > temp) //如果子节点大于父节点，将子节点值赋给父节点（不用进行交换）
			{
				a[i] = a[j];
				i = j;
			}
			else
			{
				break;
			}
		}
		a[i] = temp;//将temp值放到最终的位置
	}
	void swap(vector<int>& a, int i, int j)
	{
		int temp = a[i];
		a[i] = a[j];
		a[j] = temp;
	}
	```
	2. 实现：最小堆排序，这里使用的是从上向下调整，相交与最大堆排序，其实就只要更改调整数(最小)堆的向下调整算法
	```cpp
	void heapAdjust(vector<int>& a, int i, int len)
	{
		int temp, j;
		temp = a[i];//先取出当前元素i
		//从i结点的左子结点开始，也就是2i+1处开始
		for (j = 2 * i + 1; j < len; j = 2 * j + 1)
		{
			if (j + 1 < len && a[j] > a[j + 1]) //如果左子结点大于右子结点，j指向右子结点
				j++;
			if (a[j] < temp) //如果子节点小于父节点，将子节点值赋给父节点（不用进行交换）
			{
				a[i] = a[j];
				i = j;
			}
			else
			{
				break;
			}
		}
		a[i] = temp;//将temp值放到最终的位置
	}
	```
6. 归并排序
	归并排序(Merge Sort)就是利用归并思想对数列进行排序。根据具体的实现，归并排序包括"从上往下"和"从下往上"2种方式
	1. 从下往上的归并排序：将待排序的数列分成若干个长度为1的子数列，然后将这些数列两两合并；得到若干个长度为2的有序数列，再将这些数列两两合并；得到若干个长度为4的有序数列，再将它们两两合并；直接合并成一个数列为止。这样就得到了我们想要的排序结果。(参考下面的图片)
	2. 从上往下的归并排序：它与"从下往上"在排序上是反方向的。它基本包括3步：
	① 分解 -- 将当前区间一分为二，即求分裂点 mid = (low + high)/2;
	② 求解 -- 递归地对两个子区间a[low...mid] 和 a[mid+1...high]进行归并排序。递归的终结条件是子区间长度为1。
	③ 合并 -- 将已排序的两个子区间a[low...mid]和 a[mid+1...high]归并为一个有序的区间a[low...high]。

	![](https://images0.cnblogs.com/i/497634/201403/151853346211212.jpg)
	
	递归实现（从上往下）：从上往下的归并排序采用了递归的方式实现。它的原理非常简单，如下图：

	![](https://images0.cnblogs.com/i/497634/201403/151855240908900.jpg)

	通过"从上往下的归并排序"来对数组{80,30,60,40,20,10,50,70}进行排序时：
   1. 将数组{80,30,60,40,20,10,50,70}看作由两个有序的子数组{80,30,60,40}和{20,10,50,70}组成。对两个有序子树组进行排序即可。
   2. 将子数组{80,30,60,40}看作由两个有序的子数组{80,30}和{60,40}组成。
       将子数组{20,10,50,70}看作由两个有序的子数组{20,10}和{50,70}组成。
   3. 将子数组{80,30}看作由两个有序的子数组{80}和{30}组成。
       将子数组{60,40}看作由两个有序的子数组{60}和{40}组成。
       将子数组{20,10}看作由两个有序的子数组{20}和{10}组成。
       将子数组{50,70}看作由两个有序的子数组{50}和{70}组成。

	```cpp
	#include<iostream>
	#include<vector>
	void MergeSort(vector<int>& a);
	void MSort(vector<int>& a, vector<int>& temp, int left, int right);
	void Merge(vector<int>& a, vector<int>& temp, int left, int mid, int right);
	int main()
	{
		vector<int>a = { 9,1,5,8,3,7,4,6,2 };
		for (auto x : a) { cout << x << " "; };
		MergeSort(a);
		cout << endl;
		for (auto x : a) { cout << x << " "; };

		return 0;
	}
	void MergeSort(vector<int>& a)
	{
		vector<int>temp(a.size());//在排序前，先建好一个长度等于原数组长度的临时数组，避免递归中频繁开辟空间
		MSort(a, temp, 0, a.size() - 1);
	}
	void MSort(vector<int>& a, vector<int>& temp, int left, int right)
	{
		if (left < right)
		{
			int mid = left + (right - left) / 2;
			MSort(a, temp, left, mid);//左边归并排序，使得左子序列有序
			MSort(a, temp, mid + 1, right);//右边归并排序，使得右子序列有序
			Merge(a, temp, left, mid, right);//将两个有序子数组合并操作
		}
	}
	void Merge(vector<int>& a, vector<int>& temp, int left, int mid, int right)
	{
		int i = left;//左序列指针
		int j = mid + 1;//右序列指针
		int t = 0;//临时数组指针
		while (i<=mid && j<=right)
		{
			if (a[i] <= a[j])
				temp[t++] = a[i++];
			else
				temp[t++] = a[j++];
		}
		while (i<=mid) //将左边剩余元素填充到temp中
		{
			temp[t++] = a[i++];
		}
		while (j <= right) //将右边元素填充到temp中
		{
			temp[t++] = a[j++];
		}
		t = 0;
		while (left<=right)//将temp中的元素全部拷贝到原数组中
		{
			a[left++] = temp[t++];
		}
	}
	```
	非递归实现(也就是归并排序从下往上)：

	![](https://images0.cnblogs.com/i/497634/201403/151857010277686.jpg)

	通过"从下往上的归并排序"来对数组{80,30,60,40,20,10,50,70}进行排序时：
   4. 将数组{80,30,60,40,20,10,50,70}看作由8个有序的子数组{80},{30},{60},{40},{20},{10},{50}和{70}组成。
   5. 将这8个有序的子数列两两合并。得到4个有序的子树列{30,80},{40,60},{10,20}和{50,70}。
   6. 将这4个有序的子数列两两合并。得到2个有序的子树列{30,40,60,80}和{10,20,50,70}。
   7. 将这2个有序的子数列两两合并。得到1个有序的子树列{10,20,30,40,50,60,70,80}。

	```cpp
	void merge_sort_down2up(vector<int>& a)
	{
		int len = a.size();

		if (len <= 0)
			return;
		for (int n = 1; n < len; n *= 2) //每次合并的长度,1->2->4
			merge_groups(a, n);
	}
	// 对数组a做若干次合并：数组a的总长度为len，将它分为若干个长度为gap的子数组；
    // 将"每2个相邻的子数组" 进行合并排序。
	void merge_groups(vector<int>& a, int gap)
	{
		int len = a.size();
		int i;
		int twolen = 2 * gap; // 两个相邻的子数组的长度
		vector<int>temp(a.size());
		// 将"每2个相邻的子数组" 进行合并排序。
		for (i = 0; i + 2 * gap - 1 < len; i += (2 * gap))
		{
			Merge(a, temp, i, i + gap - 1, i+2*gap-1);
		}
		// 若 i+gap-1 < len-1，则剩余一个子数组没有配对。
    	// 将该子数组合并到已排序的数组中。
		if (i + gap - 1 < len - 1)
			Merge(a, temp,i, i + gap - 1, len - 1);

	}
	```
7. 快速排序
	快排的过程中，每一次我们要取一个元素作为枢纽值，以这个数字来将序列划分为两部分。在此我们采用三数取中法，也就是取左端、中间、右端三个数，然后进行排序，将中间数作为枢纽值。

	//把枢纽值放到首位
	```cpp
	void QuickSort(vector<int>& a)//函数入口
	{
		QSort(a, 0, a.size() - 1);
	}
	void QSort(vector<int>& a, int left, int right)
	{
		int pivot;
		if (left < right)
		{
			pivot = Partition(a, left, right); //将a一分为2，算出枢纽值
			QSort(a, left, pivot - 1);
			QSort(a, pivot + 1, right);
		}
	}
	int Partition(vector<int>& a, int left, int right)
	{
		//三元取中（mid,min,max），获取枢纽值，枢纽值被放在序列首尾
		int pivotkey;
		int mid = left + (right - left) / 2;
		if (a[left] > a[right])
			swap(a, left, right);
		if (a[mid] > a[left]) //将中间值，放到首位
			swap(a, left, mid);
		if (a[mid] > a[right])
			swap(a, mid, right);
		pivotkey = a[left];
		int key = pivotkey;//将中间值，备份到key
		while (left<right)
		{
			//将小于枢纽值的放在左边，直接赋值即可，枢纽值已被保存
			while (left<right && a[right]>pivotkey)
				right--;
			a[left] = a[right];
			//将大于枢纽值的放在左边，直接赋值即可
			while (left < right && a[left] < pivotkey)
				left++;
			a[right] = a[left];
		}
		//最后将枢纽值放到中间位置
		a[left] = key;
		return left;
	}
	```

	对 QSort可以进行优化，缩短递归的堆栈深度;
	```cpp
	void QSort(vector<int>& a, int left, int right)
	{
		int pivot;
		while (left < right)
		{
			pivot = Partition(a, left, right); //将a一分为2，算出枢纽值
			QSort(a, left, pivot - 1);
			left=pivot+1;
		}
	}
	```
	当讲if改成while后，因为第一次递归以后，变量left就没有用了，所以讲`left=pivot+1`，再循环后，来一次`pivot = Partition(a, left, right); `,等同于`QSort(a, pivot + 1, right);`,结果相同，但是因采用迭代而不是递归的方法可以所见堆栈深度，从而提高整体性能。

## 20 字典树(Trie)
(208,820,17_13,211,676)
字典树是一种能够快速插入和查询字符串的多叉树结构。
节点编号各不相同，根节点编号为0，其他节点用来标识路径，还可以标记单词插入的次数。边表示字符
Trie维护字符串的集合，支持两种操作：
1. 向集合中插入一个字符串
2. 在集合中查询一个字符串

建立字典树：
**儿子数组：ch[p][j]**：存储总结点p沿着j这条边走到子节点。
边为26个小写字符（a~z）对应的映射值0~25，
每个节点最多有26个分叉；
比如说ch[2][1]=2:表示从节点2沿着边1（b）可以做到节点2；
**计数数组cnt[p]**：存储以节点p结尾的单词的插入次数
**节点编号idx**：用来给节点编号
1. 空Trie仅有一个跟接待你，编号为0
2. 从根开始插入，枚举字符串的每个字符
   如果有儿子，则p指针走到儿子
   如果没有儿子，则先创建儿子，p指针再走到儿子
3. 再单词结束点记录插入次数
```cpp
int N = 200; //字符串的最大个数
vector<vector<int>>ch(N,vector<int>(26));//树
vector<int>cnt(N);//统计每个单词的次数
int idx = 0;//节点编号
//动态调整ch大小
void ensureCapacity() {
	if (idex >= ch.size()) {
		ch.resize(ch.size() * 2, vector<int>(26, 0));
		cnt.resize(cnt.size() * 2, 0);
	}
}
void Insert(string s)
{
	int p = 0;
	for (int i = 0; s[i]; i++)
	{
		int j = s[i] - 'a';//每个字符投影到0~25
		if (!ch[p][j])
			ch[p][j] = ++idx;
		ensureCapacity();//动态调整ch大小
		p = ch[p][j];
	}
	cnt[p]++;//插入次数统计
}
//查询
int query(string s)
{
	int p = 0;
	for (int i = 0; s[i]; i++)
	{
		int j = s[i] - 'a';
		if (!ch[p][j])
			return 0;
		p = ch[p][j];
	}
	return cnt[p];
}
```
在 Trie 实现中，一个常见的错误是在数组越界访问，因为你的 Trie 数组 ch 仅初始化了 2000 个节点，但是你的插入和查找操作可能会访问超出这个范围的节点。
你需要确保在访问 ch 数组的索引时，不会超出其范围。一个简单的方法是动态调整 ch 数组的大小。每次当 idex 达到当前的容量限制时，可以扩大 ch 数组的容量。

上面用这种二维数组的，会出现越界问题，可以用动态调整大小。可以换种思路来写，上面是先确定节点的个数，但是我们可以先不用确定，直接在Insert中新建一维向量即可，且一维向量中存的是指针

```cpp
class Trie {
private:
    vector<Trie*> children;// 子节点数组，大小为26
    bool isEnd;// 是否是一个单词的结束

    Trie* searchPrefix(const string& prefix) {
        Trie* node = this; // 初始化指针 node 指向当前 Trie 对象
        for (char ch : prefix) {
            int index = ch - 'a'; // 计算字符 ch 在 children 数组中的索引
            if (node->children[index] == nullptr) {
                return nullptr; // 如果对应的子节点不存在，返回 nullptr
            }
            node = node->children[index]; // 移动指针 node 到对应的子节点
        }
        return node; // 返回最后一个字符对应的节点
    }

public:
    Trie() : children(26, nullptr), isEnd(false) {}

    void insert(const string& word) {
        Trie* node = this; // 初始化指针 node 指向当前 Trie 对象
        for (char ch : word) {
            int index = ch - 'a';
            if (node->children[index] == nullptr) {
                node->children[index] = new Trie(); // 如果对应的子节点不存在，创建一个新的 Trie 节点
            }
            node = node->children[index]; // 移动指针 node 到对应的子节点
        }
        node->isEnd = true; // 标记单词的结束节点
    }

    bool search(const string& word) {
        Trie* node = searchPrefix(word); // 查找单词对应的最后一个节点
        return node != nullptr && node->isEnd; // 检查该节点是否存在并且是单词的结束节点
    }

    bool startsWith(const string& prefix) {
        return searchPrefix(prefix) != nullptr; // 查找前缀对应的最后一个节点，检查是否存在
    }

    ~Trie() {
        for (Trie* child : children) {
            if (child != nullptr) {
                delete child; // 递归删除所有子节点，防止内存泄漏
            }
        }
    }
};
```

## 21回溯
(labuladong的博客内容)
（46，47，39，40，77，78，90，60，93）
字符串中的回溯问题（17，784，22）
游戏问题（51，37，488，539）
**本质就是暴力搜索，dfs**

回溯的核心框架：
```cpp
result = []
def backtrack(路径, 选择列表):
    if 满足结束条件:
        result.add(路径)
        return
	for 选择 in 选择列表:
		# 做选择(前序遍历)
		将该选择从选择列表移除
		路径.add(选择)
		backtrack(路径, 选择列表)
		# 撤销选择（后序遍历）
		路径.remove(选择)
		将该选择再加入选择列表
```
感觉，回溯法的核心就是在于，做选择和撤销选择（在递归调用之前「做选择」，在递归调用之后「撤销选择」，特别简单。也就是在前序遍历的时候做选择，在后序遍历的时候撤销选择，回到初始状态）。本质就是DFS

所谓的前序遍历和后序遍历，他们只是两个很有用的时间点，下面这张图很形象：

![](https://pic2.zhimg.com/80/v2-9d33f25719dbe5ccdf5d2b6134ad8b4d_720w.webp)

前序遍历的代码在进入某一个节点之前的那个时间点执行，后序遍历代码在离开某个节点之后的那个时间点执行。

所以说，我们只要在递归之前做出选择，在递归之后撤销刚才的选择，就能正确得到每个节点的选择列表和路径。

**Leetcode 46self**(排序问题)
```cpp
class Solution {
public:
    vector<vector<int>>res;
    void backtracking(vector<int>& nums,vector<int>&s)
    {
        if(nums.size()==s.size())
        {
            res.push_back(s);
            return;
        }
        for(int i=0;i<nums.size();i++)
        {
            if(nums[i]==INT_MAX)
                continue;
            else
            {
                int flag=nums[i];//选择那一条路径
                s.push_back(nums[i]);
                nums[i]=INT_MAX;
                backtracking(nums,s);
                nums[i]=flag;//返回到之前的状态
                s.pop_back();
            }
        }
        return;
    }
    vector<vector<int>> permute(vector<int>& nums) {
        vector<int>s;
        backtracking(nums,s);
        return res;
    }  
};
```
有的时候，我们并不想得到所有合法的答案，只想要一个答案，怎么办呢？比如解数独的算法，找所有解法复杂度太高，只要找到一种解法就可以。
```cpp
// 函数找到一个答案后就返回 true
bool backtrack(vector<string>& board, int row) {
    // 触发结束条件
    if (row == board.size()) {
        res.push_back(board);
        return true;
    }
    ...
    for (int col = 0; col < n; col++) {
        ...
        board[row][col] = 'Q';

        if (backtrack(board, row + 1))
            return true;

        board[row][col] = '.';
    }

    return false;
}
```
这样修改后，只要找到一个答案，for 循环的后续递归穷举都会被阻断


### 回溯法其他形式
排列 组合 子集问题
无论是排列、组合还是子集问题，简单说无非就是让你从序列 nums 中以给定规则取若干元素，主要有以下几种变体：

形式一、元素无重不可复选，即 nums 中的元素都是唯一的，每个元素最多只能被使用一次，这也是最基本的形式。

以组合为例，如果输入 nums = [2,3,6,7]，和为 7 的组合应该只有 [7]。

形式二、元素可重不可复选，即 nums 中的元素可以存在重复，每个元素最多只能被使用一次。

以组合为例，如果输入 nums = [2,5,2,1,2]，和为 7 的组合应该有两种 [2,2,2,1] 和 [5,2]。

形式三、元素无重可复选，即 nums 中的元素都是唯一的，每个元素可以被使用若干次。

以组合为例，如果输入 nums = [2,3,6,7]，和为 7 的组合应该有两种 [2,2,3] 和 [7]。

当然，也可以说有第四种形式，即元素可重可复选。但既然元素可复选，那又何必存在重复元素呢？元素去重之后就等同于形式三，所以这种情况不用考虑。

上面用组合问题举的例子，但排列、组合、子集问题都可以有这三种基本形式，所以共有 9 种变化。

除此之外，题目也可以再添加各种限制条件，比如让你求和为 target 且元素个数为 k 的组合，那这么一来又可以衍生出一堆变体

但无论形式怎么变化，其本质就是穷举所有解，而这些解呈现树形结构，所以合理使用回溯算法框架，稍改代码框架即可把这些问题一网打尽。

如下子集问题和排列问题的回溯树，就可以解决所有排列组合子集相关的问题：

![](https://labuladong.online/algo/images/%E6%8E%92%E5%88%97%E7%BB%84%E5%90%88/1.jpeg)

![](https://labuladong.online/algo/images/%E6%8E%92%E5%88%97%E7%BB%84%E5%90%88/2.jpeg)

1. 子集（元素无重不可复选）
（leetcode78）题目给你输入一个无重复元素的数组 nums，其中每个元素最多使用一次，请你返回 nums 的所有子集。(注意子集不考虑顺序，也就是：[1,2]==[2,1])

比如输入 nums = [1,2,3]，算法应该返回如下子集：
`[ [],[1],[2],[3],[1,2],[1,3],[2,3],[1,2,3] ]`

首先，生成元素个数为 0 的子集，即空集 `[]`，为了方便表示，我称之为 `S_0`。
然后，在 `S_0` 的基础上生成元素个数为 1 的所有子集，我称为 `S_1`：

![](https://labuladong.online/algo/images/%E6%8E%92%E5%88%97%E7%BB%84%E5%90%88/3.jpeg)

接下来，我们可以在 `S_1` 的基础上推导出 `S_2`，即元素个数为 2 的所有子集：

![](https://labuladong.online/algo/images/%E6%8E%92%E5%88%97%E7%BB%84%E5%90%88/4.jpeg)

为什么集合 [2] 只需要添加 3，而不添加前面的 1 呢？

	因为集合中的元素不用考虑顺序，[1,2,3] 中 2 后面只有 3，如果你添加了前面的 1，那么 [2,1] 会和之前已经生成的子集 [1,2] 重复。

**换句话说，我们通过保证元素之间的相对顺序不变来防止出现重复的子集。**，也就是for循环中初始start

接着，我们可以通过 S_2 推出 S_3，实际上 S_3 中只有一个集合 [1,2,3]，它是通过 [1,2] 推出的。

整个推导过程就是这样一棵树：

![](https://labuladong.online/algo/images/%E6%8E%92%E5%88%97%E7%BB%84%E5%90%88/5.jpeg)

如果把根节点作为第 0 层，将每个节点和根节点之间树枝上的元素作为该节点的值，那么第 n 层的所有节点就是大小为 n 的所有子集。

那么再进一步，如果想计算所有子集，那只要遍历这棵多叉树，把所有节点的值收集起来不就行了？

```cpp
class Solution {
private:
    vector<vector<int>> res;
    // 记录回溯算法的递归路径
    vector<int> track;

public:
    // 主函数
    vector<vector<int>> subsets(vector<int>& nums) {
        backtrack(nums, 0);
        return res;
    }

    // 回溯算法核心函数，遍历子集问题的回溯树
    void backtrack(vector<int>& nums, int start) {

        // 前序位置，每个节点的值都是一个子集
        res.push_back(track);

        // 回溯算法标准框架
        for (int i = start; i < nums.size(); i++) { //这里起始位start,来使子集不重复
            // 做选择
            track.push_back(nums[i]);
            // 通过 start 参数控制树枝的遍历，避免产生重复的子集
            backtrack(nums, i + 1);
            // 撤销选择
            track.pop_back();
        }
    }
```

我们使用 start 参数控制树枝的生长避免产生重复的子集，用 track 记录根节点到每个节点的路径的值，同时在前序位置把每个节点的路径值收集起来，完成回溯树的遍历就收集了所有子集：

最后，`backtrack` 函数开头看似没有 `base case`，会不会进入无限递归？
	其实不会的，当 `start == nums.length` 时，叶子节点的值会被装入 res，但 for 循环不会执行，也就结束了递归。

2. 组合（元素无重不可复选）
组合也不考虑顺序，也就是：[1,2]==[2,1]
（leetcode77）
如果你能够成功的生成所有无重子集，那么你稍微改改代码就能生成所有无重组合了

你比如说，让你在 nums = [1,2,3] 中拿 2 个元素形成所有的组合，你怎么做

稍微想想就会发现，大小为 2 的所有组合，不就是所有大小为 2 的子集嘛。

所以我说组合和子集是一样的：大小为 k 的组合就是大小为 k 的子集。

给定两个整数 n 和 k，返回范围 [1, n] 中所有可能的 k 个数的组合。

比如 combine(3, 2) 的返回值应该是：
	`[ [1,2],[1,3],[2,3] ]`

还是以 nums = [1,2,3] 为例，刚才让你求所有子集，就是把所有节点的值都收集起来；现在你只需要把第 2 层（根节点视为第 0 层）的节点收集起来，就是大小为 2 的所有组合：

![](https://labuladong.online/algo/images/%E6%8E%92%E5%88%97%E7%BB%84%E5%90%88/6.jpeg)

反映到代码上，只需要稍改 base case，控制算法仅仅收集第 k 层节点的值即可：

```cpp
class Solution {
public:
    vector<vector<int>> res;
    // 记录回溯算法的递归路径
    deque<int> track;

    // 主函数
    vector<vector<int>> combine(int n, int k) {
        backtrack(1, n, k);
        return res;
    }

    void backtrack(int start, int n, int k) {
        // base case
        if (k == track.size()) {
            // 遍历到了第 k 层，收集当前节点的值
            res.push_back(vector<int>(track.begin(), track.end()));
            return;
        }

        // 回溯算法标准框架
        for (int i = start; i <= n; i++) {
            // 选择
            track.push_back(i);
            // 通过 start 参数控制树枝的遍历，避免产生重复的子集
            backtrack(i + 1, n, k);
            // 撤销选择
            track.pop_back();
        }
    }
};
```

3. 排序（元素无重不可复选）
起始就是开始的部分；
给定一个不含重复数字的数组 nums，返回其所有可能的全排列。

比如输入 nums = [1,2,3]，函数的返回值应该是：
	[
    [1,2,3],[1,3,2],
    [2,1,3],[2,3,1],
    [3,1,2],[3,2,1]
	]

刚才讲的组合/子集问题使用 `start` 变量保证元素 `nums[start]` 之后只会出现 `nums[start+1..]` 中的元素，通过固定元素的相对位置保证不出现重复的子集。

但排列问题本身就是让你穷举元素的位置，`nums[i]` 之后也可以出现 `nums[i]`左边的元素，所以之前的那一套玩不转了，需要额外使用 `used` 数组来标记哪些元素还可以被选择。

![](https://labuladong.online/algo/images/%E6%8E%92%E5%88%97%E7%BB%84%E5%90%88/7.jpeg)

我们用 `used` 数组标记已经在路径上的元素避免重复选择，然后收集所有叶子节点上的值，就是所有全排列的结果：

```cpp
class Solution {
public:
    // 存储所有排列结果的列表
    vector<vector<int>> res;
    // 记录回溯算法的递归路径
    list<int> track;
    // 标记数字使用状态的数组，0 表示未被使用，1 表示已被使用
    bool* used;

    /* 主函数，输入一组不重复的数字，返回它们的全排列 */
    vector<vector<int>> permute(vector<int>& nums) {
        used = new bool[nums.size()]();
        // 满足回溯框架时需要添加 bool 类型默认初始化为 false
        backtrack(nums);
        return res;
    }

    // 回溯算法核心函数
    void backtrack(vector<int>& nums) {
        // base case，到达叶子节点
        if (track.size() == nums.size()) {
            // 收集叶子节点上的值
            res.push_back(vector<int>(track.begin(), track.end()));
            return;
        }
        // 回溯算法标准框架
        for (int i = 0; i < nums.size(); i++) {
            // 已经存在 track 中的元素，不能重复选择
            if (used[i]) {
                continue;
            }
            // 做选择
            used[i] = true;
            track.push_back(nums[i]);
            // 进入下一层回溯树
            backtrack(nums);
            // 取消选择
            track.pop_back();
            used[i] = false;
        }
    }
};
```

但如果题目不让你算全排列，而是让你算元素个数为 k 的排列，怎么算？

也很简单，改下 `backtrack` 函数的 `base case`，仅收集第 k 层的节点值即可：
```cpp
// 回溯算法核心函数
void backtrack(vector<int>& nums, int k, vector<vector<int>>& res, vector<int>& track) {
    // base case，到达第 k 层，收集节点的值
    if (track.size() == k) {
        // 第 k 层节点的值就是大小为 k 的排列
        res.push_back(track);
        return;
    }

    // 回溯算法标准框架
    for (int i = 0; i < nums.size(); i++) {
        // ...
        backtrack(nums, k, res, track);
        // ...
    }
}
```

4. 子集/组合（元素可重不可复选）
刚才讲的标准子集问题输入的 nums 是没有重复元素的，但如果存在重复元素，怎么处理呢？
(leetcode 90)
给你一个整数数组 nums，其中可能包含重复元素，请你返回该数组所有可能的子集。

比如输入 nums = [1,2,2]，你应该输出：
	[ [],[1],[2],[1,2],[2,2],[1,2,2] ]

当然，按道理说「集合」不应该包含重复元素的，但既然题目这样问了，我们就忽略这个细节吧，仔细思考一下这道题怎么做才是正事。

就以 nums = [1,2,2] 为例，为了区别两个 2 是不同元素，后面我们写作 nums = [1,2,2']。

按照之前的思路画出子集的树形结构，显然，两条值相同的相邻树枝会产生重复：

![](https://labuladong.online/algo/images/%E6%8E%92%E5%88%97%E7%BB%84%E5%90%88/8.jpeg)

	[ 
    [],
    [1],[2],[2'],
    [1,2],[1,2'],[2,2'],
    [1,2,2']
	]

你可以看到，[2] 和 [1,2] 这两个结果出现了重复，所以我们需要进行剪枝，如果一个节点有多条值相同的树枝相邻，则只遍历第一条，剩下的都剪掉，不要去遍历：

![](https://labuladong.online/algo/images/%E6%8E%92%E5%88%97%E7%BB%84%E5%90%88/9.jpeg)

体现在代码上，需要先进行排序，让相同的元素靠在一起，如果发现 nums[i] == nums[i-1]，则跳过：

```cpp
class Solution {
    vector<vector<int>> res; // 输出结果
    vector<int> track; // 搜索路径
public:
    vector<vector<int>> subsetsWithDup(vector<int>& nums) {
        sort(nums.begin(), nums.end()); // 排序，让相同的元素靠在一起
        backtrack(nums, 0);
        return res; // 返回结果
    }

    void backtrack(vector<int>& nums, int start) { // start 为当前的枚举位置
        res.emplace_back(track); // 前序位置，每个节点的值都是一个子集
        for(int i = start; i < nums.size(); i++) {
            if (i > start && nums[i] == nums[i - 1]) { // 剪枝逻辑，值相同的相邻树枝，只遍历第一条
                continue;
            }
            track.emplace_back(nums[i]); // 添加至路径
            backtrack(nums, i + 1); // 进入下一层决策树
            track.pop_back(); // 回溯
        }
    }
};
```
这段代码和之前标准的子集问题的代码几乎相同，就是添加了排序和剪枝的逻辑。

至于为什么要这样剪枝，结合前面的图应该也很容易理解，这样带重复元素的子集问题也解决了。

5. 排列（元素可重不可复选）

排列问题的输入如果存在重复，比子集/组合问题稍微复杂一点，我们看看力扣第 47 题

给你输入一个可包含重复数字的序列 nums，请你写一个算法，返回所有可能的全排列，函数签名如下：

比如输入 nums = [1,2,2]，函数返回：
	[ [1,2,2],[2,1,2],[2,2,1] ]

```cpp
class Solution {
public:
    // 保存结果
    vector<vector<int>> res;
    // 记录当前位置的元素
    vector<int> track;
    // 记录元素是否被使用
    vector<bool> used;

    // 主函数
    vector<vector<int>> permuteUnique(vector<int>& nums) {
        // 排序，让相同的元素靠在一起
        sort(nums.begin(), nums.end());
        // 初始化used数组
        used = vector<bool>(nums.size(), false);
        // 回溯
        backtrack(nums);
        // 返回结果
        return res;
    }

    // 回溯函数
    void backtrack(vector<int>& nums) {
        // 当长度相等时，将结果记录
        if (track.size() == nums.size()) {
            res.push_back(track);
            return;
        }

        // 遍历没有被使用过的元素
        for (int i = 0; i < nums.size(); i++) {
            if (used[i]) {
                continue;
            }
            // 新添加的剪枝逻辑，固定相同的元素在排列中的相对位置
            if (i > 0 && nums[i] == nums[i - 1] && !used[i - 1]) {
                continue;
            }
            // 添加元素，标记为使用过
            track.push_back(nums[i]);
            used[i] = true;
            // 继续回溯
            backtrack(nums);
            // 回溯
            track.pop_back();
            used[i] = false;
        }
    }
};
```

你对比一下之前的标准全排列解法代码，这段解法代码只有两处不同
1.对 nums 进行了排序。
2.添加了一句额外的剪枝逻辑。
类比输入包含重复元素的子集/组合问题，你大概应该理解这么做是为了防止出现重复结果。

但是注意排列问题的剪枝逻辑，和子集/组合问题的剪枝逻辑略有不同：新增了 !used[i - 1] 的逻辑判断。

6. 子集/组合（元素无重可复选）
输入数组无重复元素，但每个元素可以被无限次使用。
leetcode39
给你一个无重复元素的整数数组 candidates 和一个目标和 target，找出 candidates 中可以使数字和为目标数 target 的所有组合。candidates 中的每个数字可以无限制重复被选取。

比如输入 candidates = [1,2,3], target = 3，算法应该返回：
	[ [1,1,1],[1,2],[3] ]

这道题说是组合问题，实际上也是子集问题：`candidates` 的哪些子集的和为 `target`？

想解决这种类型的问题，也得回到回溯树上，我们不妨先思考思考，标准的子集/组合问题是如何保证不重复使用元素的？

答案在于 backtrack 递归时输入的参数 start：

```cpp
// 无重组合的回溯算法框架
void backtrack(vector<int>& nums, int start) {
    for (int i = start; i < nums.size(); i++) {
        // ...
        // 递归遍历下一层回溯树，注意参数
        backtrack(nums, i + 1);
        // ...
    }
}
```
这个 i 从 start 开始，那么下一层回溯树就是从 start + 1 开始，从而保证 nums[start] 这个元素不会被重复使用：

![](https://labuladong.online/algo/images/%E6%8E%92%E5%88%97%E7%BB%84%E5%90%88/1.jpeg)

那么反过来，如果我想让每个元素被重复使用，我只要把 i + 1 改成 i 即可：

```cpp
// 可重组合的回溯算法框架
void backtrack(vector<int>& nums, int start) {
    for (int i = start; i < nums.size(); i++) {
        // ...
        // 递归遍历下一层回溯树，注意参数
        backtrack(nums, i);
        // ...
    }
}
```
这相当于给之前的回溯树添加了一条树枝，在遍历这棵树的过程中，一个元素可以被无限次使用：

![](https://labuladong.online/algo/images/%E6%8E%92%E5%88%97%E7%BB%84%E5%90%88/10.jpeg)

当然，这样这棵回溯树会永远生长下去，所以我们的递归函数需要设置合适的 base case 以结束算法，即路径和大于 target 时就没必要再遍历下去了。

```cpp
class Solution {
public:
    vector<vector<int>> res;
    // 记录回溯的路径
    deque<int> track;
    // 记录 track 中的路径和
    int trackSum = 0;

    vector<vector<int>> combinationSum(vector<int>& candidates, int target) {
        backtrack(candidates, 0, target);
        return res;
    }

    // 回溯算法主函数
    void backtrack(vector<int>& nums, int start, int target) {
        // base case，找到目标和，记录结果
        if (trackSum == target) {
            res.push_back(vector<int>(track.begin(), track.end()));
            return;
        }
        // base case，超过目标和，停止向下遍历
        if (trackSum > target) {
            return;
        }
        // 回溯算法标准框架
        for (int i = start; i < nums.size(); i++) {
            // 选择 nums[i]
            trackSum += nums[i];
            track.push_back(nums[i]);
            // 递归遍历下一层回溯树
            // 同一元素可重复使用，注意参数
            backtrack(nums, i, target);
            // 撤销选择 nums[i]
            trackSum -= nums[i];
            track.pop_back();
        }
    }
};
```

7. 排列（元素无重可复选）

不妨先想一下，nums 数组中的元素无重复且可复选的情况下，会有哪些排列？

比如输入 nums = [1,2,3]，那么这种条件下的全排列共有 3^3 = 27 种：
	[
  [1,1,1],[1,1,2],[1,1,3],[1,2,1],[1,2,2],[1,2,3],[1,3,1],[1,3,2],[1,3,3],
  [2,1,1],[2,1,2],[2,1,3],[2,2,1],[2,2,2],[2,2,3],[2,3,1],[2,3,2],[2,3,3],
  [3,1,1],[3,1,2],[3,1,3],[3,2,1],[3,2,2],[3,2,3],[3,3,1],[3,3,2],[3,3,3]
	]

标准的全排列算法利用 used 数组进行剪枝，避免重复使用同一个元素。如果允许重复使用元素的话，直接放飞自我，去除所有 used 数组的剪枝逻辑就行了。

```cpp
class Solution {
public:
    vector<vector<int>> res;
    deque<int> track;

    vector<vector<int>> permuteUnique(vector<int>& nums) {
        sort(nums.begin(), nums.end());
        backtrack(nums);
        return res;
    }

    void backtrack(vector<int>& nums) {
        if (track.size() == nums.size()) {
            res.push_back(vector(track.begin(), track.end()));
            return;
        }

        for (int i = 0; i < nums.size(); i++) {
            //剪枝操作，判断当前节点是否已经在track中
            if (i > 0 && nums[i] == nums[i - 1] && find(track.begin(), track.end(), nums[i - 1]) != track.end()) {
                continue;
            }
            track.push_back(nums[i]);
            backtrack(nums);
            track.pop_back();
        }
    }
};
```

**总结**
来回顾一下排列/组合/子集问题的三种形式在代码上的区别。
由于子集问题和组合问题本质上是一样的，无非就是 base case 有一些区别，所以把这两个问题放在一起看。
形式一、元素无重不可复选，即 nums 中的元素都是唯一的，每个元素最多只能被使用一次，backtrack 核心代码如下：
```cpp
void backtrack(vector<int>& nums, int start) {
    // 回溯算法标准框架
    for (int i = start; i < nums.size(); i++) {
        // 做选择
        track.push_back(nums[i]);
        // 注意参数
        backtrack(nums, i + 1);
        // 撤销选择
        track.pop_back();
    }
}

void backtrack(vector<int>& nums) {
    for (int i = 0; i < nums.size(); i++) {
        // 剪枝逻辑
        if (used[i]) {
            continue;
        }
        // 做选择
        used[i] = true;
        track.push_back(nums[i]);
        backtrack(nums);
        // 撤销选择
        track.pop_back();
        used[i] = false;
    }
}
```

形式二、元素可重不可复选，即 nums 中的元素可以存在重复，每个元素最多只能被使用一次，其关键在于排序和剪枝，backtrack 核心代码如下：

```cpp
sort(nums.begin(), nums.end());

/* 组合/子集问题回溯算法框架 */
void backtrack(vector<int>& nums, int start) {
    //回溯算法标准框架
    for (int i = start; i < nums.size(); i++) {
        // 剪枝逻辑，跳过值相同的相邻树枝
        if (i > start && nums[i] == nums[i - 1]) {
            continue;
        }
        // 做选择
        track.push_back(nums[i]);
        // 注意参数
        backtrack(nums, i + 1);
        // 撤销选择
        track.pop_back();
    }
}

sort(nums.begin(), nums.end());

/* 排列问题回溯算法框架 */
void backtrack(vector<int>& nums, vector<bool>& used) {
    for (int i = 0; i < nums.size(); i++) {
        // 剪枝逻辑
        if (used[i]) {
            continue;
        }
        // 剪枝逻辑，固定相同的元素在排列中的相对位置
        if (i > 0 && nums[i] == nums[i - 1] && !used[i - 1]) {
            continue;
        }
        // 做选择
        used[i] = true;
        track.push_back(nums[i]);

        backtrack(nums, used);
        // 撤销选择
        track.pop_back();
        used[i] = false;
    }
}
```
形式三、元素无重可复选，即 nums 中的元素都是唯一的，每个元素可以被使用若干次，只要删掉去重逻辑即可，backtrack 核心代码如下：
```cpp
/* 组合/子集问题回溯算法框架 */
void backtrack(vector<int>& nums, int start, deque<int>& track) {
    // 回溯算法标准框架
    for (int i = start; i < nums.size(); i++) {
        // 做选择
        track.push_back(nums[i]);
        // 注意参数
        backtrack(nums, i, track);
        // 撤销选择
        track.pop_back();
    }
}


/* 排列问题回溯算法框架 */
void backtrack(vector<int>& nums, deque<int>& track) {
    for (int i = 0; i < nums.size(); i++) {
        // 做选择
        track.push_back(nums[i]);
        backtrack(nums, track);
        // 撤销选择
        track.pop_back();
    }
}
```

## 22 分治

分治（divide and conquer），全称分而治之，是一种非常重要且常见的算法策略。分治通常基于递归实现，包括“分”和“治”两个步骤。分治通常基于递归实现
1.分（划分阶段）：递归地将原问题分解为两个或多个子问题，直至到达最小子问题时终止。
2.治（合并阶段）：从已知解的最小子问题开始，从底至顶地将子问题的解进行合并，从而构建出原问题的解。














