#include "ld016.h"
#include "ble.h"
#include "ut_sensor.h"
#include <EEPROM.h>

int debug = 1,btupdate = 0;
int ledPin = 8;
int redledPin = 2;
int yellowledPin = 3;
int greenledPin = 4;

int distance = 2,light_time = 1,photo = 100,motion_delta = 100,radar_on = 1,ut_distance = 1;
int yellow_limit = 7,green_limit = 10,red_limit = 3,motion_time_limit = 5 ;
bool stream_data = false,test_radar = false;


void setup(){
  Serial.begin(115200);
  Serial1.begin(115200,SERIAL_8N1,20,21);
  delay(3000);
  Serial.println("Begin !");
  Serial.println("Begin ok!");
  EEPROM.begin(2048);
  load_up_eeprom(false,false);

  blsetup();
  utsetup();
  pinMode(ledPin, OUTPUT);
  pinMode(redledPin, OUTPUT);
  pinMode(yellowledPin, OUTPUT);
  pinMode(greenledPin, OUTPUT);
  pinMode(6, INPUT_PULLUP);
  delay(5000);
  setup_ld016();
  delay(5000);
  get_ldo16_setup();
  delay(5000);
  get_ldo16_setup();
  digitalWrite(greenledPin,1); 
  digitalWrite(yellowledPin,1); 
  digitalWrite(redledPin,1); 
  delay(1000);
  digitalWrite(greenledPin,0); 
  digitalWrite(yellowledPin,0); 
  digitalWrite(redledPin,0); 
  }

void loop(){
  static int motion = 0,cycle_started = 0;
  int ut_distance ;
    ut_distance = avg5();//get_ut_distance();
  if(test_radar){
    if(digitalRead(6))digitalWrite(greenledPin,1); 
    else digitalWrite(greenledPin,0);
    blloop(String("radar m=" + String(digitalRead(6)) + String(",p=") + String(get_distance()) + String(",l=") + String(get_lighting_time()) + String(",s=") + String(get_photosensitivity()) + String(",d=") + String(get_motion_detection_delta()) +  String("\r\n")));
    motion = 0; 
    cycle_started = 0;
  }else{
    if(digitalRead(6)){
      motion += 1;
      if(motion > motion_time_limit)motion = motion_time_limit;
      if(motion > motion_time_limit / 2)cycle_started = 1;
    }else{
      motion -= 1;
      if(motion < 0)motion = 0;
      if(motion == 0)cycle_started = 0;
    }
    digitalWrite(ledPin,!digitalRead(6));
//    if(ut_distance == 0)ut_distance = 144;
    if(cycle_started ){
      if(ut_distance > yellow_limit){
        digitalWrite(greenledPin,0); 
        digitalWrite(yellowledPin,1); 
        digitalWrite(redledPin,1); 
      }
      else if(ut_distance < yellow_limit && ut_distance > red_limit){
        digitalWrite(greenledPin,1); 
        digitalWrite(yellowledPin,0); 
        digitalWrite(redledPin,1); 
      }
      else if(ut_distance <  red_limit){
        digitalWrite(greenledPin,1); 
        digitalWrite(yellowledPin,1); 
        digitalWrite(redledPin,0); 
      }
    }else{
      digitalWrite(greenledPin,1); 
      digitalWrite(yellowledPin,1); 
      digitalWrite(redledPin,1); 
    }
  }
  delay(500);
  Serial.printf("distance = %d,motion = %d,cycle_started = %d\n\r",ut_distance,motion,cycle_started);
  if(stream_data) blloop(String("motion = ") + String(motion) + String(", cycle_started =") + String(cycle_started) + String(", ut distance =") + String(ut_distance) +  String("\r\n"));
  process_input();
}

int avg5(){
  int avg =0;
  for(int x = 0;x < 5;x++){
    avg += get_ut_distance();
    delay(60);
  }
  return avg / 5;
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
  // set_light_on_off(radar_on);
  //delay(1000);
//  set_PWM_Duty_Cycle(500);//50%
  // set_radar_on_off(0);
  // Serial.printf("get_radar_on_off = %d\n\r",get_radar_on_off());
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
    yellow_limit = 50;
    red_limit = 20;
    motion_time_limit = 5;
  }
  if(eeprom_initialized != 1 || update || clear_data){
    EEPROM.put(0,1);
    EEPROM.put(4,distance);
    EEPROM.put(8,light_time);
    EEPROM.put(12,photo);
    EEPROM.put(16,motion_delta);
    EEPROM.put(20,radar_on);
    EEPROM.put(24,yellow_limit);
    EEPROM.put(28,red_limit);
    EEPROM.put(32,motion_time_limit);
  }else{
    EEPROM.get(4,distance);
    EEPROM.get(8,light_time);
    EEPROM.get(12,photo);
    EEPROM.get(16,motion_delta);
    EEPROM.get(20,radar_on);
    EEPROM.get(24,yellow_limit);
    EEPROM.get(28,red_limit);
    EEPROM.get(32,motion_time_limit);
  }
  EEPROM.commit();
  print_setup();
}

