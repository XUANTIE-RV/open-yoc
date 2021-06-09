## AES

### 概要

该文档主要描述AES模块的CSI2.0自动化测试AT命令。 文档中所涉及的这些命令是用来测试AES模块的接口工作是否正常、接口返回值是否符合预期。



### 测试原理

测试时，应给被测试板发送AES测试命令进行相应的测试；测试程序会按照按照测试命令的配置进行相应的加密解密测试， 若加密解密的结果与预期一致，则相应的测试成功完成，否则测试失败。



### 测试流程

- 测试前应当准备好测试板（以下简称A板）
- 发送AES测试命令进行测试 （测试命令功能请参照下文）



### AT+SET_AES_KEY

| 命令格式 | AT+SET_AES_KEY=key,key_len                                   |
| -------- | ------------------------------------------------------------ |
| 命令功能 | 设置AES加解密所需的key和key_len                              |
| 命令参数 | key：key的内容<br/>key_len：key 的bit长度，定义参见如下<br>16：128bit，24：192bit，32：256bit |
| 示例     | 发送命令AT+SET_AES_KEY=166594C0FB18D5962CA4392210E6E52F,16到A板,  A板会设置相应的key和key_len，设置完成后， A板对应串口会打印OK；否则设置失败 |



### AT+SET_AES_IV

| 命令格式 | AT+SET_AES_KEY=iv                                            |
| -------- | ------------------------------------------------------------ |
| 命令功能 | 设置AES cbc模式加解密所需的IV                                |
| 命令参数 | iv：iv的内容                                                 |
| 示例     | 发送命令AT+SET_AES_iv=166594C0FB18D5962CA4392210E6E52F到A板,  A板会设置相应的iv，设置完成后， A板对应串口会打印OK；否则设置失败 |



### AT+AES_ECB_ENCRYPT

| 命令格式 | AT+AES_ECB_ENCRYPT=idx,data,data_len                         |
| -------- | ------------------------------------------------------------ |
| 命令功能 | AES加解密模块ecb模式接口测试<br>对明文进行AES算法的ecb模式加密，对应串口会打印加密后的数据 |
| 命令参数 | idx：控制器号<br>data：加密的数据<br/>data_len：数据的长度   |
| 示例     | 发送命令AT+AES_ECB_ENCRYPT=0,36E66E8306907E696571F6453F055A59,16到A板,  A板对应串口会打印加密后的数据，测试完成后， A板对应串口会打印OK；否则测试失败 |



### AT+AES_ECB_DECRYPT

| 命令格式 | AT+AES_ECB_DECRYPT=idx,data,data_len                         |
| -------- | ------------------------------------------------------------ |
| 命令功能 | AES加解密模块ecb模式接口测试<br>对密文进行AES算法的ecb模式解密，对应串口会打印解密后的数据 |
| 命令参数 | idx：控制器号<br>data：解密的数据<br/>data_len：数据的长度   |
| 示例     | 发送命令AT+AES_ECB_DECRYPT=0,F534319818F2942EC155D40F88284508,16到A板,  A板对应串口会打印加密后的数据，测试完成后， A板对应串口会打印OK；否则测试失败 |

### AT+AES_CBC_ENCRYPT

| 命令格式 | AT+AES_ECB_ENCRYPT=idx,data,data_len                         |
| -------- | ------------------------------------------------------------ |
| 命令功能 | AES加解密模块cbc模式接口测试<br>对明文进行AES算法的cbc模式加密，对应串口会打印加密后的数据 |
| 命令参数 | idx：控制器号<br>data：加密的数据<br/>data_len：数据的长度   |
| 示例     | 发送命令AT+AES_CBC_ENCRYPT=0,36E66E8306907E696571F6453F055A59,16到A板,  A板对应串口会打印加密后的数据，测试完成后， A板对应串口会打印OK；否则测试失败 |

### AT+AES_CBC_DECRYPT

| 命令格式 | AT+AES_CBC_DECRYPT=idx,data,data_len                         |
| -------- | ------------------------------------------------------------ |
| 命令功能 | AES加解密模块cbc模式接口测试<br>对明文进行AES算法的cbc模式解密，对应串口会打印解密后的数据 |
| 命令参数 | idx：控制器号<br>data：解密的数据<br/>data_len：数据的长度   |
| 示例     | 发送命令AT+AES_CBC_DECRYPT=0,36E66E8306907E696571F6453F055A59,16到A板,  A板对应串口会打印加密后的数据，测试完成后， A板对应串口会打印OK；否则测试失败 |

