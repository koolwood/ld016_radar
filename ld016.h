// High-performance 5.8GHz radar
// module HLK-LD016-5G

extern int debug;

int ld016_command(int size,char group,char command,int payload, int rsize,int * rpayload){
	char com[size] = {0x58,group,command};
	char result[rsize];
  int sum = 0;

	if(size == 6)com[3] = payload & 0xff;
	else if(size == 7){
		com[3] = payload & 0xff;
		com[4] = payload >> 8;
	}
	for(int i = 0;i < size-2;i++)sum += com[i];
	com[size - 2] = sum & 0xff;
	com[size -1] = sum >> 8;
	if(debug){
		Serial.printf("command = ");
		for(int i = 0;i < size;i++)Serial.printf("%x ",com[i]);
		Serial.println("");
	}
	for(int i = 0;i < size;i++)Serial1.print(com[i]);

	Serial1.readBytes(result,rsize);
  if(strlen(result) == 0){
		Serial.println("error no data");
		return -1;
	}else{
		if(debug){
			Serial.printf("result = ");
			for(int i = 0;i < rsize;i++)Serial.printf("%x ",result[i]);
			Serial.println("");
			Serial.println("");
		}
		if(rpayload != NULL){
			if(rsize == 6)*rpayload = (int)result[3];
			else *rpayload = (int)result[3] + ((int)result[4] << 8);
		}
	} 
	debug = 0;
	return 1;
}

// 3.1.1 Setting the motion sensing distance
// Parameters: para1: distance level 0 - 15
// e.g., set distance level to 15 
// HEX is: 58 02 01 0F 6A 00 
// Resp: 59 02 01 00 5C 00 (success), Other: fail 
int set_distance(int distance){
	return ld016_command(6,2,1,distance,6,NULL);
}

// 3.1.2 Get motion sensing distance
// Send command format (HEX): 58 03 00 5B 00
// Reply frame: 59 03 01 0F 6C 00 // eg get distance level: 15
int get_distance(){
	int distance;
	if(ld016_command(5,3,0,0,6,&distance) > 0)
		return distance;
	else return -1;
}

// 3.1.3 Setting the lighting time
// Send command format (HEX): 58 04 02 xx xx xx xx
// e.g., set time to 1s 
// HEX is: 58 04 02 01 00 5F 00 
// Resp: 59 04 01 00 5E 00 (success), Other: fail 
int set_lighting_time(int time){
	return ld016_command(7,4,2,time,6,NULL);
}

// 3.1.4 Get the lighting time
// Send command format (HEX): 58 05 00 5D 00
// Reply frame: 59 05 02 01 00 61 00 // eg get lot: 0x0001 (1s)
int get_lighting_time(){
	int result;
	if(ld016_command(5,5,0,1,7,&result) > 0)
		return result;
	else return -1;
}

// 3.1.5 Setting the photosensitivity threshold
// Send command format (HEX): 58 06 02 xx xx xx xx
// e.g., set lux to 1000 
// HEX is: 58 06 02 E8 03 4B 01 
// Resp: 59 06 01 00 60 00 (success), Other: fail 
int set_photosensitivity(int value){
	return ld016_command(7,6,2,value,6,NULL);
}

// 3.1.6 Obtaining the photosensitivity threshold
// Send command format (HEX): 58 07 00 5F 00
// Reply frame: 59 07 02 E8 03 4D 01 // e.g. get lux: 0x03E8
int get_photosensitivity(){
	int result;
	if(ld016_command(5,7,0,1,7,&result) > 0)
		return result;
	else return -1;
}

// 3.1.7 Set the motion detection delta value
// Send command format (HEX): 58 d2 02 xx xx xx xx Reply frame: 
// e.g., set delta to 300 
// HEX is: 58 d2 02 2c 01 59 01 
// Resp: 59 D2 01 00 2C 01 
int set_motion_detection_delta(int value){
	return ld016_command(7,0xd2,2,value,6,NULL);
}

// 3.1.8 Obtaining motion detection delta value
// Send command format (HEX): 58 d3 00 2b 01
// Response frame: 59 D3 02 2C 01 5B 01 // eg get delta, return value: 300
int get_motion_detection_delta(){
	int result;
	if(ld016_command(5,0xd3,0,1,7,&result) > 0)
		return result;
	else return -1;
}

