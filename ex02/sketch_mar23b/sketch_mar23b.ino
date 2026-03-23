// 作业第6条：用millis()实现1Hz稳定LED闪烁
// 硬件对应：红色LED → GPIO13
// 非阻塞设计，不使用delay()，符合作业要求

// 1. 定义硬件与参数
const int ledPin = 13;          // 你的红色LED对应引脚
const long interval = 500;      // 状态翻转间隔(ms)，亮/灭各500ms，总周期1000ms=1Hz

// 2. 时间记录变量（必须用unsigned long，避免溢出错误）
unsigned long previousMillis = 0;  // 上一次LED状态翻转的时间
int ledState = LOW;                 // LED当前的亮灭状态

void setup() {
  pinMode(ledPin, OUTPUT);       // 初始化LED引脚为输出模式
  Serial.begin(115200);          // 初始化串口，方便调试观察
  Serial.println("1Hz LED闪烁程序已启动");
}

void loop() {
  // 核心：获取当前系统运行的毫秒数
  unsigned long currentMillis = millis();

  // 判断：当前时间 - 上一次翻转时间 ≥ 设定的间隔，就执行状态翻转
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;  // 更新上一次翻转的时间
    ledState = !ledState;            // 翻转LED状态（LOW变HIGH，HIGH变LOW）
    digitalWrite(ledPin, ledState);  // 把状态写入引脚，控制LED亮灭

    // 串口输出调试信息，方便你观察状态变化
    Serial.print("当前时间：");
    Serial.print(currentMillis);
    Serial.print("ms | LED状态：");
    Serial.println(ledState == HIGH ? "点亮" : "熄灭");
  }

  // 这里可以写其他任务代码，不会被LED闪烁阻塞
}