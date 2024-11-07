1. 检测两个整数在二进制表示中是否只有一位不同，可以使用按位运算

具体来说，如果两个数在二进制表示中只有一位不同，那么它们的异或运算结果将是一个二进制位图，且该二进制位图中只有一个位置为 1。
```cpp
bool hasOnlyOneBitDifferent(int x, int y) {
    // 计算异或结果
    int xorResult = x ^ y;
    
    // 检查异或结果是否是 2 的幂
    return (xorResult != 0) && ((xorResult & (xorResult - 1)) == 0);
}
```
如果 xorResult 是 2 的幂，那么它在二进制表示中只有一个 1。
(xorResult & (xorResult - 1)) == 0 是一个常见的技巧，用于检查一个数是否是 2 的幂。如果 xorResult 是 2 的幂，那么 xorResult - 1 将把唯一的 1 变成 0，并把所有低位变成 1，

2. 快速幂算法利用了以下性质：
   
如果 n 是偶数，`x^n = (x^(n/2)) * (x^(n/2))`
如果 n 是奇数，`x^n = x * x^(n-1)`

3. 去除重复元素
比如：nums=[2,2,2,3,4,5,6],求两个元素相加等于targert=6，的组合，但是不能有重复组合；
```cpp
int left=0,right=len-1;
while(left<right)
{
    if(left>j+1 && nums[left]==nums[left-1]) //先判断元素是否和前一个元素相等
    {
        left++;
        continue;
    }  
    if(right<len-1 && nums[right]==nums[right+1])
    {
        right--;
        continue;
    }
    if(nums[left]+nums[left+1]>target) //一些剪枝
        break;
    if(nums[right]+nums[right-1]<target)
        break;
    if(nums[left]+nums[right]==target)
    {
        res.push_back({nums[i],nums[j],nums[left],nums[right]});
        left++;
        right--;
    }
    else if(nums[left]+nums[right]>target)
        right--;
    else if(nums[left]+nums[right]<target)
        left++;
}
```




