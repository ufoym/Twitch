#include <vector>
#include "opencv2/opencv.hpp"
#include "opencv2/stitching.hpp"


void detectCameras(const int max_id = 8) {
	cv::Mat frame;
	for (int i = 0; i < max_id; ++i) {
		cv::VideoCapture cap(i);
		if (cap.isOpened())
			if (cap.read(frame))
				cv::imshow(std::to_string(i), frame);
	}
	cv::waitKey(0);
	cv::destroyAllWindows();
}

class Twicam
{
public:
	Twicam(const int devL, const int devR) {
		capL = cv::VideoCapture(devL);
		capR = cv::VideoCapture(devR);
		if (!capL.isOpened() || !capR.isOpened()) {
			std::cout << "[Error] Cannot open cameras!" << std::endl;
			isOpened = false;
		}
		else {
			isOpened = true;
		}
	}
	~Twicam() {
		capL.release();
		capR.release();
	}

	bool next(cv::Mat & frameL, cv::Mat & frameR) {
		if (!capL.read(frameL) || !capR.read(frameR)) {
			std::cout << "[Error] Cannot get frames!" << std::endl;
			return false;
		}
		else {
			return true;
		}
	}

private:
	cv::VideoCapture capL;
	cv::VideoCapture capR;
	bool isOpened;
};

#include <ctime>

class FPS
{
protected:
	unsigned int m_fpscount;
	unsigned int m_start;
	float m_interval;
	float m_fps;

public:
	FPS() : m_interval(5), m_fpscount(0), m_fps(-1) {}
	void update() {
		m_fpscount++;
		float time_interval = float(clock() - m_start) / CLOCKS_PER_SEC;
		if (time_interval > m_interval) {
			m_fps = m_fpscount / time_interval;
			m_fpscount = 0;
			m_start = clock();
		}
		if (m_fps > 0) {
			std::cout << "FPS: " << m_fps << std::endl;
		}
	}
};


int main(int argc, char* argv[])
{
	detectCameras();
	FPS fps;
	cv::Mat pano;
	cv::Mat frameL, frameR;
	Twicam twicam(0, 2);
	cv::Stitcher stitcher = cv::Stitcher::createDefault(false);
	while (twicam.next(frameL, frameR)) {
		std::vector<cv::Mat> imgs = { frameL, frameR };
		cv::Stitcher::Status status = stitcher.stitch(imgs, pano);
		fps.update();
		if (status != cv::Stitcher::OK) {
			std::cout << "[Error] Cannot stitch! Error code = " << int(status) << std::endl;
			return -1;
		}
		cv::imshow("L", frameL);
		cv::imshow("R", frameR);
		cv::imshow("pano", pano);
		cv::waitKey(10);
	}
	return 0;
}