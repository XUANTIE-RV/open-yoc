.. _env_setup:

软件开发环境搭建
================= 

我们支持如下开发环境：

#. Python 3 + MDK-KEIL

#. Python 3 + VS Code + GCC(ARM)

下载地址： 

`GCC(ARM) 9.2.1 20191025 <https://developer.arm.com/-/media/Files/downloads/gnu-rm/9-2019q4/gcc-arm-none-eabi-9-2019-q4-major-win32.zip?revision=20c5df9c-9870-47e2-b994-2a652fb99075&la=en&hash=347C07EEEB848CC8944F943D8E1EAAB55A6CA0BC>`_ 

`VS Code (64bit) <https://go.microsoft.com/fwlink/?Linkid=852157>`_ 

`Python 3.8.2 (64bit) <https://www.python.org/ftp/python/3.8.2/python-3.8.2-amd64.exe>`_

`MDK_KEIL <http://www.keil.com/fid/qtcbv3wb9c9j1wrdw6w1a24gf9liqqd1ig1yd1/files/umdkarm/MDK525.EXE>`_

SDK 下载链接：

`gitee <https://gitee.com/linkedsemi/ls_ble_sdk>`_  

`github <https://github.com/linkedsemi/ls_ble_sdk>`_


一、Python 3 + MDK-KEIL
-------------------------

1. keil版本需要安装5.25以上,或者直接使用我们链接提供的版本， **不建议使用5.29以上版本**

2. 打开下载的 `ls_ble_sdk <https://gitee.com/linkedsemi/ls_ble_sdk>`_ 目录，将当前目录下的 ``tools\prog\LinkedSemi\flash_prog.elf`` 文件复制一份并修改文件名为flash_prog.FLM,并将flash_prog.FLM文件拷贝到keil安装目录下的 ``ARM\Flash`` 路径中 

3. 将fromelf执行文件的所在路径添加系统环境变量中，重启keil生效，该文件所在路径在keil的安装目录下面 ``Keil_v5\ARM\ARMCC\bin`` ，否则在使用keil编译时会报“fromelf不是内部或外部命令，也不是可运行的程序或批处理文件”的警告。（ `如何设置添加环境变量 <https://jingyan.baidu.com/article/47a29f24610740c0142399ea.html>`_ ） 

 .. image:: AddFromelf.png

4. 安装 Python3 及Python依赖库

安装 Python 相关详细说明
++++++++++++++++++++++++
1. 勾选 Add Python 3.8 to PATH，然后选择 Customize install 开始安装

 .. image:: python_01.png

2. 确认 pip 模块已经被勾选，其它配置默认就行，然后点击 Next 进行下一步

 .. image:: python_02.png

3. 点击 Install 等待安装完成

4. `以系统管理员身份打开命令行 <https://jingyan.baidu.com/article/f0e83a255d020522e4910155.html>`_ ，然后切换到SDK根下目录，例如SDK下载在D盘下 ``D:\ls_ble_sdk`` ,首先在CMD界面内输入 ``D:`` 切换到D盘，然后再使用 ``cd ls_ble_sdk`` 进入到SDK根目录里

 .. image:: cmd_01.png

5. 在第四步完成的基础上执行下述命令，安装Python依赖库::

    pip install -r requirements.txt

6. python依赖库安装完成之后，设置后缀名为.py的文件默认打开方式为Python，在命令行中执行::

    hexmerge.py -h

   若得到hexmerge帮助信息，则环境安装基本完成。

二、Python 3 + VS Code + GCC(ARM)
-----------------------------------

* 安装Python 3(勾选安装pip模块、添加Python到PATH)、VS Code

* 解压GCC，将 ``{GCC_SETUP_DIR}/bin`` 路径添加到系统环境变量PATH中

* 进入SDK根目录，以系统管理员身份打开命令行，执行下述命令，安装Python依赖库::

    pip install -r requirements.txt

* 设置后缀名为.py的文件默认打开方式为Python，设置完成后，在命令行中执行::

    hexmerge.py -h

  若得到hexmerge帮助信息，则设置成功

* 用VS Code打开SDK目录，点击左侧Extensions（快捷键 ``Ctrl + Shift + X`` ），在搜索框输入 ``@recommended`` ,安装所有Workspace Recommendations插件
