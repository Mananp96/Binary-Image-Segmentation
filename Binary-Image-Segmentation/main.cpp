#include <opencv2/opencv.hpp>
#include<fstream>
#include <utility>
#include<queue>
#include<vector>

using namespace cv;
using namespace std;

bool *ispixelChecked;
int maxIntPixel = 0;
vector < pair < int, int > > differMatrix2;

int inspectelement(vector < int > new_vector, int a) {
	int size = new_vector.size();
	for (int i = 0; i < size; i++)
		if (new_vector[i] == a)
			return 0;
	return 1;
}
int inspect(vector<int> sinkNodes, int l) {
	int size = sinkNodes.size();
	for (int i = 0; i < size; i++) {
		if (sinkNodes[i] == l) {
			return 1;
		}
	}
	return 0;
}

void addIntoQueue(queue < vector < int > > &Queue, vector<int> &sourceNodes) {
	for (int i = 0; i < sourceNodes.size(); i++) {
		vector<int> temp_vector;
		temp_vector.push_back(sourceNodes[i]);
		Queue.push(temp_vector);
	}
}

void coloring(int imageHeight, int imageWidth, Mat &out_image, vector<int> &sinkNodes, vector<int> &sourceNodes, queue < int > &colored_pixels) {
	for (int w = 0; w < imageHeight; w++) {
		for (int h = 0; h < imageWidth; h++) {
			Vec3b color;
			color[0] = 0;
			color[1] = 0;
			color[2] = 0;
			out_image.at<Vec3b>(w, h) = color;
		}
	}
	int firstNode = sourceNodes[0];
	Vec3b color2;
	color2[0] = 255;
	color2[1] = 255;
	color2[2] = 255;

	colored_pixels.push(firstNode);
	int x, y;
	x = firstNode / imageWidth;
	y = firstNode % imageWidth;
	out_image.at < Vec3b >(x, y) = color2;
}

void BFS(Mat &out_image, vector < pair < int, int > >  *weightVector, int imageWidth,
	vector < pair < int, int > > differMatrix2, queue < int > &colored_pixels) {
	while (!colored_pixels.empty()) {
		int firstPixel = colored_pixels.front();
		colored_pixels.pop();
		for (int i = 0; i < weightVector[firstPixel].size(); ++i) {
			if (differMatrix2[weightVector[firstPixel][i].first].second == 0 && weightVector[firstPixel][i].second > 0) {
				Vec3b color2;
				color2[0] = 255;
				color2[1] = 255;
				color2[2] = 225;
				int x3, x4;
				x3 = (weightVector[firstPixel][i].first) / imageWidth;
				x4 = (weightVector[firstPixel][i].first) % imageWidth;
				out_image.at < Vec3b >(x3, x4) = color2;
				colored_pixels.push(weightVector[firstPixel][i].first);
				differMatrix2[weightVector[firstPixel][i].first].second = 1;
			}
		}
	}
}
bool bfs2(vector<int> &sinkNodes,vector<int> &sourceNodes, vector < pair < int, int > > &differMatrix,
	int total_pixel, vector < pair < int, int > >  *weightVector, vector < int > route) {
	queue < vector < int > > Queue;
	memset(ispixelChecked, 0, sizeof(ispixelChecked));
	while (!Queue.empty()) {
		Queue.pop();
	}
	addIntoQueue(Queue, sourceNodes);
	vector < int > pathVector;
	while (!Queue.empty()) {
		pathVector = Queue.front();
		Queue.pop();
		int lastPixel = pathVector.back();
		
		for (int i = 0; i < weightVector[lastPixel].size(); ++i) {
			if (!ispixelChecked[weightVector[lastPixel][i].first] && inspectelement(pathVector, weightVector[lastPixel][i].first) && weightVector[lastPixel][i].second > 0 && differMatrix[weightVector[lastPixel][i].first].second == 0) {
				vector < int > temp_vector(pathVector);
				temp_vector.push_back(weightVector[lastPixel][i].first);
				differMatrix[weightVector[lastPixel][i].first].second = 1;
				ispixelChecked[weightVector[lastPixel][i].first] = true;
				Queue.push(temp_vector);
			}
		}
		if (inspect(sinkNodes, lastPixel) == 1) {
			route.clear();
			route = pathVector;
			pathVector.clear();
			differMatrix.clear();
			for (int i = 0; i < total_pixel; i++) {
				differMatrix.push_back(make_pair(i, 0));
				ispixelChecked[i] = false;
			}
			return true;
		}
	}
}

