#include <WiFi.h>
#include <WebServer.h>

// -------------------------- 配置参数 --------------------------
const char* ssid     = "将军的人王一衡";       // 替换为你的WiFi
const char* password = "11111111";       // 替换为你的WiFi密码

// 引脚定义
const int ledPin = 2;          // 板载LED引脚（部分ESP32为LED_BUILTIN）
const int touchPin = T0;       // 触摸引脚T0对应GPIO4

// 系统状态全局变量
bool isArmed = false;          // 布防状态：false=撤防，true=布防
bool isAlarm = false;          // 报警状态：false=正常，true=报警中

// Web服务器对象（监听80端口）
WebServer server(80);

// -------------------------- 网页HTML --------------------------
const char* htmlPage = R"HTML(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 安防报警器</title>
    <style>
        * {
            box-sizing: border-box;
            font-family: Arial, sans-serif;
            margin: 0;
            padding: 0;
        }
        body {
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
            min-height: 100vh;
            background-color: #f0f0f0;
        }
        .container {
            background: white;
            padding: 2rem;
            border-radius: 15px;
            box-shadow: 0 0 15px rgba(0,0,0,0.1);
            text-align: center;
            width: 90%;
            max-width: 400px;
        }
        h1 {
            color: #333;
            margin-bottom: 1.5rem;
        }
        .status {
            font-size: 1.2rem;
            margin-bottom: 2rem;
            padding: 1rem;
            border-radius: 8px;
            font-weight: bold;
        }
        .armed {
            background-color: #ff4444;
            color: white;
        }
        .disarmed {
            background-color: #00C851;
            color: white;
        }
        .alarm {
            background-color: #ff0000;
            color: white;
            animation: flash 0.5s infinite;
        }
        @keyframes flash {
            0% { opacity: 1; }
            50% { opacity: 0.5; }
            100% { opacity: 1; }
        }
        .btn-group {
            display: flex;
            gap: 1rem;
            justify-content: center;
        }
        button {
            padding: 1rem 2rem;
            font-size: 1.1rem;
            border: none;
            border-radius: 8px;
            cursor: pointer;
            transition: all 0.3s;
            font-weight: bold;
        }
        .arm-btn {
            background-color: #ff4444;
            color: white;
        }
        .disarm-btn {
            background-color: #00C851;
            color: white;
        }
        button:hover {
            transform: translateY(-2px);
            box-shadow: 0 5px 15px rgba(0,0,0,0.2);
        }
        button:active {
            transform: translateY(0);
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>ESP32 安防系统</h1>
        <div class="status %STATUS%">%STATUS_TEXT%</div>
        <div class="btn-group">
            <button class="arm-btn" onclick="location.href='/arm'">布防(Arm)</button>
            <button class="disarm-btn" onclick="location.href='/disarm'">撤防(Disarm)</button>
        </div>
    </div>
</body>
</html>
)HTML";

// -------------------------- 网页处理函数 --------------------------
// 处理主页请求
void handleRoot() {
    String statusClass, statusText;
    
    if (isAlarm) {
        statusClass = "alarm";
        statusText = "🚨 报警中！🚨";
    } else if (isArmed) {
        statusClass = "armed";
        statusText = "🔴 已布防";
    } else {
        statusClass = "disarmed";
        statusText = "🟢 已撤防";
    }

    String html = htmlPage;
    html.replace("%STATUS%", statusClass);
    html.replace("%STATUS_TEXT%", statusText);
    server.send(200, "text/html", html);
}

// 处理布防请求
void handleArm() {
    isArmed = true;
    isAlarm = false;  // 布防时重置报警状态
    digitalWrite(ledPin, LOW);  // 确保LED熄灭
    server.sendHeader("Location", "/");
    server.send(303);
}

// 处理撤防请求
void handleDisarm() {
    isArmed = false;
    isAlarm = false;  // 撤防时关闭报警
    digitalWrite(ledPin, LOW);  // 熄灭LED
    server.sendHeader("Location", "/");
    server.send(303);
}

// -------------------------- 初始化 --------------------------
void setup() {
    Serial.begin(115200);
    
    // 初始化LED引脚
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);  // 初始熄灭

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
    server.on("/arm", handleArm);
    server.on("/disarm", handleDisarm);

    // 启动Web服务器
    server.begin();
    Serial.println("Web服务器已启动");
}

// -------------------------- 主循环 --------------------------
void loop() {
    server.handleClient();  // 处理Web请求

    // 1. 读取触摸传感器值（ESP32触摸引脚为电容式，触碰时值降低）
    int touchValue = touchRead(touchPin);
    const int touchThreshold = 40;  // 触摸阈值，可根据实际调整

    // 2. 状态逻辑处理
    if (isArmed && !isAlarm) {  // 已布防且未报警时，检测触摸
        if (touchValue < touchThreshold) {  // 检测到触碰
            isAlarm = true;  // 触发报警
            Serial.println("检测到入侵！触发报警！");
        }
    }

    // 3. 报警LED控制
    if (isAlarm) {
        // 高频闪烁（100ms亮/100ms灭）
        digitalWrite(ledPin, HIGH);
        delay(100);
        digitalWrite(ledPin, LOW);
        delay(100);
    } else {
        // 非报警状态，LED保持熄灭
        digitalWrite(ledPin, LOW);
    }
}