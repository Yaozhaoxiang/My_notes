# lab1 mapreduce

运行命令：
1. go build -buildmode=plugin ../mrapps/wc.go

+ -buildmode=plugin：生成一个插件(动态库)；运行时通过plugin包加载并调用其导出函数和变量；

2. go run mrsequential.go wc.so pg*.txt

+ wc.so: 动态库文件
+ pg*.txt： 当前目录下所有文件名以pg开头并以.txt结尾的文件

## job

你的任务是实现一个分布式的 MapReduce 系统，包括两个程序：协调器（coordinator）和工作者（worker）。系统中会有一个协调器进程和一个或多个并行运行的工作者进程。在真实系统中，工作者会运行在多台不同的机器上，但在这个实验中，所有进程都会在一台机器上运行。工作者通过 RPC 与协调器通信。每个工作者进程会向协调器请求任务，从一个或多个文件中读取任务的输入，执行任务，然后将输出写入一个或多个文件。协调器需要检测某个工作者是否在合理时间内（本实验设置为 10 秒）未完成任务，并将该任务重新分配给另一个工作者。

我们为你提供了一些初始代码。协调器和工作者的主程序分别位于 main/mrcoordinator.go 和 main/mrworker.go 中；不要修改这些文件。你的实现应放在 mr/coordinator.go、mr/worker.go 和 mr/rpc.go 文件中。

以下是运行你的代码以完成单词计数（word-count）MapReduce 应用的步骤。首先，确保单词计数插件已编译：

```go
$ go build -buildmode=plugin ../mrapps/wc.go
```

在主目录下运行协调器：

```go
$ rm mr-out*
$ go run mrcoordinator.go pg-*.txt
```

传递给 mrcoordinator.go 的 pg-*.txt 参数是输入文件；每个文件对应一个“分片”（split），是一个 Map 任务的输入。

在一个或多个终端窗口中运行一些工作者：
```go
$ go run mrworker.go wc.so
```

当工作者和协调器完成工作后，可以查看 mr-out-* 中的输出。当你完成实验时，输出文件的排序联合应该与顺序输出匹配，例如：
```go
$ cat mr-out-* | sort | more
A 509
ABOUT 2
ACT 8
...

```

我们为你提供了一个测试脚本 main/test-mr.sh。该脚本会检查单词计数（wc）和索引器（indexer）MapReduce 应用在 pg-xxx.txt 文件输入下是否生成正确的输出。此外，它还会检查你的实现是否支持并行执行 Map 和 Reduce 任务，以及是否能从工作者崩溃中恢复。

如果你现在运行测试脚本，协调器会因为永远不会结束而挂起：
```bash
$ cd ~/6.5840/src/main
$ bash test-mr.sh
*** Starting wc test.

```
你可以将 mr/coordinator.go 中 Done 函数的 ret := false 改为 ret := true，这样协调器会立即退出。然后运行：

```bash
$ bash test-mr.sh
*** Starting wc test.
sort: No such file or directory
cmp: EOF on mr-wc-all
--- wc output is not the same as mr-correct-wc.txt
--- wc test: FAIL
$

```

测试脚本期望在名为 mr-out-X 的文件中看到输出，每个 Reduce 任务对应一个文件。而 mr/coordinator.go 和 mr/worker.go 的空实现既不会生成这些文件，也不会执行任何其他操作，因此测试会失败。

当你完成实验后，测试脚本的输出应如下所示：
```go
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
$

```

你可能会看到一些来自 Go RPC 包的错误消息，例如：
```c
2019/12/16 13:27:09 rpc.Register: method "Done" has 1 input parameters; needs exactly three

```
可以忽略这些消息；将协调器注册为 RPC 服务器时，会检查所有方法是否符合 RPC 要求（需要 3 个输入参数）；我们知道 Done 方法不会通过 RPC 调用。

1. Map 阶段

+ Map 阶段应将中间键（intermediate keys）划分为多个桶，每个桶对应一个 Reduce 任务，总共有 nReduce 个 Reduce 任务。nReduce 是 main/mrcoordinator.go 传递给 MakeCoordinator() 的参数。
+ 每个 Map 任务应为 Reduce 任务生成 nReduce 个中间文件，供后续 Reduce 阶段使用。

2. Reduce 输出

+ 工作者（worker）实现应该将第 X 个 Reduce 任务的输出放入文件 mr-out-X 中。
+ 每个 mr-out-X 文件应包含一行对应一个 Reduce 函数的输出。每行的格式应使用 Go 的 "%v %v" 格式化字符串生成，传入键（key）和值（value）。可以查看 main/mrsequential.go 中标注为“this is the correct format”（“这是正确格式”）的注释行。
+ 如果你的实现与此格式偏差过大，测试脚本会失败。

3. 可修改的文件

+ 你可以修改 mr/worker.go、mr/coordinator.go 和 mr/rpc.go 文件。
+ 为了测试，你可以临时修改其他文件，但要确保你的代码在恢复到原始版本时仍能正常工作；我们会使用原始版本进行测试。

4. 中间文件存储

