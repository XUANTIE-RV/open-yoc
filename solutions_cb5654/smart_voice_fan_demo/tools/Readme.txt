功能：将.wav文件转换成方案中firmware目录中的音频固件

WAV文件格式要求：
PCM/ADPCM，16比特位，16K/8K采样率，单通道

系统要求：windows 64bit或linux 64bit

1.创建一个wav_file文件夹，里面放置的文件名与语音控制序号一一对应
  示例中存放了如下文件：
  wav_file/1.wav
  wav_file/2.wav

2.执行名

命令示例：
voicemaker wav_file 2
wav_file  资源文件的存放目录
数字2     文件个数 

Window环境准备好资源，按以下命令执行
..\smart_voice_fan_demo\tools\windows>voiceMaker.exe
please put the wav files in wav_file folder, and input the last wav number:
2
输出wakeup.bin文件

Linux环境准备好资源，按以下命令执行
../smart_voice_fan_demo/tools/Linux$ ./voiceMaker 
please put the wav files in wav_file folder, and input the last wav number:
2
输出wakeup.bin文件

