#include <iostream>
#include "GPIOlib.h"
#include <opencv2/opencv.hpp>

using namespace std;
using namespace GPIO;
using namespace cv;

const bool DEBUG = true;
//const int MAX_SPEED = 50;
//const int TURN_SPEED = 10;
//const int TURN_ANGLE = 5;
//const float THRESHOLD = 0.03;

int main()
{
    int MAX_SPEED, TURN_SPEED, TURN_ANGLE;
    float THRESHOLD;
    cin >> MAX_SPEED >> TURN_SPEED >> TURN_ANGLE >> THRESHOLD;
    VideoCapture capture(0);
    if (!capture.isOpened())
    {
        cerr << "Failed to open camera!";
        return 1;
    }
    capture.set(CV_CAP_PROP_FRAME_WIDTH, 640);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
    // capture.set(CV_CAP_PROP_FPS, 25);
    int dWidth = capture.get(CV_CAP_PROP_FRAME_WIDTH);
    int dHeight = capture.get(CV_CAP_PROP_FRAME_HEIGHT);
    if (DEBUG)
        cout << "Frame Size: " << dWidth << "x" << dHeight << endl;

    cout << "Start engine..." << endl;
    init();
    turnTo(0);

    int state = -1;
    int cycle = 0;
    bool end = false;

    Mat image, imgLeft, imgRight;
    Rect roiL(0, 0, dWidth / 2, dHeight);
    Rect roiR(dWidth / 2, 0, dWidth / 2, dHeight);
	imgLeft = image(roiL);
	imgRight = image(roiR);
	float initL = 1 - countNonZero(imgLeft);
	float initR = 1 - countNonZero(imgRight);
	while (true)
    {
        capture >> image;
        if (image.empty())
            break;

        if (cycle < 90)
        {
            if (cycle == 0)
            {
                cout << "Fasten seat belt..." << endl;
            }
            if (cycle % 30 == 0)
            {
                cout << (3 - cycle / 30) << "..." << endl;
            }
            cycle++;
            continue;
        }
        imwrite("camera.jpg", image);
        imshow("Camera", image);

        cvtColor(image, image, CV_BGR2GRAY);
        threshold(image, image, 80, 255, THRESH_BINARY);
        imshow("Processed", image);

        imgLeft = image(roiL);
        imgRight = image(roiR);
        float rateL = 1 - countNonZero(imgLeft) * 2.0 / dWidth / dHeight;
        float rateR = 1 - countNonZero(imgRight) * 2.0 / dWidth / dHeight;
        if (DEBUG)
            cout << "L=" << rateL << ", R=" << rateR << ", ts=" << capture.get(CV_CAP_PROP_POS_MSEC) << endl;

		float deltL = fabs(rateL - initL);
		float deltR = fabs(rateR - initR);
        if (deltL < THRESHOLD && deltR < THRESHOLD && state != 0)
        {
            turnTo(0);
            controlLeft(FORWARD, MAX_SPEED);
            controlRight(FORWARD, MAX_SPEED);
            state = 0;
            if (DEBUG)
                cout << "State set to 0" << endl;
        }
        else if (deltL >= THRESHOLD && deltR < THRESHOLD && state != 1)
        {
            //turnTo(MAX_ANGLE);
            init();
            turnTo(TURN_ANGLE);
            //controlLeft(FORWARD, TURN_SPEED);
            controlLeft(FORWARD, TURN_SPEED);
            state = 1;
            if (DEBUG)
                cout << "State set to 1" << endl;
            cout << "Watch out, backing!" << endl;
        }
        else if (deltR >= THRESHOLD && deltL < THRESHOLD && state != 2)
        {
            //turnTo(-MAX_ANGLE);
            init();
            turnTo(-TURN_ANGLE);
            controlRight(FORWARD, TURN_SPEED);
            //controlRight(FORWARD, TURN_SPEED);
            state = 2;
            if (DEBUG)
                cout << "State set to 2" << endl;
            cout << "Watch out, backing!" << endl;
        }
        else if (deltL >= THRESHOLD && deltR >= THRESHOLD)
        {
            if (!end)
            {
                end = true;
                cout << "Reached finish line." << endl;
                init();
                turnTo(0);
                controlLeft(FORWARD, MAX_SPEED);
                controlRight(FORWARD, MAX_SPEED);
                delay(500);
                init();
                cout << "Stop gracefully." << endl;
                return 0;
            }
            cout << "Error. Stop." << endl;
            init();
            return 1;
        }
        waitKey(1);
    }
    return 0;
}
