# goenv

## 安装

1. 克隆 goenv 仓库
确保你已经将 goenv 仓库克隆到你的主目录下：

```bash
git clone https://github.com/syndbg/goenv.git ~/.goenv
```

2. 配置环境变量
将 goenv 添加到环境变量中，以便终端可以识别它。编辑你的 Shell 配置文件，例如：

如果使用 bash：

`nano ~/.bashrc`
如果使用 zsh：

`nano ~/.zshrc`

然后在文件末尾添加以下内容：

```bash
export GOENV_ROOT="$HOME/.goenv"
export PATH="$GOENV_ROOT/bin:$PATH"
eval "$(goenv init -)"
```

保存文件并重新加载配置：
```bash
source ~/.bashrc  # 或 source ~/.zshrc
```


4. 安装 Go 版本
使用 goenv 下载并安装 Go：

列出可用的 Go 版本：`goenv install -l`

安装一个具体的版本，例如 1.21.0：`goenv install 1.21.0`

设置全局的 Go 版本：`goenv global 1.21.0`

验证 Go 版本是否正确：`go version`


