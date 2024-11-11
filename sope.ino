#include <Servo.h>

// 아두이노 핀 할당
#define PIN_IR    A0         // IR 센서 A0 핀
#define PIN_LED   9
#define PIN_SERVO 10

#define _DUTY_MIN 100       // 서보 모터 시계 방향 (0도) 위치
#define _DUTY_NEU 1500       // 서보 모터 중립 위치 (90도)
#define _DUTY_MAX 3000       // 서보 모터 반시계 방향 (180도) 위치

#define _DIST_MIN  100.0     // 최소 거리 100mm
#define _DIST_MAX  250.0     // 최대 거리 250mm

#define EMA_ALPHA 0.2        // EMA 필터 알파 값
#define LOOP_INTERVAL 50     // 루프 간격 (밀리초)

Servo myservo;
unsigned long last_loop_time = 0;   // 단위: 밀리초

float dist_prev = _DIST_MIN;
float dist_ema = _DIST_MIN;

void setup() {
  pinMode(PIN_LED, OUTPUT);
  
  myservo.attach(PIN_SERVO); 
  myservo.writeMicroseconds(_DUTY_NEU);
  
  Serial.begin(1000000);    // 1,000,000 bps
}

void loop() {
  unsigned long time_curr = millis();
  int duty;
  float a_value, dist_raw;

  // 다음 이벤트 시간까지 대기
  if (time_curr < (last_loop_time + LOOP_INTERVAL))
    return;
  last_loop_time += LOOP_INTERVAL;

  a_value = analogRead(PIN_IR);
  dist_raw = ((6762.0 / (a_value - 9.0)) - 4.0) * 10.0;

  // 거리 범위 필터와 LED 제어
  if (dist_raw >= _DIST_MIN && dist_raw <= _DIST_MAX) {
    digitalWrite(PIN_LED, HIGH);  // 거리가 10cm ~ 25cm 범위 안일 때 LED 켜기
  } else {
    digitalWrite(PIN_LED, LOW);   // 범위를 벗어나면 LED 끄기
  }

  // EMA 필터 계산
  dist_ema = EMA_ALPHA * dist_raw + (1 - EMA_ALPHA) * dist_prev;
  dist_prev = dist_ema;  // 이전 거리 업데이트

  // dist_ema를 듀티 사이클로 비례 변환
  duty = ((dist_ema - _DIST_MIN) / (_DIST_MAX - _DIST_MIN)) * (_DUTY_MAX - _DUTY_MIN) + _DUTY_MIN;
  
  myservo.writeMicroseconds(duty);

  // 디버깅을 위한 시리얼 출력
  Serial.print("_DUTY_MIN:");  Serial.print(_DUTY_MIN);
  Serial.print("_DIST_MIN:");  Serial.print(_DIST_MIN);
  Serial.print(",IR:");        Serial.print(a_value);
  Serial.print(",dist_raw:");  Serial.print(dist_raw);
  Serial.print(",ema:");       Serial.print(dist_ema);
  Serial.print(",servo:");     Serial.print(duty);
  Serial.print(",_DIST_MAX:"); Serial.print(_DIST_MAX);
  Serial.print(",_DUTY_MAX:"); Serial.print(_DUTY_MAX);
  Serial.println("");
}
