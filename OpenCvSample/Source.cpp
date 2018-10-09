#include <opencv2/core/utility.hpp>
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"

using namespace cv;
int main(int argc, const char** argv)
{
	Mat image;

	image = imread("down.png", CV_LOAD_IMAGE_COLOR);

	imshow("Test", image);
	waitKey(0);
	return 0;
}