//implementation of max_flow min_cut algorithm
void maxFlow_minCut(vector<int> &sinkNodes, vector<int> &sourceNodes, vector < pair < int, int > > &differMatrix,
	int total_pixel, vector < pair < int, int > >  *weightVector, int imageHeight, int imageWidth, Mat &out_image) {

	
	queue < int > colored_pixels;
	vector < int > route;
	while (bfs2(sinkNodes, sourceNodes, differMatrix,total_pixel, weightVector, route)) {
		
			int MINN = 200;
			for (int i = 0; i < route.size() - 1; i++) {
				for (int j = 0; j < weightVector[route[i]].size(); j++) {
					if (weightVector[route[i]][j].first == route[i + 1]) {
						MINN = min(MINN, weightVector[route[i]][j].second);
					}
				}

			}

			for (int i = 0; i < route.size() - 1; i++) {
				for (int j = 0; j < weightVector[route[i]].size(); j++) {
					if (weightVector[route[i]][j].first == route[i + 1]) {
						weightVector[route[i]][j].second = weightVector[route[i]][j].second - MINN;
					}
					if (weightVector[route[i + 1]][j].first == route[i]) {
						weightVector[route[i + 1]][j].second = weightVector[route[i + 1]][j].second + MINN;
					}
				}
			}
		
	}
	coloring(imageHeight, imageWidth, out_image, sinkNodes, sourceNodes, colored_pixels);
	BFS(out_image, weightVector, imageWidth, differMatrix2, colored_pixels);
}


//Covert the image to BW/grayscale image.
void convertToGrayscale(Mat &in_image, Mat &gray_image, Mat &gradient_x, Mat &gradient_y) {
	GaussianBlur(in_image, in_image, Size(3, 3), 0, 0, BORDER_DEFAULT);
	cvtColor(in_image, gray_image, COLOR_BGR2GRAY);
	Scharr(gray_image, gradient_x, CV_16S, 1, 0, 1, 0, BORDER_DEFAULT);
	convertScaleAbs(gradient_x, gradient_x);
	Scharr(gray_image, gradient_y, CV_16S, 0, 1, 1, 0, BORDER_DEFAULT);
	convertScaleAbs(gradient_y, gradient_y);
}

void findMaxIntPixel(int imageHeight, int imageWidth, Mat gray_image) {
	for (int i = 0; i < imageHeight; i++) {
		for (int j = 0; j < imageWidth; j++) {
			int currentPixel = gray_image.at < uchar >(i, j);
			maxIntPixel = max(currentPixel, maxIntPixel);
		}
	}
}

// adds the edge between two pixels
void setEdge(vector< pair<int, int> >  *adjlist, int k, int v, int weight) {
	adjlist[k].push_back(make_pair(v, weight));
}

