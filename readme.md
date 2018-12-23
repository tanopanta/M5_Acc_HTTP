# M5 Acc HTTP   
加速度、ジャイロを20秒に一回HTTPで送信。   
## 利用方法   
1. myconfig.h.tmpにパラメータを設定し、myconfig.hに変更する。    
パターン１   
M5Stack -> Thingsboard -> サーバー -> Thingsboard    
パターン2   
M5Stack -> サーバー   

1. ```flask_server/acc_flask_server.py```を実行