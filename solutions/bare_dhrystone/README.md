# 概述

`bare_dhrystone` 是一个玄铁RTOS SDK中面向baremetal领域的dhrystone示例。dhrystone是一种基准测试程序，用于评估处理器的速度和效率。其主要用于测试整数运算速度和效率，不涵盖浮点运算、IO、网络等方面。

该示例需要在对应fpga平台上运行，基于QEMU运行无实际意义。

# 基于Linux编译运行

## 编译

```bash
./do_build.sh <cpu> <platform>
```
- cpu: <br />
        e902 e902m e902t e902mt e906 e906f e906fd e906p e906fp e906fdp e907 e907f e907fd e907p e907fp e907fdp <br />
        r910 r920 r908 r908fd r908fdv r908-cp r908fd-cp r908fdv-cp <br />
        c906 c906fd c906fdv c908 c908v c908i c910v2 c910v3 c910v3-cp c920v2 c920v3 c920v3-cp <br />
        c907 c907fd c907fdv c907fdvm c907-rv32 c907fd-rv32 c907fdv-rv32 c907fdvm-rv32
- platform: <br />
        smartl xiaohui

以玄铁`e907fdp`类型为例，可以使用如下命令编译:
```bash
./do_build.sh e907fdp smartl
```

## 运行

示例可以基于玄铁FPGA平台运行。

### 基于FPGA平台运行

如何基于FPGA平台运行请参考《玄铁RTOS SDK用户手册》

### 运行结果
正常运行串口输出内容参考如下
```
bare_dhrystore demo start!

Dhrystone Benchmark, Version 2.1 (Language: C)


Execution starts, 1000000 runs through Dhrystone
Execution ends

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
  Ptr_Comp:          1342240032                                                                   
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
  Ptr_Comp:          1342240032                                                                   
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
                                                                                                  
Microseconds for one run through Dhrystone:    2.8                                                
Dhrystones per Second:                      361865.8                                              
Score (DMIPS/MHz):                            4.12                                                
                                                                                                  
bare_dhrystone runs success!    
```

# 基于Windows IDE(CDS/CDK)编译运行

如何基于CDS/CDK等IDE编译运行具体请参考《玄铁RTOS SDK用户手册》

# 相关文档和工具

相关文档和工具下载请从玄铁官方站点 https://www.xrvm.cn 搜索下载

1、《玄铁RTOS SDK用户手册》

2、玄铁模拟器QEMU工具


# 注意事项

1、Linux平台下基础环境搭建请参考《玄铁RTOS SDK用户手册》

2、如果出现无法编译情况，请使用`sudo pip install yoctools -U`更新最新版本的yoctools再做尝试。

3、玄铁exx系列cpu仅支持smartl平台，cxx/rxx系列cpu仅支持xiaohui平台。

4、某些示例跟硬件特性相关，QEMU中相关功能可能未模拟，只能在相应FPGA硬件平台上运行。

