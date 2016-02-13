

#include <iostream>
#include <map>
#include<conio.h>
#include <iostream>
#include <fstream>
#include <opencv2\/opencv.hpp>

void printHashTable();

using namespace std;

int **LblMat;
std::map<int, int> LblHashTable;

const int BgLbl = 1000000 + 5;;
const int BgLblVal = 1000000 + 5;
const int Ib = 0;
const int If = 255;

template <typename T>
static T **Allocate2DDynamicArray(int nRows, int nCols){
	T **dynamicArray;
	dynamicArray = new T*[nRows];
	for (int i = 0; i < nRows; i++)
		dynamicArray[i] = new T[nCols]();
	return dynamicArray;
	}

template <typename T>
void DeAllocate2DDynamicArray(T **Arr, int nRows, int nCols){
	for (int i = 0; i < nRows; i++){
		delete[] Arr[i];
		}
	delete[]Arr;
}

void initLbl(cv::Mat &src_img){
	//src_img = cv::imread(img_path.c_str(), CV_LOAD_IMAGE_GRAYSCALE);
	int rows = src_img.rows;
	int cols = src_img.cols;
	LblMat = Allocate2DDynamicArray<int>(rows, cols);

	// Assumption 1 pixel Border doesnt have any blob 

	// label the upper horiszontal border ; 
	for (int i = 0; i < cols; i++)
		LblMat[0][i] = BgLbl;
	//label the lower horizontal border ; 
	for (int i = 0; i < cols; i++)
		LblMat[rows - 1][i] = BgLbl;
	//label the left vertical border 
	for (int i = 0; i < rows; i++)
		LblMat[i][0] = BgLbl;
	//label the right vertical border 
	for (int i = 0; i < rows; i++)
		LblMat[i][cols - 1] = BgLbl;
	}

inline int getupdatedLblFirstForwardScan(int *ImgArr, int r, int c){
	const int size = 5;
	if (ImgArr[size - 1] == Ib)
		return BgLbl;

	static int plbl = 0;
	bool flag = false;

	int minHtScore = INT_MAX;
	int a, b, tmp_HtScore;

	int add = 0;
	//cheking the all pixels are Bg
	for (int i = 0; i < size - 1; i++){
		add += ImgArr[i];
	}
	if (add == 0){
		flag = true;
	}

	if (flag){
		++plbl;
		LblHashTable[plbl] = plbl;
		return plbl;
	}
	else{

		int lblval = 0;

		vector<int> vec_lbl;

		a = -1, b = -1;
		lblval = LblMat[r + a][c + b];
		if (lblval != BgLbl){
			vec_lbl.push_back(lblval);
		}
		minHtScore = LblHashTable[lblval];

		a = -1, b = 0;
		lblval = LblMat[r + a][c + b];
		if (lblval != BgLbl){
			vec_lbl.push_back(lblval);
			}
		tmp_HtScore = LblHashTable[lblval];
		if (minHtScore > tmp_HtScore)
			minHtScore = tmp_HtScore;

		a = -1, b = 1;
		lblval = LblMat[r + a][c + b];
		if (lblval != BgLbl){
			vec_lbl.push_back(lblval);
			}
		tmp_HtScore = LblHashTable[lblval];
		if (minHtScore > tmp_HtScore)
			minHtScore = tmp_HtScore;

		a = 0, b = -1;
		lblval = LblMat[r + a][c + b];
		if (lblval != BgLbl){
			vec_lbl.push_back(lblval);
		}
		tmp_HtScore = LblHashTable[lblval];
		if (minHtScore > tmp_HtScore)
			minHtScore = tmp_HtScore;

		for (int i = 0; i < vec_lbl.size(); i++){
			LblHashTable[vec_lbl[i]] = minHtScore;
		}

		return minHtScore;
	}

}

