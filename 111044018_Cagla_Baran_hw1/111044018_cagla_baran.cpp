#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>
#include "opencv2/opencv.hpp"
#include <iostream>


using namespace std;
using namespace cv;

/// Global variables
int threshold_value = 0;
int const max_BINARY_value = 255;

Mat src_gray, dst;
char* window_name = "Binary Threshold";
char* trackbar_value = "Value";

//Global Functions
Mat binaryImage(int xTresh, Mat src_gray1){

	int x, y;
	
	for (y = 0; y < src_gray1.rows; y++)
	{
		for (x = 0; x < src_gray1.cols; x++)
		{
			if (src_gray1.at<uchar>(y, x) <= xTresh)
			{
				src_gray1.at<uchar>(y, x) = 0;
			}
			else if (src_gray1.at<uchar>(y, x) > xTresh)
			{
				src_gray1.at<uchar>(y, x) = 255;
			}
		}
	}
	
	return src_gray1;

}


void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
	Mat &img = *((Mat*)(userdata));
	Mat img2=img.clone();
	if (event == EVENT_LBUTTONDOWN)
	{
		destroyWindow("Binary image");
		
		cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
		img2 = binaryImage(x, img2);

		namedWindow("Binary image", CV_WINDOW_AUTOSIZE);
		imshow("Binary image", img2);

	}

}



/// Function headers
int part1(void);

int part2(void);
void Threshold_Binary(int, void*);



/**
* @function main
*/
int main(int argc, char** argv)
{
	int part;
	cout << "Select the part, 1 or 2" << endl;
	cin >> part;
	if (part == 1)
		part1();
	else if (part == 2)
		part2();
	return 0;
}


int part1(void)
{
	Mat src, dst;
	int hist[256];
	int x = 0, y = 0, temp, i;
	int hist_w = 255; int hist_h = 400;
	int bin_w = cvRound((double)hist_w / 256);
	int key,a;

	for (i = 0; i < 256; i++)
	{
		hist[i] = 0;
	}

	VideoCapture cap(0); // open the default camera
	if (!cap.isOpened())  // check if we succeeded
		return -1;


	for (;;)
	{
		Mat frame;
		cap >> frame; // get a new frame from camera
		
		namedWindow("Video", CV_WINDOW_AUTOSIZE);
		imshow("Video", frame);

		key = waitKey(20);
		if ((char)key == 27)
		{
			cout << "kapandi\n";
			break;
		}
		//press button Enter
		if ((char)key == 13)
		{
			destroyWindow("Binary image");
			src = frame;

			/// Convert the image to Gray
			cvtColor(src, src_gray, CV_RGB2GRAY);
			/// Create a window to display results

			imshow("Gray Image", src_gray);
			for (y = 0; y < src_gray.rows; y++)
			{
				for (x = 0; x < src_gray.cols; x++)
				{
					Scalar intensity = src_gray.at<uchar>(y, x);
					temp = src_gray.at<uchar>(y, x);
					hist[temp] += 1;
				}
			}

			Mat histImage(hist_h, hist_w, CV_8UC1, Scalar(0, 0, 0));

			// find the maximum intensity element from histogram
			int max = hist[0];
			for (int i = 1; i < 256; i++){
				if (max < hist[i]){
					max = hist[i];
				}
			}

			// normalize the histogram between 0 and histImage.rows

			for (int i = 0; i < 255; i++){
				hist[i] = ((double)hist[i] / max)*histImage.rows;
			}


			// draw the intensity line for histogram
			for (int i = 0; i < 255; i++)
			{
				line(histImage, Point(bin_w*(i), hist_h),
					Point(bin_w*(i), hist_h - hist[i]),
					Scalar(255, 0, 0), 2, 8, 0);
			}

			// display histogram
			namedWindow("Intensity Histogram", CV_WINDOW_AUTOSIZE);
			imshow("Intensity Histogram", histImage);
			setMouseCallback("Intensity Histogram", CallBackFunc, &src_gray);

		}

		

	}
	

}


int part2(void){

	VideoCapture cap(0); // open the default camera
	if (!cap.isOpened())  // check if we succeeded
		return -1;

	for (;;)
	{
		Mat frame;
		cap >> frame; // get a new frame from camera

		/// Convert the image to Gray
		cvtColor(frame, src_gray, CV_RGB2GRAY);

		/// Create a window to display results
		namedWindow(window_name, CV_WINDOW_AUTOSIZE);

		createTrackbar(trackbar_value,
			window_name, &threshold_value,
			max_BINARY_value, Threshold_Binary);

		/// Call the function to initialize
		Threshold_Binary(0, 0);


		if (waitKey(30) >= 0) break;
	}
}


/**
* @function Threshold_Binary
*/
void Threshold_Binary(int, void*)
{
	//Binary Treshold
	threshold(src_gray, dst, threshold_value, max_BINARY_value, 0);

	imshow(window_name, dst);
}



