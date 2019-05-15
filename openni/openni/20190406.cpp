#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string>
#include "Getsetoff.h"
#include "OpenNI.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/highgui/highgui_c.h>
#include "opencv2/imgproc/imgproc.hpp"
using namespace std;
using namespace cv;
using namespace openni;


int main(int argc, char** argv)
{
	//Get_setoff(So);
	//cout << "after rotation : " << "(" << So[0] << ", " << So[1] << ", " << So[2] << ")" << endl;
	ofstream OutFile("C:/Users/Kkoutianwu/Desktop/整合/setoff.txt");
	OutFile << 0 << ", " << 0 << ", " << 35;
	OutFile.close();
	MyClass my;
	my.Get_setoff();
	cout << "after rotation : " << "(" << my.tranxyz[0] << ", " << my.tranxyz[1] << ", " << my.tranxyz[2] << ")" << endl;
	while (1);
	return 0;

}


MyClass::MyClass()
{
}

MyClass::~MyClass()
{
}

void MyClass::onMouse(int events, int x, int y, int, void* userdata)
{
	MyClass* temp = reinterpret_cast<MyClass*>(userdata);
	temp->on_Mouse(events, x, y);

}

void MyClass::on_Mouse(int events, int x, int y)
{
	switch (events)
	{
	case CV_EVENT_LBUTTONDOWN:
		xtag = x;
		ytag = y;
		cout << "(" << y << "," << x << ")" << endl;
		//e = 1;
		break;
	case CV_EVENT_MOUSEMOVE:
		//your code here
		break;
	case CV_EVENT_LBUTTONUP:
		//your code here
		break;
	}
}
void MyClass::Get_setoff(void)
{

	rotation[0][0] = 0;
	rotation[0][1] = 0;
	rotation[0][2] = -1;

	rotation[1][0] = -1;
	rotation[1][1] = 0;
	rotation[1][2] = 0;

	rotation[2][0] = 0;
	rotation[2][1] = 1;
	rotation[2][2] = 0;
	Status result = STATUS_OK;

	//OpenNI2 
	VideoFrameRef oniDepthImg;
	VideoFrameRef oniColorImg;

	//OpenCV 
	cv::Mat cvDepthImg;
	cv::Mat cvBGRImg;
	cv::Mat cvFusionImg;

	cv::namedWindow("depth");
	cv::namedWindow("image");
	cv::namedWindow("融合");
	char key = 0;

	// 初始化
	result = OpenNI::initialize();

	// 打开摄像头
	Device device;
	result = device.open(openni::ANY_DEVICE);


	// 创建视频流
	VideoStream oniDepthStream;
	result = oniDepthStream.create(device, openni::SENSOR_DEPTH);

	//
	//深度图片的模式设置
	VideoMode modeDepth;
	modeDepth.setResolution(640, 480);
	modeDepth.setFps(30);
	modeDepth.setPixelFormat(PIXEL_FORMAT_DEPTH_1_MM);
	oniDepthStream.setVideoMode(modeDepth);
	// start depth stream
	result = oniDepthStream.start();


	VideoStream oniColorStream;//视频流句柄
	result = oniColorStream.create(device, openni::SENSOR_COLOR);//初始化的方法必须有一个初始化过后的设备对象
	// 色彩流的模式
	VideoMode modeColor;
	modeColor.setResolution(640, 480);
	modeColor.setFps(40);
	modeColor.setPixelFormat(PIXEL_FORMAT_RGB888);
	oniColorStream.setVideoMode(modeColor);


	// 设置数据对齐模式
	if (device.isImageRegistrationModeSupported(IMAGE_REGISTRATION_DEPTH_TO_COLOR))
	{
		device.setImageRegistrationMode(IMAGE_REGISTRATION_DEPTH_TO_COLOR);
	}
	// 彩色视频流 
	result = oniColorStream.start();

	while (key != 27)
	{

		// read frame

		if (oniColorStream.readFrame(&oniColorImg) == STATUS_OK)//轮训的读取方式，将视频帧读到oniColorImg里面去
		{
			// openni的默认是rgb，需要转换到bgr中去
			cv::Mat cvRGBImg(oniColorImg.getHeight(), oniColorImg.getWidth(), CV_8UC3, (void*)oniColorImg.getData());
			cv::cvtColor(cvRGBImg, cvBGRImg, CV_RGB2BGR);
			cv::imshow("image", cvBGRImg);
		}
		ifstream readFile("C:/Users/Kkoutianwu/Desktop/整合/flag.txt");//读标志位文件		
		readFile >> temp; //遇到空格输出停止，空格后的内容无法输出
		cout << temp << endl;
		readFile.close();
		if (!strcmp(temp , "save_image"))//保存图片
		{
			imwrite("C:/Users/Kkoutianwu/Desktop/整合/BGR.png", cvBGRImg);
			ofstream OutFile("C:/Users/Kkoutianwu/Desktop/整合/flag.txt");//利用构造函数创建txt文本，并且打开该文本
			OutFile << "Don't";
			OutFile.close();

		}
		if (oniDepthStream.readFrame(&oniDepthImg) == STATUS_OK)
		{

			float depthv;
			cv::Mat cvRawImg16U(oniDepthImg.getHeight(), oniDepthImg.getWidth(), CV_16UC1, (void*)oniDepthImg.getData());
			cvRawImg16U.convertTo(cvDepthImg, CV_8U, 255.0 / (oniDepthStream.getMaxPixelValue()));

			// 深度图片的gray转换到彩色的
			cv::cvtColor(cvDepthImg, cvFusionImg, CV_GRAY2BGR);
			cv::imshow("depth", cvDepthImg);
			DepthPixel* pDepth = (DepthPixel*)oniDepthImg.getData();
			//DepthPixel* pDepth = (DepthPixel*)frame.getData();

			//cv::setMouseCallback("image", MyClass::onMouse, this);
			ifstream readFile("C:/Users/Kkoutianwu/Desktop/整合/2Dsetoff.txt");//读标志位文件		
			readFile.getline(tempa, 8, 0);  //遇到空格输出停止，空格后的内容无法输出   
			if (strcmp(tempb, tempa))
			{
				istringstream istr1(tempa);
				istr1 >> xtag >> ytag;
				cout << xtag << ytag << endl;
			}
			else
			{
				xtag = 0;
				ytag = 0;

			}
			readFile.close();
			strcpy(tempb, tempa);
			if (xtag != 0 && ytag != 0 )
			{
				float x = 2, y = 0, z = 0;
				depthv = pDepth[ytag*oniDepthImg.getWidth() + xtag];
				cout << depthv << endl;
				//CoordinateConverter::convertDepthToWorld(oniColorStream, (float)xtag, (float)ytag, depthv, &x, &y, &z);
				CoordinateConverter::convertDepthToWorld(oniDepthStream, (float)xtag, (float)ytag, depthv, &x, &y, &z);
				z -= 1200;
				// xyzdat[xtag-1][ytag-1][0]= x; xyzdat[xtag - 1][ytag - 1][1] = y; xyzdat[xtag - 1][ytag - 1][2] = z;
				cout << "kinect : " << "(" << x << ", " << y << ", " << z << ")" << endl;
				for (int i = 0; i <= 2; i++)
				{
					tranxyz[i] = x * rotation[i][0] + y * rotation[i][1] + z * rotation[i][2];
				}
				xtag = 0;
				ytag = 0;
				ofstream OutFile("C:/Users/Kkoutianwu/Desktop/整合/setoff.txt");
				OutFile << tranxyz[0] / 10 << ", " << tranxyz[1] / 10 << ", " << tranxyz[2] / 10;
				OutFile.close();
				//break;
			}
		}
		//融合
		cv::addWeighted(cvBGRImg, 0.7, cvFusionImg, 0.3, 0, cvFusionImg);
		cv::imshow("融合", cvFusionImg);
		key = cv::waitKey(20);

	}

	//cv destroy
	cv::destroyWindow("depth");
	cv::destroyWindow("image");
	cv::destroyWindow("fusion");

	//OpenNI2 的销毁
	oniDepthStream.destroy();
	oniColorStream.destroy();
	device.close();//设备关闭
	OpenNI::shutdown();
	cvDestroyAllWindows();
}