+ 工作者应将 Map 阶段的中间输出存储在当前目录下，以便工作者可以在 Reduce 阶段将其作为输入读取。

5. 任务完成检测

+ main/mrcoordinator.go 期望 mr/coordinator.go 实现一个 Done() 方法。当 MapReduce 任务完全完成时，该方法应返回 true；此时，mrcoordinator.go 会退出。

6. 工作者退出机制

+ 当任务完全完成后，工作者进程也应退出。一种简单的实现方式是利用 call() 的返回值：如果工作者无法联系到协调器，可以假设协调器已经退出（因为任务完成），从而终止自身进程。
+ 根据你的设计，可能还会用到一种“请退出”（"please exit"）的伪任务，协调器可以将此任务分配给工作者以指示它们退出。

### hints

1. 开发和调试建议

+ 可以参考 Guidance 页面的开发和调试技巧。
+ 开始时，可以修改 mr/worker.go 中的 Worker() 函数，使其通过 RPC 请求协调器分配任务。然后修改协调器，让其返回尚未开始的 Map 任务的文件名。接着，修改工作者读取该文件并调用应用程序的 Map 函数（如 mrsequential.go 中所示）。

2. 加载 Map 和 Reduce 函数

+ 应用程序的 Map 和 Reduce 函数通过 Go 的 plugin 包动态加载，插件文件名以 .so 结尾。

3. 重新编译插件

+ 如果修改了 mr/ 目录下的内容，可能需要重新编译 MapReduce 插件，例如：

```bash
go build -buildmode=plugin ../mrapps/wc.go

```

4. 共享文件系统

+ 本实验依赖工作者共享文件系统。当所有工作者运行在同一台机器上时，这比较简单；若在不同机器上运行，则需要类似 GFS 的全局文件系统。

5. 中间文件命名约定

+ 合理的中间文件命名格式为 mr-X-Y，其中 X 是 Map 任务编号，Y 是 Reduce 任务编号。

6. 存储键值对

+ 工作者的 Map 任务代码需要一种方式将中间键值对存储到文件中，以便 Reduce 任务能够正确读取。可以使用 Go 的 encoding/json 包：

写入文件：
```go
enc := json.NewEncoder(file)
for _, kv := range ... {
    err := enc.Encode(&kv)
}
```

读取文件：
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

7. 选择 Reduce 任务

+ 工作者的 Map 部分可以使用 worker.go 中的 ihash(key) 函数为给定键选择 Reduce 任务。

8. 参考代码

+ 可以借用 mrsequential.go 中的代码来读取 Map 输入文件、对 Map 和 Reduce 之间的中间键值对排序，以及将 Reduce 输出存储到文件。

9. 并发处理

+ 协调器作为 RPC 服务器是并发的，注意对共享数据加锁。

10. 竞态检测

使用 Go 的竞态检测器：
```bash
go run -race

```

+ test-mr.sh 脚本开头提供了如何用 -race 运行的注释。尽管评分时不会使用竞态检测器，但代码存在竞态条件可能会导致测试失败。

11. 任务等待机制

+ Reduce 任务需要等待 Map 完成，工作者可以定期请求协调器分配任务，期间使用 time.Sleep() 暂停。
+ 或者，协调器中的相关 RPC 处理程序可以通过循环等待，使用 time.Sleep() 或 sync.Cond。Go 会为每个 RPC 调用生成独立线程，因此一个处理程序的等待不会阻止协调器处理其他 RPC。

12. 任务超时和重新分配

+ 协调器无法可靠地区分崩溃、卡顿或运行缓慢的工作者。最佳实践是等待一段时间后放弃该任务并重新分配。在本实验中，协调器应等待 10 秒后假设工作者已死亡并重新分配任务。

13. 备份任务

+ 如果实现备份任务（第 3.6 节），请注意，测试会检查你的代码是否在工作者正常执行任务时未调度多余任务。备份任务应仅在较长时间（例如 10 秒）后调度。

14. 崩溃恢复测试

+ 使用 mrapps/crash.go 插件，它会在 Map 和 Reduce 函数中随机退出。

15. 确保文件一致性

+ 为避免崩溃时生成部分写入的文件，可以使用临时文件并在完成后通过原子重命名替换原文件。使用 ioutil.TempFile 创建临时文件，用 os.Rename 进行原子重命名。

16. 测试脚本调试

+ test-mr.sh 在 mr-tmp 子目录中运行所有进程。如果出错，可以查看中间或输出文件。
+ 可以临时修改 test-mr.sh，让其在失败测试后退出，避免脚本继续运行覆盖输出文件。

17. 重复测试

+ 使用 test-mr-many.sh 脚本多次运行 test-mr.sh，以发现低概率的 bug。

18. RPC 注意事项

+ Go RPC 只发送首字母大写的结构字段，子结构的字段也必须大写。
+ 调用 RPC 的 call() 函数时，回复结构体应包含所有默认值。例如：
```go
reply := SomeType{}
call(..., &reply)

```
+ 如果在调用前设置了非默认字段，RPC 系统可能会返回错误的值。

