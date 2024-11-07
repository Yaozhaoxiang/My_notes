50. Pow(x, n)

实现 pow(x, n) ，即计算 x 的整数 n 次幂函数（即，xn ）。

快速幂算法将时间复杂度从 O(n) 降到 O(log n)，极大地减少了递归调用的次数，从而避免堆栈溢出问题。

快速幂算法利用了以下性质：

如果 n 是偶数，`x^n = (x^(n/2)) * (x^(n/2))`
如果 n 是奇数，`x^n = x * x^(n-1)`

```cpp
class Solution {
public:
    // 返回x^n,n为正数
    double mPow(double x, int n)
    {   
        if (n == 0) return 1;
        double half = mPow(x, n / 2);
        if (n % 2 == 0) {
            return half * half;
        } else {
            return x * half * half;
        }
    }
    double myPow(double x, int n) {
        long longN = n; // 将 n 转换为 long 类型
        if (longN > 0) {
            return mPow(x, longN);
        } else if (longN < 0) {
            return mPow(1 / x, -longN);
        } else {
            return 1;
        }
    }
};
```

**标答**
更加简练
```cpp
class Solution {
public:
    double quickMul(double x, long long N) {
        if (N == 0) {
            return 1.0;
        }
        double y = quickMul(x, N / 2);
        return N % 2 == 0 ? y * y : y * y * x;
    }

    double myPow(double x, int n) {
        long long N = n;
        return N >= 0 ? quickMul(x, N) : 1.0 / quickMul(x, -N);
    }
};

作者：力扣官方题解
链接：https://leetcode.cn/problems/powx-n/solutions/238559/powx-n-by-leetcode-solution/
来源：力扣（LeetCode）
著作权归作者所有。商业转载请联系作者获得授权，非商业转载请注明出处。
```










