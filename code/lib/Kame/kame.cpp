#include "kame.h"

Kame::Kame(){
    for(int i = 0; i < 8; i++){
        calibration[i] = 0;
        reverse[i] = false;
        _servo_position[i] = 90;
    }
}



void Kame::init(bool load_calibration){
    if(load_calibration){
        loadCalibration();
    }

    // Set reverse movement
    for (int i=0; i<8; i++) reverse[i] = false;

    // Reverse vertical servos (2, 3, 6, 7) - these control leg lifting/lowering
    // If vertical servos are physically reversed, uncomment the lines below
    // If only some servos need reversing, comment out the ones that work correctly
    reverse[2] = true;  // Front Left vertical (Servo 2)
    reverse[3] = true;  // Front Right vertical (Servo 3)
    reverse[6] = true;  // Back Left vertical (Servo 6)
    reverse[7] = true;  // Back Right vertical (Servo 7)
    
    // Reverse horizontal servos (0, 1, 4, 5) - these control leg rotation/spreading
    // If horizontal servos are physically reversed, uncomment the lines below
    reverse[0] = true;  // Front Left horizontal (Servo 0)
    reverse[1] = true;  // Front Right horizontal (Servo 1)
    reverse[4] = true;  // Back Left horizontal (Servo 4)
    reverse[5] = true;  // Back Right horizontal (Servo 5)

    // Init an oscillator for each servo
    pca.begin();
    pca.setPWMFreq(50);   // servo frequency

    for(int i = 0; i < 8; i++){
        oscillator[i].start();
    }

    
    arm();
}


void Kame::setCalibration(int new_calibration[8]){
    for(int i=0; i<8; i++) calibration[i] = new_calibration[i];
}

int* Kame::loadCalibration(){
    if (NVS.begin()){
        for (int i = 0; i < 8; i++){
            String key = "servo" + String(i);
            calibration[i] = (int)NVS.getInt(key, 0);
        }
        return calibration;
    }
    return nullptr;
}


void Kame::saveCalibration(int new_calibration[8]){
    if (NVS.begin()){
        for (int i = 0; i < 8; i++){
            calibration[i] = new_calibration[i];  // copy first
            String key = "servo" + String(i);
            NVS.setInt(key, (int32_t)new_calibration[i]); // save correct value
        }
    }
}


void Kame::arm(){
    _armed = true;
}

void Kame::disarm(){
    _armed = false;
    for (int i=0; i<8; i++){
        pca.setPWM(i, 0, 0);   // turn off pulse
        _servo_position[i] = -1.0;
    }
}

void Kame::reverseServo(int id){
    if (reverse[id])
        reverse[id] = 0;
    else
        reverse[id] = 1;
}


void Kame::setServo(int id, float target){
    if(!_armed) return;

    float angle = target + calibration[id];
    angle = constrain(angle, 0.0, 180.0);

    if (reverse[id]) {
        angle = 180.0 - angle;
    }

    // Map angle → PCA9685 pulse
    // Adjust these if needed
    int pulse = map(angle, 0, 180, 110, 510);

    pca.setPWM(id, 0, pulse);
    _servo_position[id] = target;
}


float Kame::getServo(int id){
    return _servo_position[id];
}

void Kame::moveServos(int ms, float target[8]) {
    if (ms>10){
        for (int i=0; i<8; i++)
            _increment[i] = (target[i] - _servo_position[i]) / (ms / 10.0);

        _final_time = millis() + ms;

        while (millis() < _final_time){
            _partial_time = millis() + 10;
            for (int i = 0; i < 8; i++) setServo(i, _servo_position[i] + _increment[i]);
            while (millis() < _partial_time); //pause
        }
    }
    else{
        for (int i=0; i<8; i++) setServo(i, target[i]);
    }
    for (int i=0; i<8; i++) _servo_position[i] = target[i];
}

