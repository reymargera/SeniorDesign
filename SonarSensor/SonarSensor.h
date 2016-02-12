/*
	Library for controlling sonar sensor on the smart cart
	Copyright (c) Team Captain Crunch 2016
*/

#ifndef SonarSensor_h
#define SonarSensor_h

class SonarSensor {
	public:
		SonarSensor();
		float readDistance(int sensor);
		float readSensor1();
		float readSensor2();
		float readSensor3();
		float* readAll();
		bool aboveThreshold(int threshold);
		float readAverage();
		float triangulate(int SensorA, int SensorB);
		void printDistance(int sensor, float distance);
};

#endif