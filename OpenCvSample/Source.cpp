#include <opencv2/core/utility.hpp>
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"

#include <iostream>
#include <fstream>
#include <bitset>

#include <fftw3.h>

using namespace cv;
using namespace std;

vector<char> FtoBA(string filename) {
	ifstream inputf;
	inputf.open(filename, ios::binary);
	vector<char> buffer((istreambuf_iterator<char>(inputf)), (istreambuf_iterator<char>()));
	inputf.close();
	return buffer;
}

void BAtoF(string filename, vector<char> buffer) {
	ofstream outputf;
	outputf.open(filename, ios::binary);
	for (std::vector<char>::iterator it = buffer.begin(); it != buffer.end(); ++it) outputf << static_cast<unsigned char>(bitset<8>(*it).to_ulong());
	outputf.close();
}

int main(int argc, const char** argv)
{
	//Szebinek szerkesztésre

	//ifstream inputf;
	//ofstream outputf;
	//inputf.open("beappendbinary.txt", ios::binary);
	//outputf.open("bitsappendbinary.txt", ios::binary);
	//vector<char> buffer((istreambuf_iterator<char>(inputf)),(istreambuf_iterator<char>()));
	//inputf.close();
	//for (std::vector<char>::iterator it = buffer.begin(); it != buffer.end(); ++it) outputf << bitset<8>(*it);
	//outputf.close();
	//inputf.open("bitsappendbinary.txt", ios::binary);
	//outputf.open("appendbinary.txt", ios::binary);
	//bitset<8> v8;
	//unsigned long i8;
	//if (inputf.is_open()) while (inputf.peek() != EOF) {
	//	inputf >> v8;
	//	i8=v8.to_ulong();
	//	outputf << static_cast<unsigned char>(i8);
	//}
	//inputf.close();
	//outputf.close();
	

	//Tesztfutás függvényekre

	/*BAtoF("kki.txt", FtoBA("be.txt"));*/
	


	Mat image;
	image = imread("down.png", CV_LOAD_IMAGE_COLOR);
	imshow("Test", image);

	fftw_complex *in, *out;
	fftw_plan p;

	int N = 32;

	in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);
	out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);
	p = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

	fftw_execute(p); /* repeat as needed */

	fftw_destroy_plan(p);
	fftw_free(in); fftw_free(out);

	waitKey(0);
	
	return 0;
}