# lab1

您的任务是实现一个分布式 MapReduce，它由两个程序组成：协调者（coordinator）和工作者（worker）。将仅有一个协调者进程，以及一个或多个并行执行的工作者进程。在真实的系统中，工作者会运行在不同的机器上，但在本次实验中，您将在同一台机器上运行所有这些进程。工作者将通过远程过程调用（RPC）与协调者通信。每个工作者进程会向协调者请求任务，从一个或多个文件读取任务输入，执行任务，并将任务输出写入一个或多个文件。如果协调者发现某个工作者未能在合理的时间内完成其任务（对于这次实验，使用十秒钟作为时限），则应将相同的任务分配给另一个工作者。

我们为您提供了一些初始代码以帮助您开始。协调者和工作者的“main”主程序分别位于 `main/mrcoordinator.go` 和 `main/mrworker.go` 文件中；请不要修改这些文件。您应该将自己的实现代码放在 `mr/coordinator.go`、`mr/worker.go` 和 `mr/rpc.go` 文件中。

以下是针对单词计数 MapReduce 应用程序如何运行您的代码的方法。首先，请确保单词计数插件是最新的：

```bash
$ go build -buildmode=plugin ../mrapps/wc.go
```
在主目录中，运行协调者。
```bash
$ rm mr-out*
$ go run mrcoordinator.go pg-*.txt
```
`pg-*.txt` 参数是传递给 `mrcoordinator.go` 的输入文件；每个文件对应一个“分割”，并作为 Map 任务的输入。

在一个或多个其他窗口中，运行一些工作者：
```bash
$ go run mrworker.go wc.so
```
当工作者和协调者完成工作后，查看 `mr-out-*` 中的输出。当你完成了实验，排序合并后的输出文件应该与顺序执行的输出相匹配，例如：
```bash
$ cat mr-out-* | sort | more
A 509
ABOUT 2
ACT 8
...
```
我们为您提供了一个测试脚本 `main/test-mr.sh`。测试检查了 `wc` 和 `indexer` MapReduce 应用程序在以 `pg-xxx.txt` 文件作为输入时是否能产生正确的输出。测试还验证了您的实现是否能够并行运行 Map 和 Reduce 任务，以及是否可以从正在执行任务时崩溃的工作者中恢复。

如果您现在运行测试脚本，它将挂起，因为协调者永远不会结束：
```bash
$ cd ~/6.5840/src/main
$ bash test-mr.sh
*** Starting wc test.
```
您可以将 `Done` 函数中的 `ret := false` 改为 `true`（位于 `mr/coordinator.go`），以便协调者立即退出。然后：
```bash
$ bash test-mr.sh
*** Starting wc test.
sort: No such file or directory
cmp: EOF on mr-wc-all
--- wc output is not the same as mr-correct-wc.txt
--- wc test: FAIL
```
测试脚本期望在名为 `mr-out-X` 的文件中看到输出，每个 Reduce 任务对应一个文件。空的 `mr/coordinator.go` 和 `mr/worker.go` 实现不会生成这些文件（或者做任何其他事情），因此测试会失败。

完成所有工作后，测试脚本的输出应该如下所示：
```bash
$ bash test-mr.sh
*** Starting wc test.
--- wc test: PASS
*** Starting indexer test.
--- indexer test: PASS
*** Starting map parallelism test.
--- map parallelism test: PASS
*** Starting reduce parallelism test.
--- reduce parallelism test: PASS
*** Starting job count test.
--- job count test: PASS
*** Starting early exit test.
--- early exit test: PASS
*** Starting crash test.
--- crash test: PASS
*** PASSED ALL TESTS
```

您可能会看到来自 Go RPC 包的一些错误信息，类似于：
```
2019/12/16 13:27:09 rpc.Register: method "Done" has 1 input parameters; needs exactly three
```
忽略这些消息；将协调者注册为 RPC 服务器时会检查其所有方法是否适合用于 RPC（有三个输入参数）；我们知道 `Done` 不是通过 RPC 调用的。

