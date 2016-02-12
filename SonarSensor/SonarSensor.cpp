#include "Arduino.h"
#include "SonarSensor.h"


#define TRIG    	9   //Trigger Pin, Controls All Sensors
#define SENSOR1 	8   //Echo Pin For Sensor 1 (Left)
#define SENSOR2 	7   //Echo Pin For Sensor 2 (Center)
#define SENSOR3		6   //Echo Pin For Sensor 3 (Right)
#define DISTANCE12  30  //Distnace From Sensor 1 To Sensor 2
#define DISTANCE13	60	//Distance From Sensor 1 To Sensor 3
#define DISTANCE23  30	//Distance From Sensor 2 To Sensor 3
#define SENSORCOUNT 3   //Number of sensors being used

SonarSensor::SonarSensor(){
	pinMode(TRIG, OUTPUT);
	pinMode(SENSOR1, INPUT);
	pinMode(SENSOR2, INPUT);
	pinMode(SENSOR3, INPUT);
}

float SonarSensor::readDistance(int sensor) {
	delay(70);
	float duration, distance;
	digitalWrite(TRIG, LOW);
	delayMicroseconds(2);
	digitalWrite(TRIG, HIGH);
	delayMicroseconds(10);
	digitalWrite(TRIG, LOW);
	duration = pulseIn(sensor,HIGH);
	distance = (duration / 2) / 29.1;
	printDistance(sensor, distance);
	return distance;
}

//Returns Distance In cm For Sensor 1 (Left)
float SonarSensor::readSensor1() {
	return readDistance(SENSOR1);	
}

//Returns Distance In cm For Sensor 2 (Center)
float SonarSensor::readSensor2() {
	return readDistance(SENSOR2);
}

//Returns Distance In cm For Sensor 3 (Right)
float SonarSensor::readSensor3() {
	return readDistance(SENSOR3);
}

//Returns Array Containing Distance Read From Each Sensor From Left To Right
float* SonarSensor::readAll() {
	static float distances[3];
	distances[0] = readSensor1();
	distances[1] = readSensor2();
	distances[2] = readSensor3();
	return distances;
}

// Returns true if all sensors are above the given threshold
bool SonarSensor::aboveThreshold(int threshold) {
	float* readings[3] = {readAll()};
	for(int i = 0; i < SENSORCOUNT; i++) {
		if(*readings[i] < threshold)
			return false;
	}
	return true;
}

//Returns The Average Distance Read From Each Sensor
float SonarSensor::readAverage() {
	float distances = 0;
	distances += readSensor1();
	distances += readSensor2();
	distances += readSensor3();
	distances /= 3;
	return distances;
}

//Attempts To Use Triangulation To Pinpoint An Object's Distance
//Uses Herons Formula
//Area = sqrt(s * (s-a) * (s-b) * (s-c) )
//Area = .5 * b * h
float SonarSensor::triangulate(int SensorA, int SensorB) {
	float DistanceA = readDistance(SensorA);
	float DistanceB = readDistance(SensorB);
	float DistanceC;

	if(SensorA == SENSOR1) {
		if(SensorB == SENSOR2)
			DistanceC = DISTANCE12;
		else
			DistanceC = DISTANCE13;
	}
	else
		DistanceC = DISTANCE23;

	float s = (DistanceA + DistanceB + DistanceC) / 2;
	float area = sqrt(s * (s - DistanceA) * (s - DistanceB) * (s - DistanceC));
	float height = (2 * area) / DistanceC;

	return height;
}

void SonarSensor::printDistance(int sensor, float distance) {
	String prefix;
	String length = String(distance, 2);
	String units = " cm";
	switch(sensor) {
		case SENSOR1:
			prefix = "Sensor 1 Distance: ";
			break;
		case SENSOR2:
			prefix = "Sensor 2 Distance: ";
			break;
		case SENSOR3:
			prefix = "Sensor 3 Distance: ";
			break;
		default:
			prefix = "-------";
			break;
	}

	Serial.print(prefix);
	Serial.print(length);
	Serial.println(units);
}