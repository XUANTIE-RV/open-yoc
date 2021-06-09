## CODEC

### 概要
该文档主要描述CODEC模块（codec 是音频编解码器，其具有音频采集（将模拟量转换为数字量）和音频播放功能（数字音频转换为模拟量），同
时可以进行数据滤波、增益等。）的CSI2.0自动化测试AT命令。 文档中所涉及的这些命令是用来测试CODEC的输入和输出以及状态读取等操作是否符合预期。


### 测试原理
测试该项目，只需要一块测试板通过串口工具和PC相连。测试时PC发送AT指令启动测试，打印OK说明测试成功


### 测试流程
- 测试前应当准备好测试板
- 发送CODEC测试命令进行测试 （测试命令功能请参照下文）


### AT命令列表
| AT命令列表                                                   | 功能                                                         |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| AT+CODEC_INTERFACE                                           | CODEC模块接口测试。该命令将会测试SPI模块下所有的CSI 接口， 测试CSI接口时将会传入异常值、非法值，验证CSI接口返回值是否符合期望 |
| AT+CODEC_SYNC_OUTPUT=idx,ch_idx,sample_rate,bit_width,sound_track_num,<br/>data_size,buffer_size,digital_gain,analog_gain,mix_gain,period,mode | 测试CODEC模块同步输出。用DMA同步方式输出一段数据，通过返回值和回调确定输出是否成功 |
| AT+CODEC_SYNC_INPUT=idx,ch_idx,sample_rate,bit_width,sound_track_num,<br/>data_size,buffer_size,digital_gain,analog_gain,mix_gain,period,mode | 测试CODEC模块同步输入。用DMA同步方式输入一段数据，通过返回值和回调确定输入是否成功 |
| AT+CODEC_ASYNC_OUTPUT=idx,ch_idx,sample_rate,bit_width,sound_track_num,<br/>data_size,buffer_size,digital_gain,analog_gain,mix_gain,period,mode | 测试CODEC模块异步输出。用DMA异步方式输出一段数据，通过返回值和回调确定输出是否成功 |
| AT+CODEC_ASYNC_INPUT=idx,ch_idx,sample_rate,bit_width,sound_track_num,<br/>data_size,buffer_size,digital_gain,analog_gain,mix_gain,period,mode | 测试CODEC模块异步输入。用DMA异步方式输入一段数据，通过返回值和回调确定输入是否成功 |
| AT+CODEC_OUTPUT_CHANNEL_STATE=idx,ch_idx                     | 测试CODEC模块写状态。用DMA同步方式输出一段数据，在过程中测试写状态是否正确 |
| AT+CODEC_INPUT_CHANNEL_STATE=idx,ch_idx                      | 测试CODEC模块读状态。用DMA同步方式输入一段数据，在过程中测试读状态是否正确 |
| AT+CODEC_OUTPUT_BUFFER=idx,ch_idx,buffer_size                | 测试CODEC模块输出缓存的剩余数量。分别在同步模式和异步模式输出一段数据。看缓存的剩余数量和预期是否一致 |
| AT+CODEC_OUTPUT_MUTE=idx,ch_idx,sample_rate,bit_width,sound_track_num,<br/>data_size,buffer_size,digital_gain,analog_gain,mix_gain,period,mode | 测试CODEC模块输出通道的静音。在DMA异步输出时启动静音功能。通过返回值确定输出是否正确 |
| AT+CODEC_INPUT_MUTE=idx,ch_idx,sample_rate,bit_width,sound_track_num,<br/>data_size,buffer_size,digital_gain,analog_gain,mix_gain,period,mode | 测试CODEC模块输入通道的静音。在DMA异步输入时启动静音功能。通过返回值确定输入是否正确 |
| AT+CODEC_PAUSE_RESUME=idx,ch_idx,sample_rate,bit_width,sound_track_num,<br/>data_size,buffer_size,mode | 测试CODEC模块输出通道暂停和恢复功能。用DMA同步方式输出一段数据。输出时启用暂停和恢复功能，通过读取输出的剩余缓存来确定暂停和恢复功能的有效性 |


### 参数列表

