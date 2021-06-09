## RSA

### 概要

该文档主要描述RSA模块的CSI2.0自动化测试AT命令。 文档中所涉及的这些命令是用来测试RSA模块的接口工作是否正常、接口返回值是否符合预期。



### 测试原理

测试时，应给被测试板发送RSA测试命令进行相应的测试；测试程序会按照按照测试命令的配置进行相应的加密解密测试， 若加密解密的结果与预期一致，则相应的测试成功完成，否则测试失败。



### 测试流程

- 测试前应当准备好测试板（以下简称A板）
- 发送RSA测试命令进行测试 （测试命令功能请参照下文）



### RSA测试命令列表

| AT命令                                   | 所需测试参数                                                             | 功能                       |
| ----------------------------------------| ------------------------------------------------------------------------ | -------------------------- |
| AT+RSA_SET_KEY                          | key_type,key_data,length_of_key                                          | 设置key数据                   |
| AT+RSA_ENCRYPT_SYNC                     | idx,key_bits,padding_type,hash_type,plain_data,length_of_data            | 同步模式下的加密               |
| AT+RSA_DECRYPT_SYNC                     | idx,key_bits,padding_type,hash_type,plain_data,length_of_data            | 同步模式下的解密               |
| AT+RSA_ENCRYPT_ASYNC                    | idx,key_bits,padding_type,hash_type,plain_data,length_of_data            | 异步模式下的加密     `          |
| AT+RSA_DECRYPT_ASYNC                    | idx,key_bits,padding_type,hash_type,plain_data,length_of_data            | 异步模式下的解密                |
| AT+RSA_ENCRYPT_DECRYPT_SYNC_PERFORMANCE | idx,key_bits,padding_type,hash_type,rand_data,encrypt_times,decrypt_times,length_of_data,test_loops | 同步模式下的加密、解密性能测试   |
| AT+RSA_ENCRYPT_DECRYPT_ASYNC_PERFORMANCE| idx,key_bits,padding_type,hash_type,rand_data,encrypt_times,decrypt_times,length_of_data,test_loops | 异步模式下的加密、解密性能测试   |
| AT+RSA_SIGN_VERIFY_SYNC                 | idx,key_bits,padding_type,hash_type,plain_data,length_of_data            | 同步模式下的签名、验签          |
| AT+RSA_SIGN_VERIFY_ASYNC                | idx,key_bits,padding_type,hash_type,plain_data,length_of_data            | 异步模式下的签名、验签          |
| AT+RSA_SIGN_VERIFY_SYNC_PERFORMANCE     | idx,key_bits,padding_type,hash_type,rand_data,sign_times,verify_times,length_of_data,test_loops | 同步模式下的签名、验签性能测试   |
| AT+RSA_SIGN_VERIFY_ASYNC_PERFORMANCE    | idx,key_bits,padding_type,hash_type,rand_data,sign_times,verify_times,length_of_data,test_loops | 异步模式下的签名、验签性能测试   |
| AT+RSA_ENCRYPT_DECRYPT_SYNC_STABILITY   | idx,key_bits,padding_type,hash_type,rand_data,encrypt_times,decrypt_times,length_of_data,test_loops | 同步模式下的加密、解密稳定性测试 |
| AT+RSA_ENCRYPT_DECRYPT_ASYNC_STABILITY  | idx,key_bits,padding_type,hash_type,rand_data,encrypt_times,decrypt_times,length_of_data,test_loops | 异步模式下的加密、解密稳定性测试 |
| AT+RSA_SIGN_VERIFY_SYNC_STABILITY       | idx,key_bits,padding_type,hash_type,rand_data,sign_times,verify_times,length_of_data,test_loops | 同步模式下的签名、验签稳定性测试 |
| AT+RSA_SIGN_VERIFY_ASYNC_STABILITY      | idx,key_bits,padding_type,hash_type,rand_data,sign_times,verify_times,length_of_data,test_loops | 异步模式下的签名、验签稳定性测试 |


### AT+RSA_SET_KEY

| 命令格式 |AT+RSA_SET_KEY= key_type,key_data,length_of_key|
|------------------------------------------|--------------------------------------------------------------------------------------------------|
| 命令功能 | 该命令用来设置RSA KEY值|
| 命令参数 | key_type：key类型， 0- d value,  1- e value,  2- n value<br><br>key_data： key数据， 16进制格式字符串<br><br>length_of_key: key数据长度 |
| 示例 | 发送命令AT+RSA_SET_KEY=0,0948F99D692F4ABD0A0B7B41A7562B14B2C3A183F23953DA376AD3E849594F08C9CF759560C9F960B880F807CD6FD98811463937C9C0F44C3475355743577CD627870C966D475E76DC2A92BE681B2A99923E90DE45479F861B292DACC42B9F32D47B796C80013C082D22A51821FBD52D868E7C714F0E553FDF218A270B2DF999,128到A板,  完成设置rsa d值key, 最终打印OK |


### AT+RSA_ENCRYPT_SYNC

| 命令格式 |AT+RSA_ENCRYPT_SYNC=idx,key_bits,padding_type,hash_type,plain_data,length_of_data |
|------------------------------------------|--------------------------------------------------------------------------------------------------|
| 命令功能 | RSA模块同步加密测试<br>调用同步模式接口，用公钥对明文进行RSA算法的加密，再用私钥对得到的密文进行解密， 检测是否解密后的原文与原始数据是否一致 |
| 命令参数 | idx：控制器号<br><br>key_bits：加密位，定义参数如下<br>0：192 Key bits<br>1：256 Key bits<br>2：512 Key bits<br>3：1024 Key bits<br>4：2048 Key bits<br>5：3072 Key bits<br>6：4096 Key bits<br><br>padding_type：填充方式，定义参数如下<br>0：不填充<br>1：PKCS1填充<br>2：PKCS1 OAEP填充<br>3：SSLV23填充<br>4：X931填充<br>5：PSS填充<br><br> |
| 示例 | 发送命令AT+RSA_DECRYPT_SYNC=0,3,1,2,27C8C0AABB3288FC824149A1302AEA3E9D18A9721A3B03DF93ADB6F187A961FE90C06F8FEA1D92677A2545742DBE39EAFB3C5E872B1B8996B5ED336C2AADDE54740AC0849CC52BE98E89CCA88CEF483BF5FA42CADF4F12F1D4B8F846A7DF985980FB787E2A42A5A20220812ECFA24C7C655BFEEF44D31110B401520AE9ECF6D7,128到A板, 测试完成后， A板对应串口会打印加密后的密文，解密后的原文；最终打印OK |

### AT+RSA_DECRYPT_SYNC 

| 命令格式 | AT+RSA_DECRYPT_SYNC =idx,key_bits,padding_type,hash_type,enprypted_data,length_of_data |
|------------------------------------------|--------------------------------------------------------------------------------------------------|
| 命令功能 | RSA模块解密测试<br>调用同步模式接口，用私钥对密文进行解密 |
| 命令参数 | idx：控制器号<br><br>key_bits：加密位，定义参数如下<br>0：192 Key bits<br>1：256 Key bits<br>2：512 Key bits<br>3：1024 Key bits<br>4：2048 Key bits<br>5：3072 Key bits<br>6：4096 Key bits<br><br>padding_type：填充方式，定义参数如下<br>0：不填充<br>1：PKCS1填充<br>2：PKCS1 OAEP填充<br>3：SSLV23填充<br>4：X931填充<br>5：PSS填充<br><br> |
| 示例 | 发送命令AT+RSA_DECRYPT_SYNC=0,3,1,2,1A50EAED3D9F54F129F471173F562AFC15F871C2C2AE1FCD3594FC2E5EEDAF2B046F6BD2B68F552B45DEDE10E3D0364F914825C45E5A1F43A37780D6769406327A6D91C8C0FF1C76D8706777BC4B1DD343566F61051AE0E3C5B9D33DC1C10F783F4712FDD22F1DD319067D8A220CC1323B4BC59CF219FA5D456967ABDDA05F9C,128到A板,  测试完成后， A板对应串口会打印解密后的原文；最终打印OK， 否则测试失败 |

### AT+RSA_ENCRYPT_ASYNC

| 命令格式 | AT+RSA_ENCRYPT_ASYNC=idx,key_bits,padding_type,hash_type,plain_data,length_of_data |
|------------------------------------------|--------------------------------------------------------------------------------------------------|
| 命令功能 | RSA模块异步加密测试<br>调用异步模式接口，用公钥对明文进行加密 |
| 命令参数 | idx：控制器号<br><br>key_bits：加密位，定义参数如下<br>0：192 Key bits<br>1：256 Key bits<br>2：512 Key bits<br>3：1024 Key bits<br>4：2048 Key bits<br>5：3072 Key bits<br>6：4096 Key bits<br><br>padding_type：填充方式，定义参数如下<br>0：不填充<br>1：PKCS1填充<br>2：PKCS1 OAEP填充<br>3：SSLV23填充<br>4：X931填充<br>5：PSS填充<br><br>hash_type：hash类型<br>0: MD5<br>1: SHA1<BR>2: SHA224<BR>3: SHA256<BR>4： SHA384<br>5:  SHA512<br> |
| 示例 | 发送命令AT+RSA_DECRYPT_SYNC=0,3,0,2,27C8C0AABB3288FC824149A1302AEA3E9D18A9721A3B03DF93ADB6F187A961FE90C06F8FEA1D92677A2545742DBE39EAFB3C5E872B1B8996B5ED336C2AADDE54740AC0849CC52BE98E89CCA88CEF483BF5FA42CADF4F12F1D4B8F846A7DF985980FB787E2A42A5A20220812ECFA24C7C655BFEEF44D31110B401520AE9ECF6D7,128到A板, 测试完成后， A板对应串口会打印加密后的密文，解密后的原文；最终打印OK |

### AT+RSA_DECRYPT_ASYNC

| 命令格式 | AT+RSA_DECRYPT_ASYNC=idx,key_bits,padding_type,hash_type,enprypted_data,length_of_data |
|------------------------------------------|--------------------------------------------------------------------------------------------------|
| 命令功能 | RSA模块异步解密测试<br>调用异步模式接口，用私钥对密文进行解密 |
| 命令参数 | idx：控制器号<br><br>key_bits：加密位，定义参数如下<br>0：192 Key bits<br>1：256 Key bits<br>2：512 Key bits<br>3：1024 Key bits<br>4：2048 Key bits<br>5：3072 Key bits<br>6：4096 Key bits<br><br>padding_type：填充方式，定义参数如下<br>0：不填充<br>1：PKCS1填充<br>2：PKCS1 OAEP填充<br>3：SSLV23填充<br>4：X931填充<br>5：PSS填充<br><br>hash_type：hash类型，<br>0: MD5<br/>1: SHA1<BR>2: SHA224<BR>3: SHA256<BR>4： SHA384<br/>5:  SHA512<br/> |
| 示例 | 发送命令AT+RSA_DECRYPT_SYNC=0,3,0,2,1A50EAED3D9F54F129F471173F562AFC15F871C2C2AE1FCD3594FC2E5EEDAF2B046F6BD2B68F552B45DEDE10E3D0364F914825C45E5A1F43A37780D6769406327A6D91C8C0FF1C76D8706777BC4B1DD343566F61051AE0E3C5B9D33DC1C10F783F4712FDD22F1DD319067D8A220CC1323B4BC59CF219FA5D456967ABDDA05F9C,128到A板,  测试完成后， A板对应串口会打印解密后的原文；最终打印OK， 否则测试失败 |


### AT+RSA_ENCRYPT_DECRYPT_SYNC_PERFORMANCE
| 命令格式 | AT+RSA_ENCRYPT_DECRYPT_SYNC_PERFORMANCE=idx,key_bits,padding_type,hash_type,rand_data,encrypt_times,decrypt_times,length_of_data,test_loops |
|------------------------------------------|--------------------------------------------------------------------------------------------------|
| 命令功能 | RSA模块同步加密、解密性能测试<br> |
| 命令参数 | idx：控制器号<br><br>key_bits：加密位，定义参数如下<br>0：192 Key bits<br>1：256 Key bits<br>2：512 Key bits<br>3：1024 Key bits<br>4：2048 Key bits<br>5：3072 Key bits<br>6：4096 Key bits<br><br>padding_type：填充方式，定义参数如下<br>0：不填充<br>1：PKCS1填充<br>2：PKCS1 OAEP填充<br>3：SSLV23填充<br>4：X931填充<br>5：PSS填充<br><br>hash_type：hash类型，<br>0: MD5<br/>1: SHA1<BR>2: SHA224<BR>3: SHA256<BR>4： SHA384<br/>5:  SHA512<br/><br/>rand_data:是否每次产生循环随机产生数据<br/><br/>encrypt_times:加密次数<br/><br/>decrypt_times:解密次数<br/><br/>length_of_data:数据长度<br/><br/>test_loops:循环次数 |
| 示例 | 发送命令AT+RSA_ENCRYPT_DECRYPT_SYNC_PERFORMANCE=0,3,1,3,1,10,10,128,100到A板,  测试完成后， A板对应串口会打印性能数据， 打印OK；否则测试失败 |


### AT+RSA_ENCRYPT_DECRYPT_ASYNC_PERFORMANCE
| 命令格式 | AT+RSA_ENCRYPT_DECRYPT_ASYNC_PERFORMANCE=idx,key_bits,padding_type,hash_type,rand_data,encrypt_times,decrypt_times,length_of_data,test_loops |
|------------------------------------------|--------------------------------------------------------------------------------------------------|
| 命令功能 | RSA模块异步加密、解密性能测试<br> |
| 命令参数 | idx：控制器号<br><br>key_bits：加密位，定义参数如下<br>0：192 Key bits<br>1：256 Key bits<br>2：512 Key bits<br>3：1024 Key bits<br>4：2048 Key bits<br>5：3072 Key bits<br>6：4096 Key bits<br><br>padding_type：填充方式，定义参数如下<br>0：不填充<br>1：PKCS1填充<br>2：PKCS1 OAEP填充<br>3：SSLV23填充<br>4：X931填充<br>5：PSS填充<br><br>hash_type：hash类型，<br>0: MD5<br/>1: SHA1<BR>2: SHA224<BR>3: SHA256<BR>4： SHA384<br/>5:  SHA512<br/><br/>rand_data:是否每次产生循环随机产生数据<br/><br/>encrypt_times:加密次数<br/><br/>decrypt_times:解密次数<br/><br/>length_of_data:数据长度<br/><br/>test_loops:循环次数 |
| 示例 | 发送命令AT+RSA_ENCRYPT_DECRYPT_ASYNC_PERFORMANCE=0,3,1,3,1,10,10,128,100到A板,  测试完成后， A板对应串口会打印性能数据， 打印OK；否则测试失败 |


### AT+RSA_SIGN_VERIFY_SYNC
| 命令格式 | AT+RSA_SIGN_VERIFY_SYNC=idx,key_bits,padding_type,hash_type,plain_data,length_of_data |
|------------------------------------------|--------------------------------------------------------------------------------------------------|
| 命令功能 | RSA模块同步签名、验签<br>调用同步模式接口，进行签名，并检查验签是否能够成功 |
| 命令参数 | idx：控制器号<br><br>key_bits：加密位，定义参数如下<br>0：192 Key bits<br>1：256 Key bits<br>2：512 Key bits<br>3：1024 Key bits<br>4：2048 Key bits<br>5：3072 Key bits<br>6：4096 Key bits<br><br>padding_type：填充方式，定义参数如下<br>0：不填充<br>1：PKCS1填充<br>2：PKCS1 OAEP填充<br>3：SSLV23填充<br>4：X931填充<br>5：PSS填充<br><br>hash_type：hash类型，<br>0: MD5<br/>1: SHA1<BR>2: SHA224<BR>3: SHA256<BR>4： SHA384<br/>5:  SHA512<br/> |
| 示例 | 发送命令AT+RSA_SIGN_VERIFY_SYNC=0,3,0,3,1A50EAED3D9F54F129F471173F562AFC15F871C2C2AE1FCD3594FC2E5EEDAF2B046F6BD2B68F552B45DEDE10E3D0364F914825C45E5A1F43A37780D6769406327A6D91C8C0FF1C76D8706777BC4B1DD343566F61051AE0E3C5B9D33DC1C10F783F4712FDD22F1DD319067D8A220CC1323B4BC59CF219FA5D456967ABDDA05F9C,128到A板,  测试完成后， A板对应串口会打印签名数据，并打印验签结果， 打印OK；否则测试失败 |


### AT+RSA_SIGN_VERIFY_ASYNC
| 命令格式 | AT+RSA_SIGN_VERIFY_SYNC=idx,key_bits,padding_type,hash_type,plain_data,length_of_data |
|------------------------------------------|--------------------------------------------------------------------------------------------------|
| 命令功能 | RSA模块异步签名、验签<br>调用异步模式接口，进行签名，并检查验签是否能够成功 |
| 命令参数 | idx：控制器号<br><br>key_bits：加密位，定义参数如下<br>0：192 Key bits<br>1：256 Key bits<br>2：512 Key bits<br>3：1024 Key bits<br>4：2048 Key bits<br>5：3072 Key bits<br>6：4096 Key bits<br><br>padding_type：填充方式，定义参数如下<br>0：不填充<br>1：PKCS1填充<br>2：PKCS1 OAEP填充<br>3：SSLV23填充<br>4：X931填充<br>5：PSS填充<br><br>hash_type：hash类型，<br>0: MD5<br/>1: SHA1<BR>2: SHA224<BR>3: SHA256<BR>4： SHA384<br/>5:  SHA512<br/> |
| 示例 | 发送命令AT+RSA_SIGN_VERIFY_ASYNC=0,3,0,3,1A50EAED3D9F54F129F471173F562AFC15F871C2C2AE1FCD3594FC2E5EEDAF2B046F6BD2B68F552B45DEDE10E3D0364F914825C45E5A1F43A37780D6769406327A6D91C8C0FF1C76D8706777BC4B1DD343566F61051AE0E3C5B9D33DC1C10F783F4712FDD22F1DD319067D8A220CC1323B4BC59CF219FA5D456967ABDDA05F9C,128到A板,  测试完成后， A板对应串口会打印签名数据，并打印验签结果， 打印OK；否则测试失败 |


### AT+RSA_SIGN_VERIFY_SYNC_PERFORMANCE
| 命令格式 | AT+RSA_SIGN_VERIFY_SYNC_PERFORMANCE=idx,key_bits,padding_type,hash_type,rand_data,sign_times,verify_times,length_of_data,test_loops |
|------------------------------------------|--------------------------------------------------------------------------------------------------|
| 命令功能 | RSA模块异步签名、验签性能测试<br> |
| 命令参数 | idx：控制器号<br><br>key_bits：加密位，定义参数如下<br>0：192 Key bits<br>1：256 Key bits<br>2：512 Key bits<br>3：1024 Key bits<br>4：2048 Key bits<br>5：3072 Key bits<br>6：4096 Key bits<br><br>padding_type：填充方式，定义参数如下<br>0：不填充<br>1：PKCS1填充<br>2：PKCS1 OAEP填充<br>3：SSLV23填充<br>4：X931填充<br>5：PSS填充<br><br>hash_type：hash类型，<br>0: MD5<br/>1: SHA1<BR>2: SHA224<BR>3: SHA256<BR>4： SHA384<br/>5:  SHA512<br/><br/>rand_data:是否每次产生循环随机产生数据<br/><br/>sign_times:签名次数<br/><br/>verify_times:验签次数<br/><br/>length_of_data:数据长度<br/><br/>test_loops:循环次数 |
| 示例 | 发送命令AT+RSA_SIGN_VERIFY_SYNC_PERFORMANCE=0,3,1,3,1,10,10,128,100到A板,  测试完成后， A板对应串口会打印性能数据， 打印OK；否则测试失败 |

### AT+RSA_SIGN_VERIFY_ASYNC_PERFORMANCE
| 命令格式 | AT+RSA_SIGN_VERIFY_ASYNC_PERFORMANCE=idx,key_bits,padding_type,hash_type,rand_data,sign_times,verify_times,length_of_data,test_loops |
|------------------------------------------|--------------------------------------------------------------------------------------------------|
| 命令功能 | RSA模块异步签名、验签性能测试<br> |
| 命令参数 | idx：控制器号<br><br>key_bits：加密位，定义参数如下<br>0：192 Key bits<br>1：256 Key bits<br>2：512 Key bits<br>3：1024 Key bits<br>4：2048 Key bits<br>5：3072 Key bits<br>6：4096 Key bits<br><br>padding_type：填充方式，定义参数如下<br>0：不填充<br>1：PKCS1填充<br>2：PKCS1 OAEP填充<br>3：SSLV23填充<br>4：X931填充<br>5：PSS填充<br><br>hash_type：hash类型，<br>0: MD5<br/>1: SHA1<BR>2: SHA224<BR>3: SHA256<BR>4： SHA384<br/>5:  SHA512<br/><br/>rand_data:是否每次产生循环随机产生数据<br/><br/>sign_times:签名次数<br/><br/>verify_times:验签次数<br/><br/>length_of_data:数据长度<br/><br/>test_loops:循环次数 |
| 示例 | 发送命令AT+RSA_SIGN_VERIFY_ASYNC_PERFORMANCE=0,3,1,3,1,10,10,128,100到A板,  测试完成后， A板对应串口会打印性能数据， 打印OK；否则测试失败 |


### AT+RSA_ENCRYPT_DECRYPT_SYNC_STABILITY
| 命令格式 | AT+RSA_SIGN_VERIFY_SYNC_STABILITY=idx,key_bits,padding_type,hash_type,rand_data,encrypt_times,decrypt_times,length_of_data,test_loops |
|------------------------------------------|--------------------------------------------------------------------------------------------------|
| 命令功能 | RSA模块同步加密、解密可靠性测试<br> |
| 命令参数 | idx：控制器号<br><br>key_bits：加密位，定义参数如下<br>0：192 Key bits<br>1：256 Key bits<br>2：512 Key bits<br>3：1024 Key bits<br>4：2048 Key bits<br>5：3072 Key bits<br>6：4096 Key bits<br><br>padding_type：填充方式，定义参数如下<br>0：不填充<br>1：PKCS1填充<br>2：PKCS1 OAEP填充<br>3：SSLV23填充<br>4：X931填充<br>5：PSS填充<br><br>hash_type：hash类型，<br>0: MD5<br/>1: SHA1<BR>2: SHA224<BR>3: SHA256<BR>4： SHA384<br/>5:  SHA512<br/><br/>rand_data:是否每次产生循环随机产生数据<br/><br/>encrypt_times:加密次数<br/><br/>decrypt_times:解密次数<br/><br/>length_of_data:数据长度<br/><br/>test_loops:循环次数 |
| 示例 | 发送命令AT+RSA_ENCRYPT_DECRYPT_SYNC_STABILITY=0,3,1,3,1,10,10,128,100到A板,  测试完成后， A板对应串口会打印性能数据， 打印OK；否则测试失败 |


### AT+RSA_ENCRYPT_DECRYPT_ASYNC_STABILITY
| 命令格式 | AT+RSA_SIGN_VERIFY_ASYNC_STABILITY=idx,key_bits,padding_type,hash_type,rand_data,encrypt_times,decrypt_times,length_of_data,test_loops |
|------------------------------------------|--------------------------------------------------------------------------------------------------|
| 命令功能 | RSA模块异步加密、解密可靠性测试<br> |
| 命令参数 | idx：控制器号<br><br>key_bits：加密位，定义参数如下<br>0：192 Key bits<br>1：256 Key bits<br>2：512 Key bits<br>3：1024 Key bits<br>4：2048 Key bits<br>5：3072 Key bits<br>6：4096 Key bits<br><br>padding_type：填充方式，定义参数如下<br>0：不填充<br>1：PKCS1填充<br>2：PKCS1 OAEP填充<br>3：SSLV23填充<br>4：X931填充<br>5：PSS填充<br><br>hash_type：hash类型，<br>0: MD5<br/>1: SHA1<BR>2: SHA224<BR>3: SHA256<BR>4： SHA384<br/>5:  SHA512<br/><br/>rand_data:是否每次产生循环随机产生数据<br/><br/>encrypt_times:加密次数<br/><br/>decrypt_times:解密次数<br/><br/>length_of_data:数据长度<br/><br/>test_loops:循环次数 |
| 示例 | 发送命令AT+RSA_ENCRYPT_DECRYPT_ASYNC_STABILITY=0,3,1,3,1,10,10,128,100到A板,  测试完成后， A板对应串口会打印性能数据， 打印OK；否则测试失败 |

### AT+RSA_SIGN_VERIFY_SYNC_STABILITY
| 命令格式 | AT+RSA_SIGN_VERIFY_SYNC_STABILITY=idx,key_bits,padding_type,hash_type,rand_data,sign_times,verify_times,length_of_data,test_loops |
|------------------------------------------|--------------------------------------------------------------------------------------------------|
| 命令功能 | RSA模块同步签名、验签可靠性测试<br> |
| 命令参数 | idx：控制器号<br><br>key_bits：加密位，定义参数如下<br>0：192 Key bits<br>1：256 Key bits<br>2：512 Key bits<br>3：1024 Key bits<br>4：2048 Key bits<br>5：3072 Key bits<br>6：4096 Key bits<br><br>padding_type：填充方式，定义参数如下<br>0：不填充<br>1：PKCS1填充<br>2：PKCS1 OAEP填充<br>3：SSLV23填充<br>4：X931填充<br>5：PSS填充<br><br>hash_type：hash类型，<br>0: MD5<br/>1: SHA1<BR>2: SHA224<BR>3: SHA256<BR>4： SHA384<br/>5:  SHA512<br/><br/>rand_data:是否每次产生循环随机产生数据<br/><br/>sign_times:签名次数<br/><br/>verify_times:验签次数<br/><br/>length_of_data:数据长度<br/><br/>test_loops:循环次数 |
| 示例 | 发送命令AT+RSA_SIGN_VERIFY_SYNC_STABILITY=0,3,1,3,1,10,10,128,100到A板,  测试完成后， A板对应串口会打印性能数据， 打印OK；否则测试失败 |

### AT+RSA_SIGN_VERIFY_ASYNC_STABILITY
| 命令格式 | AT+RSA_SIGN_VERIFY_ASYNC_STABILITY=idx,key_bits,padding_type,hash_type,rand_data,sign_times,verify_times,length_of_data,test_loops |
|------------------------------------------|--------------------------------------------------------------------------------------------------|
| 命令功能 | RSA模块异步签名、验签可靠性测试<br> |
| 命令参数 | idx：控制器号<br><br>key_bits：加密位，定义参数如下<br>0：192 Key bits<br>1：256 Key bits<br>2：512 Key bits<br>3：1024 Key bits<br>4：2048 Key bits<br>5：3072 Key bits<br>6：4096 Key bits<br><br>padding_type：填充方式，定义参数如下<br>0：不填充<br>1：PKCS1填充<br>2：PKCS1 OAEP填充<br>3：SSLV23填充<br>4：X931填充<br>5：PSS填充<br><br>hash_type：hash类型，<br>0: MD5<br/>1: SHA1<BR>2: SHA224<BR>3: SHA256<BR>4： SHA384<br/>5:  SHA512<br/><br/>rand_data:是否每次产生循环随机产生数据<br/><br/>sign_times:签名次数<br/><br/>verify_times:验签次数<br/><br/>length_of_data:数据长度<br/><br/>test_loops:循环次数 |
| 示例 | 发送命令AT+RSA_SIGN_VERIFY_ASYNC_STABILITY=0,3,1,3,1,10,10,128,100到A板,  测试完成后， A板对应串口会打印性能数据， 打印OK；否则测试失败 |