void Kame::execute(float steps, int period[8], int amplitude[8], int offset[8], int phase[8]){

    for (int i=0; i<8; i++){
        oscillator[i].setPeriod(period[i]);
        oscillator[i].setAmplitude(amplitude[i]);
        oscillator[i].setPhase(phase[i]);
        oscillator[i].setOffset(offset[i]);
    }

    unsigned long global_time = millis();

    for (int i=0; i<8; i++) oscillator[i].setTime(global_time);

    _final_time = millis() + period[0]*steps;
    while (millis() < _final_time){
        for (int i=0; i<8; i++){
            setServo(i, oscillator[i].refresh());
        }
        yield();
    }
}

void Kame::turnR(float steps, int T=600){
    int x_amp = 15;
    int z_amp = 15;
    int ap = 15;
    int hi = 23;
    int period[] = {T, T, T, T, T, T, T, T};
    int amplitude[] = {x_amp,x_amp,z_amp,z_amp,x_amp,x_amp,z_amp,z_amp};
    int offset[] = {90+ap,90-ap,90-hi,90+hi,90-ap,90+ap,90+hi,90-hi};
    int phase[] = {0,180,90,90,180,0,90,90};

    execute(steps, period, amplitude, offset, phase);
}

void Kame::turnL(float steps, int T=600){
    int x_amp = 15;
    int z_amp = 15;
    int ap = 15;
    int hi = 23;
    int period[] = {T, T, T, T, T, T, T, T};
    int amplitude[] = {x_amp,x_amp,z_amp,z_amp,x_amp,x_amp,z_amp,z_amp};
    int offset[] = {90+ap,90-ap,90-hi,90+hi,90-ap,90+ap,90+hi,90-hi};
    int phase[] = {180,0,90,90,0,180,90,90};

    execute(steps, period, amplitude, offset, phase);
}

void Kame::dance(float steps, int T=600){
    int x_amp = 0;
    int z_amp = 40;
    int ap = 30;
    int hi = 20;
    int period[] = {T, T, T, T, T, T, T, T};
    int amplitude[] = {x_amp,x_amp,z_amp,z_amp,x_amp,x_amp,z_amp,z_amp};
    int offset[] = {90+ap,90-ap,90-hi,90+hi,90-ap,90+ap,90+hi,90-hi};
    int phase[] = {0,0,0,270,0,0,90,180};

    execute(steps, period, amplitude, offset, phase);
}

void Kame::frontBack(float steps, int T=600){
    int x_amp = 30;
    int z_amp = 25;
    int ap = 20;
    int hi = 30;
    int period[] = {T, T, T, T, T, T, T, T};
    int amplitude[] = {x_amp,x_amp,z_amp,z_amp,x_amp,x_amp,z_amp,z_amp};
    int offset[] = {90+ap,90-ap,90-hi,90+hi,90-ap,90+ap,90+hi,90-hi};
    int phase[] = {0,180,270,90,0,180,90,270};

    execute(steps, period, amplitude, offset, phase);
}

void Kame::run(float steps, int T=5000){
    int x_amp = 15;
    int z_amp = 15;
    int ap = 15;
    int hi = 15;
    int front_x = 6;
    int period[] = {T, T, T, T, T, T, T, T};
    int amplitude[] = {x_amp,x_amp,z_amp,z_amp,x_amp,x_amp,z_amp,z_amp};
    int offset[] = {    90+ap-front_x,
                        90-ap+front_x,
                        90-hi,
                        90+hi,
                        90-ap-front_x,
                        90+ap+front_x,
                        90+hi,
                        90-hi
                    };
    int phase[] = {0,0,90,90,180,180,90,90};

    execute(steps, period, amplitude, offset, phase);
}

