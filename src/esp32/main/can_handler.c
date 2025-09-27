#include "can_handler.h"

#define bitRead(value, bit) (((value) >> (bit)) & 0x01) 
#define TAG "CAN_TASK"

Gear gearState = P;
bool engineOn = false;
bool pBrakeOn = true;
bool BrakeOn = false; 

void can_init()
{
    // Khởi tạo TWAI
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_33, GPIO_NUM_32, TWAI_MODE_NORMAL);
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
    twai_driver_install(&g_config, &t_config, &f_config);
    twai_start();
}

void can_handler()
{
    twai_message_t rx_msg;
    while (1) {
        if (twai_receive(&rx_msg, pdMS_TO_TICKS(1000)) == ESP_OK) {
            // In ra terminal
            ESP_LOGI(TAG, "ID: 0x%" PRIX32 ", Len: %d, Data:", rx_msg.identifier, rx_msg.data_length_code);           
            for (int i = 0; i < rx_msg.data_length_code; i++) {
                printf("%02X ", rx_msg.data[i]);
            }
            printf("\n");

            // Cập nhật nội dung hiển thị OLED
            // snprintf(oled_msg, sizeof(oled_msg), "ID:%X Len:%d D:%02X %02X", 
            //     rx_msg.identifier, rx_msg.data_length_code, rx_msg.data[0], rx_msg.data[1]);
        }
        else{
            printf ("RECEIVE CAN FAILED!!! \n");
        }

        // ====== 1. CVT_191 — Điều khiển cần số (ID: 0x191) ======
        if (rx_msg.identifier == 0x191) {
            bool bit0 = bitRead(rx_msg.data[0], 0); // P
            bool bit1 = bitRead(rx_msg.data[0], 1); // R
            bool bit2 = bitRead(rx_msg.data[0], 2); // N
            bool bit3 = bitRead(rx_msg.data[0], 3); // D
            bool bit4 = bitRead(rx_msg.data[0], 4); // S

            if (bit0 || bit1 || bit2 || bit3 || bit4) {          
                if (bit0) {
                    gearState = P;
                }
                else if (bit1) {
                    gearState = R;
                }
                else if (bit2) {
                    gearState = N;
                }
                else if (bit3) {
                    gearState = D;
                }
                else if (bit4) {
                    gearState = S;
                }
            }
        }

        // ====== 2. ENG_17C — Trạng thái động cơ và phanh chân ======
        if (rx_msg.identifier == 0x17C) {
            // 1. Đọc 2 byte từ bit 16–31 → data[2] và data[3]
            uint16_t rawSpeed = ((uint16_t)rx_msg.data[2] << 8) | rx_msg.data[3];
            // 2. Cập nhật engineOn theo ngưỡng 400 rpm
            if (rawSpeed >= 400) {   //**************
                engineOn = true;
            } else {
                engineOn = false;
            }
        }

        // ====== 3. METER_1A6 — Phanh tay ======
        if (rx_msg.identifier == 0x1A6){
            // Đọc bit số 2 trong message (bit thứ 2 trong tổng 64 bit, tức là byte 0, bit 2)
            pBrakeOn = (rx_msg.data[0] >> 2) & 0x01;

        }

        //====== 4.VSA_1A4 - Áp suất bàn đạp phanh =====   
        if (rx_msg.identifier == 0x1A4) {
            // Tái tạo giá trị áp suất phanh từ data[0] (bit 0–3) và data[1] (bit 8–15)
            uint16_t rawPressure =  ((rx_msg.data[0] & 0x0F)<<8) |rx_msg.data[1] ;
            float brakePressure = rawPressure *  23.96 - 2443.92;
            // Kiểm tra ngưỡng để xác định có đang đạp phanh hay không
            if (brakePressure > 100) {
                BrakeOn = 1;
            } else {
                BrakeOn = 0;
            }
        }

#ifdef Monitor_Debugging
        //Print all the information to monitor
        //1. About Gear
        switch (gearState)
        {
            case P:
                printf("Gear: P (Parking) \n");
                break;
            case R:
                printf("Gear: R (Reverse) \n");
                break;
           case N:
                printf("Gear: N (Neutral) \n");
                break;
           case D:
                printf("Gear: D (Drive) \n");
                break;
           case S:
                printf("Gear: S (Sport) \n");
                break;
            default:
                break;
        }
        
        //2. About engine
        printf (engineOn ? "Engine is ON \n" : "Engine is OFF \n");

        //3. About Parking Brake
        if (pBrakeOn) {
            printf("Phanh tay ON \n");
        } else {
            printf("Phanh tay OFF \n");
        }

        //4. About Pedal Brake
        printf (BrakeOn ? "Phan chan ON \n" : "Phan chan OFF \n");
#endif
    
    }
}

Gear getGear(){
    return gearState;
}
bool getEngineOn(){
    return engineOn;
}
bool getpBrakeOn(){
    return pBrakeOn;
}
bool getBrakeOn(){
    return BrakeOn;
}