void performScans(cv::Mat &src_img){

	const int scan_arr_size = 5;
	int imgArr[scan_arr_size] = { 0 };
	int lblArr[scan_arr_size] = { 1 };
	int a = 0, b = 0;
	int ROWS = src_img.rows;
	int COLS = src_img.cols;
	int updatedlbl = 0; 
	// Initial First Scan
	for (int i = 1; i < (ROWS - 1); i++){
		for (int j = 1; j < (COLS - 1); j++){
			a = -1, b = -1;
			imgArr[0] = src_img.at<uchar>(i + a, j + b);
			
			a = -1, b = 0;
			imgArr[1] = src_img.at<uchar>(i + a, j + b);
			
			a = -1, b = 1;
			imgArr[2] = src_img.at<uchar>(i + a, j + b);
		
			a = 0, b = -1;
			imgArr[3] = src_img.at<uchar>(i + a, j + b);
	
			a = 0, b = 0;
			imgArr[4] = src_img.at<uchar>(i + a, j + b);
			
			updatedlbl = getupdatedLblFirstForwardScan(imgArr, i, j);
			LblMat[i][j] = updatedlbl;
		}
	}

	std::cout << "Initial First Scan Done"<<std::endl;
	
	bool isLblChange = false;
	a = b = 0;

	int count = 0;
	vector<int> vec_fgpxScores(5);
	int lblval, minHtScore, tmpHtScore;
	lblval = minHtScore = tmpHtScore = INT_MAX;
	while (true){
		// backward scan 
		for (int i = ROWS - 1; i >= 1; i--){
			for (int j = COLS - 1; j >= 1; j--){
				if (src_img.at<uchar>(i, j) != Ib){

					vector<int> vec_lbl;

					a = 0, b = 0;  // 1
					lblval = LblMat[i + a][j + b];
					if (lblval != BgLbl){
						vec_lbl.push_back(lblval);
						}
					minHtScore = LblHashTable[lblval];


					a = 0, b = 1; //2 
					lblval = LblMat[i + a][j + b];
					if (lblval != BgLbl){
						vec_lbl.push_back(lblval);
						}
					tmpHtScore = LblHashTable[lblval];
					if (minHtScore > tmpHtScore){
						minHtScore = tmpHtScore;
						}


					a = 1, b = -1; //3 
					lblval = LblMat[i + a][j + b];
					if (lblval != BgLbl){
						vec_lbl.push_back(lblval);
						}
					tmpHtScore = LblHashTable[lblval];
					if (minHtScore > tmpHtScore){
						minHtScore = tmpHtScore;
						}


					a = 1, b = 0; // 4 
					lblval = LblMat[i + a][j + b];
					if (lblval != BgLbl){
						vec_lbl.push_back(lblval);
						}
					tmpHtScore = LblHashTable[lblval];
					if (minHtScore > tmpHtScore){
						minHtScore = tmpHtScore;
						}

					a = 1, b = 1; //5 
					lblval = LblMat[i + a][j + b];
					if (lblval != BgLbl){
						vec_lbl.push_back(lblval);
						}
					tmpHtScore = LblHashTable[lblval];
					if (minHtScore > tmpHtScore){
						minHtScore = tmpHtScore;
						}

					if (LblMat[i][j] != minHtScore){
						LblMat[i][j] = minHtScore;
						isLblChange = true;
						}

					//update the hash table
					for (int i = 0; i < vec_lbl.size(); i++){
						LblHashTable[vec_lbl[i]] = minHtScore;
					}

				} // end of if else 
			}
		}
		
		// forward scan 
		a = b = 0;
		for (int i = 1; i < (ROWS - 1); i++){
			for (int j = 1; j < (COLS - 1); j++){
				if (src_img.at<uchar>(i, j) != Ib){

					vector<int> vec_lbl;

					a = -1, b = -1;   //1 
					lblval = LblMat[i + a][j + b];
					if (lblval != BgLbl){
						vec_lbl.push_back(lblval);
						}
					minHtScore = LblHashTable[lblval];

					a = -1, b = 0;   //2 
					lblval = LblMat[i + a][j + b];
					if (lblval != BgLbl){
						vec_lbl.push_back(lblval);
						}
					tmpHtScore = LblHashTable[lblval];
					if (minHtScore > tmpHtScore){
						minHtScore = tmpHtScore;
						}

					a = -1, b = 1;   //3
					lblval = LblMat[i + a][j + b];
					if (lblval != BgLbl){
						vec_lbl.push_back(lblval);
						}
					tmpHtScore = LblHashTable[lblval];
					if (minHtScore > tmpHtScore){
						minHtScore = tmpHtScore;
						}

					a = 0, b = -1;  //4
					lblval = LblMat[i + a][j + b];
					if (lblval != BgLbl){
						vec_lbl.push_back(lblval);
						}
					tmpHtScore = LblHashTable[lblval];
					if (minHtScore > tmpHtScore){
						minHtScore = tmpHtScore;
						}

					a = 0, b = 0;  //5
					lblval = LblMat[i + a][j + b];
					if (lblval != BgLbl){
						vec_lbl.push_back(lblval);
						}
					tmpHtScore = LblHashTable[lblval];
					if (minHtScore > tmpHtScore){
						minHtScore = tmpHtScore;
						}

					if (LblMat[i][j] != minHtScore){
						LblMat[i][j] = minHtScore;
						isLblChange = true;
						}

					//update the hash table with min
					for (int i = 0; i < vec_lbl.size(); i++){
						LblHashTable[vec_lbl[i]] = minHtScore;
						}
					}
				}
		} // end of for 
		
		cout << ++count << " " << isLblChange << endl;
		if (!isLblChange){
			break;
		}

		isLblChange = false;

	} // end of while 

}

