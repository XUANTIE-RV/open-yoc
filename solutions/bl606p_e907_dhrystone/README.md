# 概述
bl606p_e907_dhrystone是CPU测评的demo。

# 使用
## CDK
在CDK的首页，通过搜索bl606p_e907_dhrystone，可以找到bl606p_e907_dhrystone，然后创建工程。

CDK的使用可以参考YoCBook [《CDK开发快速上手》](https://www.xrvm.cn/document?temp=use-cdk-to-get-started-quickly&slug=yocbook) 章节

## 通过命令行
需要先安装[yoctools](https://www.xrvm.cn/document?temp=yoctools&slug=yocbook)。

### DEMO获取

```bash
mkdir workspace
cd workspace
yoc init
yoc install bl606p_e907_dhrystone
```

### 编译&烧录

注意：
    烧录时请注意当前目录下的`gdbinitflash`文件中的`target remote localhost:1025`内容需要改成用户实际连接时的 XuanTie Debug Server 中显示的对应的内容。

bl606p_e907_dhrystone只适用于bl606p_e907的CPU测评。

1. 编译

   ~~~bash
   make clean
   make
   ~~~

2. 烧写

   ~~~bash
   make flash
   ~~~

### 调试

```bash
riscv64-unknown-elf-gdb yoc.elf -x gdbinit
```

# 运行
烧录完成之后按下复位键，串口会有以下打印输出。

~~~uart
start dhrystone

Dhrystone Benchmark, Version 2.1 (Language: C)


Execution starts, 1000000 runs through Dhrystone

[10:32:20.554]收←◆Execution ends

Final values of the variables used in the benchmark:

Int_Glob:            5
        should be:   5
Bool_Glob:           1
        should be:   1
Ch_1_Glob:           A
        should be:   A
Ch_2_Glob:           B
        should be:   B
Arr_1_Glob[8]:       7
        should be:   7
Arr_2_Glob[8][7]:    1000010
        should be:   Number_Of_Runs + 10
Ptr_Glob->
  Ptr_Comp:          1040
        should be:   (implementation-dependent)
  Discr:             0
        should be:   0
  Enum_Comp:         2
        should be:   2
  Int_Comp:          17
        should be:   17
  Str_Comp:          DHRYSTONE PROGRAM, SOME STRING
        should be:   DHRYSTONE PROGRAM, SOME STRING
Next_Ptr_Glob->
  Ptr_Comp:          1040
        should be:   (implementation-dependent), same as above
  Discr:             0
        should be:   0
  Enum_Comp:         1
        should be:   1
  Int_Comp:          18
        should be:   18
  Str_Comp:          DHRYSTONE PROGRAM, SOME STRING
        should be:   DHRYSTONE PROGRAM, SOME STRING
Int_1_Loc:           5
        should be:   5
Int_2_Loc:           13
        should be:   13
Int_3_Loc:           7
        should be:   7
Enum_Loc:            1
        should be:   1
Str_1_Loc:           DHRYSTONE PROGRAM, 1'ST STRING
        should be:   DHRYSTONE PROGRAM, 1'ST STRING
Str_2_Loc:           DHRYSTONE PROGRAM, 2'ND STRING
        should be:   DHRYSTONE PROGRAM, 2'ND STRING

user time: 4400065
Microseconds for one run through Dhrystone:    4.4 
Dhrystones per Second:                      227269.4 
Score (DMIPS/MHz):                            0.40
~~~

