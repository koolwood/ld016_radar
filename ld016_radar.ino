#include "ld016.h"
#include "ble.h"
#include <EEPROM.h>

int debug = 0,btupdate = 0;
int ledPin = 8;
int distance = 2,light_time = 1,photo = 100,motion_delta = 100,radar_on = 1;


void setup(){
  Serial.begin(115200);
  Serial1.begin(115200,SERIAL_8N1,9,7);
  delay(3000);
  Serial.println("Begin !");
  Serial.println("Begin ok!");
  EEPROM.begin(2048);
  load_up_eeprom(false,false);

  blsetup();

  pinMode(ledPin, OUTPUT);
  pinMode(6, INPUT_PULLUP);
//  get_ldo16_setup();
  setup_ld016();
  get_ldo16_setup();
 }

void loop()
{
//  int val = Sensor.readPresenceDetection();
static bool val = false;
  digitalWrite(ledPin,digitalRead(6));
  val = !val;
  delay(500);
  process_input();
}

void get_ldo16_setup(){
  Serial.printf("\n\rdistance = %d\n\r",get_distance());
  Serial.printf("get_lighting_time = %d\n\r",get_lighting_time());
  Serial.printf("get_photosensitivity = %d\n\r",get_photosensitivity());
  Serial.printf("get_motion_detection_delta = %d\n\r",get_motion_detection_delta());
  // Serial.printf("get_micro_motion_detection_sensing_distance = %d\n\r",get_micro_motion_detection_sensing_distance());
  Serial.printf("get_radar_on_off = %d\n\r",get_radar_on_off());
  Serial.printf("get_save_radar_status = %d\n\r\n\r",get_save_radar_status());
}

void setup_ld016(){
  set_distance(distance);
  set_lighting_time(light_time);
  set_photosensitivity(photo);
  set_motion_detection_delta(motion_delta);
  // set_micro_motion_detection_sensing_distance(15);
  // set_light_on_off(0);
  // delay(1000);
  set_light_on_off(radar_on);
  delay(1000);
//  set_PWM_Duty_Cycle(500);//50%
  set_radar_on_off(0);
  Serial.printf("get_radar_on_off = %d\n\r",get_radar_on_off());
  set_radar_on_off(1);
  Serial.printf("get_radar_on_off = %d\n\r",get_radar_on_off());
  // set_save_radar(1);
  // Serial.printf("get_save_radar_status = %d\n\r",get_save_radar_status());
  // set_save_radar(0);
  // Serial.printf("get_save_radar_status = %d\n\r",get_save_radar_status());
}
void load_up_eeprom(bool clear_data,bool update){
  int eeprom_initialized = 0;
  EEPROM.get(0,eeprom_initialized);
  if(eeprom_initialized != 1 || clear_data){
    distance = 2;
    light_time = 1;
    photo = 100;
    motion_delta = 100;
    radar_on = 1;
  }
  if(eeprom_initialized != 1 || update || clear_data){
    EEPROM.put(0,1);
    EEPROM.put(4,distance);
    EEPROM.put(8,light_time);
    EEPROM.put(12,photo);
    EEPROM.put(16,motion_delta);
    EEPROM.put(20,radar_on);
  }else{
    EEPROM.get(4,distance);
    EEPROM.get(8,light_time);
    EEPROM.get(12,photo);
    EEPROM.get(16,motion_delta);
    EEPROM.get(20,radar_on);
  }
  EEPROM.commit();
}

void process_input() {
  String mess;
  if (rxdata != 0) {
    if (rxdata == 'h') mess = "connected\r\nc=clear eeprom, d=distance, l=light time\r\np=photo, m=motion delta, w=update eeprom\r";
    else if (rxdata == 'c'){ mess = "clearing data\r\n";load_up_eeprom(true,false);}//reset eeprom
    else if (rxdata == 'D'){distance++;if(distance>15)distance=15; mess = String("increased distance to ") + String(distance) +String("\r\n");}
    else if (rxdata == 'd'){distance--;if(distance<1)distance=1; mess = String("decreased distance to ") + String(distance) +String("\r\n");}
    else if (rxdata == 'L'){light_time++;if(light_time>10)light_time=10; mess = String("increased light_time to ") + String(light_time) +String("\r\n");}
    else if (rxdata == 'l'){light_time--;if(light_time<1)light_time=1; mess = String("decreased light_time to ") + String(light_time) +String("\r\n");}
    else if (rxdata == 'P'){photo+=100;if(photo>2000)photo=2000; mess = String("increased photo to ") + String(photo) +String("\r\n");}
    else if (rxdata == 'p'){photo-=100;if(photo<100)photo=100; mess = String("decreased photo to ") + String(photo) +String("\r\n");}
    else if (rxdata == 'M'){motion_delta+=100;if(motion_delta>2000)motion_delta=2000; mess = String("increased motion_delta to ") + String(motion_delta) +String("\r\n");}
    else if (rxdata == 'm'){motion_delta-=100;if(motion_delta<100)motion_delta=100; mess = String("decreased motion_delta to ") + String(motion_delta) +String("\r\n");}
    else if (rxdata == 'w'){ mess = "update eeprom\r\n";load_up_eeprom(false,true);}
    else mess = "invalid data send 'h' for help\n";
    setup_ld016();
    get_ldo16_setup();
 
    rxdata = 0;
    blloop(mess);
  }
}