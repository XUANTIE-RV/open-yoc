# 产测语音数据接收服务器

## 创建`download`目录

```bash
mkdir download
```

## 启动服务器

python server.py [interface]:[port] [/path/to/save]

```python
python server.py 192.168.1.102:8090 ./download
or
python server.py 0.0.0.0:8090 ./download
```
