// ex05 多档位触摸调速呼吸灯 - ESP32 3.3.7 新版API兼容
const int touchPin = T0;       // 触摸引脚（T0对应GPIO4）
const int ledPin = 2;          // LED输出引脚（GPIO2）

// 触摸抗干扰配置（彻底解决误触发）
int touchBaseline = 0;         // 无触摸时的基线值
const int touchSensitivity = 20; // 触摸灵敏度：数值越大，越难触发（抗干扰越强）
const unsigned long debounceDelay = 50; // 软件防抖延时（ms）

// 呼吸灯档位配置
int speedGear = 1;            // 当前档位：1(慢) → 2(中) → 3(快) → 1循环
const int gearDelay[] = {30, 15, 5}; // 3个档位对应的延时（ms），数值越小呼吸越快
int ledValue = 0;              // PWM占空比（0-255）
int fadeStep = 5;              // 呼吸渐变步长

// PWM通用配置（新版API）
#define PWM_FREQ 5000
#define PWM_RESOLUTION 8       // 8位分辨率，占空比范围0-255

// 触摸状态变量
bool lastTouchState = false;
unsigned long lastDebounceTime = 0;

void setup() {
  // 新版ESP32 PWM API：直接绑定引脚、频率、分辨率
  ledcAttach(ledPin, PWM_FREQ, PWM_RESOLUTION);
  
  Serial.begin(115200);
  
  // 开机自动校准触摸基线（关键：开机时请勿触摸！）
  Serial.println("=== ex05 多档位呼吸灯 ===");
  Serial.println("正在校准触摸基线...请勿触摸引线！");
  for(int i = 0; i < 10; i++) {
    touchBaseline += touchRead(touchPin);
    delay(10);
  }
  touchBaseline /= 10;
  Serial.print("校准完成，基线值：");
  Serial.println(touchBaseline);
  Serial.println("就绪！当前档位：1（缓慢呼吸）");
}

void loop() {
  // ===================== 1. 触摸检测与档位切换（边缘检测+防抖） =====================
  int touchValue = touchRead(touchPin);
  // 动态阈值判断：只有触摸值比基线低足够多，才判定为有效触摸
  bool currentTouchState = (touchValue < (touchBaseline - touchSensitivity));

  // 软件防抖：状态变化后延时确认，避免手抖误触发
  if (currentTouchState != lastTouchState) {
    lastDebounceTime = millis();
  }

  // 防抖超时后，仅在「按下瞬间」触发档位切换（边缘检测）
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (currentTouchState == true && lastTouchState == false) {
      // 循环切换档位：1 → 2 → 3 → 1
      speedGear = speedGear % 3 + 1;
      
      // 串口打印档位信息（调试用）
      Serial.print("触摸触发！当前档位：");
      Serial.print(speedGear);
      if(speedGear == 1) Serial.println("（缓慢呼吸）");
      else if(speedGear == 2) Serial.println("（中等呼吸）");
      else Serial.println("（急促呼吸）");
      
      // 触摸后重新校准基线，适应环境变化，避免长期漂移
      touchBaseline = touchRead(touchPin);
    }
  }
  lastTouchState = currentTouchState;

  // ===================== 2. 呼吸灯PWM渐变（根据档位调速） =====================
  ledValue += fadeStep;
  // 到达亮度边界（0/255）后反向渐变，实现呼吸效果
  if (ledValue <= 0 || ledValue >= 255) {
    fadeStep = -fadeStep;
  }
  
  // 新版API：直接写引脚号，无需通道号
  ledcWrite(ledPin, ledValue);

  // 根据当前档位调整呼吸速度（修改delay时长，完全符合作业要求）
  delay(gearDelay[speedGear - 1]);
}