#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cmath>
#include <iostream>

using namespace std;
using namespace cv;


/// Global variables

vector<vector<Point> > contours;
vector<Vec4i> hierarchy;
Mat src;
Mat marked;
int thresh = 0;


int main(int argc, char** argv)
{

	int indexMarked = 0;
	int indexCounter = 0;
	int x_mean = 0;
	int y_mean = 0;

	bool xSmallySmall = false;
	bool xSmallyBig = false;
	bool xBigySmall = false;
	bool xBigyBig = false;


	int counterRect = 0, counterCircle = 0;
	int numOfMarkerRect = 0, numOfMarkerOneCircle = 0, numOfMarkerTwoCircle = 0;


	Scalar colorGreen = Scalar(0, 255, 0);
	Scalar colorBlue = Scalar(255, 0, 0);
	Scalar colorRed = Scalar(0, 0, 255);

	
	VideoCapture cap(0); // open the default camera
	if (!cap.isOpened())  // check if we succeeded
	return -1;

	Mat frame;
	for (;;)
	{

		cap >> frame; // get a new frame from camera
		namedWindow("Video", CV_WINDOW_AUTOSIZE);

		src = frame;
		if (src.empty())
			return -1;

		// Convert to grayscale
		cv::Mat gray, binary;
		cv::cvtColor(src, gray, CV_BGR2GRAY);
		threshold(gray, binary, 128, 255, 0);


		// Use Canny instead of threshold to catch squares with gradient shading
		cv::Mat bw;
		cv::Canny(binary, bw, 0, 50, 5);

		marked = src.clone();

		// Find contours
		cv::findContours(bw.clone(), contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);

		for (int i = 0; i < contours.size(); i++)
		{
			//şeklin başlangıcını tutuyorum;
			indexCounter++;

			//Dikdörtgeni yakalamak için x lerin ve y lerin ortalamasını alıp buna göre bir algoritma geliştirdim
			for (int j = 0; j < contours[i].size(); j++) 
			{
				x_mean += contours[i][j].x;
				y_mean += contours[i][j].y;
			}
			x_mean = x_mean / contours[i].size();
			y_mean = y_mean / contours[i].size();

			for (int j = 0; j < contours[i].size(); j++) 
			{
				
				//Dikdörtgen için
				if (contours[i][j].x + thresh< x_mean && contours[i][j].y + thresh < y_mean)
					xSmallySmall = true;
				else if (contours[i][j].x + thresh< x_mean && contours[i][j].y - thresh > y_mean)
					xSmallyBig = true;	
				else if (contours[i][j].x - thresh> x_mean && contours[i][j].y + thresh < y_mean)
					xBigySmall = true;
				else if (contours[i][j].x - thresh> x_mean && contours[i][j].y - thresh> y_mean)
					xBigyBig = true;

			}

			if (xSmallySmall == true && xSmallyBig == true && xBigySmall == true && xBigyBig == true)
			{
				//cout << "Rectangle Buldum" << endl;
				xSmallySmall = false;
				xSmallyBig = false;
				xBigySmall = false;
				xBigyBig = false;
				counterRect++;
			}

			x_mean = 0;
			y_mean = 0;

			if (indexCounter == 10 && hierarchy[i-1][0]==-1 && hierarchy[i][2] == -1 &&counterRect>=8)
			{
				numOfMarkerOneCircle++;
				drawContours(marked, contours, i - 8, colorRed, 8, 8, hierarchy);
			}
			else if (indexCounter == 10 && hierarchy[i - 1][0] != -1 && hierarchy[i][2] == -1 && counterRect >= 8)
			{
				drawContours(marked, contours, i - 8, colorBlue, 8, 8, hierarchy);
				numOfMarkerTwoCircle++;
			}
			else if (indexCounter == 8 && counterRect == 8 && hierarchy[i][2] == -1)
			{
				numOfMarkerRect++;
				drawContours(marked, contours, i -6, colorGreen, 8, 8, hierarchy);
			}

			//yeni bir şekle geçildiyse
			if (hierarchy[i][2] == -1)
			{
				counterRect = 0;
				counterCircle = 0;
				indexCounter = 0;
			}
		}

		cout << "dikdortgen marker sayisi= " << numOfMarkerRect << endl;
		cout << "tek daireli marker sayisi= " << numOfMarkerOneCircle << endl;
		cout << "iki daireli marker sayisi= " << numOfMarkerTwoCircle << endl;

		numOfMarkerRect = 0;
		numOfMarkerOneCircle = 0;
		numOfMarkerTwoCircle = 0;

		imshow("Video", marked);
		if (waitKey(30) >= 0) break;

	}
	return 0;
}