### AT+AES_CFB128_ENCRYPT

| 命令格式 | AT+AES_CFB1_ENCRYPT=idx,data,data_len,num                    |
| -------- | ------------------------------------------------------------ |
| 命令功能 | AES加解密模块cfb1模式接口测试<br>对明文进行AES算法的cfb128模式加密，对应串口会打印加密后的数据 |
| 命令参数 | idx：控制器号<br>data：加密的数据<br/>data_len：数据的长度<br/>num：加密的次数 |
| 示例     | 发送命令AT+AES_CFB1_ENCRYPT=0,36E66E8306907E696571F6453F055A59,16到A板,  A板对应串口会打印加密后的数据，测试完成后， A板对应串口会打印OK；否则测试失败 |

### AT+AES_CFB128_DECRYPT

| 命令格式 | AT+AES_CFB128_DECRYPT=idx,data,data_len,num                  |
| -------- | ------------------------------------------------------------ |
| 命令功能 | AES加解密模块cfb128模式接口测试<br>对明文进行AES算法的cfb128模式解密，对应串口会打印解密后的数据 |
| 命令参数 | idx：控制器号<br>data：解密的数据<br/>data_len：数据的长度<br/>num：解密的次数 |
| 示例     | 发送命令AT+AES_CFB128_DECRYPT=0,36E66E8306907E696571F6453F055A59,16到A板,  A板对应串口会打印解密后的数据，测试完成后， A板对应串口会打印OK；否则测试失败 |



### AT+AES_ECB_ENCRYPT_PERFORMANCE

| 命令格式 | AT+AES_ECB_ENCRYPT_PERFORMANCE=idx,key_len,data_len,loop        |
| -------- | ------------------------------------------------------------ |
| 命令功能 | 对明文进行AES算法的ecb模式加密性能测试，对应串口会打印性能值 |
| 命令参数 | idx：控制器号<br>key_len：密钥长度<br/>data_len：数据的长度<br/>loop：循环的次数 |
| 示例     | 发送命令AT+AES_ECB_ENCRYPT=0,16,16,1000到A板,  A板对应串口会打印性能值，测试完成后， A板对应串口会打印OK；否则测试失败 |



### AT+AES_ECB_DECRYPT_PERFORMANCE

| 命令格式 | AT+AES_ECB_DECRYPT=idx,key_len,data_len,loop                    |
| -------- | ------------------------------------------------------------ |
| 命令功能 | 对明文进行AES算法的ecb模式解密性能测试，对应串口会打印性能值 |
| 命令参数 | idx：控制器号<br>key_len：密钥长度<br/>data_len：数据的长度<br/>loop：循环的次数 |
| 示例     | 发送命令AT+AES_ECB_DECRYPT=0,16,16,1000到A板,  A板对应串口会打印性能值，测试完成后， A板对应串口会打印OK；否则测试失败 |

### AT+AES_CBC_ENCRYPT_PERFORMANCE

| 命令格式 | AT+AES_ECB_ENCRYPT_PERFORMANCE=idx,key_len,data_len,loop        |
| -------- | ------------------------------------------------------------ |
| 命令功能 | 对明文进行AES算法的cbc模式加密性能测试，对应串口会打印性能值 |
| 命令参数 | idx：控制器号<br>key_len：密钥长度<br/>data_len：数据的长度<br/>loop：循环的次数 |
| 示例     | 发送命令AT+AES_CBC_ENCRYPT_PERFORMANCE=0,16,16,1000到A板,  A板对应串口会打印性能值，测试完成后， A板对应串口会打印OK；否则测试失败 |

### AT+AES_CBC_DECRYPT_PERFORMANCE

| 命令格式 | AT+AES_CBC_DECRYPT=idx,key_len,data_len,loop                    |
| -------- | ------------------------------------------------------------ |
| 命令功能 | 对明文进行AES算法的cbc模式解密性能测试，对应串口会打印性能值 |
| 命令参数 | idx：控制器号<br>key_len：密钥长度<br/>data_len：数据的长度<br/>loop：循环的次数 |
| 示例     | 发送命令AT+AES_CBC_DECRYPT=0,16,16,1000到A板,  A板对应串口会打印性能值， A板对应串口会打印OK；否则测试失败 |

### AT+AES_ECB_ENCRYPT_DECRYPT_STABILITY

