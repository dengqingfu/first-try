#include <WiFi.h>
#include <WebServer.h>

// -------------------------- 配置参数 --------------------------
const char* ssid     = "将军的人王一衡";       // 替换为你的WiFi
const char* password = "11111111";       // 替换为你的WiFi密码

// 引脚定义
const int touchPin = T0;       // 触摸引脚T0对应GPIO4

// Web服务器对象（监听80端口）
WebServer server(80);

// -------------------------- 网页HTML（含AJAX） --------------------------
const char* htmlPage = R"HTML(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 实时传感器仪表盘</title>
    <style>
        * {
            box-sizing: border-box;
            font-family: 'Arial', sans-serif;
            margin: 0;
            padding: 0;
        }
        body {
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
            min-height: 100vh;
            background-color: #1a1a2e;
            color: white;
        }
        .container {
            text-align: center;
            padding: 2rem;
            border-radius: 20px;
            background-color: #16213e;
            box-shadow: 0 0 30px rgba(0,0,0,0.5);
            width: 90%;
            max-width: 500px;
        }
        h1 {
            font-size: 2rem;
            margin-bottom: 2rem;
            color: #00f5d4;
        }
        .sensor-value {
            font-size: 5rem;
            font-weight: bold;
            margin: 2rem 0;
            padding: 1.5rem;
            border-radius: 15px;
            background-color: #0f3460;
            transition: all 0.3s ease;
        }
        .near {
            color: #ff2a6d;
            transform: scale(1.05);
        }
        .far {
            color: #00f5d4;
        }
        .desc {
            font-size: 1.2rem;
            margin-top: 1rem;
            color: #a2a2a2;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>ESP32 触摸传感器仪表盘</h1>
        <div class="sensor-value far" id="sensorValue">--</div>
        <p class="desc">手靠近引脚数值变小，离开恢复</p >
    </div>

    <script>
        // AJAX 定时拉取传感器数据
        function updateSensorValue() {
            fetch('/sensor')
                .then(response => response.text())
                .then(data => {
                    const value = parseInt(data);
                    const element = document.getElementById('sensorValue');
                    element.textContent = value;
                    
                    // 根据数值动态改变样式（手靠近变红放大）
                    if (value < 40) {
                        element.classList.add('near');
                        element.classList.remove('far');
                    } else {
                        element.classList.add('far');
                        element.classList.remove('near');
                    }
                })
                .catch(error => {
                    console.error('数据拉取失败:', error);
                    document.getElementById('sensorValue').textContent = 'ERROR';
                });
        }

        // 页面加载时立即更新，之后每100ms更新一次（10Hz，实时流畅）
        updateSensorValue();
        setInterval(updateSensorValue, 100);
    </script>
</body>
</html>
)HTML";

// -------------------------- 路由处理函数 --------------------------
// 处理主页请求，返回HTML页面
void handleRoot() {
    server.send(200, "text/html", htmlPage);
}

// 处理传感器数据请求，返回触摸值
void handleSensor() {
    int touchValue = touchRead(touchPin);
    server.send(200, "text/plain", String(touchValue));
}

// -------------------------- 初始化 --------------------------
void setup() {
    Serial.begin(115200);
    
    // 连接WiFi
    WiFi.begin(ssid, password);
    Serial.print("连接WiFi中");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi连接成功！");
    Serial.print("IP地址: ");
    Serial.println(WiFi.localIP());

    // 注册Web路由
    server.on("/", handleRoot);
    server.on("/sensor", handleSensor);

    // 启动Web服务器
    server.begin();
    Serial.println("Web服务器已启动");
}

// -------------------------- 主循环 --------------------------
void loop() {
    server.handleClient();  // 处理Web请求
    // 无需额外操作，触摸值在handleSensor中实时读取
}