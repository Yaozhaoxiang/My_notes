>给你一个字符串数组，请你将 字母异位词 组合在一起。可以按任意顺序返回结果列表。
字母异位词 是由重新排列源单词的所有字母得到的一个新单词。

题目很简单：

**我的写法：**
```cpp
class Solution {
public:
    vector<vector<string>> groupAnagrams(vector<string>& strs) {
        vector<vector<string>> res {};
        unordered_map<string,int> map {};

        int n = strs.size();
        for(int i=0;i<n;++i){
            string temp = strs[i];
            sort(temp.begin(),temp.end());
            auto it = map.find(temp);
            if(it != map.end()){
                res[it->second].emplace_back(strs[i]);
            }else{
                vector<string> a;
                a.emplace_back(strs[i]);
                res.emplace_back(move(a));
                map[temp] = res.size()-1;
            }
        }
        return res;
    }
};
```

**优化**
1. **减少查找和插入次数**: 使用 unordered_map<string, vector<string>> 来直接存储异位词组，这样就不需要额外存储索引，并且查找和插入更高效。

2. **减少拷贝操作**: 在 for 循环中直接使用 emplace_back 插入元素，减少不必要的临时变量。

3. **预留空间**: 在返回结果前调用 reserve() 为 res 预留足够的空间，减少动态分配内存的次数。

```cpp
vector<vector<string>> groupAnagrams(vector<string>& strs) {
    unordered_map<string, vector<string>> map;  // 用 unordered_map 存储排序后的字符串及其对应的异位词组

    for (const auto& str : strs) {
        string sorted_str = str;  // 复制字符串
        sort(sorted_str.begin(), sorted_str.end());  // 对字符串排序
        map[sorted_str].emplace_back(str);  // 将原字符串放入对应的异位词组
    }

    vector<vector<string>> res;
    res.reserve(map.size());  // 预留空间，避免多次内存分配
    for (auto& [key, group] : map) {
        res.emplace_back(move(group));  // 将异位词组添加到结果中
    }

    return res;
}
```

**解法二**
```cpp
auto arrayHash = [](const array<int, 26>& arr) -> size_t {
    size_t hash = 0;
    for (int num : arr) {
        hash = hash * 31 + num;  // 31 是一个常用的哈希常量
    }
    return hash;
};

unordered_map<array<int, 26>, vector<string>, decltype(arrayHash)> mp(0, arrayHash);

for (const string& str : strs) {
    array<int, 26> counts = {};
    for (char c : str) {
        counts[c - 'a']++;
    }
    mp[counts].emplace_back(str);
}

vector<vector<string>> ans;
for (const auto& [key, group] : mp) {
    ans.emplace_back(group);
}

return ans;
```

