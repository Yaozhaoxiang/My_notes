# 命令
### 1.	`cp -r source_dir/* destination_dir/`
这个命令会递归地复制 `source_dir` 文件夹中的所有文件和子目录到 `destination_dir` 中。
>`cp` 是复制命令。
`-r` 表示递归复制，包括子目录。
`source_dir/*` 表示要复制 `source_dir` 文件夹中的所有文件和子目录。
`destination_dir/` 是目标文件夹。

### 2.	添加文件到暂存区：
>`git add  .`
`git add file1.txt file2.txt`

### 3.提交到本地仓库：
>`git commit -m "Commit message"`

### 4.推送到远程仓库的指定分支

  1. 确保本地分支存在使用 `git branch` 命令检查本地存在哪些分支
  2. 使用 `git checkout -b branch_name` 
      创建并切换到新分支
      `git checkout -b main`  这将创建一个名为 `main` 的新分支并切换到该分  支。
  3. `git push origin <branch-name>`
   将 `<branch-name>` 替换为你希望推送到的远程分支名称。

### 5.  拉取远程分支

如果需要在本地创建一个远程已存在的分支的副本，可以使用 `git fetch` 命令获取远程分支的信息，然后使用 `git checkout` 命令创建并切换到本地分支。

`git fetch origin traps`
`git checkout -b traps origin/traps`

这将从远程仓库的 traps 分支创建一个本地副本，并切换到该分支。

### 6. git remote
  1. 查看远程仓库
  `git remote -v`
  这个命令会显示远程仓库的名称及其对应的 URL 地址。
  
  2. 添加远程仓库
  `git remote add <名称> <URL>`
  
  比如：
  `git remote add origin https://github.com/user/repo.git`
  这将添加一个名为 origin 的远程仓库，链接到指定的 GitHub 仓库地址。
  
  3. 删除远程仓库
  `git remote remove <名称>`
  
  示例：
  `git remote remove origin`
  这个命令将删除名为 origin 的远程仓库。
  
  4. 重命名远程仓库
  `git remote rename <旧名称> <新名称>`

### 8.git branch
   1. 查看本地分支
      `git branch`:不带任何参数会列出当前仓库中的所有本地分支，并用 * 标记当前检出的分支。
   2. 创建新分支
      `git branch feature-xyz`创建一个新的分支，但不会切换到新分支
   3. 删除分支
      `git branch -d <branch-name>`
   4. 强制删除分支
      `git branch -D <branch-name>`如果分支没有被完全合并，使用 -d 选项会失败。-D 是 -d --force 的简写，会强制删除该分支
   5. 重命名分支
     `git branch -m <new-branch-name>`重命名当前分支
     `git branch -m old-name new-name`如果需要重命名其他分支，可以指定当前名称和新名称：

### 9. 拉取代码
1. git pull

git pull 命令会从远程仓库拉取最新的更改，并将其合并到你的当前分支中。常用语法如下：
`git pull <remote_name> <branch_name>`
  <remote_name>: 远程仓库的名称，通常是 origin，也可能是其他自定义的名称。
  <branch_name>: 分支名称，通常是 main 或 master，但也可能是其他分支

2. git fetch

git fetch 只会从远程仓库获取最新的更新，并将其存储在本地，但不会自动合并到你的当前分支。你可以在拉取后手动合并。
`git fetch <remote_name> <branch_name>`
之后你可以选择手动合并代码：
`git merge origin/main`

git merge 通常不会直接覆盖本地内容，而是将远程分支的更改与当前分支的更改合并在一起。如果两个分支中存在相同文件的不同修改，Git 会尝试自动合并，但如果发生冲突（即无法自动合并），Git 会提示你手动解决冲突。

1. 无冲突：如果远程分支的更改与本地分支的更改没有冲突，Git 会自动将它们合并。这不会覆盖本地的内容，而是将远程的更改合并进来。

