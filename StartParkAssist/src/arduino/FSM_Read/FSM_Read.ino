#include <U8glib.h>
#include <SPI.h>
#include <mcp2515.h>
#include <Wire.h>

// OLED
U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NONE);

// CAN
struct can_frame canMsg;
struct can_frame hmiMsg;
MCP2515 mcp2515(10);  // CS = D10

// Timing
unsigned long lastDisplayTime = 0;
const unsigned long DISPLAY_TIMEOUT = 3000; // ms

// FSM State
enum CarState {
  START_UP,
  ENGAGE_P_BRAKE_S,
  RELEASE_P_BRAKE,
  PULL_OVER_SAFELY,
  ROLL_AWAY,
  ENGAGE_P_BRAKE_P,
  SHIFT_TO_P,
  PARK_CORRECTLY,
  DRIVING
};

// FSM Events
enum CarEvent {
  NONE,
  SHIFTING_TO_P,
  NOT_SHIFTING_TO_P,
  PARK_BRAKE_ON,
  PARK_BRAKE_OFF,
  BRAKE_PEDAL_ON,
  BRAKE_PEDAL_OFF,
  ENGINE_ON,
  ENGINE_OFF
};
CarState currentState = PARK_CORRECTLY;
CarEvent currentEvent = NONE;

// Init value
int gearState = 0; // 0=P, 1=R, 2=N, 3=D, 4=S
bool engineOn = false;
int engineSpeed = 0;
bool pedalBrake = false;
bool parkBrake = true;
bool gotCVT = false;
bool gotENG = false;

// OLED init messages
String displayMessage1 = "";
String displayMessage2 = "";
String displayMessage3 = "";
String gearMessage = "";
String engineMessage = "Engine OFF";

// OLED display logic
void showOLED() {
  u8g.firstPage();
  do {
    u8g.setFont(u8g_font_6x10);
    u8g.drawStr(0, 12, gearMessage.c_str());
    u8g.drawStr(0, 24, engineMessage.c_str());
    if (displayMessage1 != "") {
      u8g.drawStr(0, 36, displayMessage1.c_str());
      u8g.drawStr(0, 48, displayMessage2.c_str());
      u8g.drawStr(0, 60, displayMessage3.c_str());
    }
  } while (u8g.nextPage());
}