| 命令格式 | AT+AES_ECB_DECRYPT=idx,key_len,data_len,loop                    |
| -------- | ------------------------------------------------------------ |
| 命令功能 | 对明文进行AES算法的ecb模式加解密可靠性测试 |
| 命令参数 | idx：控制器号<br>key_len：密钥长度<br/>data_len：数据的长度<br/>loop：循环的次数 |
| 示例     | 发送命令AT+AES_ECB_ENCRYPT_DECRYPT_STABILITY=0,16,16,1000到A板, A板对应串口会打印OK；否则测试失败 |

### AT+AES_CBC_ENCRYPT_DECRYPT_STABILITY

| 命令格式 | AT+AES_CBC_DECRYPT=idx,key_len,data_len,loop                    |
| -------- | ------------------------------------------------------------ |
| 命令功能 | 对明文进行AES算法的cbc模式加解密可靠性 |
| 命令参数 | idx：控制器号<br>key_len：密钥长度<br/>data_len：数据的长度<br/>loop：循环的次数 |
| 示例     | 发送命令AT+AES_ECB_ENCRYPT_DECRYPT_STABILITY=0,16,16,1000到A板,  A板对应串口会打印OK；否则测试失败 |


### AT+AES_CBC_ENCRYPT_ALIGNED

| 命令格式 | AT+AES_CBC_ENCRYPT_ALIGNED=idx,src_data,data_len,key_align_len,data_align_len,right_align_len |
| -------- | ------------------------------------------------------------ |
| 命令功能 | AES加解密模块CBC模式指定字节对齐加密测试 |
| 命令参数 | idx：控制器号<br>src_data：解密的数据<br/>data_len：数据的长度<br/>key_align_len：密钥指定字节对齐<br/>data_align_len：数据指定字节对齐<br>right_aligh_len：正确对齐字节 |
| 示例     | 发送命令AT+AES_CBC_ENCRYPT_ALIGNED=0,36E66E8306907E696571F6453F055A59,16,16,64到A板,  A板对应串口会打印加密后的数据，测试完成后， A板对应串口会打印OK；否则测试失败 |

### AT+AES_CBC_DECRYPT_ALIGNED

| 命令格式 | AT+AES_CBC_DECRYPT_ALIGNED=idx,src_data,data_len,key_align_len,data_align_len,right_align_len |
| -------- | ------------------------------------------------------------ |
| 命令功能 | AES加解密模块CBC模式指定字节对齐解密测试 |
| 命令参数 | idx：控制器号<br>src_data：解密的数据<br/>data_len：数据的长度<br/>key_align_len：密钥指定字节对齐<br/>data_align_len：数据指定字节对齐<br>right_aligh_len：正确对齐字节 |
| 示例     | 发送命令AT+AES_CBC_DECRYPT_ALIGNED=0,36E66E8306907E696571F6453F055A59,16,16,64到A板,  A板对应串口会打印解密后的数据，测试完成后， A板对应串口会打印OK；否则测试失败 |

### AT+AES_ECB_ENCRYPT_ALIGNED

| 命令格式 | AT+AES_ECB_ENCRYPT_ALIGNED=idx,src_data,data_len,key_align_len,data_align_len,right_align_len |
| -------- | ------------------------------------------------------------ |
| 命令功能 | AES加解密模块ECB模式指定字节对齐加密测试 |
| 命令参数 | idx：控制器号<br>src_data：解密的数据<br/>data_len：数据的长度<br/>key_align_len：密钥指定字节对齐<br/>data_align_len：数据指定字节对齐<br>right_aligh_len：正确对齐字节 |
| 示例     | 发送命令AT+AES_ECB_ENCRYPT_ALIGNED=0,36E66E8306907E696571F6453F055A59,16,16,64到A板,  A板对应串口会打印加密后的数据，测试完成后， A板对应串口会打印OK；否则测试失败 |

### AT+AES_ECB_ENCRYPT_ALIGNED

| 命令格式 | AT+AES_ECB_ENCRYPT_ALIGNED=idx,src_data,data_len,key_align_len,data_align_len,right_align_len |
| -------- | ------------------------------------------------------------ |
| 命令功能 | AES加解密模块ECB模式指定字节对齐解密测试 |
| 命令参数 | idx：控制器号<br>src_data：解密的数据<br/>data_len：数据的长度<br/>key_align_len：密钥指定字节对齐<br/>data_align_len：数据指定字节对齐<br>right_aligh_len：正确对齐字节 |
| 示例     | 发送命令AT+AES_ECB_DECRYPT_ALIGNED=0,36E66E8306907E696571F6453F055A59,16,16,64到A板,  A板对应串口会打印解密后的数据，测试完成后， A板对应串口会打印OK；否则测试失败 |
