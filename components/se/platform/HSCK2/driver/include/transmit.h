

/*
数据交互
功能: 此接口实现发送指令数据并接收响应数据的功能。
??:
	ucInputData				输入，要发送的数据缓冲区；
	uiInputDataLength		输入，要发送的缓冲区的长度
	ucOutputData			输出，返回数据的缓存区
	puiOutputDataLength		输出，表示实际返回的数据长度；
	uiTimeout				输入超时时间
返回值:
	返回0成功，其他值见错误码。
*/
unsigned int Transmit(unsigned char *ucInputData, unsigned int uiInputDataLength, unsigned char *ucOutputData, 
												unsigned int *puiOutputDataLength, unsigned int uiTimeout);

