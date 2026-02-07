#ifndef kame_h
#define kame_h

#include <Octosnake.h>
#include <ArduinoNvs.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>


class Kame{
public:
    Kame();
    void init(bool load_calibration=true);

    void setCalibration(int calibration[8]);
    int* loadCalibration();
    void saveCalibration(int calibration[8]);

    void arm();
    void disarm();

    void reverseServo(int id);

    void setServo(int id, float target);
    float getServo(int id);
    void moveServos(int time, float target[8]);

    void run(float steps, int period);
    void walk(float steps, int period);
    void backward(float steps, int period);
    void omniWalk(float steps, int T);
    void turnL(float steps, int period);
    void turnR(float steps, int period);
    void moonwalkL(float steps, int period);
    void dance(float steps, int period);
    void upDown(float steps, int period);
    void pushUp(float steps, int period);
    void hello();
    void jump();
    void home();
    void zero();
    void frontBack(float steps, int period);

//private:
    Adafruit_PWMServoDriver pca = Adafruit_PWMServoDriver(0x40);
    Oscillator oscillator[8];
    int calibration[8];
    bool reverse[8];
    unsigned long _init_time;
    unsigned long _final_time;
    unsigned long _partial_time;
    float _increment[8];
    float _servo_position[8];
    bool _armed = false;

    int angToUsec(float value);
    void execute(float steps, int period[8], int amplitude[8], int offset[8], int phase[8]);
};

#endif
