/*
ESP32 警车双闪反相渐变灯效 稳定版
完全避开LEDC编译报错，功能完全符合实验要求
硬件引脚：
- LED1：GPIO2（原LED）
- LED2：GPIO5（新增LED）
效果：两个LED平滑交替渐变，一个变亮的同时另一个同步变暗
*/

// ===================== 硬件引脚定义 =====================
#define LED_A_PIN 2   // 第一个LED引脚
#define LED_B_PIN 5   // 第二个LED引脚

// ===================== 渐变核心参数 =====================
int brightnessA = 0;         // LEDA当前亮度（0-255）
int fadeStep = 1;             // 亮度变化步长，数值越大渐变越快
const int updateInterval = 10;// 亮度更新间隔，单位ms，越小渐变越平滑
unsigned long lastUpdateTime = 0;

// ===================== 初始化 =====================
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // 初始化两个LED引脚为输出模式
  pinMode(LED_A_PIN, OUTPUT);
  pinMode(LED_B_PIN, OUTPUT);
  
  // 上电初始状态：LEDA熄灭，LEDB全亮（反相初始状态）
  analogWrite(LED_A_PIN, 0);
  analogWrite(LED_B_PIN, 255);

  Serial.println("===== 警车双闪反相渐变灯效 就绪 =====");
  Serial.println("效果：两个LED平滑交替渐变，一个变亮的同时另一个变暗");
}

// ===================== 主循环 =====================
void loop() {
  // 非阻塞式亮度更新，不占用CPU，渐变更平滑
  if (millis() - lastUpdateTime >= updateInterval) {
    lastUpdateTime = millis();

    // 更新LEDA的亮度
    brightnessA += fadeStep;

    // 亮度到顶/到底时，自动反转渐变方向
    if (brightnessA >= 255 || brightnessA <= 0) {
      fadeStep = -fadeStep;
      brightnessA = constrain(brightnessA, 0, 255); // 限制亮度范围，避免溢出
    }

    // 核心反相逻辑：LEDB亮度 = 255 - LEDA亮度
    // 完美实现：LEDA从0→255时，LEDB恰好从255→0
    int brightnessB = 255 - brightnessA;

    // 设置两个LED的亮度
    analogWrite(LED_A_PIN, brightnessA);
    analogWrite(LED_B_PIN, brightnessB);

    // 串口打印亮度值，方便调试观察
    Serial.print("LEDA亮度：");
    Serial.print(brightnessA);
    Serial.print(" | LEDB亮度：");
    Serial.println(brightnessB);
  }
}