| 参数名字        | 释义                            | 取值范围                                                     |
| --------------- | ------------------------------- | ------------------------------------------------------------ |
| idx             | 表示使用那一组外设，0表示codec0 | 0x0 - 0xFFFFFFFF                                             |
| ch_idx          | 表示通道的序号<br>              | 0x0 - 0xFFFFFFFF                                             |
| sample_rate     | 设置的采样波特率                | 8000U<br/>11025U<br/>12000U<br/>16000U<br/>22050U<br/>24000U<br/>32000U<br/>44100U<br/>48000U<br/>96000U<br/>192000U<br/>256000U |
| bit_width       | 表示采样的宽度                  | 16U<br/>24U<br/>32U                                          |
| sound_track_num | 声道数                          | 0x0 - 0xFFFFFFFF                                             |
| data_size       | 需要的数据长度                  | 0x0 - 0xFFFFFFFF                                             |
| buffer_size     | 缓冲区的数据长度                | 0x0 - 0xFFFFFFFF                                             |
| digital_gain    | 设置通道的数码增益              | 0x0 - 0xFFFFFFFF                                             |
| analog_gain     | 设置通道的模拟增益              | 0x0 - 0xFFFFFFFF                                             |
| mix_gain        | 设置通道的混频增益              | 0x0 - 0xFFFFFFFF                                             |
| period          | 设置完成多少数据发送上报周期    | 0x0 - 0xFFFFFFFF                                             |
| mode            | 设置模式                        | 0：单端模式<br/>1：差分模式                                  |



### 命令详解

#### AT+CODEC_INTERFACE

| 命令格式             | AT+CODEC_INTERFACE                                           |
| -------------------- | ------------------------------------------------------------ |
| 命令功能             | CODEC模块接口测试。该命令将会测试SPI模块下所有的CSI 接口， 测试CSI接口时将会传入异常值、非法值，验证CSI接口返回值是否符合期望 |
| 可搭配的辅助测试命令 | 无                                                           |
| 示例                 | 步骤1:发送测试接口指令<br>AT+CODEC_INTERFACE<br>测试完成后， 板子对应串口会打印OK；否则测试失败 |
| 备注                 |                                                              |



#### AT+CODEC_SYNC_OUTPUT

| 命令格式             | AT+CODEC_SYNC_OUTPUT=idx,ch_idx,sample_rate,bit_width,sound_track_num,<br/>data_size,buffer_size,digital_gain,analog_gain,mix_gain,period,mode |
| -------------------- | ------------------------------------------------------------ |
| 命令功能             | 测试CODEC模块同步输入。用DMA同步方式输入一段数据，通过返回值和回调确定输入是否成功 |
| 可搭配的辅助测试命令 | 无                                                           |
| 示例                 | 步骤1:发送测试CODEC模块同步输出指令<br>AT+CODEC_SYNC_OUTPUT=0,0,48000,16,1,2048,1024,0,46,0,512,1<br>这表示外设为codec0,通道序号为0，采样波特率为48000，16位采样宽度，声道数为1，需要发送的数据大小为2K，缓冲区大小为1K,数字增益为0 db,模拟增益为46db,混频增益为0，发送上报周期为512，设置模式为差分<br>测试完成后， 板子对应串口会打印OK；否则测试失败<br/> |
| 备注                 |                                                              |



#### AT+CODEC_SYNC_INPUT

| 命令格式             | AT+CODEC_SYNC_INPUT=idx,ch_idx,sample_rate,bit_width,sound_track_num,<br/>data_size,buffer_size,digital_gain,analog_gain,mix_gain,period,mode |
| -------------------- | ------------------------------------------------------------ |
| 命令功能             | 测试CODEC模块同步输入。用DMA同步方式输入一段数据，通过返回值和回调确定输入是否成功 |
| 可搭配的辅助测试命令 | 无                                                           |
| 示例                 | 步骤1:发送测试CODEC模块同步输入指令<br>AT+CODEC_SYNC_INPUT=0,0,48000,16,1,2048,1024,0,46,0,512,1<br>这表示外设为codec0,通道序号为0，采样波特率为48000，16位采样宽度，声道数为1，需要发送的数据大小为2K，缓冲区大小为1K,数字增益为0 db,模拟增益为46db,混频增益为0，发送上报周期为512，设置模式为差分<br>测试完成后， 板子对应串口会打印OK；否则测试失败<br/> |
| 备注                 |                                                              |



#### AT+CODEC_SYNC_INPUT

