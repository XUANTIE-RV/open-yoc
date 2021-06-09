## SHA

### 概要

该文档主要描述SHA模块的CSI2.0自动化测试AT命令。 文档中所涉及的这些命令是用来测试SHA模块的接口工作是否正常、接口返回值是否符合预期。



### 测试原理

测试时，应给被测试板发送SHA测试命令进行相应的测试；测试程序会按照按照测试命令的配置进行相应的计算解密测试，若计算解密的结果与预期一致，则相应的测试成功完成，否则测试失败。


### 测试流程

- 测试前应当准备好测试板（以下简称A板）
- 发送SHA测试命令进行测试 （测试命令功能请参照下文）

### RSA测试命令列表

| AT命令                      | 所需测试参数                                                     | 功能            |
| --------------------------- | ---------------------------------------------------------------- | --------------- |
| AT+SHA_SET_SHA_MODE         | key_type,key_data,length_of_key | 设置key数据 |
| AT+SHA_SYNC                 | idx,update_or_finish,source_data,data_len                        | 同步模式下的计算、解密|
| AT+SHA_ASYNC                | idx,update_or_finish,source_data,data_len                        | 异步模式下的计算、解密|
| AT+SHA_SYNC_PERFORMANCE     | sha_idx,sha_mode,rand_data,update_times,finish_times,test_loops,data_length           | 同步模式下的性能测试|
| AT+SHA_ASYNC_PERFORMANCE    | sha_idx,sha_mode,rand_data,update_times,finish_times,test_loops,data_length           | 异步模式下的性能测试|
| AT+SHA_SYNC_STABILITY       | sha_idx,sha_mode,rand_data,update_times,finish_times,test_loops,data_length           | 同步模式下的可靠性测试|
| AT+SHA_ASYNC_STABILITY      | sha_idx,sha_mode,rand_data,update_times,finish_times,test_loops,data_length           | 异步模式下的可靠性测试|


### AT+SHA_SET_SHA_MODE

| 命令格式 | AT+SHA_SET_SHA_MODE=tst_sha_mode |
|------------------------------------------|--------------------------------------------------------------------------------------------------|
| 命令功能 | 设置SHA模式<br>|
| 命令参数 | tst_sha_mode：1：sha1,2：sha256,3:sha224,4:sha512,5:sha384,6:sha512_256,7:sha_512_224 |
| 示例 | 发送命令AT+SHA_SET_SHA_MODE=0到A板,  测试完成后， A板对应串口会打印OK；否则测试失败 |

### AT+SHA_SYNC

| 命令格式 | AT+SHA_SYNC=idx,update_or_finish,source_data,data_len |
|------------------------------------------|--------------------------------------------------------------------------------------------------|
| 命令功能 | 同步模式下的SHA计算
| 命令参数 | 发送命令AT+SHA_MODE=1到A板, 设置A板的SHA模式为sha1模式，然后选择一次update，只需要发送一次AT+SHA_SYNC=0,0,13DDE153E69570680FBDDF575CD090C1AE60C4D613A303FCE7A74C7E288F85A2,32（如果选择两次update，则需要发送update_or_finish=0时AT命令两次），最后选择finish，发送命令：AT+SHA_SYNC=0,1,13DDE153E69570680FBDDF575CD090C1AE60C4D613A303FCE7A74C7E288F85A2,32，测试完成后， A板对应串口会打印计算后的密文，并且会打印OK，将串口打印的密文与OPENSSL软件计算后的结果比较，如果结果相同，并且A板打印OK，即为测试成功；否则测试失败|

### AT+SHA_ASYNC

