#include <M5Stack.h>
#include <Ticker.h>
#include <WiFi.h>
#include "utility/MPU9250.h"
#include <ArduinoJson.h>

#include "myconfig.h"

volatile bool startFlg = false;
volatile bool readSensorFlg = false;

struct sensorData {
  String accX;
  String accY;
  String accZ;
  String gyroX;
  String gyroY;
  String gyroZ;
};

void getAcc(MPU9250* IMU,sensorData* pSensorData);
WiFiClient client;
//位置情報を定期的に更新するタスク
void taskAcc(void * pvParameters) {
    Ticker tickerSensor; // センサの値を読む
    Ticker tickerStartRead; // 計測開始
    
    Wire.begin(); //タスクの中でbegin
    MPU9250 IMU;
    delay(10);
    IMU.calibrateMPU9250(IMU.gyroBias, IMU.accelBias);
    delay(10);
    IMU.initMPU9250();

    
    // 20秒ごとに計測を開始
    tickerStartRead.attach_ms(20000, _upStartFlg);
    delay(1000);
    startFlg = true;
    for(;;) {
        // 16ミリ秒ごと(62.5Hz)にセンサーリード
        if(startFlg) {
          startFlg = false;
          tickerSensor.attach_ms(16, _readSensor);
          sensorData s = {"","","","","",""};
          // 2秒分取り終わったら抜ける
          int count = 0;
          while(count < 62.5 * 2) {
            if(readSensorFlg) {
              readSensorFlg = false;
              
              getAcc(&IMU, &s);
              count++;
            }
            delay(1);
          }
          tickerSensor.detach();
          // 送信
          DynamicJsonBuffer jsonBuffer(JSON_OBJECT_SIZE(7));
          JsonObject &root = jsonBuffer.createObject();
          root["type"] = "acc";
          root["x"] = s.accX;
          root["y"] = s.accY;
          root["z"] = s.accZ;
          root["gx"] = s.gyroX;
          root["gy"] = s.gyroX;
          root["gz"] = s.gyroX;

          if (!client.connect(thingboardHost, httpPort)) {
            Serial.println("Connection failed");
            continue;
          }
          
          String request = String("POST ") + String(thingboardURL);
          request += " HTTP/1.1\r\n";
          request += "Host: " + String(thingboardHost) + "\r\n";
          request += "User-Agent: ESP32\r\n";
          request += "Content-Type:application/json\r\n";
          request += "Content-Length:" + String(root.measureLength()) + "\r\n";
          request += "Connection: close\r\n\r\n";
          client.print(request);
          root.printTo(client);
          
          unsigned long timeout = millis();
          while (client.available() == 0) {
              if (millis() - timeout > 5000) {
                  Serial.println(">>> Client Timeout !");
                  client.stop();
                  break;
              }
              delay(1);
          }
          String response = "";
          while (client.available()) {
            response += client.readString();
            yield ();
          }
          Serial.println(response);
        }
        delay(100);
    }
}


void setup() {
    M5.begin();;
    dacWrite(25, 0); // Speaker OFF(スピーカーノイズ対策)
    
    WiFi.begin(ssid, password); // Wi-Fi APに接続
    while (WiFi.status() != WL_CONNECTED) {  //  Wi-Fi AP接続待ち
        delay(100);
    }


    while(!M5.BtnA.wasReleased()) {
        M5.lcd.setCursor(0,0);
        M5.lcd.print("push button A");
        M5.update();
        delay(200);
    }
    M5.lcd.clear(BLACK);

    
        // 加速度用タスクのセット
    xTaskCreatePinnedToCore(
                    taskAcc,     /* Function to implement the task */
                    "taskAcc",   /* Name of the task */
                    8192,      /* Stack size in words */
                    NULL,      /* Task input parameter */
                    1,         /* Priority of the task */
                    NULL,      /* Task handle. */
                    0);        /* Core where the task should run */
}


void loop() {
    // メインの処理    
    delay(1000);
}


void getAcc(MPU9250* IMU, sensorData* pSensorData) {
  // センサから各種情報を読み取り
  IMU->readAccelData(IMU->accelCount);
  IMU->getAres();
  IMU->readGyroData(IMU->gyroCount);
  IMU->getGres();

  // 取得した加速度に解像度をかけて、バイアス値を引く
  IMU->ay = (float)IMU->accelCount[1]*IMU->aRes - IMU->accelBias[1];
  IMU->az = (float)IMU->accelCount[2]*IMU->aRes - IMU->accelBias[2];
  IMU->ax = (float)IMU->accelCount[0]*IMU->aRes - IMU->accelBias[0];

  // 取得したジャイロに解像度をかける
  IMU->gx = (float)IMU->gyroCount[0]*IMU->gRes;
  IMU->gy = (float)IMU->gyroCount[1]*IMU->gRes;
  IMU->gz = (float)IMU->gyroCount[2]*IMU->gRes;

  // 四元数を更新する際に必ず呼び出し
  IMU->updateTime();

  // 加速度・ジャイロを与えられた構造体に代入
  pSensorData -> accX += String((int)(1000*IMU->ax)) + ",";
  pSensorData -> accY += String((int)(1000*IMU->ay)) + ",";
  pSensorData -> accZ += String((int)(1000*IMU->az)) + ",";
  pSensorData -> gyroX += String((int)(IMU->gx)) + ",";
  pSensorData -> gyroY += String((int)(IMU->gy)) + ",";
  pSensorData -> gyroZ += String((int)(IMU->gz)) + ",";

}

//ハンドラ－１（センサーを読んでバッファリング）
void _readSensor() {
    readSensorFlg = true;
}

//ハンドラ－２（SD保存のフラグを管理）
void _upStartFlg() {
    startFlg= true;
}
