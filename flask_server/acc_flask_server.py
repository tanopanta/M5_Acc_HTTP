from flask import Flask, render_template, request, redirect, url_for, Response, g
import json
from datetime import datetime

import random


# 自身の名称を app という名前でインスタンス化する
app = Flask(__name__)


# ここからウェブアプリケーション用のルーティングを記述
# index にアクセスしたときの処理
@app.route('/')
def index():
    # index.html をレンダリングする
    return render_template('index.html')



# /post にアクセスしたときの処理
@app.route('/acc_post', methods=['POST'])
def acc_post():
    js = request.json 
    
    # 例) {"x": "1,2,3,4,5,", "y": "0,0,0,0,0,", ...}
    
    accX = [int(i) for i in js["x"].split(",")[:-1]]
    accY = [int(i) for i in js["y"].split(",")[:-1]]
    accZ = [int(i) for i in js["z"].split(",")[:-1]]
    gyroX = [int(i) for i in js["gx"].split(",")[:-1]]
    gyroY = [int(i) for i in js["gy"].split(",")[:-1]]
    gyroZ = [int(i) for i in js["gz"].split(",")[:-1]]
    print(gyroX)
    
    state_list = ["stay", "walk", "up", "down", "other"]
    state = random.choice(state_list)
    return Response(json.dumps({"state": state}))

if __name__ == '__main__':
    app.debug = True # デバッグモード有効化
    app.run(host='0.0.0.0') # どこからでもアクセス可能に
