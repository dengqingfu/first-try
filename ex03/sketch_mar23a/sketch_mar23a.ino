// 作业第8条：用millis()实现SOS摩尔斯电码闪烁
// 硬件对应：红色LED → GPIO13
// 非阻塞设计，完全使用millis()控制时序，无delay()
// SOS标准：3短闪(点) → 3长闪(划) → 3短闪(点) → 长停顿

// 1. 硬件与时间参数定义
const int ledPin = 13;                  // LED对应引脚
#define DOT_TIME 200                    // 摩尔斯码基础单位时间(ms)，短闪时长，可修改调整速度
#define DASH_TIME (3 * DOT_TIME)        // 长闪时长 = 3倍基础单位
#define INNER_SPACE DOT_TIME            // 同一字符内，闪与闪之间的间隔
#define LETTER_SPACE (3 * DOT_TIME)     // 字符(S和O)之间的间隔
#define END_SPACE (7 * DOT_TIME)        // 完整SOS结束后的长停顿

// 2. 状态机与时间记录变量
unsigned long previousMillis = 0;       // 上一次状态切换的时间
int currentState = 0;                   // 当前状态编号，0~17对应完整SOS的18个步骤
int ledState = LOW;                      // LED当前状态

void setup() {
  pinMode(ledPin, OUTPUT);              // 初始化LED引脚为输出模式
  digitalWrite(ledPin, ledState);       // 初始状态熄灭
  Serial.begin(115200);                 // 初始化串口调试
  Serial.println("SOS信号程序已启动，循环播放中...");
}

void loop() {
  unsigned long currentMillis = millis();  // 获取当前系统运行时间
  unsigned long stateDuration = 0;         // 当前状态的持续时长

  // 状态机：根据当前状态，设置LED状态、当前状态的持续时长
  switch(currentState) {
    // ---------------------- 第一个S字符（3个短闪） ----------------------
    case 0: // S第1个短闪：亮
      ledState = HIGH;
      stateDuration = DOT_TIME;
      Serial.println("S: 第1个短闪");
      break;
    case 1: // 短闪间隔：灭
      ledState = LOW;
      stateDuration = INNER_SPACE;
      break;
    case 2: // S第2个短闪：亮
      ledState = HIGH;
      stateDuration = DOT_TIME;
      Serial.println("S: 第2个短闪");
      break;
    case 3: // 短闪间隔：灭
      ledState = LOW;
      stateDuration = INNER_SPACE;
      break;
    case 4: // S第3个短闪：亮
      ledState = HIGH;
      stateDuration = DOT_TIME;
      Serial.println("S: 第3个短闪");
      break;
    case 5: // S和O之间的字符间隔：灭
      ledState = LOW;
      stateDuration = LETTER_SPACE;
      break;

    // ---------------------- O字符（3个长闪） ----------------------
    case 6: // O第1个长闪：亮
      ledState = HIGH;
      stateDuration = DASH_TIME;
      Serial.println("O: 第1个长闪");
      break;
    case 7: // 长闪间隔：灭
      ledState = LOW;
      stateDuration = INNER_SPACE;
      break;
    case 8: // O第2个长闪：亮
      ledState = HIGH;
      stateDuration = DASH_TIME;
      Serial.println("O: 第2个长闪");
      break;
    case 9: // 长闪间隔：灭
      ledState = LOW;
      stateDuration = INNER_SPACE;
      break;
    case 10: // O第3个长闪：亮
      ledState = HIGH;
      stateDuration = DASH_TIME;
      Serial.println("O: 第3个长闪");
      break;
    case 11: // O和S之间的字符间隔：灭
      ledState = LOW;
      stateDuration = LETTER_SPACE;
      break;

    // ---------------------- 第二个S字符（3个短闪） ----------------------
    case 12: // S第1个短闪：亮
      ledState = HIGH;
      stateDuration = DOT_TIME;
      Serial.println("S: 第1个短闪");
      break;
    case 13: // 短闪间隔：灭
      ledState = LOW;
      stateDuration = INNER_SPACE;
      break;
    case 14: // S第2个短闪：亮
      ledState = HIGH;
      stateDuration = DOT_TIME;
      Serial.println("S: 第2个短闪");
      break;
    case 15: // 短闪间隔：灭
      ledState = LOW;
      stateDuration = INNER_SPACE;
      break;
    case 16: // S第3个短闪：亮
      ledState = HIGH;
      stateDuration = DOT_TIME;
      Serial.println("S: 第3个短闪");
      break;
    case 17: // 完整SOS结束后的长停顿：灭
      ledState = LOW;
      stateDuration = END_SPACE;
      Serial.println("--- SOS循环结束，长停顿 ---");
      break;
  }

  // 核心：判断当前状态是否达到持续时长，达到则切换到下一个状态
  if (currentMillis - previousMillis >= stateDuration) {
    previousMillis = currentMillis;       // 更新状态切换时间
    digitalWrite(ledPin, ledState);       // 更新LED状态
    currentState++;                        // 切换到下一个状态
    if (currentState > 17) {              // 所有状态走完，回到开头循环
      currentState = 0;
    }
  }

  // 这里可以添加其他任务代码，不会被SOS闪烁阻塞
}