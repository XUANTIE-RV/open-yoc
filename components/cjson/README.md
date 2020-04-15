# 概述


cJSON是使用ANSI-C开发的一套超轻量级的的JSON解析器。


# 示例代码

## 解析 JSON 数据包

```c
#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"

// 被解析的JSON数据包
char text[] = "{\"timestamp\":\"2013-11-19T08:50:11\",\"value\":1}";

int main (int argc, const char * argv[])
{
    cJSON *json , *json_value , *json_timestamp;
    // 解析数据包
    json = cJSON_Parse(text);
    if (!json) {
        printf("Error before: [%s]\n",cJSON_GetErrorPtr());
    }
    else {
        // 解析开关值
        json_value = cJSON_GetObjectItem(json, "value");
        if (json_value->type == cJSON_Number) {
            // 从 valueint 中获得结果
            printf("value:%d\r\n",json_value->valueint);
        }

        // 解析时间戳
        json_timestamp = cJSON_GetObjectItem( json , "timestamp");
        if (json_timestamp->type == cJSON_String) {
            // valuestring中获得结果
            printf("%s\r\n",json_timestamp->valuestring);
        }

        // 释放内存空间
        cJSON_Delete(json);
    }

    return 0;
}
```

## 组装JSON数据包

```c
#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"

int main (int argc, const char * argv[])
{
    // 创建JSON Object
    cJSON *root = cJSON_CreateObject();

    // 加入节点（键值对），节点名称为value，节点值为123.4
    cJSON_AddNumberToObject(root, "value", 123.4);

    // 打印JSON数据包
    char *out = cJSON_Print(root);
    printf("%s\n",out);

    // 释放内存
    cJSON_Delete(root);
    free(out);

    return 0;
}
```

# 参考文档

[cJSON学习笔记](https://blog.csdn.net/xukai871105/article/details/17094113)
[使用cJSON解析JSON数据](http://hahaya.github.io/use-cJSON)