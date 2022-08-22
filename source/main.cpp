#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <windows.h>
#include <cmath>

using namespace cv;
using namespace std;

Mat Frame;
VideoCapture cap(0);

bool start = true;

const Mat _5 = imread("resources/5.jpg", 1);
const Mat _10 = imread("resources/10.jpg", 1);
const Mat _20 = imread("resources/20.jpg", 1);
const Mat _50 = imread("resources/50.jpg", 1);
const Mat _100 = imread("resources/100.jpg", 1);
const Mat _200 = imread("resources/200.jpg", 1);

void frameToWindow()
{
    cap >> Frame;
    imshow("Display window - Original", Frame);
	if (start)
	{
		system("cls");
		start = false;
	}
}

Mat Crop()
{

	Mat src = Frame;
	Mat gray;
	vector<Vec3f> circles;
	cvtColor(src, gray, COLOR_BGR2GRAY);
	medianBlur(gray, gray, 5);
	HoughCircles(gray, circles, HOUGH_GRADIENT, 1, gray.rows / 16, 100, 30, 1, 100 ); // change the last two parameters min, max
	
	/*
	for (size_t i = 0; i < circles.size(); i++)
	{
		Vec3i c = circles[i];
		Point center = Point(c[0], c[1]);
		// circle center
		circle(src, center, 1, Scalar(0, 100, 100), 3, LINE_AA);
		// circle outline
		int radius = c[2];
		circle(src, center, radius, Scalar(255, 0, 255), 3, LINE_AA);
	}
	*/

	if (circles.size() != 0)
	{
		Vec3f circ = circles[0];

		Mat1b mask(src.size(), uchar(0));
		circle(mask, Point(circ[0], circ[1]), circ[2], Scalar(255), FILLED);

		Rect bbox(circ[0] - circ[2], circ[1] - circ[2], 2 * circ[2], 2 * circ[2]);

		Mat3b res(src.size(), Vec3b(0, 0, 0));

		src.copyTo(res, mask);

		res = res(bbox);

		return res;
	}
	else
	{
		Mat image(240, 240, CV_8UC3, Scalar(0, 0, 0));
		return image;
	}
}

double match(Mat coin)
{
	Mat scoreImg;
	double maxScore;

	matchTemplate(coin, Crop(), scoreImg, TM_SQDIFF_NORMED);
	minMaxLoc(scoreImg, 0, &maxScore);
	//cout << (1 - maxScore) << endl;

	return 1 - maxScore;
}

void compare()
{
	vector<double> percent;
	percent.push_back(match(_5));
	percent.push_back(match(_10));
	percent.push_back(match(_20));
	percent.push_back(match(_50));
	percent.push_back(match(_100));
	percent.push_back(match(_200));

	double sum = 0;
	for (int i = 0; i < 6; i++)
	{
		sum += (percent[i]);
	}

	if (sum != 0)
	{
		if ((1 - match(_5) == *max_element(percent.begin(), percent.end())) && *max_element(percent.begin(), percent.end()) > 0.5)
		{
			cout << "5Ft - " << *max_element(percent.begin(), percent.end()) * 100 << '%' << endl;
		}
		if ((1 - match(_10) == *max_element(percent.begin(), percent.end())) && *max_element(percent.begin(), percent.end()) > 0.5)
		{
			cout << "10Ft - " << *max_element(percent.begin(), percent.end()) * 100 << '%' << endl;
		}
		if ((1 - match(_20) == *max_element(percent.begin(), percent.end())) && *max_element(percent.begin(), percent.end()) > 0.5)
		{
			cout << "20Ft - " << *max_element(percent.begin(), percent.end()) * 100 << '%' << endl;
		}
		if ((1 - match(_50) == *max_element(percent.begin(), percent.end())) && *max_element(percent.begin(), percent.end()) > 0.5)
		{
			cout << "50Ft - " << *max_element(percent.begin(), percent.end()) * 100 << '%' << endl;
		}
		if ((1 - match(_100) == *max_element(percent.begin(), percent.end())) && *max_element(percent.begin(), percent.end()) > 0.5)
		{
			cout << "100Ft - " << *max_element(percent.begin(), percent.end()) * 100 << '%' << endl;
		}
		if ((1 - match(_200) == *max_element(percent.begin(), percent.end())) && *max_element(percent.begin(), percent.end()) > 0.5)
		{
			cout << "200Ft - " << *max_element(percent.begin(), percent.end()) * 100 << '%' << endl;
		}
	}
}

int main() 
{
	if (!cap.isOpened()) 
	{
		cout << "Can't open camera.";
	}

	while (true) 
	{

		frameToWindow();
		compare();

		char c = (char)waitKey(25);
		if ((c == 27) or (!cap.isOpened()))
		{
			return 0;
		}
	}

	return 0;
}