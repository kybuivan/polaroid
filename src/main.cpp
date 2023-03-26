#include <iostream>
#include <string>
#include <vector>
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#define PPI 300
#define CM2INCH 1/2.54

std::string folderPath = "E:/IN ANH/2023/thang 3/03/6x9 polaroid/*.jpg";
float width = 6;
float height = 9;
float borderOfset = 0.25;

float cm2pixel(float d)
{
	return d*PPI*CM2INCH;
}

cv::Scalar bgColor = cv::Scalar(255, 255, 255);
cv::Scalar borderColor = cv::Scalar(255, 255, 255);

cv::Mat resizeKeepAspectRatio(const cv::Mat& input, const cv::Size& dstSize, const cv::Scalar& bgcolor)
{
	cv::Mat output;

	double h1 = dstSize.width * (input.rows / (double)input.cols);
	double w2 = dstSize.height * (input.cols / (double)input.rows);
	if (h1 <= dstSize.height) {
		cv::resize(input, output, cv::Size(dstSize.width, h1), 0, 0, cv::INTER_CUBIC);
	}
	else {
		cv::resize(input, output, cv::Size(w2, dstSize.height), 0, 0, cv::INTER_CUBIC);
	}

	int top = (dstSize.height - output.rows) / 2;
	int down = (dstSize.height - output.rows + 1) / 2;
	int left = (dstSize.width - output.cols) / 2;
	int right = (dstSize.width - output.cols + 1) / 2;

	cv::copyMakeBorder(output, output, top, down, left, right, cv::BORDER_CONSTANT, bgcolor);

	return output;
}

void inspection(std::string imagePath)
{
	std::cout << "inspection: " << imagePath << std::endl;
	cv::Mat im = cv::imread(imagePath, cv::IMREAD_ANYCOLOR );

	cv::Rect borderRect;
	float borderOfsetPixel = cm2pixel(borderOfset);
	cv::Size size = cv::Size(cm2pixel(width),cm2pixel(height));
	if(im.cols > im.rows)
	{
		cv::rotate(im, im, cv::ROTATE_90_CLOCKWISE);
		borderRect = cv::Rect(borderOfsetPixel*2, borderOfsetPixel, size.width - borderOfsetPixel*3, size.height - borderOfsetPixel*2);
	}
	else
	{
		borderRect = cv::Rect(borderOfsetPixel, borderOfsetPixel, size.width - borderOfsetPixel*2, size.height - borderOfsetPixel*5);
	}

	cv::Mat resizeImg = resizeKeepAspectRatio(im, borderRect.size(), bgColor);
	cv::Mat ouput = cv::Mat(size, im.type(), borderColor);
	resizeImg.copyTo(ouput(borderRect));
	cv::imwrite(imagePath+".png",ouput);
}

int main()
{
    std::vector<std::string> filenames;
	cv::glob(folderPath, filenames);

	for (size_t i=0; i<filenames.size(); i++)
	{
		inspection(filenames[i]);
	}
    return 0;
}