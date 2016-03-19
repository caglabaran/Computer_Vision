#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cmath>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>


using namespace std;
using namespace cv;


/// Global variables

vector<vector<Point> > contours;
vector<Vec4i> hierarchy;
Mat src;
Mat marked;
int thresh = 0;

//3. odev ekleme
// Read 3D points
vector<Point3f> objectPoints;
vector<Point2f> imagePoints;
vector<Point2f> projectedPoints;
vector<Point3f> kupPoints;

Mat KMat(3, 3, DataType<double>::type); // Intrisic matrix
Mat distVect(5, 1, DataType<double>::type);   // Distortion vector

Mat rVec(3, 1, DataType<double>::type); // Rotation vector
Mat tVec(3, 1, DataType<double>::type); // Translation vector

int main(int argc, char** argv)
{	

	double x, y, z;

	x = 0.0; y = 0.0; z = 0.0;
	objectPoints.push_back(cv::Point3f(x, y, z));
	x = 1.25; y = 0.0; z = 0.0;
	objectPoints.push_back(cv::Point3f(x, y, z));
	x = 1.25; y = 1.0; z = 0.0;
	objectPoints.push_back(cv::Point3f(x, y, z));
	x = 0.0; y = 1.0; z = 0.0;
	objectPoints.push_back(cv::Point3f(x, y, z));

	kupPoints.push_back(Point3f(0, 0, 0));
	kupPoints.push_back(Point3f(1.25, 0, 0));
	kupPoints.push_back(Point3f(1.25, 1.0, 0));
	kupPoints.push_back(Point3f(0, 1.0, 0));
	kupPoints.push_back(Point3f(0, 0, -1));
	kupPoints.push_back(Point3f(1.25, 0, -1));
	kupPoints.push_back(Point3f(1.25, 1.0, -1));
	kupPoints.push_back(Point3f(0, 1.0, -1));


	KMat.at<double>(0, 0) = 6.3585216077219388e+002;
	KMat.at<double>(0, 1) = 0;
	KMat.at<double>(0, 2) = 3.2350084024499176e+002;

	KMat.at<double>(1, 0) = 0;
	KMat.at<double>(1, 1) = 5.9399035978625886e+002;
	KMat.at<double>(1, 2) = 2.2361238185214100e+002;

	KMat.at<double>(2, 0) = 0;
	KMat.at<double>(2, 1) = 0;
	KMat.at<double>(2, 2) = 1;

	
	distVect.at<double>(0) = 1.0879099851422658e-001;
	distVect.at<double>(1) = -8.9424776206369150e-001;
	distVect.at<double>(2) = 3.2219216457340279e-003;
	distVect.at<double>(3) = 6.1805913914659292e-003;
	distVect.at<double>(4) = 1.8554984937535517e+000;

	std::cout << "Intrisic matrix: " << KMat << std::endl << std::endl;
	std::cout << "Distortion coef: " << distVect << std::endl << std::endl;



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
		imagePoints.clear();

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
				if (contours[i][j].x + thresh < x_mean && contours[i][j].y + thresh < y_mean)
				{
					xSmallySmall = true;
				}
				else if (contours[i][j].x + thresh< x_mean && contours[i][j].y - thresh > y_mean)
				{
					xSmallyBig = true;
				}
				else if (contours[i][j].x - thresh > x_mean && contours[i][j].y + thresh < y_mean)
				{
					xBigySmall = true;
				}
				else if (contours[i][j].x - thresh > x_mean && contours[i][j].y - thresh> y_mean)
				{
					xBigyBig = true;
				}

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

			if (indexCounter == 10 && hierarchy[i - 1][0] == -1 && hierarchy[i][2] == -1 && counterRect >= 8)
			{
				numOfMarkerOneCircle++;
				if (i > 8)
				{
					approxPolyDP(Mat(contours[i - 8]), contours[i - 8], 3, true);
					if (contours[i - 8].size() == 4){
						imagePoints.push_back(contours[i - 8][0]);
						imagePoints.push_back(contours[i - 8][1]);
						imagePoints.push_back(contours[i - 8][2]);
						imagePoints.push_back(contours[i - 8][3]);
						break;
					}

				}
			}
			else if (indexCounter == 10 && hierarchy[i - 1][0] != -1 && hierarchy[i][2] == -1 && counterRect >= 8)
			{
				numOfMarkerTwoCircle++;
				if (i > 8){
					approxPolyDP(Mat(contours[i - 8]), contours[i - 8], 3, true);
					if (contours[i - 8].size() == 4){
						imagePoints.push_back(contours[i - 8][0]);
						imagePoints.push_back(contours[i - 8][1]);
						imagePoints.push_back(contours[i - 8][2]);
						imagePoints.push_back(contours[i - 8][3]);
						break;
					}
				}
				

			}
			else if (indexCounter == 8 && counterRect == 8 && hierarchy[i][2] == -1)
			{
				numOfMarkerRect++;
				if (i > 6){
					approxPolyDP(Mat(contours[i - 6]), contours[i - 6], 3, true);

					if (contours[i - 6].size() == 4){

						imagePoints.push_back(contours[i - 6][0]);
						imagePoints.push_back(contours[i - 6][1]);
						imagePoints.push_back(contours[i - 6][2]);
						imagePoints.push_back(contours[i - 6][3]);
						break;
					}
				}
				
			}

			//yeni bir şekle geçildiyse
			if (hierarchy[i][2] == -1)
			{
				counterRect = 0;
				counterCircle = 0;
				indexCounter = 0;
			}
		}
		if (imagePoints.size() != 0)
		{
			circle(marked, imagePoints[0], 5, colorGreen, 5);
			circle(marked, imagePoints[1], 5, colorGreen, 5);
			circle(marked, imagePoints[2], 5, colorGreen, 5);
			circle(marked, imagePoints[3], 5, colorGreen, 5);

			

			if (objectPoints.size() > 0){
				solvePnP(objectPoints, imagePoints, KMat, distVect, rVec, tVec);
				projectPoints(kupPoints, rVec, tVec, KMat, distVect, projectedPoints);
				for (int j = 0; j < 8; j++)
				{
					for (int k = 0; k < 8; k++)
					{
						line(marked, projectedPoints[j], projectedPoints[k], colorBlue, 3);
					}
				}
			}
			

		}
		
		

		/*cout << "dikdortgen marker sayisi= " << numOfMarkerRect << endl;
		cout << "tek daireli marker sayisi= " << numOfMarkerOneCircle << endl;
		cout << "iki daireli marker sayisi= " << numOfMarkerTwoCircle << endl;*/

		numOfMarkerRect = 0;
		numOfMarkerOneCircle = 0;
		numOfMarkerTwoCircle = 0;

		imshow("Video", marked);
		if (waitKey(30) >= 0) break;

	}
	return 0;
}



