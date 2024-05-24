# ImageProc像素格式限制
## PC
- CvtColor：支持YUV420P/NV12/YUYV422/RGB888/BGR888输入，支持YUV420P/YUV422P/RGB888/BGR888/RGB888P/BGR888P输出
- Resize：支持YUV420P/RGB888/BGR888输入
- CvtResize：支持YUV420P/NV12/YUYV422/RGB888/BGR888输入，支持YUV420P/YUV422P/RGB888/BGR888/RGB888P/BGR888P输出
- Crop：支持YUV420P输入
## Light
- CvtColor：支持NV12输入，支持RGB888P/ARGB8888等RGB系输出
- Resize：支持NV12输入
- CvtResize：支持NV12输入，支持RGB888P/ARGB8888等RGB系输出
- Crop：支持NV12输入

# ImageProc分辨率限制
## PC
- 偶数倍
## Light
- 64倍(后续可以优化为偶数倍)

# OSD像素格式限制
## PC
- Line：只支持YUV420P输入
- Rectangle：只支持YUV420P输入
## Light
- Line：只支持ARGB8888等RGB系带Alpha输入
- Rectangle：只支持ARGB8888等RGB系带Alpha输入

# OSD分辨率限制
## PC
- 偶数倍
## Light
- 64倍(后续可以优化为偶数倍)