void create_testImg(){
	cv::Mat src_img = cv::Mat::zeros(6, 8, CV_8UC1);

	src_img.at<uchar>(1, 1) = 255;
	src_img.at<uchar>(1, 2) = 255;
	src_img.at<uchar>(2, 1) = 255;
	src_img.at<uchar>(2, 2) = 255;

	src_img.at<uchar>(3, 5) = 255;
	src_img.at<uchar>(3, 6) = 255;
	src_img.at<uchar>(4, 5) = 255;
	src_img.at<uchar>(4, 6) = 255;
	imwrite("src_img.png", src_img);
	}

void printHashTable(){
	//update the hash table
	for (std::map<int, int>::iterator it = LblHashTable.begin(); it != LblHashTable.end(); ++it){
		std::cout << it->first << " " << it->second << endl;
		}
	}

void getBlobsLabels(vector<int> &blobLbls){

	int max_map = 0;
	for (std::map<int, int>::iterator it = LblHashTable.begin(); it != LblHashTable.end(); ++it){
		if (it->first != BgLbl && it->second > max_map){
			max_map = it->second;
			}
	}

	vector<bool> lbl_flags(max_map + 1, false);
	for (std::map<int, int>::iterator it = LblHashTable.begin(); it != LblHashTable.end(); ++it){
		switch (it->second){
			case 0:
				break;
			case BgLblVal:
				break;
			default:
				if (!lbl_flags[it->second]){
					blobLbls.push_back(it->second);
					lbl_flags[it->second] = true;
				}
		}
	}

}

int main(){

	LblHashTable[BgLbl] = BgLblVal;
	LblHashTable[0] = 0;

	cv::Mat src_img;

	src_img = cv::imread("Conformity1.tif", CV_LOAD_IMAGE_GRAYSCALE);

	int rows = src_img.rows;
	int cols = src_img.cols;

	initLbl(src_img);
	performScans(src_img);
	vector<int> lbls;
	
	std::cout << "Count Blobs" << endl;
	getBlobsLabels(lbls);
#if 0 
	for (int i = 0; i < lbls.size(); i++){
		std::cout << "Labels = " << lbls[i] << endl;
	}
#endif 
	std::cout << "No of Blobs= " << lbls.size() << endl;

	DeAllocate2DDynamicArray<int>(LblMat, rows, cols);

	std::cout << "Press Enter to Exit" << endl;
	getchar();
	return 0;
}
