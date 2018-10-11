#include <cstdlib>
#include <iostream>
#include <vector>
#include "PID.h"
#include "GPIOlib.h"


#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

#define PI 3.1415926

//Uncomment this line at run-time to skip GUI rendering
#define _DEBUG

using namespace cv;
using namespace std;
using namespace GPIO;

const string CAM_PATH = "/dev/video0";
const string MAIN_WINDOW_NAME = "Processed Image";
const string CANNY_WINDOW_NAME = "Canny";

// TODO：需要调节 CANNY 的阈值和 HOUGH 的阈值
const int CANNY_LOWER_BOUND = 50;
const int CANNY_UPPER_BOUND = 250;
const int HOUGH_THRESHOLD = 80;

const double dWidth = 640;            //the width of frames of the video
const double dHeight = 480;        //the height of frames of the video

const int initial_motor_speed = 20;

int main() {

    PID_incremental pid1(0.35, 0.65, 0.005);
//    float target=1000.0;
//    float actual=0;
//    float pid_increment=0.0;
//    int N=50;
//    pid1.pid_show();
//    cout<<"target="<<target<<endl;
//    for(;N>0;N--)
//    {
//        pid_increment=pid1.pid_control(target,actual);
//        actual+=pid_increment;
//        cout<<"N="<<50-N<<"   actual="<<actual<<endl;
//    }
//    pid1.pid_show();

    VideoCapture capture(CAM_PATH);
    //If this fails, try to open as a video camera, through the use of an integer param
    if (!capture.isOpened()) {
        capture.open(atoi(CAM_PATH.c_str()));
    }

    capture.set(CV_CAP_PROP_FRAME_WIDTH, dWidth);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT, dHeight);

    clog << "Frame Size: " << capture.get(CV_CAP_PROP_FRAME_WIDTH) << "x" << capture.get(CV_CAP_PROP_FRAME_HEIGHT)
         << endl;

    Mat image;
    while (true) {
        capture >> image;
        if (image.empty())
            break;

        //Set the ROI for the image (ROI: Region of Interest)
        // TODO: 需要调节选择的 ROI
        Rect roi(0, 2 * image.rows / 3, image.cols, image.rows / 3); // 选下1/3部分作为ROI
        Mat imgROI = image(roi);

        //Canny algorithm
        Mat contours;
        Canny(imgROI, contours, CANNY_LOWER_BOUND, CANNY_UPPER_BOUND);
#ifdef _DEBUG
        imshow(CANNY_WINDOW_NAME, contours);
#endif

        vector<Vec2f> lines;
        HoughLines(contours, lines, 1, PI / 180, HOUGH_THRESHOLD);
        Mat result(imgROI.size(), CV_8U, Scalar(255));
        imgROI.copyTo(result);
        clog << lines.size() << endl;

        float maxRad = -2 * PI;
        float minRad = 2 * PI;
        //Draw the lines and judge the slope
        for (vector<Vec2f>::const_iterator it = lines.begin(); it != lines.end(); ++it) {
            float rho = (*it)[0];            //First element is distance rho
            float theta = (*it)[1];        //Second element is angle theta

            //Filter to remove vertical and horizontal lines,
            //and atan(0.09) equals about 5 degrees.
            if ((theta > 0.09 && theta < 1.48) || (theta > 1.62 && theta < 3.05)) {
                if (theta > maxRad)
                    maxRad = theta;
                if (theta < minRad)
                    minRad = theta;

#ifdef _DEBUG
                //point of intersection of the line with first row
                Point pt1(rho / cos(theta), 0);
                //point of intersection of the line with last row
                Point pt2((rho - result.rows * sin(theta)) / cos(theta), result.rows);
                //Draw a line
                line(result, pt1, pt2, Scalar(0, 255, 255), 3, CV_AA);
#endif
            }

#ifdef _DEBUG
//			clog<<"Line: ("<<rho<<","<<fixed<<setprecision(2)<<theta*180/PI<<")\n";
#endif
        }

#ifdef _DEBUG
        stringstream overlayedText;
        overlayedText << "Lines: " << lines.size();
        putText(result, overlayedText.str(), Point(10, result.rows - 10), 2, 0.8, Scalar(0, 0, 255), 0);
        imshow(MAIN_WINDOW_NAME, result);
#endif

        float target = 0;
        float actual = minRad;
        float pid_increment = 0.0;
        cout << "target=" << target << endl;
        pid_increment = pid1.pid_control(target, actual);

        int left_motor_speed = initial_motor_speed - pid_increment;
        int right_motor_speed = initial_motor_speed + pid_increment;

        left_motor_speed = min(left_motor_speed, 50);
        right_motor_speed = min(right_motor_speed, 50);

        left_motor_speed = max(left_motor_speed, 0);
        right_motor_speed = max(right_motor_speed, 0);

        controlLeft(FORWARD, left_motor_speed);
        controlRight(FORWARD, right_motor_speed);

        lines.clear();
        waitKey(1);
    }

//    float target=1000.0;
//    float actual=0;
//    int N;
//    PID_position pid2(0.59,0.35,0.002);
//    pid2.pid_show();
//    cout<<"target="<<target<<endl;
//    N=100;
//    for(;N>0;N--)
//    {
//        actual=pid2.pid_control(target,actual);
//        cout<<"N="<<100-N<<"   actual="<<actual<<endl;
//    }
//    pid2.pid_show();

    return 0;
}
