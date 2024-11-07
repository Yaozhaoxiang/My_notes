# wsl
1. 
`/mnt` 是一个特殊的目录，用于访问 Windows 文件系统。通过 /mnt 目录，你可以访问 Windows 中的各个驱动器和目录。

`/mnt/c：`代表 Windows 的 C 盘（通常是系统盘）。

2. 
可以在 PowerShell 中运行下面命令显示所有发行版的详细信息
`wsl --list --verbose`
命令 `wsl --set-version` 可更改指定分发版的版本，其用法为
例如：
`wsl --set-version Ubuntu-20.04 2`

3. 
可以使用`wsl -l -v`查看列表以及状态
使用`wsl --shutdown linuxname`，关闭系统

4. 关闭
`wsl --shutdown`
立即终止所有正在运行的发行版和 WSL 2 轻量级实用工具虚拟机
`wsl --terminate <Distribution Name>`
若要终止指定的发行版或阻止其运行，请将 <Distribution Name> 替换为目标发行版的名称。

5. 启动某一个
`wsl -d <Distribution Name>`

首先运行 `logout` 命令退出 WSL，接着通过以下命令关闭正在运行的第二个 Ubuntu 实例：
关闭
`wsl --terminate <Distribution Name>`
打开
`wsl -d <Distribution Name>`
5. 切换用户
su root 20010111