几个规则：
- Map 阶段应该将中间键分为 nReduce 个桶，nReduce 是 Reduce 任务的数量——这是 `main/mrcoordinator.go` 传递给 `MakeCoordinator()` 的参数。每个映射器应该创建 nReduce 个中间文件供 Reduce 任务使用。
- 工作者实现应将第 X 个 Reduce 任务的输出放在文件 `mr-out-X` 中。
- 每个 `mr-out-X` 文件应该包含一行 Reduce 函数的输出。行应该使用 Go 的 "%v %v" 格式化字符串生成，并调用键和值。请参阅 `main/mrsequential.go` 中注释为“这是正确格式”的那一行。如果您的实现偏离此格式太多，测试脚本将会失败。
- 您可以修改 `mr/worker.go`、`mr/coordinator.go` 和 `mr/rpc.go`。您可以临时修改其他文件进行测试，但请确保您的代码可以在原始版本上正常工作；我们将使用原始版本进行测试。
- 工作者应该将中间 Map 输出保存在当前目录的文件中，稍后工作者可以读取这些文件作为 Reduce 任务的输入。
- `main/mrcoordinator.go` 期望 `mr/coordinator.go` 实现一个 `Done()` 方法，在 MapReduce 作业完全完成后返回 true；此时，`mrcoordinator.go` 将退出。
- 当作业完全完成后，工作者进程应该退出。一种简单的实现方式是使用 `call()` 的返回值：如果工作者无法联系到协调者，则可以假设协调者已经退出，因为作业已完成，所以工作者也可以终止。根据您的设计，您可能还需要一个“请退出”的伪任务，由协调者分配给工作者。
- 提示：指导页面有一些开发和调试的小贴士。
- 开始的一种方法是修改 `mr/worker.go` 的 `Worker()` 来向协调者发送一个请求任务的 RPC。然后修改协调者以响应未启动的 Map 任务的文件名。接着修改工作者来读取该文件并调用应用程序的 Map 函数，如 `mrsequential.go` 所示。
- 应用程序的 Map 和 Reduce 函数是在运行时使用 Go 的插件包从以 `.so` 结尾的文件加载的。
- 如果您更改了 `mr/` 目录下的任何内容，您可能需要重新构建任何使用的 MapReduce 插件，比如使用 `go build -buildmode=plugin ../mrapps/wc.go`。
- 本实验依赖于工作者共享一个文件系统。当所有工作者在同一台机器上运行时，这很简单，但如果工作者在不同的机器上运行，则需要像 GFS 这样的全局文件系统。
- 中间文件的一个合理的命名约定是 `mr-X-Y`，其中 X 是 Map 任务编号，Y 是 Reduce 任务编号。
- 工作者的 Map 任务代码需要一种方式将中间键值对存储在文件中，以便在 Reduce 任务期间正确读取。一种可能性是使用 Go 的 `encoding/json` 包。要将键值对以 JSON 格式写入打开的文件：
  ```go
  enc := json.NewEncoder(file)
  for _, kv := ... {
    err := enc.Encode(&kv)
  ```
  要读取这样的文件：
  ```go
  dec := json.NewDecoder(file)
  for {
    var kv KeyValue
    if err := dec.Decode(&kv); err != nil {
      break
    }
    kva = append(kva, kv)
  }
  ```
- 您可以借用 `mrsequential.go` 中的一些代码来读取 Map 输入文件，在 Map 和 Reduce 之间对中间键值对进行排序，并将 Reduce 输出存储在文件中。
- 作为 RPC 服务器的协调者将是并发的；不要忘记锁定共享数据。
- 使用 Go 的竞态检测器，带有 `go run -race`。`test-mr.sh` 在开头有一条评论告诉您如何使用 `-race` 运行它。当我们评估您的实验时，我们不会使用竞态检测器。然而，如果您的代码存在竞态，即使没有竞态检测器，它也可能在我们测试时失败。
- 工作者有时需要等待，例如 Reduce 不能开始直到最后一个 Map 完成。一种可能性是工作者定期向协调者询问工作，每次请求之间使用 `time.Sleep()` 睡眠。另一种可能性是在协调者的相关 RPC 处理程序中有一个等待循环，既可以使用 `time.Sleep()` 也可以使用 `sync.Cond`。Go 在自己的线程中运行每个 RPC 的处理程序，因此一个处理程序等待不会阻止协调者处理其他 RPC。
- 协调者无法可靠地区分崩溃的工作者、因某种原因停滞但仍存活的工作者，以及执行速度过慢而无用的工作者。最好的办法是让协调者等待一段时间，然后放弃并重新分配任务给不同的工作者。对于这个实验，让协调者等待十秒钟；之后协调者应该假设工作者已经死亡（当然，它可能并没有）。
- 如果您选择实现备份任务（Section 3.6），请注意我们测试的是您的代码在工作者不崩溃的情况下执行任务时不会调度多余的任务。备份任务只应在相对较长的时间（例如，10秒）后安排。
- 为了测试崩溃恢复，您可以使用 `mrapps/crash.go` 应用程序插件。它会在 Map 和 Reduce 函数中随机退出。
- 为了确保在崩溃存在的情况下没有人观察到部分写入的文件，MapReduce 论文提到了使用临时文件并在完全写入后原子重命名的技巧。您可以使用 `ioutil.TempFile` 创建临时文件，并使用 `os.Rename` 原子重命名它。
- `test-mr.sh` 在子目录 `mr-tmp` 中运行所有进程，因此如果出现问题并且您想查看中间或输出文件，请在那里查找。您可以临时修改 `test-mr.sh` 以在失败的测试后退出，这样脚本不会继续测试（并覆盖输出文件）。
- `test-mr-many.sh` 会连续多次运行 `test-mr.sh`，您可能想要这样做以发现低概率的 bug。它接受一个参数，指定运行测试的次数。您不应并行运行多个 `test-mr.sh` 实例，因为协调者会重用相同的套接字，导致冲突。
- Go RPC 只发送结构体中名称以大写字母开头的字段。子结构也必须有大写的字段名。
- 调用 RPC `call()` 函数时，回复结构应包含所有默认值。RPC 调用应该像这样：
  ```go
  reply := SomeType{}
  call(..., &reply)
  ```
  在调用之前不要设置 `reply` 的任何字段。如果您传递了非默认值的回复结构，RPC 系统可能会静默地返回不正确的值。