2. 有冲突：如果本地内容与远程分支内容冲突，Git 会标记冲突的文件，你需要手动解决冲突，然后提交解决后的内容。冲突文件不会被自动覆盖。

### git checkout
git checkout minnow/check2-startercode

git checkout 的主要目的就是将本地工作目录的内容切换为指定分支、提交或路径的内容，使得本地内容与目标一致。

分支切换：如果 minnow/check2-startercode 是一个远程分支（例如 minnow 是远程仓库的名称），git checkout minnow/check2-startercode 将会尝试在本地切换到该分支。
  本地文件变更：在切换到这个分支时，Git 会根据该分支的内容更新你的工作目录。如果远程分支中的文件内容与当前分支不同，你的本地文件将会改变，以匹配远程分支的内容。

  未提交的更改：如果你在切换分支之前有未提交的更改，Git 可能会提示你是否需要暂存这些更改（使用 git stash），否则，无法切换到该分支。

### 10 与远程分支关联
fatal: No remote for the current branch 表示当前分支没有与任何远程分支关联，导致 Git 无法进行 merge 操作。

1. 检查当前分支的状态： 首先，使用以下命令查看当前分支状态，确认当前分支以及它是否跟踪了远程分支。
`git branch -vv`

这将列出所有分支，并显示每个分支的跟踪信息。如果当前分支没有跟踪任何远程分支，后面会显示 [gone] 或者不会有远程分支信息。

2. 设置当前分支的远程跟踪分支： 如果你知道要关联的远程分支，可以手动设置它。例如，假设你的远程仓库名为 origin，并且要关联 main 分支：

`git branch --set-upstream-to=origin/main`
这样，当前分支将与远程的 main 分支关联，你就可以进行 git merge 等操作了。

3. 检查远程仓库配置： 使用以下命令来查看你的远程仓库配置：
`git remote -v`
确保远程仓库已经设置好，如果没有设置，或者远程仓库的配置有问题，可以使用以下命令添加或修复远程仓库：
`git remote add origin <remote_repository_url>`

### 11 git merge <branch>
这个命令会将 <branch> 分支的更改合并到你当前所在的分支。

1. 普通合并：  
  如果两个分支之间的更改没有冲突，Git 会自动合并，并在当前分支中生成一个新的合并提交。 
  合并后，当前分支会包含 <branch> 中的所有更改。
2. 冲突处理：
  如果两个分支之间的更改有冲突（例如同一文件的同一部分被不同分支修改），Git 会提示你解决冲突。
  Git 会将冲突标记在冲突文件中，你需要手动编辑文件，解决冲突，然后使用 git add 将解决后的文件标记为已解决。
  完成冲突解决后，使用 git commit 来完成合并


### 上传另外一个仓库
1. 查看当前远程仓库
`git remote -v`

2. 添加新的远程仓库
你可以添加一个新的远程仓库，比如名为 new-origin，对应新的仓库地址：
`git remote add new-origin https://github.com/your-username/new-repo.git`

3. 查看 minnow 远程分支
`git branch -r`
这会显示所有远程分支的信息。

4. 获取 minnow 的分支
首先，你需要确保你的本地 Git 仓库已更新远程信息：
`git fetch minnow`
这将从远程 minnow 仓库获取最新的分支信息。

5.  切换到要拉取的分支
你可以创建一个新的本地分支，并将其与远程 minnow 分支关联。例如，如果你想拉取 minnow 仓库中的 lab3 分支：
`git checkout -b lab3 minnow/lab3`
这里的命令做了以下几件事：
  git checkout -b lab3：创建一个名为 lab3 的新本地分支并切换到该分支。
  minnow/lab3：将新本地分支设置为跟踪远程 minnow 仓库中的 lab3 分支。

git checkout -b project0  //创建新的分支：你可以使用 git checkout -b project0 命令在当前提交上创建一个新的分支。
git push origin project0 //推送到远程：创建分支后，你需要将该分支推送到 origin。使用以下命令

