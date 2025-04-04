Tmux 是一个终端复用器

# Tmux是什么
1. 会话与进程
命令行的典型使用方式是，打开一个终端窗口（terminal window，以下简称 "窗口"），在里面输入命令。用户与计算机的这种临时的交互，称为一次 "会话"（session） 。

会话的一个重要特点是，窗口与其中启动的进程是连在一起的。打开窗口，会话开始；关闭窗口，会话结束，会话内部的进程也会随之终止，不管有没有运行完。

一个典型的例子就是，SSH 登录远程计算机，打开一个远程窗口执行命令。这时，网络突然断线，再次登录的时候，是找不回上一次执行的命令的。因为上一次 SSH 会话已经终止了，里面的进程也随之消失了。

为了解决这个问题，会话与窗口可以 "解绑"：窗口关闭时，会话并不终止，而是继续运行，等到以后需要的时候，再让会话 "绑定" 其他窗口。
2. Tmux 的作用
Tmux 就是会话与窗口的 "解绑" 工具，将它们彻底分离。

（1）它允许在单个窗口中，同时访问多个会话。这对于同时运行多个命令行程序很有用。

（2）它可以让新窗口 "接入" 已经存在的会话。

（3）它允许每个会话有多个连接窗口，因此可以多人实时共享会话。

（4）它还支持窗口任意的垂直和水平拆分。

# 二、基本用法
1. 启动与退出
键入 `tmux` 命令，就进入了 `Tmux` 窗口

按下 `Ctrl+d` 或者显式输入 `exit` 命令，就可以退出 `Tmux` 窗口。

2. 前缀键

Tmux 窗口有大量的快捷键。所有快捷键都要通过前缀键唤起。默认的前缀键是 `Ctrl+b`，即先按下 `Ctrl+b`，快捷键才会生效。

举例来说，帮助命令的快捷键是` Ctrl+b ?`。它的用法是，在 Tmux 窗口中，先按下 `Ctrl+b`，再按下 `?`，就会显示帮助信息。

然后，按下 `ESC` 键或 `q` 键，就可以退出帮助。

# 三、会话管理
1. 新建会话
第一个启动的 Tmux 窗口，编号是 0，第二个窗口的编号是 1，以此类推。这些窗口对应的会话，就是 0 号会话、1 号会话。

使用编号区分会话，不太直观，更好的方法是为会话起名。
`tmux new -s <session-name>`

2. 分离会话
在 Tmux 窗口中，按下 `Ctrl+b d` 或者输入 `tmux detach` 命令，就会将当前会话与窗口分离。

上面命令执行后，就会退出当前 Tmux 窗口，但是会话和里面的进程仍然在后台运行。

`tmux ls` 命令可以查看当前所有的 Tmux 会话。

3. 接入会话
`tmux attach `命令用于重新接入某个已存在的会话。
```sh
# 使用会话编号
$ tmux attach -t 0

# 使用会话名称
$ tmux attach -t <session-name>
```
4. 杀死会话
`tmux kill-session`命令用于杀死某个会话。
```sh
# 使用会话编号
$ tmux kill-session -t 0

# 使用会话名称
$ tmux kill-session -t <session-name>
```
5. 切换会话
`tmux switch` 命令用于切换会话。

```sh
# 使用会话编号
$ tmux switch -t 0

# 使用会话名称
$ tmux switch -t <session-name>
```
6. 重命名会话
`tmux rename-session` 命令用于重命名会话。

```sh
$ tmux rename-session -t 0 <new-name>
```

7. 会话快捷键
`Ctrl+b d`：分离当前会话。
`Ctrl+b s`：列出所有会话。
`Ctrl+b $`：重命名当前会话。

# 四、窗格操作
Tmux 可以将窗口分成多个窗格（pane），每个窗格运行不同的命令。以下命令都是在 Tmux 窗口中执行。
1. 划分窗格

`tmux split-window` 命令用来划分窗格。
```sh
# 划分上下两个窗格
$ tmux split-window

# 划分左右两个窗格
$ tmux split-window -h
```
2. 移动光标
`tmux select-pane` 命令用来移动光标位置。

```sh
# 光标切换到上方窗格
$ tmux select-pane -U

# 光标切换到下方窗格
$ tmux select-pane -D

# 光标切换到左边窗格
$ tmux select-pane -L

# 光标切换到右边窗格
$ tmux select-pane -R
```
3. 交换窗格位置
`tmux swap-pane` 命令用来交换窗格位置。
```sh
# 当前窗格上移
$ tmux swap-pane -U

# 当前窗格下移
$ tmux swap-pane -D

# 当前窗格左移
$ tmux swap-pane -L

# 当前窗格左移
$ tmux swap-pane -R
```
4. 窗格快捷键

`Ctrl+b %`：划分左右两个窗格。
`Ctrl+b "`：划分上下两个窗格。
Ctrl+b <arrow key>：光标切换到其他窗格。是指向要切换到的窗格的方向键，比如切换到下方窗格，就按方向键↓。
`Ctrl+b ;`：光标切换到上一个窗格。
`Ctrl+b o`：光标切换到下一个窗格。
`Ctrl+b {`：当前窗格左移。
`Ctrl+b }`：当前窗格右移。
`Ctrl+b Ctrl+o`：当前窗格上移。
`Ctrl+b Alt+o`：当前窗格下移。
`Ctrl+b x`：关闭当前窗格。
`Ctrl+b !`：将当前窗格拆分为一个独立窗口。
`Ctrl+b z`：当前窗格全屏显示，再使用一次会变回原来大小。
`Ctrl+b Ctrl+<arrow key>`：按箭头方向调整窗格大小。
`Ctrl+b q`：显示窗格编号。

# 五、窗口管理
除了将一个窗口划分成多个窗格，Tmux 也允许新建多个窗口。

1. 新建窗口
`tmux new-window` 命令用来创建新窗口。
```sh
$ tmux new-window

# 新建一个指定名称的窗口
$ tmux new-window -n <window-name>
```
2. 切换窗口

`tmux select-window` 命令用来切换窗口。

```sh
# 切换到指定编号的窗口
$ tmux select-window -t <window-number>

# 切换到指定名称的窗口
$ tmux select-window -t <window-name>
```
3. 重命名窗口
`tmux rename-window` 命令用于为当前窗口起名（或重命名）。

`$ tmux rename-window <new-name>`

4. 快捷键
`Ctrl+b c`：创建一个新窗口，状态栏会显示多个窗口的信息。
`Ctrl+b p`：切换到上一个窗口（按照状态栏上的顺序）。
`Ctrl+b n`：切换到下一个窗口。
`Ctrl+b <number>`：切换到指定编号的窗口，其中的是状态栏上的窗口编号。
`Ctrl+b w`：从列表中选择窗口。
`Ctrl+b ,`：窗口重命名。



