#include "CMT.h"
#include "KinectVideoCapture.h"
#include <cstdio>

//#define DEBUG_MODE
#define USE_KINECTV2 1

#ifdef DEBUG_MODE
#include <QDebug>
#endif

cv::Point current_pos, tl, br;
bool topleft_selected;
bool released_once;
bool bottomright_selected;

void num2str(char *str, int length, int num)
{
    for(int i = 0; i < length-1; i++)
    {
        str[length-i-2] = '0'+num%10;
        num /= 10;
    }
    str[length-1] = 0;
}

void mouseHandler(int event, int x, int y, int flags, void* param)
{
	current_pos.x = x;
	current_pos.y = y;
	
	if (topleft_selected  && event != CV_EVENT_LBUTTONDOWN && !flags)
	{
		released_once = true;
	}

	if (event == CV_EVENT_LBUTTONDOWN && flags)
	{
		if (topleft_selected == false){
			tl = current_pos;
			topleft_selected = true;
			std::cout << "Top Left: " << tl << std::endl;
		}
		else if (released_once){
			bottomright_selected = true;
			br = current_pos;
			std::cout << "Bottom Right: " << br << std::endl;
		}
	}
}

bool getROI(cv::Point2f& topLeft, cv::Point2f& bottomRight, cv::Mat& capImage){
	bool ret = false;
#ifdef USE_KINECTV2
	KinectVideoCapture cap = KinectVideoCapture(0);
#else
	cv::VideoCapture cap = cv::VideoCapture(0);
#endif
	cv::Mat img;
	cv::Mat imDraw;
	std::string title("get_rect");
	int k;
	if (!cap.isOpened()){
		printf("Cannot read from video input device\n");
		return ret;
	}
	Sleep(3000);
	cv::namedWindow(title);
	cv::moveWindow(title, 100, 100);
	cv::setMouseCallback(title, mouseHandler, 0);
	cap >> img;
	cv::imshow(title, img);
	while (!bottomright_selected)
	{
		img.copyTo(imDraw);
		if (topleft_selected){
			cv::rectangle(imDraw, tl, current_pos, cv::Scalar(255, 0, 0));
		}
		cv::imshow(title, imDraw);
		k = cv::waitKey(1);
		if (k == 27)
		{
			break;
		}
	}
	cv::destroyWindow(title);
	if (topleft_selected && bottomright_selected){
		topLeft = tl;
		bottomRight = br;
		capImage = img;
		ret = true;
	}
	return ret;
}

int main(int argc, char *argv[])
{

	char *path = "sequences/cokecan/img";
	char *ext = "png";
	int numLength = 5;
	int key;
	std::string title = "track";
	//char numString[numLength+1];
	char numString[6];
	char filename[255];
	int start = 0;
	int end = 291;
	cv::Point2f initTopLeft(150, 82);
	cv::Point2f initBottomDown(170, 118);
	cv::Mat initImage;
	CMT cmt;
	if (getROI(initTopLeft, initBottomDown, initImage)){
		cv::Mat im_gray;
		cv::cvtColor(initImage, im_gray, CV_RGB2GRAY);
		cmt.initialise(im_gray, initTopLeft, initBottomDown);
	}

	try{
#ifdef USE_KINECTV2
		KinectVideoCapture cap = KinectVideoCapture(0);
#else
		cv::VideoCapture cap = cv::VideoCapture(0);
#endif
		cv::Mat capImg;
		cv::Mat im_gray;
		//cmt.estimateRotation = false;
		while (1)
		{
			cap >> capImg;
			cv::cvtColor(capImg, im_gray, CV_RGB2GRAY);
			cmt.processFrame(im_gray);

			for (int i = 0; i < cmt.trackedKeypoints.size(); i++)
				cv::circle(capImg, cmt.trackedKeypoints[i].first.pt, 3, cv::Scalar(255, 255, 255));
			cv::line(capImg, cmt.topLeft, cmt.topRight, cv::Scalar(255, 255, 255));
			cv::line(capImg, cmt.topRight, cmt.bottomRight, cv::Scalar(255, 255, 255));
			cv::line(capImg, cmt.bottomRight, cmt.bottomLeft, cv::Scalar(255, 255, 255));
			cv::line(capImg, cmt.bottomLeft, cmt.topLeft, cv::Scalar(255, 255, 255));
			cv::imshow(title, capImg);
			key = cv::waitKey(1);
			if (key == 27)
			{
				break;
			}
		}
	}
	catch (cv::Exception& ex){
		puts(ex.msg.c_str());
	}
	return 0;
}