| 命令格式             | AT+CODEC_SYNC_INPUT=idx,ch_idx,sample_rate,bit_width,sound_track_num,<br/>data_size,buffer_size,digital_gain,analog_gain,mix_gain,period,mode |
| -------------------- | ------------------------------------------------------------ |
| 命令功能             | 测试CODEC模块同步输入。用DMA同步方式输入一段数据，通过返回值和回调确定输入是否成功 |
| 可搭配的辅助测试命令 | 无                                                           |
| 示例                 | 步骤1:发送测试CODEC模块同步输入指令<br>AT+CODEC_SYNC_INPUT=0,0,48000,16,1,2048,1024,0,46,0,512,1<br>这表示外设为codec0,通道序号为0，采样波特率为48000，16位采样宽度，声道数为1，需要发送的数据大小为2K，缓冲区大小为1K,数字增益为0 db,模拟增益为46db,混频增益为0，发送上报周期为512，设置模式为差分<br>测试完成后， 板子对应串口会打印OK；否则测试失败<br/> |
| 备注                 |                                                              |



#### AT+CODEC_ASYNC_OUTPUT

| 命令格式             | AT+CODEC_ASYNC_OUTPUT=idx,ch_idx,sample_rate,bit_width,sound_track_num,<br/>data_size,buffer_size,digital_gain,analog_gain,mix_gain,period,mode |
| -------------------- | ------------------------------------------------------------ |
| 命令功能             | 测试CODEC模块异步输出。用DMA异步方式输出一段数据，通过返回值和回调确定输出是否成功 |
| 可搭配的辅助测试命令 | 无                                                           |
| 示例                 | 步骤1:发送测试CODEC模块异步输出指令<br>AT+CODEC_ASYNC_OUTPUT=0,0,48000,16,1,2048,1024,0,46,0,512,1<br>这表示外设为codec0,通道序号为0，采样波特率为48000，16位采样宽度，声道数为1，需要发送的数据大小为2K，缓冲区大小为1K,数字增益为0 db,模拟增益为46db,混频增益为0，发送上报周期为512，设置模式为差分<br>测试完成后， 板子对应串口会打印OK；否则测试失败<br/> |
| 备注                 |                                                              |



#### AT+CODEC_ASYNC_INPUT

| 命令格式             | AT+CODEC_ASYNC_INPUT=idx,ch_idx,sample_rate,bit_width,sound_track_num,<br/>data_size,buffer_size,digital_gain,analog_gain,mix_gain,period,mode |
| -------------------- | ------------------------------------------------------------ |
| 命令功能             | 测试CODEC模块异步输入。用DMA异步方式输入一段数据，通过返回值和回调确定输入是否成功 |
| 可搭配的辅助测试命令 | 无                                                           |
| 示例                 | 步骤1:发送测试CODEC模块异步输入指令<br>AT+CODEC_ASYNC_INPUT=0,0,48000,16,1,2048,1024,0,46,0,512,1<br>这表示外设为codec0,通道序号为0，采样波特率为48000，16位采样宽度，声道数为1，需要发送的数据大小为2K，缓冲区大小为1K,数字增益为0 db,模拟增益为46db,混频增益为0，发送上报周期为512，设置模式为差分<br>测试完成后， 板子对应串口会打印OK；否则测试失败<br/> |
| 备注                 |                                                              |



#### AT+CODEC_OUTPUT_CHANNEL_STATE

| 命令格式             | AT+CODEC_OUTPUT_CHANNEL_STATE=idx,ch_idx                     |
| -------------------- | ------------------------------------------------------------ |
| 命令功能             | 测试CODEC模块写状态。用DMA同步方式输出一段数据，在过程中测试写状态是否正确 |
| 可搭配的辅助测试命令 | 无                                                           |
| 示例                 | 步骤1:发送测试CODEC模块写状态指令<br>AT+CODEC_OUTPUT_CHANNEL_STATE=0,0<br>这表示外设为codec0,通道序号为0<br>测试完成后， 板子对应串口会打印OK；否则测试失败<br/> |
| 备注                 |                                                              |



#### AT+CODEC_INPUT_CHANNEL_STATE

| 命令格式             | AT+CODEC_INPUT_CHANNEL_STATE=idx,ch_idx                      |
| -------------------- | ------------------------------------------------------------ |
| 命令功能             | 测试CODEC模块读状态。用DMA同步方式输入一段数据，在过程中测试读状态是否正确 |
| 可搭配的辅助测试命令 | 无                                                           |
| 示例                 | 步骤1:发送测试CODEC模块读状态指令<br>AT+CODEC_INPUT_CHANNEL_STATE=0,0<br>这表示外设为codec0,通道序号为0<br>测试完成后， 板子对应串口会打印OK；否则测试失败<br/> |
| 备注                 |                                                              |



