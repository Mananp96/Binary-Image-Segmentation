#include "pch.h"
#include <opencv2/opencv.hpp>
#include <fstream>
#include <utility>
using namespace cv;
using namespace std;

void convertToGrayscale(Mat &in_image, Mat &gray_image, Mat &grad_x, Mat &grad_y) {
	GaussianBlur(in_image, in_image, Size(3, 3), 0, 0, BORDER_DEFAULT);
	cvtColor(in_image, gray_image, COLOR_BGR2GRAY);
	Scharr(gray_image, grad_x, CV_16S, 1, 0, 1, 0, BORDER_DEFAULT);
	convertScaleAbs(grad_x, grad_x);
	Scharr(gray_image, grad_y, CV_16S, 0, 1, 1, 0, BORDER_DEFAULT);
	convertScaleAbs(grad_y, grad_y);
}

void setEdge(vector< list< pair<int, int> > > &adjlist, int k, int v, int weight)
{
	adjlist[k].push_back(make_pair(v,weight));
}

void weightCalc() {

}

/**
@function main
*/
int main(int argc, char** argv)
{
	/* if (argc != 4) {
		cout << "Usage: ../seg input_image initialization_file output_mask" << endl;
		return -1;
	}*/

	Mat in_image, gray_image;
	Mat gradient;
	Mat grad_x, grad_y;

	in_image = imread("simple.png"/*argv[1]*/);
	if (!in_image.data)
	{
		cout << "Could not load input image!!!" << endl;
		return -1;
	}

	if (in_image.channels() != 3) {
		cout << "Image does not have 3 channels!!! " << in_image.depth() << endl;
		return -1;
	}
	convertToGrayscale(in_image, gray_image,grad_x,grad_y);
	addWeighted(grad_x, 0.5, grad_y, 0.5, 0, gradient);
	// the output image
	Mat out_image = in_image.clone();
	ifstream f("config.txt"/*argv[2]*/);
	if (!f) {
		cout << "Could not load initial mask file!!!" << endl;
		return -1;
	}
	pair<int, int> sourcePair;
	pair<int, int> sinkPair;

	vector<pair<int,int>> source;
	vector<pair<int,int>> sink;
	int width = in_image.cols;
	int height = in_image.rows;
	int total_pixel = width * height;
	int n;
	int pixelCount = 0;
	int maxIntPixel = 0;
	f >> n;

	// get the initial pixels
	for (int i = 0; i < n; ++i) {
		int row, col, w;
		f >> row >> col >> w;

		if (row < 0 || row >= width || col < 0 || col >= height) {
			cout << "I valid pixel mask!" << endl;
			return -1;
		}

		if (w == 1) { //foreground
			sourcePair.first = row;
			sourcePair.second = col;
			source.push_back(sourcePair);
			//source.push_back(make_pair(x,y));
		}
		else { //background
			sinkPair.first = row;
			sinkPair.second = col;
			sink.push_back(sinkPair);
		}
		//out_image.at<Vec3b>(y, x) = pixel;
	}

	//getting the maximum intensity pixel - maxIntPixel
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			pixelCount++;
			int currentPixel = gray_image.at < uchar >(j, i);
			if (currentPixel > maxIntPixel) {
				maxIntPixel = currentPixel;
			}
		}
	}
	cout << "total pixel" << pixelCount << endl;
	cout << "maxIntPixel" << maxIntPixel << endl;
	cout << "row " << "column " << "weights" << endl;
	vector< list< pair<int, int> > > adjlist(total_pixel);
	//weight calculation
	for (int pixel = 0; pixel < total_pixel; ++pixel) {
		int x = total_pixel / width;
		int y = total_pixel % width;
		if (pixel - 1 >= 0 && y - 1 >= 0) {
			int leftPixelWeight = maxIntPixel - ((gray_image.at<uchar>(x, y) + gray_image.at<uchar>(x, y - 1))/2);
			setEdge(adjlist, pixel, pixel - 1, leftPixelWeight);
		}
		if (pixel + 1 < total_pixel && y + 1 < width) {
			int rightPixelWeight = maxIntPixel - ((gray_image.at<uchar>(x, y) + gray_image.at<uchar>(x, y + 1)) / 2);
			setEdge(adjlist, pixel, pixel + 1, rightPixelWeight);
		}

		if (x + 1 < height && pixel + width < total_pixel) {
			int downPixelWeight = maxIntPixel - ((gray_image.at<uchar>(x, y) + gray_image.at<uchar>(x+1, y)) / 2);
			setEdge(adjlist, pixel, pixel + width, downPixelWeight);
		}

		if (x - 1 >= 0 && pixel - width >= 0) {
			int upPixelWeight = maxIntPixel - ((gray_image.at<uchar>(x, y) + gray_image.at<uchar>(x-1, y)) / 2);
			setEdge(adjlist, pixel, pixel - width, upPixelWeight);
		}

	}
	//call function to cal weight

	// write it on disk
	imwrite("output.png"/*argv[3]*/, out_image);
	namedWindow("Original image", WINDOW_AUTOSIZE);
	namedWindow("Show Marked Pixels", WINDOW_AUTOSIZE);
	imshow("Original image", in_image);
	imshow("Show Marked Pixels", out_image);
	imshow("gradient image", gradient);
	waitKey(0);
	return 0;
}