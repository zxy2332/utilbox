# 启动

执行 MainHttpService.exe 命令行均有默认值，可为空

## 命令行：

```
  -p, --port               Http Sever port (int [=8000]) http端口号
  -g, --graph_path_data    Graph Path Data file (string [=PathData.csv]) 
  -o, --out_file           Out file path (string [=out/out.json]) 输出的路径文件
  -d, --debug              Open debug output (bool [=1]) 
  -l, --log_level          Log Level:TRACE(0);DEBUG(1);INFO(2);WARNING(3);ERROR(4);CRITICAL(5);OFF(6); (int [=0])
```

## 接口

### localhost:8000/CalculateShortest

方式：post

body示例:

``` json
{
  "StartID": 1,
  "EndID": 24,
  "State": [
    {
      "NodeID": 23,
      "State": 1
    },
    {
      "NodeID": 13,
      "State": 0
    },
    {
      "NodeID": 6,
      "State": 0
    },
    {
      "NodeID": 11,
      "State": 1
    },
    {
      "NodeID": 30,
      "State": 1
    },
    {
      "NodeID": 4,
      "State": 1
    }
  ]
}
```

响应示例：

``` json
{
    "path": [
        "10.10993 5.79457 3.029964",
        "5.91 5.79457 3.029964",
        "3.859186 5.79457 3.029964",
        "3.859239 5.794571 -0.4179725",
        "3.859239 5.794571 -3.61343",
        "5.91 5.794571 -3.61343",
        "10.10999 5.794571 -3.61343",
        "10.10999 5.794571 -5.328853",
        "10.10999 0.7058266 -5.329227"
    ]
}
```