#### AT+CODEC_OUTPUT_BUFFER

| 命令格式             | AT+CODEC_OUTPUT_BUFFER=idx,ch_idx,buffer_size                |
| -------------------- | ------------------------------------------------------------ |
| 命令功能             | 测试CODEC模块输出缓存的剩余数量。分别在同步模式和异步模式输出一段数据。看缓存的剩余数量和预期是否一致 |
| 可搭配的辅助测试命令 | 无                                                           |
| 示例                 | 步骤1:发送测试CODEC模块输出缓存指令<br>AT+CODEC_OUTPUT_BUFFER=0,0,1024<br>这表示外设为codec0,通道序号为0，缓冲区大小为1024<br>测试完成后， 板子对应串口会打印OK；否则测试失败<br/> |
| 备注                 |                                                              |



#### AT+CODEC_OUTPUT_MUTE

| 命令格式             | AT+CODEC_OUTPUT_MUTE=idx,ch_idx,sample_rate,bit_width,sound_track_num,<br/>data_size,buffer_size,digital_gain,analog_gain,mix_gain,period,mode |
| -------------------- | ------------------------------------------------------------ |
| 命令功能             | 测试CODEC模块输出通道的静音。在DMA异步输出时启动静音功能。通过返回值确定输出是否正确 |
| 可搭配的辅助测试命令 | 无                                                           |
| 示例                 | 步骤1:发送测试CODEC模块输出通道静音指令<br>AT+CODEC_OUTPUT_MUTE=0,0,48000,16,1,2048,1024,0,46,0,512,1<br>这表示外设为codec0,通道序号为0，采样波特率为48000，16位采样宽度，声道数为1，需要发送的数据大小为2K，缓冲区大小为1K,数字增益为0 db,模拟增益为46db,混频增益为0，发送上报周期为512，设置模式为差分<br/>测试完成后， 板子对应串口会打印OK；否则测试失败<br/> |
| 备注                 |                                                              |



#### AT+CODEC_INPUT_MUTE

| 命令格式             | AT+CODEC_INPUT_MUTE=idx,ch_idx,sample_rate,bit_width,sound_track_num,<br/>data_size,buffer_size,digital_gain,analog_gain,mix_gain,period,mode |
| -------------------- | ------------------------------------------------------------ |
| 命令功能             | 测试CODEC模块输入通道的静音。在DMA异步输入时启动静音功能。通过返回值确定输入是否正确 |
| 可搭配的辅助测试命令 | 无                                                           |
| 示例                 | 步骤1:发送测试CODEC模块输入通道静音指令<br>AT+CODEC_INPUT_MUTE=0,0,48000,16,1,2048,1024,0,46,0,512,1<br>这表示外设为codec0,通道序号为0，采样波特率为48000，16位采样宽度，声道数为1，需要发送的数据大小为2K，缓冲区大小为1K,数字增益为0 db,模拟增益为46db,混频增益为0，发送上报周期为512，设置模式为差分<br/>测试完成后， 板子对应串口会打印OK；否则测试失败<br/> |
| 备注                 |                                                              |



#### AT+CODEC_PAUSE_RESUME

| 命令格式             | AT+CODEC_PAUSE_RESUME=idx,ch_idx,sample_rate,bit_width,sound_track_num,<br/>data_size,buffer_size,mode |
| -------------------- | ------------------------------------------------------------ |
| 命令功能             | 测试CODEC模块输出通道暂停和恢复功能。用DMA同步方式输出一段数据。输出时启用暂停和恢复功能，通过读取输出的剩余缓存来确定暂停和恢复功能的有效性 |
| 可搭配的辅助测试命令 | 无                                                           |
| 示例                 | 步骤1:发送测试CODEC模块输出通道暂停和恢复功能指令<br>AT+CODEC_PAUSE_RESUME=0,0,48000,16,1,2048,1024,1<br>这表示外设为codec0,通道序号为0，采样波特率为48000，16位采样宽度，声道数为1，需要发送的数据大小为2K，缓冲区大小为1K,设置模式为差分<br/>测试完成后， 板子对应串口会打印OK；否则测试失败<br/> |
| 备注                 |                                                              |

