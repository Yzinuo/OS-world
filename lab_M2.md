# Makefile
    $@ : 目标
    $^ : 所有依赖项
    $< : 第一个依赖项

    .PHONY : Clean :防止目录下正好有一个文件名叫clean。导致clean命令执行不了。

# 共享库

当linux代码中没有main函数，它不会编译成可运行的二进制文件，而是会生成共享库（动态链接库） libco.so

编译：
```
$(NAME)-64.so: $(DEPS) # 64bit shared library
    gcc -fPIC -shared -m64 $(CFLAGS) $(SRCS) -o $@ $(LDFLAGS)
```

其中 -fPIC -fshared 就代表编译成位置无关代码的共享库。除此之外，共享库和普通的二进制文件没有特别的区别

# 使用动态链接库

编译：
```
gcc -I.. -L.. -m64 main.c -o libco-test-64 -lco-64
gcc -I.. -L.. -m32 main.c -o libco-test-32 -lco-32
```

-   -I 选项代表 include path，使我们可以 #include <co.h>。你可以使用 gcc --verbose 编译看到编译器使用的 include paths。
-   -L 选项代表增加 link search path。
-   -l 选项代表链接某个库，链接时会自动加上 lib 的前缀，即 -lco-64 会依次在库函数的搜索路径中查找 libco-64.so 和 libco-64.a，直到找到为止。如果你将 libco-64.so 删除后用 strace 工具查看 gcc 运行时使用的系统调用，就能清晰地看到库函数解析的流程；
  
```
LD_LIBRARY_PATH=.. ./libco-test-64

```
如果不设置 LD_LIBRARY_PATH 环境变量，你将会遇到 “error while loading shared libraries: libco-xx.so: cannot open shared object file: No such file or directory” 的错误。

LD_LIBRARY_PATH 是一个环境变量，用于在程序运行时指定动态链接库（共享库）的搜索路径。当你运行一个程序，而它需要加载动态链接库时，系统会首先在 LD_LIBRARY_PATH 指定的目录中查找这些库。

如果不设置 LD_LIBRARY_PATH，系统将只在默认的库路径（如 /lib 和 /usr/lib）中搜索所需的共享库。如果你的库文件不在这些默认路径中，比如在一个本地目录或者其他非标准位置，你就需要设置 LD_LIBRARY_PATH 来包含这些目录，否则程序在尝试加载库时会报错，提示无法找到共享对象文件
