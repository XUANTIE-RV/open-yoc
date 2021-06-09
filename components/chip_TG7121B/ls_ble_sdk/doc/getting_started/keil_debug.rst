
Keil+JLink 构建、烧录、调试
============================

构建
-------------

完成keil环境安装后，以ble_dis工程为例，打开ls_ble_sdk\\dev\\examples\\ble_dis\\mdk路径下的工程文件

1. 选择 J-Link 作为调试工具

 .. image:: MDK_01.png

2. 选择SW作为调试

 .. image:: MDK_02.png

3. 在 flash download 选项卡中配置下载选项，不能选择 ``Erase Full Chip`` 选项，如果在编程算法选项内没有找到下图的选项，请检查软件开发环境搭建章节内的keil环境配置的第二条内容是否执行正确。

 .. image:: MDK_03.png

4. 设置使用 Debug Drive 进行 flash 的烧录 

 .. image:: MDK_04.png

5. 选择使用创建 Hex 文件 

 .. image:: OutputHex.png

JLINK烧录、调试的准备工作
-------------------------
#. 将JLink安装路径（JFlash.exe 、JLinkGDBServerCL.exe 所在目录） ``{JLINK_SETUP_DIR}`` 添加到系统环境变量PATH。

#. 将SDK ``tools/prog/`` 目录下所有内容拷贝到JLink安装路径 ``{JLINK_SETUP_DIR}`` 覆盖原有文件。

JFlash使用
-------------------------
#. 打开J-FLash,配置target device为``LinkedSemi LE501X``

 .. image:: jflash_start.png

2、点击File->Open data file...选择要烧录的hex文件,或者直接将文件拖入JFLash中；

3、选择烧录文件之后，点击Target->Connect,如果能够连接成功会在LOG窗口最后一行显示“Connected successfully”，否则请检查硬件接线是否正确；

4、点击 Target->Manual Programming->Erase 执行芯片全擦；

5、点击 Target->Production Programming 开始烧录选中的hex文件。

运行单独工程
----------------
#. **务必先完成JLink烧录、调试的准备工作** ；

#. 打开 ``ble_sdk_app\dev\examples\ble_uart_server\mdk`` 路径下的 ble_uart_server.uvprojx 工程文件,开始编译，编译成功会在 ``ble_sdk_app\dev\examples\ble_uart_server\mdk\UVBuild`` 路径下面生成 ble_uart_server.hex 和 info_sbl.hex 以及其他编译产生的文件；

#. 先使用JFlash工具将 info_sbl.hex 和 ``ble_sdk_app\dev\soc\arm_cm\le501x\bin\fw.hex`` 分别烧录到芯片内，烧录顺序没有限制，每次烧录这两个文件之前先执行芯片全擦动作；

#. 在通过Keil烧录之前,必须要先确保芯片内已经烧录了 fw.hex 和 info_sbl.hex文件,如果已经烧录了这两个文件并且没有执行过flash全部擦除的指令,则可以不用重复烧录 fw.hex 和 info_sbl.hex； 

#. 直接使用JFlash工具烧录 ble_uart_server.hex，或者使用keil图形界面的DownLoad选项,将编译后的文件下载到flash中

 .. image:: DownLoad.png

调试
-------------
#. 使用keil图形界面的 Start/Stop Debug Session选项进入调试模式

 .. image:: MDK_Debug.png