void print_setup(){
  String mess;
  mess = String("radar distance = ") + String(distance) +String("\r\n") + String("radar light = ") + String(light_time) +String("\r\n");
  blloop(mess);
  mess = String("radar photo = ") + String(photo) +String("\r\n") + String("radar motion = ") + String(motion_delta) +String("\r\n");
  blloop(mess);
  mess = String("yellow limit = ") + String(yellow_limit) +String("\r\n") + String("red limit= ") + String(red_limit) +String("\r\n");
  blloop(mess);
  mess = String("motion time limit = ") + String(motion_time_limit) +String("\r\n ");
  blloop(mess);

}

void process_input() {
  String mess;
  if (rxdata != 0) {
    if (rxdata == 'h')mess = "connected\r\nCapital letter increases value\r\nlower case decreases value\r\n\r\nc=clear eeprom, d=distance, l=light time\r\np=photo, m=motion delta, w=update eeprom\r\ny=yellow limit, r=red limit, t=motion time\r\ns=get setup, 1=stream, S=get setup\r\n";
    else if (rxdata == 'c'){ mess = "clearing data\r\n";load_up_eeprom(true,false);}//reset eeprom
    else if (rxdata == 'D'){distance++;if(distance>15)distance=15; mess = String("increased distance to ") + String(distance) +String("\r\n");}
    else if (rxdata == 'd'){distance--;if(distance<1)distance=1; mess = String("decreased distance to ") + String(distance) +String("\r\n");}
    else if (rxdata == 'L'){light_time++;if(light_time>10)light_time=10; mess = String("increased light_time to ") + String(light_time) +String("\r\n");}
    else if (rxdata == 'l'){light_time--;if(light_time<1)light_time=1; mess = String("decreased light_time to ") + String(light_time) +String("\r\n");}
    else if (rxdata == 'P'){photo+=100;if(photo>2000)photo=2000; mess = String("increased photo to ") + String(photo) +String("\r\n");}
    else if (rxdata == 'p'){photo-=100;if(photo<100)photo=100; mess = String("decreased photo to ") + String(photo) +String("\r\n");}
    else if (rxdata == 'M'){motion_delta+=100;if(motion_delta>2000)motion_delta=2000; mess = String("increased motion_delta to ") + String(motion_delta) +String("\r\n");}
    else if (rxdata == 'm'){motion_delta-=100;if(motion_delta<100)motion_delta=100; mess = String("decreased motion_delta to ") + String(motion_delta) +String("\r\n");}
    else if (rxdata == 'Y'){yellow_limit+=5;if(yellow_limit>80)yellow_limit=80; mess = String("increased yellow_limit to ") + String(yellow_limit) +String("\r\n");}
    else if (rxdata == 'y'){yellow_limit-=5;if(yellow_limit<red_limit + 5)yellow_limit=red_limit + 5; mess = String("decreased yellow_limit to ") + String(yellow_limit) +String("\r\n");}
    else if (rxdata == 'R'){red_limit+=5;if(red_limit>yellow_limit - 5)red_limit=yellow_limit - 5; mess = String("increased red_limit to ") + String(red_limit) +String("\r\n");}
    else if (rxdata == 'r'){red_limit-=5;if(red_limit<5)red_limit=5; mess = String("decreased red_limit to ") + String(red_limit) +String("\r\n");}
    else if (rxdata == 'T'){motion_time_limit+=2;if(motion_time_limit>60)motion_time_limit=60; mess = String("increased motion_time_limit to ") + String(motion_time_limit) +String("\r\n");}
    else if (rxdata == 't'){motion_time_limit-=2;if(motion_time_limit<4)motion_time_limit=4; mess = String("decreased motion_time_limit to ") + String(motion_time_limit) +String("\r\n");}
    else if (rxdata == 'w'){mess = "update eeprom\r\n";load_up_eeprom(false,true);}
    else if (rxdata == 's')load_up_eeprom(false,false); //send setup
    else if (rxdata == 'S')print_setup(); //send setup
    else if (rxdata == '1'){stream_data = !stream_data;if(stream_data) mess = String("stream on\r\n");else  mess = String("stream off\r\n");}
    else if (rxdata == '2'){test_radar = !test_radar;if(test_radar) mess = String("test_radar on\r\n");else  mess = String("test_radar off\r\n");}
    else mess = "invalid data send 'h' for help\n";
    setup_ld016();
    get_ldo16_setup();
 
    rxdata = 0;
    blloop(mess);
  }
}