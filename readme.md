# M5 Acc Send   
加速度、ジャイロを20秒に１回取得する。MQTTでThingsBoardへ送信し、そこから機械学習用サーバへHTTPで問い合わせ。   
（ThingsBordを経由しないバージョンは最初のほうのコミット↓)   
https://github.com/tanopanta/M5_Acc_Send/tree/0cd4492b22040621d339ac040d72f211fae842cb   

## 利用方法   
1. thingsboardのルールチェーンを作る
1. myconfig.h.tmpにパラメータを設定し、myconfig.hに変更する。     
1. ```flask_server/acc_flask_server.py```　　のサーバを起動   

## pubsubclient
デフォルトのパケットサイズが小さいので PubSubClient.hのMQTT_MAX_PACKET_SIZEを4096ぐらいに変更