// 3.1.9 Setting the micro motion detection sensing distance
// 0xE Parameters: 8-bits, para1: distance level 0 - 32 // The smaller the value, the more sensitive the sensing.
// Send command format (HEX): 58 0E 01 xx xx xx Reply frame: 
// e.g., set distance level to 15 
// HEX is: 58 0E 01 0F 76 00 
// Resp: 59 0E 01 00 68 00 
int set_micro_motion_detection_sensing_distance(int value){
	return ld016_command(6,14,1,value,6,NULL);
}

// 3.1.10 Get the micro motion detection sensing distance
// Send command format (HEX): 58 0F 00 67 00
// Reply frame: 59 0F 01 0F 78 00 // e.g. get distance level: 15
int get_micro_motion_detection_sensing_distance(){
	int result;
	if(ld016_command(5,15,0,1,6,&result) > 0)
		return result;
	else return -1;
}

// 3.1.11 Turning the light on/off
// Send command format (HEX): 58 0A 01 01 64 00 (turn on the light)
// 58 0A 01 00 63 00 (Turn off the light)
// Reply frame: 59 0A 01 00 64 00 (success), Other: fail
int set_light_on_off(int value){//1=on,0=off
	return ld016_command(6,10,1,value,6,NULL);
}

// 3.1.12 Setting PWM Duty Cycle
// Note: Set the PWM duty cycle when turning on the light
// 0xB Parameters: para1: value of duty (low byte), para2: value of duty (high byte). Unit 0.1%
// Send command format (HEX): 58 0B 02 xx xx xx xx
// e.g., set duty to 50.0%
// HEX is: 58 0B 02 F4 01 5A 01 
// Resp: 59 0B 01 00 65 00 (success) Other: fail 
int set_PWM_Duty_Cycle(int value){
	debug = 1;
	return ld016_command(7,11,2,value,6,NULL);
}

// 3.1.13 Turn on/off radar
// 0xD1 Parameter: para1: 0x1 (turn on radar), 0x0 (turn off radar)
// Send command format (HEX): 58 D1 01 01 2B 01 (turn on radar)
// 58 D1 01 00 2A 01 (Turn off radar)
// Reply frame: 59 D1 01 00 2B 01 (success), Other: fail
int set_radar_on_off(int value){
	return ld016_command(6,0xd1,1,value,6,NULL);
}

// 3.1.14 Get radar switch status
// Send command format (HEX): 58 D0 00 28 01
// Reply frame: 59 D0 01 01 2B 01 // e.g., radar is on // e.g.
// 59 D0 00 01 2A 01  radar is off
int get_radar_on_off(){
	int result;
	if(ld016_command(5,0xd0,0,1,6,&result) > 0)
		return result;
	else return -1;
}

// 3.1.15 Save radar settings
// Mainly save the set parameters (sensing distance, lighting time and light sensitivity threshold) in the module's Flash. Restarting the
// module will load the last set parameter values.
// Instruction code:
// 0x8 Parameters: para1: 0x1 (save), 0x0 (do not save)
// Send command format (HEX): 58 08 01 01 62 00 (save) 58 08 01 00 61 00 (do not save)
// Reply frame: 59 D8 01 00 62 00 (success) Other: fail
int set_save_radar(int value){//1 save ,0 don't save
	return ld016_command(6,8,1,value,6,NULL);
}

// 3.1.16 Get radar save status
// none Send command format (HEX): 58 09 00 61 00
// Reply frame: 59 09 01 01 64 00 // e.g., save is on // 
// 59 09 01 00 63 00 e.g., save is off 
int get_save_radar_status(){
	int result;
	if(ld016_command(5,9,0,1,6,&result) > 0)
		return result;
	else return -1;
}

// 3.1.17 System Reset 
// e.g., sys_reset(0x1)
// HEX is: 58 13 01 01 6d 00
int set_sys_reset(int value){//1 save ,0 don't save
	return ld016_command(6,0x13,1,value,6,NULL);
}

