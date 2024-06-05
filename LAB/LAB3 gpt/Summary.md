# perf
我们进行gpt的并行实验时，我们首先要找到整个程序最耗时的部分，然后进行并行。

我们可以使用perf工具：

**在软件工程中，分析（“程序分析”、“软件分析”）是动态程序分析的一种形式，用于测量程序的空间（内存）或时间复杂度、特定指令的使用情况或频率等。和函数调用的持续时间。最常见的是，分析信息有助于程序优化。**

如何使用：
```
sudo perf record -g ./program_name  arg

sudo perf report
```