void Kame::omniWalk(float steps, int T){
    int x_amp = 0;
    int z_amp = 20;
    int ap = 20;
    int hi = 10;
    int front_x = 12;
    int period[] = {T, T, T/2, T/2, T, T, T/2, T/2};
    int amplitude[] = {x_amp,x_amp,z_amp,z_amp,x_amp,x_amp,z_amp,z_amp};
    int offset[] = {    90+ap,
                        90-ap,
                        90-hi,
                        90+hi,
                        90-ap,
                        90+ap,
                        90+hi,
                        90-hi
                    };
    int  phase[] = {90, 90, 270, 90, 270, 270, 90, 270};

    for (int i=0; i<8; i++){
        oscillator[i].reset();
        oscillator[i].setPeriod(period[i]);
        oscillator[i].setAmplitude(amplitude[i]);
        oscillator[i].setPhase(phase[i]);
        oscillator[i].setOffset(offset[i]);
    }

    _final_time = millis() + period[0]*steps;
    _init_time = millis();
    bool side;
    while (millis() < _final_time){
        side = (int)((millis()-_init_time) / (period[0]/2)) % 2;
        setServo(0, oscillator[0].refresh());
        setServo(1, oscillator[1].refresh());
        setServo(4, oscillator[4].refresh());
        setServo(5, oscillator[5].refresh());

        if (side == 0){
            setServo(3, oscillator[3].refresh());
            setServo(6, oscillator[6].refresh());
        }
        else{
            setServo(2, oscillator[2].refresh());
            setServo(7, oscillator[7].refresh());
        }
        delay(1);
    }
}

void Kame::moonwalkL(float steps, int T=5000){
    int z_amp = 45;
    int period[] = {T, T, T, T, T, T, T, T};
    int amplitude[] = {0,0,z_amp,z_amp,0,0,z_amp,z_amp};
    int offset[] = {90, 90, 90, 90, 90, 90, 90, 90};
    int phase[] = {0,0,0,120,0,0,180,290};

    execute(steps, period, amplitude, offset, phase);
}

void Kame::walk(float steps, int T=5000){
    int x_amp = 15;
    int z_amp = 20;
    int ap = 20;
    int hi = 10;
    int front_x = 12;
    int period[] = {T, T, T/2, T/2, T, T, T/2, T/2};
    int amplitude[] = {x_amp,x_amp,z_amp,z_amp,x_amp,x_amp,z_amp,z_amp};
    int offset[] = {    90+ap-front_x,
                        90-ap+front_x,
                        90-hi,
                        90+hi,
                        90-ap-front_x,
                        90+ap+front_x,
                        90+hi,
                        90-hi
                    };
    int  phase[] = {90, 90, 270, 90, 270, 270, 90, 270};

    for (int i=0; i<8; i++){
        oscillator[i].reset();
        oscillator[i].setPeriod(period[i]);
        oscillator[i].setAmplitude(amplitude[i]);
        oscillator[i].setPhase(phase[i]);
        oscillator[i].setOffset(offset[i]);
    }

    _final_time = millis() + period[0]*steps;
    _init_time = millis();
    bool side;
    while (millis() < _final_time){
        side = (int)((millis()-_init_time) / (period[0]/2)) % 2;
        setServo(0, oscillator[0].refresh());
        setServo(1, oscillator[1].refresh());
        setServo(4, oscillator[4].refresh());
        setServo(5, oscillator[5].refresh());

        if (side == 0){
            setServo(3, oscillator[3].refresh());
            setServo(6, oscillator[6].refresh());
        }
        else{
            setServo(2, oscillator[2].refresh());
            setServo(7, oscillator[7].refresh());
        }
        delay(1);
    }
}