//main function
int main(int argc, char * * argv) {
	if (argc != 4) {
		cout << "Usage:../seg input_image initialization_file output_mask" << endl;
		return -1;
	}

	Mat in_image;
	in_image = imread(argv[1] /*, CV_LOAD_IMAGE_COLOR*/);

	if (!in_image.data) {
		cout << "Could not load input image!!!" << endl;
		return -1;
	}

	if (in_image.channels() != 3) {
		cout << "Image does not have 3 channels!!! " << in_image.depth() << endl;
		return -1;
	}

	Mat out_image = in_image.clone();
	ifstream configurationFile(argv[2]);

	if (!configurationFile) {
		cout << "Could not load initial mask file!!!" << endl;
		return -1;
	}

	int imageWidth = in_image.cols;
	int imageHeight = in_image.rows;
	int total_pixel = imageHeight * imageWidth;
	Mat gray_image, gradient_image;
	Mat gradient_x, gradient_y;

	//convert image to BW.
	convertToGrayscale(in_image, gray_image, gradient_x, gradient_y);
	addWeighted(gradient_x, 0.5, gradient_y, 0.5, 0, gradient_image);

	//finding maximum intensity pixel
	findMaxIntPixel(imageHeight, imageWidth, gray_image);

	ispixelChecked = new bool[total_pixel];
	vector < pair < int, int > > differMatrix;
	vector < pair < int, int > >   *weightVector;
	weightVector = new vector < pair < int, int > >[total_pixel];
	/*weightVector.resize(total_pixel);*/
	//calculate weights
	for (int pixel = 0; pixel < total_pixel; pixel++) {
		int x = pixel / imageWidth;
		int y = pixel % imageWidth;
		ispixelChecked[pixel] = false;
		differMatrix.push_back(make_pair(pixel, 0));
		differMatrix2.push_back(make_pair(pixel, 0));
		if (pixel - 1 >= 0 && y - 1 >= 0) {
			int leftPixelWeight = maxIntPixel - ((gradient_image.at<uchar>(x, y) + gradient_image.at<uchar>(x, y - 1)) / 2);
			setEdge(weightVector, pixel, (x*imageWidth) + (y - 1), leftPixelWeight);
		}
		if (pixel + 1 < total_pixel && y + 1 < imageWidth) {
			int rightPixelWeight = maxIntPixel - ((gradient_image.at<uchar>(x, y) + gradient_image.at<uchar>(x, y + 1)) / 2);
			setEdge(weightVector, pixel, (x*imageWidth) + (y + 1), rightPixelWeight);
		}

		if (x - 1 >= 0 && pixel - imageWidth >= 0) {
			int upPixelWeight = maxIntPixel - ((gradient_image.at<uchar>(x, y) + gradient_image.at<uchar>(x - 1, y)) / 2);
			setEdge(weightVector, pixel, (((x - 1)*imageWidth) + y), upPixelWeight);
		}

		if (x + 1 < imageHeight && pixel + imageWidth < total_pixel) {
			int downPixelWeight = maxIntPixel - ((gradient_image.at<uchar>(x, y) + gradient_image.at<uchar>(x + 1, y)) / 2);
			setEdge(weightVector, pixel, (((x + 1)*imageWidth) + y), downPixelWeight);
		}

	}


	//finding average of all weight edge 
	int total_edges = 0;
	int total_weight = 0;
	for (int i = 0; i < total_pixel; i++) {
		for (int j = 0; j < weightVector[i].size(); j++) {
			total_weight = total_weight + weightVector[i][j].second;
			total_edges++;
		}
	}

	int average_weight = total_weight / total_edges;
	for (int i = 0; i < total_pixel; i++) {
		for (int j = 0; j < weightVector[i].size(); j++) {
			if (weightVector[i][j].second < average_weight) {
				weightVector[i][j].second = 1;
			}
			else {
				weightVector[i][j].second = 100;
			}
		}
	}
	int n;
	configurationFile >> n;
	vector<int> sourceNodes;
	vector<int> sinkNodes;
	for (int i = 0; i < n; ++i) {
		int x_axis, y_axis, w;
		configurationFile >> x_axis >> y_axis >> w;
		if (x_axis < 0 || x_axis >= imageWidth || y_axis < 0 || y_axis >= imageHeight) {
			cout << "Invalid pixel mask!" << endl;
			return -1;
		}
		int position = (imageWidth * y_axis) + x_axis;
		if (w == 1) {//sourceNodes nodes
			sourceNodes.push_back(position);
		}
		else {//sinkNodes nodes
			sinkNodes.push_back(position);
		}
	}

	// min cut max flow 
	maxFlow_minCut(sinkNodes, sourceNodes, differMatrix, total_pixel, weightVector, imageHeight, imageWidth, out_image);
	imwrite(argv[3], out_image);
	// also display them both
	namedWindow("Original image", WINDOW_AUTOSIZE);
	namedWindow("Show Marked Pixels", WINDOW_AUTOSIZE);
	imshow("Original image", in_image);
	imshow("Show Marked Pixels", out_image);
	waitKey(0);
	return 0;
}