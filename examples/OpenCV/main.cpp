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
//const int TURN_SPEED = 10;
//const int TURN_ANGLE = 5;
//const float THRESHOLD = 0.03;
const int STRAIGHT = 0;
const int TURN_LEFT = 1;
const int TURN_RIGHT = 2;
const int END = 3;

int main() {
    int MAX_SPEED, TURN_SPEED, TURN_ANGLE, BACK_SPEED, BACK_TIME;
    float THRESHOLD;
    float RUSH_THRESHOLD;
    int state = -1;

    cout << "max_speed turn_speed turn_angle threshold back_speed back_time rush_threshold" << endl;
    cin >> MAX_SPEED >> TURN_SPEED >> TURN_ANGLE >> THRESHOLD >> BACK_SPEED >> BACK_TIME >> RUSH_THRESHOLD;

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
        float whiteRateL = 1 - countNonZero(imgLeft) * 2.0 / dWidth / dHeight;
        float whiteRateR = 1 - countNonZero(imgRight) * 2.0 / dWidth / dHeight;
        if (DEBUG)
            cout << "L=" << whiteRateL << ", R=" << whiteRateR << endl;

        if (whiteRateL <= THRESHOLD && whiteRateR <= THRESHOLD && state != STRAIGHT &&
            (whiteRateL > RUSH_THRESHOLD || whiteRateR > RUSH_THRESHOLD)) {
            turnTo(0);
            controlLeft(FORWARD, MAX_SPEED);
            controlRight(FORWARD, MAX_SPEED);
            state = STRAIGHT;
        } else if (whiteRateL >= THRESHOLD && whiteRateR < THRESHOLD && state != TURN_LEFT) {
            init();
            turnTo(0);
            controlLeft(BACKWARD, BACK_SPEED);
            controlRight(BACKWARD, BACK_SPEED);
            delay(BACK_TIME);
            turnTo(TURN_ANGLE);
            controlLeft(FORWARD, TURN_SPEED);
            state = TURN_LEFT;
        } else if (whiteRateR >= THRESHOLD && whiteRateL < THRESHOLD && state != TURN_RIGHT) {
            init();
            turnTo(0);
            controlLeft(BACKWARD, BACK_SPEED);
            controlRight(BACKWARD, BACK_SPEED);
            delay(BACK_TIME);
            turnTo(-TURN_ANGLE);
            controlRight(FORWARD, TURN_SPEED);
            state = TURN_RIGHT;
        } else if (whiteRateL <= RUSH_THRESHOLD && whiteRateR <= RUSH_THRESHOLD) {
            if (state != END) {
                state == END;
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
        waitKey(1);
    }
    return 0;
}


