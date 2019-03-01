#include "pch.h"
#include <opencv2/opencv.hpp>
#include <fstream>
#include <utility>
#include <queue>
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
void printList(vector< list< pair<int, int> > > &adjlist, vector<pair<int, int>> at) {
	/*for (int i = 0; i < adjlist.size(); ++i) {
		printf("adjacencyList[%d] ", i);

		list< pair<int, int> >::iterator itr = adjlist[i].begin();

		while (itr != adjlist[i].end()) {
			printf(" -> %d(%d)", (*itr).first, (*itr).second);
			++itr;
		}
		printf("\n");
	}*/
	
	list< pair<int, int> >::iterator itr = adjlist[10].begin();
	while (itr != adjlist[10].end()) {
		printf(" -> %d(%d)", (*itr).first, (*itr).second);
		
		cout<<at[(*itr).first].first<<endl;
		cout<<"sdjsbfjk " << at[10].first;
		at[10].first = (*itr).first;
		cout << "sadjkfsd " << at[10].first;
		++itr;
	}
	
	
}

void setEdge(vector< list< pair<int, int> > > &adjlist, int k, int v, int weight)
{
	adjlist[k].push_back(make_pair(v,weight));
}

void getEdgeWeight() {

}

int inspect1(vector<int> newvector, int a) {
	for (int i = 0; i < newvector.size(); i++) {
		if (newvector[i] == a) {
			return 0;
		}
	}
	return 1;
}
int inspect(vector<int> t, int l) {
	for (int i = 0; i < t.size(); i++) {
		if (t[i] == l) {
			return 1;
		}	
	}
	return 0;
}
void BFS(queue<int> qcolor, vector<pair<int,int>> comb1, vector< list< pair<int, int> > > adjlist, Mat out_image, int width) {
	while (!qcolor.empty()) {

		int last = qcolor.front();
		qcolor.pop();

			list< pair<int, int> >::iterator itr = adjlist[last].begin();
			while (itr != adjlist[last].end()) {
			if ((comb1[(*itr).first].second == 0) && (*itr).second > 0) {

				Vec3b pixel2;
				pixel2[0] = 0;
				pixel2[1] = 0;
				pixel2[2] = 225;
				int x3, x4;
				x3 = ((*itr).first)/ width;
				x4 = ((*itr).first) % width;
				out_image.at < Vec3b >(x3, x4) = pixel2;

				qcolor.push((*itr).first);
				comb1[(*itr).first].second = 1;

			}

		}
	}
}

void coloring() {

}