void Kame::backward(float steps, int T=5000){
    int x_amp = 15;
    int z_amp = 20;
    int ap = 20;
    int hi = 10;
    int front_x = -12;
    int period[] = {T, T, T/2, T/2, T, T, T/2, T/2};
    int amplitude[] = {x_amp,x_amp,z_amp,z_amp,x_amp,x_amp,z_amp,z_amp};
    int offset[] = {    90+ap-front_x,
                        90-ap+front_x,
                        90-hi,
                        90+hi,
                        90-ap-front_x,
                        90+ap+front_x,
                        90+hi,
                        90-hi
                    };
    int  phase[] = {270, 270, 270, 90, 90, 90, 90, 270};

    for (int i=0; i<8; i++){
        oscillator[i].reset();
        oscillator[i].setPeriod(period[i]);
        oscillator[i].setAmplitude(amplitude[i]);
        oscillator[i].setPhase(phase[i]);
        oscillator[i].setOffset(offset[i]);
    }

    _final_time = millis() + period[0]*steps;
    _init_time = millis();
    bool side;
    while (millis() < _final_time){
        side = (int)((millis()-_init_time) / (period[0]/2)) % 2;
        setServo(0, oscillator[0].refresh());
        setServo(1, oscillator[1].refresh());
        setServo(4, oscillator[4].refresh());
        setServo(5, oscillator[5].refresh());

        if (side == 0){
            setServo(3, oscillator[3].refresh());
            setServo(6, oscillator[6].refresh());
        }
        else{
            setServo(2, oscillator[2].refresh());
            setServo(7, oscillator[7].refresh());
        }
        delay(1);
    }
}

void Kame::upDown(float steps, int T=5000){
    int x_amp = 0;
    int z_amp = 35;
    int ap = 20;
    int hi = 25;
    int front_x = 0;
    int period[] = {T, T, T, T, T, T, T, T};
    int amplitude[] = {x_amp,x_amp,z_amp,z_amp,x_amp,x_amp,z_amp,z_amp};
    int offset[] = {    90+ap-front_x,
                        90-ap+front_x,
                        90-hi,
                        90+hi,
                        90-ap-front_x,
                        90+ap+front_x,
                        90+hi,
                        90-hi
                    };
    int phase[] = {0,0,90,270,180,180,270,90};

    execute(steps, period, amplitude, offset, phase);
}

void Kame::pushUp(float steps, int T=600){
    int z_amp = 40;
    int x_amp = 65;
    int hi = 30;
    int period[] = {T, T, T, T, T, T, T, T};
    int amplitude[] = {0,0,z_amp,z_amp,0,0,0,0};
    int offset[] = {90,90,90-hi,90+hi,90-x_amp,90+x_amp,90+hi,90-hi};
    int phase[] = {0,0,0,180,0,0,0,180};

    execute(steps, period, amplitude, offset, phase);
}

void Kame::hello(){
    float sentado[]={90+15,90-15,90-65,90+65,90+20,90-20,90+10,90-10};
    moveServos(150, sentado);
    delay(200);

    int z_amp = 40;
    int x_amp = 60;
    int T=350;
    int period[] = {T, T, T, T, T, T, T, T};
    int amplitude[] = {0,50,0,50,0,0,0,0};
    int offset[] = {90+15,40,90-65,90,90+20,90-20,90+10,90-10};
    int phase[] = {0,0,0,90,0,0,0,0};

    execute(4, period, amplitude, offset, phase);

    float goingUp[]={160,20,90,90,90-20,90+20,90+10,90-10};
    moveServos(500, goingUp);
    delay(200);
}

void Kame::jump(){
    float sentado[]={90+15,90-15,90-65,90+65,90+20,90-20,90+10,90-10};
    float ap = 20.0;
    float hi = 35.0;
    float salto[] = {90+ap,90-ap,90-hi,90+hi,90-ap*3,90+ap*3,90+hi,90-hi};
    moveServos(150, sentado);
    delay(200);
    moveServos(0, salto);
    delay(100);
    home();
}

void Kame::home(){
    int ap = 20;
    int hi = 35;
    int position[] = {90+ap,90-ap,90-hi,90+hi,90-ap,90+ap,90+hi,90-hi};
    for (int i=0; i<8; i++) setServo(i, position[i]);
}

void Kame::zero(){
    for (int i=0; i<8; i++) setServo(i, 90);
}
