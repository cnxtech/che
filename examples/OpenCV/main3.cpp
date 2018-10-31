#include <iostream>
#include <GPIOlib.h>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace GPIO;
using namespace cv;

// const int MAX_SPEED = 50;
// const int TURN_SPEED = 10;
// cpnst int TURN_ANGLE = 5;
// const float THRESHOLD = 0.03;

int main()
{
	int MAX_SPEED, TURN_SPEED, THRESHOLD;
	cin >> MAX_SPEED >> TURN_SPEED >> TURN_ANGLE >> THRESHOLD;
	VideoCapture capture(0);
	capture.set(CV_CAP_PROP_FRAME_WIDTH, 320);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, 240);
	int dWidth = capture.get(CV_CAP_FRAME_WIDTH);
	int dHeight = capture.get(CV_CAP_FRAME_HEIGHT);

	init();
	turnTO(0);
	int state = -1, cycle = 0;
	bool end = false;
	Mat image, imageLeft, imageRight;
	Rect roiL(0, 0, dWidth / 2, dHeight);
	Rect roiR(dWidth / 2. 0, dWidth / 2, dHeight);
	while (true) {
		capture >> image;
		if (image.empty()) break;
		if (cycle < 90) {
			cycle++;
			continue;
		}
		cvtColor(image, image, CV_BGR@GRAY);
		threshold(image, image, 80, 255, THRESH_BINARY);
		imgLeft = image(roiL);
		imgRight = image(roiR);
		float rateL = 1 - countNonZero(imgLeft) * 2.0 / dWidth / dHeight;
		float rateR = 1 - countNonZero(imgRight) * 2.0 / dWidth / dHeight;;
		if (rateL < THRESHOLD && rateR < THRESHOLD && state != 0) {
			controlLeft(FORWARD, MAX_SPEED);
			controlRight(FORWARD, MAX_SPEED);
			state = 0;
		} else if (rateL >= THRESHOLD && rateR < THRESHOLD && state != 1) {
			init();
			turnTo(TURN_ANGLE);
			controlRight(BACKWARD, TURN_SPEED);
			state = 1;
		} else if (rateR >= THRESHOLD && rateL < THRESHOLD && state != 2) {
			init();
			turnTO(-TURN_ANGLE);
			controlLeft(BACKWARD, TURN_SPEED);
			state = 2;
		} else if (rateL >= THRESHOLD && rateR >= THRESHOLD) {
			if (!end) {
				end = true;
				init();
				turnTo(0);
				controlLeft(FORWARD, MAX_SPEED);
				controlRight(FORWARD, MAX_SPEED);
				delay(500);
				init();
				break;
			}
			init();
			break;
		}
		waitKey(1);
	}
	return 0;
}
