// ESP32 三色LED彩虹渐变实验
// 引脚对应：红色→GPIO13  绿色→GPIO12  黄色→GPIO14
// 兼容你之前实验使用的新版ESP32 Arduino PWM API

// 定义LED对应引脚（严格按照你的要求设置）
const int redPin = 13;    // 红色LED引脚
const int greenPin = 12;  // 绿色LED引脚
const int yellowPin = 14; // 黄色LED引脚

// PWM配置（和基础呼吸灯实验参数完全一致，无需修改）
const int freq = 5000;          // PWM频率 5000Hz
const int resolution = 8;        // 8位分辨率，占空比范围 0-255

// 渐变控制变量
float angle = 0;                 // 正弦函数角度变量
const float angleStep = 1.0;     // 角度步长，数值越小渐变越细腻

void setup() {
  Serial.begin(115200);
  
  // 绑定引脚与PWM，和你之前实验的用法完全一致，自动分配通道
  ledcAttach(redPin, freq, resolution);
  ledcAttach(greenPin, freq, resolution);
  ledcAttach(yellowPin, freq, resolution);
}

void loop() {
  // 角度转弧度（Arduino的sin函数仅支持弧度输入）
  float rad = angle * PI / 180.0;
  
  // 用正弦函数计算3路LED的占空比，相位差120度实现平滑彩虹渐变
  // 公式说明：sin输出范围-1~1 → +1后转为0~2 → 乘以127.5映射到0-255的占空比范围
  int redDuty = (sin(rad) + 1) * 127.5;
  int greenDuty = (sin(rad + 2*PI/3) + 1) * 127.5; // 相位偏移120度
  int yellowDuty = (sin(rad + 4*PI/3) + 1) * 127.5; // 相位偏移240度
  
  // 写入占空比，控制LED亮度
  ledcWrite(redPin, redDuty);
  ledcWrite(greenPin, greenDuty);
  ledcWrite(yellowPin, yellowDuty);
  
  // 串口输出调试信息，和基础实验保持一致
  Serial.print("Red: ");
  Serial.print(redDuty);
  Serial.print(" | Green: ");
  Serial.print(greenDuty);
  Serial.print(" | Yellow: ");
  Serial.println(yellowDuty);
  
  // 角度递增，控制渐变速度
  angle += angleStep;
  // 角度重置，避免数值溢出（sin函数本身支持循环，此步为可选优化）
  if(angle >= 360) {
    angle = 0;
    Serial.println("--- 一个完整渐变循环完成 ---");
  }
  
  // 延时控制渐变速度，和基础实验逻辑一致：数值越大，呼吸越慢
  delay(10);
}