| 命令格式 | AT+SHA_ASYNC=idx,update_or_finish,source_data,data_len |
|------------------------------------------|--------------------------------------------------------------------------------------------------|
| 命令功能 | 异步模式下的SHA计算|
| 命令参数 | 发送命令AT+SHA_MODE=1到A板, 设置A板的SHA模式为sha1模式，然后选择一次update，只需要发送一次AT+SHA_ASYNC=0,0,13DDE153E69570680FBDDF575CD090C1AE60C4D613A303FCE7A74C7E288F85A2,32（如果选择两次update，则需要发送update_or_finish=0时AT命令两次），最后选择finish，发送命令：AT+SHA_SYNC=0,1,13DDE153E69570680FBDDF575CD090C1AE60C4D613A303FCE7A74C7E288F85A2,32，测试完成后， A板对应串口会打印计算后的密文，并且会打印OK，将串口打印的密文与OPENSSL软件计算后的结果比较，如果结果相同，并且A板打印OK，即为测试成功；否则测试失败 |


### AT+SHA_SYNC_PERFORMANCE

| 命令格式 | AT+SHA_SYNC_PERFORMANCE=sha_idx,sha_mode,rand_data,update_times,finish_times,test_loops,data_length |
|------------------------------------------|--------------------------------------------------------------------------------------------------|
| 命令功能 | SHA同步计算性能测试<br>|
| 命令参数 | idx：设备号；sha_mode:sha模式选择，1：sha1,2：sha256,3:sha224,4:sha512,5:sha384,6:sha512_256,7:sha_512_224；rand_data:是否每次数据随机，1：是，0：否；update_times：update次数选择；finish_times:finish次数；test_loops：循环次数，data_length:数据长度|
| 示例 | 发送命令AT+SHA_SYNC_PERFORMANCE=0,1,1,5,5,10,16到A板,  测试完成后， A板对应串口会打印OK；否则测试失败 |

### AT+SHA_ASYNC_PERFORMANCE

| 命令格式 | AT+SHA_ASYNC_PERFORMANCE=sha_idx,sha_mode,rand_data,update_times,finish_times,test_loops,data_length |
|------------------------------------------|--------------------------------------------------------------------------------------------------|
| 命令功能 | SHA异步计算性能测试<br>|
| 命令参数 | idx：设备号；sha_mode:sha模式选择，1：sha1,2：sha256,3:sha224,4:sha512,5:sha384,6:sha512_256,7:sha_512_224；rand_data:是否每次数据随机，1：是，0：否；update_times：update次数选择；finish_times:finish次数；test_loops：循环次数，data_length:数据长度|
| 示例 | 发送命令AT+SHA_ASYNC_PERFORMANCE=0,1,1,5,5,10,16到A板,  测试完成后， A板对应串口会打印OK；否则测试失败 |


### AT+SHA_SYNC_STABILITY

| 命令格式 | AT+SHA_SYNC_STABILITY=sha_idx,sha_mode,rand_data,update_times,finish_times,test_loops,data_length |
|------------------------------------------|--------------------------------------------------------------------------------------------------|
| 命令功能 | SHA同步计算可靠性测试<br>|
| 命令参数 | idx：设备号；sha_mode:sha模式选择，1：sha1,2：sha256,3:sha224,4:sha512,5:sha384,6:sha512_256,7:sha_512_224；rand_data:是否每次数据随机，1：是，0：否；update_times：update次数选择；finish_times:finish次数；test_loops：循环次数，data_length:数据长度|
| 示例 | 发送命令AT+SHA_SYNC_STABILITY=0,1,1,5,5,10,16到A板,  测试完成后， A板对应串口会打印OK；否则测试失败 |

### AT+SHA_ASYNC_STABILITY

| 命令格式 | AT+SHA_ASYNC_STABILITY=sha_idx,sha_mode,rand_data,update_times,finish_times,test_loops,data_length |
|------------------------------------------|--------------------------------------------------------------------------------------------------|
| 命令功能 | SHA异步计算可靠性测试<br>|
| 命令参数 | idx：设备号；sha_mode:sha模式选择，1：sha1,2：sha256,3:sha224,4:sha512,5:sha384,6:sha512_256,7:sha_512_224；rand_data:是否每次数据随机，1：是，0：否；update_times：update次数选择；finish_times:finish次数；test_loops：循环次数，data_length:数据长度|
| 示例 | 发送命令AT+SHA_ASYNC_STABILITY=0,1,1,5,5,10,16到A板,  测试完成后， A板对应串口会打印OK；否则测试失败 |