void max_flow(vector< list< pair<int, int> > > &adjlist, vector<int> &source,vector<int> &sink,
	Mat out_image, int total_pixel, vector<pair<int, int>> &visitedPixel, queue<vector<int>> &queue,
	vector<pair<int, int>> &anotherVisitedPixel, int height, int width) {
	double minFlow = LONG_MAX;
	vector < pair < int, int > > comb1;
	comb1 = anotherVisitedPixel;
	int visitedToken = 1;
	vector<int> route;
	vector<int>::iterator it;
	bool visited;
	visited = true;
	while (visited == true) {
		visited = false;
		while (!queue.empty()) {
			again: vector<int> pathvector;
			pathvector = queue.front();
			queue.pop();
			int lastelement = pathvector.back();
			if (inspect(sink, lastelement) == 1) {
				route.clear();
				route = pathvector;
				visited = true;
				//comb.clear();
				anotherVisitedPixel.clear();
				for (int i = 0; i < total_pixel; i++) {
					pair<int, int> newpair = make_pair(i, 0);
					//comb.push_back(newpair);
					anotherVisitedPixel.push_back(newpair);
				}
				while (!queue.empty()) {
					queue.pop();
				}
				for (int i = 0; i < source.size(); i++) {
					vector<int> tempvector;
					tempvector.push_back(source[i]);
					queue.push(tempvector);
				}
				break;
			}
			list< pair<int, int> >::iterator itr1 = adjlist[lastelement].begin();
			while (itr1 != adjlist[lastelement].end()) {
				//printf(" -> %d(%d)", (*itr).first, (*itr).second);
				++itr1;
				if ((inspect1(pathvector, (*itr1).first)) && ((*itr1).second) > 0 && (anotherVisitedPixel[(*itr1).first].second) == 0){
					vector < int > path(pathvector);
					path.push_back((*itr1).first);
					(anotherVisitedPixel[(*itr1).first].second) = 1;
					queue.push(path);
				}
			}
		}
		if (visited == true) {
			int minNum = 99999999;
			for (int i = 0; i < route.size() - 1; i++) {
				list< pair<int, int> >::iterator itr2 = adjlist[route[i]].begin();
				while(itr2 != adjlist[route[i]].end()){
					if ((*itr2).first == route[i + 1]) {
						if ((*itr2).second < minNum) {
							minNum = (*itr2).second;
						}
					}
				}
			}

			for (int i = 0; i < route.size() - 1; i++) {
				list< pair<int, int> >::iterator itr3 = adjlist[route[i]].begin();
				while (itr3 != adjlist[route[i]].end()) {
					if ((*itr3).first == route[i + 1]) {
						(*itr3).second = (*itr3).second - minNum;

					}
				}
			}
			for (int i = 0; i < route.size() - 1; i++) {
				list< pair<int, int> >::iterator itr4 = adjlist[route[i+1]].begin();
				while (itr4 != adjlist[route[i+1]].end()) {
					if ((*itr4).first == route[i]) {
						(*itr4).second = (*itr4).second + minNum;
					}
				}

			}
		}
	}

	//coloring
	for (int i = 0; i < height; i++){
		for (int j = 0; j < width; j++){
			Vec3b pixel;
			pixel[0] = 255;
			pixel[1] = 0;
			pixel[2] = 0;
			out_image.at<Vec3b>(i, j) = pixel;
		}
	}

	Vec3b pixel2;
	pixel2[0] = 0;
	pixel2[1] = 0;
	pixel2[2] = 255;

	std::queue < int > qcolor;
	int u = source[0];
	qcolor.push(u);
	int x1, x2;
	x1 = u / width;
	x2 = u % width;
	out_image.at < Vec3b >(x1, x2) = pixel2;
	int mino = 99999999;
	BFS(qcolor, comb1, adjlist,out_image, width);
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

	in_image = imread("testimage.png"/*argv[1]*/);
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
	ifstream f("config2.txt"/*argv[2]*/);
	if (!f) {
		cout << "Could not load initial mask file!!!" << endl;
		return -1;
	}
	/*pair<int, int> sourcePair;
	pair<int, int> sinkPair;
	vector<pair<int,int>> source;
	vector<pair<int,int>> sink;*/
	vector<int> source;
	vector<int> sink;
	int width = in_image.cols;
	int height = in_image.rows;
	int total_pixel = width * height;
	int n;
	int pixelCount = 0;
	int maxIntPixel = 0;
	queue<vector<int>> queue;
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
			/*sourcePair.first = row;
			sourcePair.second = col;
			source.push_back(sourcePair);*/
			source.push_back((width*col) + row);
			//source.push_back(make_pair(x,y));
			//source.push_back((width * y) + x);
		}
		else { //background
			/*sinkPair.first = row;
			sinkPair.second = col;
			sink.push_back(sinkPair);*/
			sink.push_back((width*col) + row);
		}
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
	vector<pair<int, int>> visitedPixel, anotherVisitedPixel;
	//weight calculation
	for (int pixel = 0; pixel < total_pixel; ++pixel) {
		int x = pixel / width;
		int y = pixel % width;
		
		visitedPixel.push_back(make_pair(pixel, 0));
		anotherVisitedPixel.push_back(make_pair(pixel, 0));
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
	//printList(adjlist,visitedPixel);
	for (int i = 0; i < source.size(); i++) {
		vector<int> tempvector;
		tempvector.push_back(source[i]);
		queue.push(tempvector);
	}
	
	max_flow(adjlist, source, sink, out_image, total_pixel, visitedPixel, queue, anotherVisitedPixel, height,width);
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