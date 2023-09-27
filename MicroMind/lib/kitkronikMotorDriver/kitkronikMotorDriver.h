// kitkronikMotorDriver.h

#ifndef KITKRONIK_MOTOR_DRIVER_H
#define KITRONIK_MOTOR_DRIVER_H

class kitkronikMotorDriver{
    public:
    kitkronikMotorDriver(int KMDP1, int KMDP2);
    void forward(int speed);
    void reverse(int speed);
    void stop();

    private:
    int KMDP1;
    int KMDP2;
};

#endif