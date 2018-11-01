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

int main()
{
    int MAX_SPEED, TURN_SPEED, TURN_ANGLE, BACK_SPEED, BACK_TIME;
    float THRESHOLD;
    float RUSH_THRESHOLD;
    int state = -1;
    bool end = false;

    cout<< "max_speed turn_speed turn_angle threshold back_speed back_time";
    cin >> MAX_SPEED >> TURN_SPEED >> TURN_ANGLE >> THRESHOLD>> BACK_SPEED >> BACK_TIME >> RUSH_THRESHOLD;

    VideoCapture capture(0);
    if (!capture.isOpened())
    {
        cerr << "Failed to open camera!";
        return 1;
    }
    //capture.set(CV_CAP_PROP_FRAME_WIDTH, 960);
    //capture.set(CV_CAP_PROP_FRAME_HEIGHT, 720);
    // capture.set(CV_CAP_PROP_FPS, 25);
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
    while (true)
    {
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

        if (rateL <= THRESHOLD && rateR <= THRESHOLD && state != 0 && (rateL > RUSH_THRESHOLD || rateR > RUSH_THRESHOLD)) 
        {
            turnTo(0);
            controlLeft(FORWARD, MAX_SPEED);
            controlRight(FORWARD, MAX_SPEED);
            state = 0;
        }
        else if (rateL >= THRESHOLD && rateR < THRESHOLD && state != 1)
        {
            init();
            turnTo(0);
            controlLeft(BACKWARD, BACK_SPEED);
            controlRight(BACKWARD, BACK_SPEED);
            delay(BACK_TIME);
            turnTo(TURN_ANGLE);
            //controlLeft(FORWARD, TURN_SPEED);
            controlLeft(FORWARD, TURN_SPEED);
            state = 1;
        }
        else if (rateR >= THRESHOLD && rateL < THRESHOLD && state != 2)
        {
            init();
            turnTo(0);
            controlLeft(BACKWARD, BACK_SPEED);
            controlRight(BACKWARD, BACK_SPEED);
            delay(BACK_TIME);
            turnTo(-TURN_ANGLE);
            controlRight(FORWARD, TURN_SPEED);
            //controlRight(FORWARD, TURN_SPEED);
            state = 2;
        }
        else if (rateL <= RUSH_THRESHOLD && rateR <= RUSH_THRESHOLD)
        {
            if (!end)
            {
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
        waitKey(1);
    }
    return 0;
}