6. 拉取最新的更改
如果你已经在本地 lab3 分支上工作，你可以拉取最新的更改：
`git pull minnow lab3`
这将从 minnow 仓库的 lab3 分支拉取最新的更改，并合并到你的本地 lab3 分支。

## 12 出现 HEAD

git branch 出现;
(HEAD detached from minnow/check4-startercode)

表示你目前处于“分离的 HEAD 状态”。这意味着你的 Git 工作目录的 HEAD 指针并没有指向某个分支，而是指向了一个特定的提交或标签。

这通常在以下情况下发生：
  1. 你检查出（checkout）了某个具体的提交，而不是一个分支。例如，如果你运行了类似 git checkout <commit-hash> 的命令，你就会进入“分离的 HEAD 状态”。

  2. 你检查出了某个远程分支，但没有创建本地分支。例如，如果你运行了 git checkout minnow/check4-startercode，而没有指定一个新分支，Git 就会让你处于分离的 HEAD 状态。

在分离的 HEAD 状态下：
  + 你可以自由地进行更改并提交，但这些提交不会自动应用到任何分支上。如果你在此状态下创建的提交，需要手动将它们附加到某个分支上，否则它们可能会丢失。
  + 你可以创建一个新的分支，将当前的工作保存到新的分支上。

如何处理：
1. 将当前状态保存到一个新的分支：
`git checkout -b my-new-branch`
这将创建一个新的分支，并将你当前的 HEAD 指向这个新的分支。

2. 回到之前的分支： 如果你只是偶然进入了这个状态并且不需要在这里进行任何操作，你可以回到你之前的分支：

`git checkout <your-previous-branch>`

## 13 错误

```bash
Warning: you are leaving 1 commit behind, not connected to
any of your branches:

  d377a15 change webget

If you want to keep it by creating a new branch, this may be a good time
to do so with:

 git branch <new-branch-name> d377a15

branch 'lab4' set up to track 'origin/lab4'.
Switched to a new branch 'lab4'
```
这个警告是 Git 在提醒你，在你执行 git checkout minnow/check4-startercode 的过程中，你将会离开一个未关联到任何分支的提交（commit 4fab5c1），也就是说这个提交不会被任何分支所引用，如果没有进一步操作，它可能在 Git 的垃圾回收机制中被丢弃。

1. 创建新分支保存该提交: 如果你想保留这个提交，可以按照提示的命令创建一个新的分支来引用它：

`git branch my-lab3-branch 4fab5c1`

这会创建一个名为 my-lab3-branch 的分支，并指向提交 4fab5c1，这样即使你切换到了其他分支，4fab5c1 也不会丢失。

2. 将 4fab5c1 这个提交合并到已经存在的 lab3 分支中
1) 使用 git cherry-pick
   1. 切换到 lab3 分支:
   首先，确保你在 lab3 分支上：
    `git checkout lab3`
   2. 应用 4fab5c1 提交:
    使用 git cherry-pick 将提交 4fab5c1 应用到当前分支：
    `git cherry-pick 4fab5c1`
    这会将 4fab5c1 提交的更改应用到 lab3 分支上。
    3. 处理冲突 (如果有):
    如果在应用过程中发生冲突，Git 会提示你处理冲突。解决冲突后，执行以下命令来完成合并：
    ```bash
    git add .
    git cherry-pick --continue
    ```

2)  使用 git merge (如果 4fab5c1 在一个孤立的地方)
  1. 创建新分支引用提交:
    如果 4fab5c1 提交没有与其他分支关联，你可以临时创建一个分支来引用它：
    `git branch temp-branch 4fab5c1`
  2. 切换到 lab3 分支:
    `git checkout lab3`
  3. 合并 temp-branch:
    使用 git merge 将 4fab5c1 合并到 lab3 分支：
    `git merge temp-branch`
  4. 删除临时分支:
    合并完成后，你可以删除临时分支：
    `git branch -d temp-branch`