// CAN input parsing
void parseCAN() {
  // Đọc toàn bộ message đang có
  while (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {

    // ======= DÒNG 1: IN DỮ LIỆU DẠNG BIT CHUẨN BIG-ENDIAN =======
    if (canMsg.can_id == 0x191) {
      Serial.print("CVT Bits:  ");
      gotCVT = true;
          for (int i = 0; i < 8; i++) {
            if (canMsg.data[i] < 0x10) Serial.print("0"); // thêm số 0 phía trước nếu < 0x10
            Serial.print(canMsg.data[i], HEX);
            Serial.print(" ");
          }
          Serial.println();  // xuống dòng
    } else if (canMsg.can_id == 0x17C) {
      Serial.print("ENG Bits:  ");
      gotENG = true;
            for (int i = 0; i < 8; i++) {
            if (canMsg.data[i] < 0x10) Serial.print("0"); // thêm số 0 phía trước nếu < 0x10
            Serial.print(canMsg.data[i], HEX);
            Serial.print(" ");
          }
          Serial.println();  // xuống dòng
    } else if (canMsg.can_id == 0x1A6){
      Serial.print("METER BITS: ");
          for (int i = 0; i < 8; i++) {
            if (canMsg.data[i] < 0x10) Serial.print("0"); // thêm số 0 phía trước nếu < 0x10
            Serial.print(canMsg.data[i], HEX);
            Serial.print(" ");
          }
          Serial.println();  // xuống dòng 
    } else if (canMsg.can_id == 0x1A4){
      Serial.print("VSA Bits: ");
            for (int i = 0; i < 8; i++) {
            if (canMsg.data[i] < 0x10) Serial.print("0"); // thêm số 0 phía trước nếu < 0x10
            Serial.print(canMsg.data[i], HEX);
            Serial.print(" ");
          }
          Serial.println();  // xuống dòng
    } 

    // ======= DÒNG 2: CẬP NHẬT TRẠNG THÁI TỪ MESSAGE =======
    if (canMsg.can_id == 0x191) {
      bool bit0 = bitRead(canMsg.data[0], 0); // P
      bool bit1 = bitRead(canMsg.data[0], 1); // R
      bool bit2 = bitRead(canMsg.data[0], 2); // N
      bool bit3 = bitRead(canMsg.data[0], 3); // D
      bool bit4 = bitRead(canMsg.data[0], 4); // S

        // Nếu có nút được nhấn
      if (bit0 || bit1 || bit2 || bit3 || bit4) {          
        if (bit0) {
          gearState = 0;
          gearMessage = "Gear: P (Parking) ";
        }
        else if (bit1) {
          gearState = 1;
          gearMessage = "Gear: R (Reverse)";
        }
        else if (bit2) {
          gearState = 2;
          gearMessage = "Gear: N (Neutral)";
        }
        else if (bit3) {
          gearState = 3;
          gearMessage = "Gear: D (Drive)";
        }
        else if (bit4) {
          gearState = 4;
          gearMessage = "Gear: S (Sport) ";
        }
        showOLED();  // Chỉ cập nhật OLED khi có sự thay đổi
      }
    // Ngược lại: không làm gì, giữ nguyên trạng thái
    }

    if (canMsg.can_id == 0x17C) {
      // 1. Đọc 2 byte từ bit 16–31 → data[2] và data[3]
      uint16_t rawSpeed = ((uint16_t)canMsg.data[2] << 8) | canMsg.data[3];
      engineSpeed = rawSpeed;

      // 2. Cập nhật engineOn theo ngưỡng 400 rpm
      if (engineSpeed >= 400) {   //**************
        engineOn = true;
      } else {
        engineOn = false;
      }

      // // 3. Đọc bit 32 (bit số 0 của data[4]) để xác định có đạp phanh hay không
      // pedalBrakeNo = (canMsg.data[4] & 0x01) ? 1 : 0;

      // 4. Cập nhật nội dung hiển thị OLED
      if (engineOn) {
        engineMessage = "Engine ON";
      } else {
        engineMessage = "Engine OFF";
      }

      showOLED();

      // 5. In thông tin ra Serial để debug
      Serial.print("Engine speed (rpm): ");
      Serial.println(engineSpeed);
      Serial.print("Engine On: ");
      Serial.println(engineOn);
      // Serial.print("Pedal Brake: Hold (1 = braking): ");
      // Serial.println(pedalBrakeNo);
    }
    if (canMsg.can_id == 0x1A6) {
      // Đọc bit số 2 trong message (bit thứ 2 trong tổng 64 bit, tức là byte 0, bit 2)
      parkBrake = (canMsg.data[0] >> 2) & 0x01;

      // Cập nhật biến parkBrake
      // In ra Serial để debug
      if (parkBrake) {
        Serial.println("Phanh tay ON");
      } else {
        Serial.println("Phanh tay OFF");
      }
    }
    if (canMsg.can_id == 0x1A4) {
      // Tái tạo giá trị áp suất phanh từ data[0] (bit 0–3) và data[1] (bit 8–15)
      uint16_t rawPressure =  ((canMsg.data[0] & 0x0F)<<8) |canMsg.data[1] ;
      float brakePressure = rawPressure *  23.96 - 2443.92;
      // Kiểm tra ngưỡng để xác định có đang đạp phanh hay không
      if (brakePressure > 100) {
        pedalBrake = 1;
      } else {
        pedalBrake = 0;
      }

      // In ra để debug
      Serial.print("raw: ");
      Serial.println(rawPressure);
      Serial.print("Pressure: ");
      Serial.print(brakePressure);
      Serial.print(" kPa | pedalBrake: ");
      Serial.println(pedalBrake);
    }
  }
}
// CAN event extraction
CarEvent getEventFromCAN() {
  static int prevGear = gearState;
  static bool prevEngineOn = engineOn;
  static bool prevParkBrake = parkBrake;
  static bool prevPedalBrake = pedalBrake;

  // Đọc message CAN, cập nhật các biến gearState, engineOn, parkBrake, pedalBrake
  parseCAN();

  if (gearState==0 && prevGear!=0) {
    prevGear = gearState;
    return SHIFTING_TO_P;
  }
  if (gearState != 0 && prevGear == 0) {
    prevGear = gearState;
    return NOT_SHIFTING_TO_P;
  }
  
  if (engineOn && !prevEngineOn) {
    prevEngineOn = engineOn;
    return ENGINE_ON;
  }
  if (!engineOn && prevEngineOn) {
    prevEngineOn = engineOn;
    return ENGINE_OFF;
  }
  if (parkBrake && !prevParkBrake) {
    prevParkBrake = parkBrake;
    return PARK_BRAKE_ON;
  }
  if (!parkBrake && prevParkBrake) {
    prevParkBrake = parkBrake;
    return PARK_BRAKE_OFF;
  }
  if (pedalBrake && !prevPedalBrake) {
    prevPedalBrake = pedalBrake;
    return BRAKE_PEDAL_ON;
  }
  if (!pedalBrake && prevPedalBrake) {
    prevPedalBrake = pedalBrake;
    return BRAKE_PEDAL_OFF;
  }

  return NONE; // Không có sự kiện đặc biệt
}

//MAIN FSM CODE
void FSM(CarEvent event){
  switch (currentState) {
    case START_UP:
      if (event == PARK_BRAKE_OFF) {
        currentState = ENGAGE_P_BRAKE_S;
      } else if (event == ENGINE_OFF) {
        currentState = PARK_CORRECTLY;
      } else if (event == NOT_SHIFTING_TO_P) {
        currentState = RELEASE_P_BRAKE;
      }
      break;
    case ENGAGE_P_BRAKE_S:
      if (event == PARK_BRAKE_ON) {
        currentState = START_UP;
      }
      break;
    case RELEASE_P_BRAKE:
      if (event == PARK_BRAKE_OFF) {
        currentState = DRIVING;
      } else if (event == SHIFTING_TO_P) {
        currentState = START_UP;
      }
      break;
    case PULL_OVER_SAFELY:
      if (event == BRAKE_PEDAL_OFF) {
        currentState = ROLL_AWAY;
      } else if (event == ENGINE_ON) {
        currentState = DRIVING;
      }
      break;
    case ROLL_AWAY:
      if (event == PARK_BRAKE_ON) {
        currentState = SHIFT_TO_P;
      } else if (event == SHIFTING_TO_P) {
        currentState = ENGAGE_P_BRAKE_P;
      } else if (event == ENGINE_ON) {
        currentState = DRIVING;
      } else if (event == BRAKE_PEDAL_ON) {
        currentState = PULL_OVER_SAFELY;
      }
      break;
    case ENGAGE_P_BRAKE_P:
      if (event == PARK_BRAKE_ON) {
        currentState = PARK_CORRECTLY;
      } else if (event == NOT_SHIFTING_TO_P) {
        currentState = ROLL_AWAY;
      }
      break;
    case SHIFT_TO_P:
      if (event == SHIFTING_TO_P) {
        currentState = PARK_CORRECTLY;
      } else if (event == PARK_BRAKE_OFF) {
        currentState = ROLL_AWAY;
      }
      break;
    case PARK_CORRECTLY:
      if (event == PARK_BRAKE_OFF) {
        currentState = ENGAGE_P_BRAKE_P;
      } else if (event == NOT_SHIFTING_TO_P) {
        currentState = SHIFT_TO_P;
      } else if (event == ENGINE_ON) {
        currentState = START_UP;
      }
      break;
    case DRIVING:
      if (event == ENGINE_OFF) {
        currentState = ROLL_AWAY;
      } else if (event == PARK_BRAKE_ON) {
        currentState = RELEASE_P_BRAKE;
      }
      break;
    // default:
    //   // Do nothing yet
  }
}

// FSM output
void updateOutput() {
  switch (currentState) {
    case START_UP:
      displayMessage1 = "Safety";
      displayMessage2 = "";
      displayMessage3 = "";
      break;
    case ENGAGE_P_BRAKE_S:
      displayMessage1 = ">>Engage Parking";
      displayMessage2 = "Brake";
      displayMessage3 = "";
      break;
    case RELEASE_P_BRAKE:
      displayMessage1 = ">>Release Parking";
      displayMessage2 = "Brake before";
      displayMessage3 = "driving";
      break;
    case DRIVING:
      displayMessage1 = "Driving";
      displayMessage2 = "";
      displayMessage3 = "";
      break;
    case PULL_OVER_SAFELY:
      displayMessage1 = "Pull over";
      displayMessage2 = "Safely";
      displayMessage3 = "";
      break;
    case ROLL_AWAY:
      displayMessage1 = "!!Roll away";
      displayMessage2 = ">>Shift to P";
      displayMessage3 = "or hold brake";
      break;
    case ENGAGE_P_BRAKE_P:
      displayMessage1 = ">>Engage Parking";
      displayMessage2 = "For properly";
      displayMessage3 = "parking";
     break;
    case SHIFT_TO_P:
      displayMessage1 = ">>Shift to P";
      displayMessage2 = "For properly";
      displayMessage3 = "parking";
      break;
    case PARK_CORRECTLY:
      displayMessage1 = "Parked";
      displayMessage2 = "Correctly";
      displayMessage3 = "";
      break;
    default:
      displayMessage1 = "";
      displayMessage2 = "";
      displayMessage3 = "";
      break;
  }
  showOLED();     // Update display
}

// CAN warning send
void sendWarning() {
  hmiMsg.can_id  = 0x1234 | CAN_EFF_FLAG;
  hmiMsg.can_dlc = 8;
  hmiMsg.data[0] = 0x01;
  mcp2515.sendMessage(&hmiMsg);
}

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);
  mcp2515.setNormalMode();
  u8g.firstPage(); do {} while (u8g.nextPage());
  Serial.println("CAN Receiver Ready...");
}

void loop() {
  //parseCAN();
  gotCVT = false;
  gotENG = false;
  currentEvent = getEventFromCAN();
  FSM(currentEvent);
  if (millis() - lastDisplayTime > DISPLAY_TIMEOUT) {
    updateOutput();
    lastDisplayTime = millis(); // Cập nhật thời gian hiển thị cuối cùng               
  }
  // if (currentState != START_UP 
  //   && currentState != DRIVING
  //   && currentState != PARK_CORRECTLY) {
  //     ;
  // }
}