#include <stdlib.h>
#include <iostream>
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include <XnCppWrapper.h>
#include <windows.h>

using namespace std;
using namespace cv;

// output for XnPoint3D
ostream& operator<<( ostream& out, const XnPoint3D& rPoint )
{
	out << "(" << rPoint.X << "," << rPoint.Y << "," << rPoint.Z << ")";
	return out;
}

// callback function for gesture recognized
void XN_CALLBACK_TYPE gestureRecog( xn::GestureGenerator &generator,
								   const XnChar *strGesture,
								   const XnPoint3D *pIDPosition,
								   const XnPoint3D *pEndPosition,
								   void *pCookie )
{
	cout << strGesture << " from " << *pIDPosition << " to " << *pEndPosition << endl;

	int imgStartX = 0;
	int imgStartY = 0;
	int imgEndX = 0;
	int imgEndY = 0;
	char locationinfo[100];

	imgStartX=(int)(640/2-(pIDPosition->X));
	imgStartY=(int)(480/2-(pIDPosition->Y));
	imgEndX=(int)(640/2-(pEndPosition->X));
	imgEndY=(int)(480/2-(pEndPosition->Y));

	IplImage* refimage=(IplImage*)pCookie;
	if(strcmp(strGesture,"RaiseHand")==0)
	{
		cvCircle(refimage,cvPoint(imgStartX,imgStartY),1,CV_RGB(255,0,0),2);
	}
	else if(strcmp(strGesture,"Wave")==0)
	{
		cvLine(refimage,cvPoint(imgStartX,imgStartY),cvPoint(imgEndX,imgEndY),CV_RGB(255,255,0),6);
		//system("notepad++ hello.txt");
	}
	else if(strcmp(strGesture,"Click")==0)
	{
		cvCircle(refimage,cvPoint(imgStartX,imgStartY),6,CV_RGB(0,0,255),12);
		//system("QQMusic");
	}

	cvSetImageROI(refimage,cvRect(40,450,640,30));
	CvFont font;
	cvInitFont( &font, CV_FONT_VECTOR0,1, 1, 0, 3, 5);
	cvSet(refimage, cvScalar(255,255,255));
	sprintf(locationinfo,"From: %d,%d to %d,%d",(int)pIDPosition->X,(int)pIDPosition->Y,(int)(pEndPosition->X),(int)(pEndPosition->Y));
	cvPutText(refimage, locationinfo ,cvPoint(30, 30), &font, CV_RGB(0,0,0));
	cvResetImageROI(refimage);
}

void clearImg(IplImage* inputimg)
{
	CvFont font;
	cvInitFont( &font, CV_FONT_VECTOR0 ,1, 1, 0, 3, 5);
	memset(inputimg->imageData,255,640*480*3);
	cvPutText(inputimg, "Hand Raise!" ,cvPoint(20, 20), &font, CV_RGB(255,0,0));
	cvPutText(inputimg, "Hand Wave!" , cvPoint(20, 50), &font, CV_RGB(255,255,0));
	cvPutText(inputimg, "Hand Push!" , cvPoint(20, 80), &font, CV_RGB(0,0,255));
	//cvPutText(inputimg, "Hand Moving!" , cvPoint(20, 110), &font, CV_RGB(200,100,255));
}

// callback function for gesture progress
void XN_CALLBACK_TYPE gestureProgress( xn::GestureGenerator &generator,
									  const XnChar *strGesture,
									  const XnPoint3D *pPosition,
									  XnFloat fProgress,
									  void *pCookie )
{
	cout << strGesture << ":" << fProgress << " at " << *pPosition << endl;
}


int main( int argc, char** argv )
{
	//IplImage* drawPadImg=cvCreateImage(cvSize(640,480),IPL_DEPTH_8U,3);
	IplImage* cameraImg=cvCreateImage(cvSize(640,480),IPL_DEPTH_8U,3);

	//cvNamedWindow("Gesture",1);
	cvNamedWindow("Camera",1);

	clearImg(cameraImg);

	XnStatus res;
	char key=0;

	// context
	xn::Context context;
	res = context.Init();
	xn::ImageMetaData imgMD;

	// create generator 
	xn::ImageGenerator imageGenerator;
	res = imageGenerator.Create( context ); 
	
	xn::GestureGenerator gestureGenerator;
	res = gestureGenerator.Create( context );

	// Add gesture
	//gestureGenerator.AddGesture( "MovingHand", NULL );
	gestureGenerator.AddGesture( "Wave", NULL );
	gestureGenerator.AddGesture( "Click", NULL );
	gestureGenerator.AddGesture( "RaiseHand", NULL );
	//gestureGenerator.AddGesture("MovingHand",NULL);

	//Register callback functions of gesture generator
	XnCallbackHandle handle;
	gestureGenerator.RegisterGestureCallbacks( gestureRecog, gestureProgress, (void*)cameraImg, handle );

	//start generate data
	context.StartGeneratingAll();
	res = context.WaitAndUpdateAll();  

	while( (key!=27) && !(res = context.WaitAndUpdateAll())  ) 
	{  
		if(key=='c')
		{
			clearImg(cameraImg);
		}

		imageGenerator.GetMetaData(imgMD);
		memcpy(cameraImg->imageData,imgMD.Data(),640*480*3);
		cvCvtColor(cameraImg,cameraImg,CV_RGB2BGR);

		//cvShowImage("Gesture",drawPadImg);
		cvShowImage("Camera",cameraImg);

		key=cvWaitKey(20);

	}
	//cvDestroyWindow("Gesture");
	cvDestroyWindow("Camera");
	//cvReleaseImage(&drawPadImg);
	cvReleaseImage(&cameraImg);
	context.StopGeneratingAll();
	context.Shutdown();

	return 0;
}