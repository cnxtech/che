//
// Created by Kry·L on 2018/11/1.
//
#include <iostream>
#include "GPIOlib.h"
#include <opencv2/opencv.hpp>

using namespace std;
using namespace GPIO;
using namespace cv;

const bool DEBUG = true;
const int RUSH_SPEED = 20;
//const int MAX_SPEED = 50;


float Kp = 10, Ki = 0.5, Kd = 0;
float error = 0, P = 0, I = 0, D = 0, PID_value = 0;
float previous_error = 0, previous_I = 0;

void calc_pid() {
    P = error;
    I = I + error;
    D = error - previous_error;
    PID_value = (Kp * P) + (Ki * I) + (Kd * D);
    previous_error = error;
}

int main() {
    int MAX_SPEED;
    float THRESHOLD;
    float RUSH_THRESHOLD;
    int state = -1;
    bool end = false;

    cout << "max_speed rush_threshold"<<endl;
    cin >> MAX_SPEED >> RUSH_THRESHOLD;

    VideoCapture capture(0);
    if (!capture.isOpened()) {
        cerr << "Failed to open camera!";
        return 1;
    }
    int dWidth = capture.get(CV_CAP_PROP_FRAME_WIDTH);
    int dHeight = capture.get(CV_CAP_PROP_FRAME_HEIGHT);
    if (DEBUG)
        cout << "Frame Size: " << dWidth << "x" << dHeight << endl;

    init();
    turnTo(0);
    cout << "Che is Ready? Go!" << endl;

    Mat image, imgLeft, imgRight;
    Rect roiL(0, 0, dWidth / 2, dHeight);
    Rect roiR(dWidth / 2, 0, dWidth / 2, dHeight);
    while (true) {
        capture >> image;
        if (image.empty())
            break;

//        imwrite("camera.jpg", image);
//        imshow("Camera", image);

        cvtColor(image, image, CV_BGR2GRAY);
        threshold(image, image, 80, 255, THRESH_BINARY);
//        imshow("Processed", image);

        imgLeft = image(roiL);
        imgRight = image(roiR);
        float rateL = 1 - countNonZero(imgLeft) * 2.0 / dWidth / dHeight;
        float rateR = 1 - countNonZero(imgRight) * 2.0 / dWidth / dHeight;
        if (DEBUG)
            cout << "L=" << rateL << ", R=" << rateR << ", ts=" << capture.get(CV_CAP_PROP_POS_MSEC) << endl;

        if (rateL <= RUSH_THRESHOLD && rateR <= RUSH_THRESHOLD) {
            if (!end) {
                end = true;
                init();
                turnTo(0);
                controlLeft(FORWARD, RUSH_SPEED);
                controlRight(FORWARD, RUSH_SPEED);
                delay(1000);
                init();
                controlLeft(FORWARD, 0);
                controlRight(FORWARD, 0);
                return 0;
            }
        }

        if ((rateL - rateR) > 0.05) {
            error = 1;
        } else if ((rateL - rateR) < -0.05) {
            error = 2;
        } else {
            error = 0;
        }

        calc_pid();

        int leftSpeed = MAX_SPEED - PID_value;
        int rightSpeed = MAX_SPEED + PID_value;
        cout << PID_value << leftSpeed << rightSpeed << endl;
        leftSpeed = min(20, max(0, leftSpeed));
        rightSpeed = min(20, max(0, rightSpeed));
        init();
        controlLeft(FORWARD, leftSpeed);
        controlRight(FORWARD, rightSpeed);
        waitKey(1);
    }
    return 0;
}


