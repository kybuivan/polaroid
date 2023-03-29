#ifndef _UTILS_H_
#define _UTILS_H_
#include "imconfig.h"
#include "imgui.h"
#include <imgui_internal.h>
#include <glad/gl.h>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"

#define PPI 300
#define CM2INCH 1 / 2.54

float cm2pixel(float d)
{
	return d * PPI * CM2INCH;
}

cv::Scalar vec2scalar(ImVec4 vec)
{
	return cv::Scalar(vec.z * 255, vec.y * 255, vec.x * 255);
}

bool RoiRefine(cv::Rect &roi, cv::Size size)
{
	roi = roi & cv::Rect(cv::Point(0, 0), size);
	return roi.area() > 0;
}

ImVec2 GetScaleImageSize(ImVec2 img_size, ImVec2 window_size)
{
	ImVec2 outSize{};
	if (img_size.x != 0 && img_size.y != 0)
	{
		double h1 = window_size.x * (img_size.y / (double)img_size.x);
		double w2 = window_size.y * (img_size.x / (double)img_size.y);
		if (h1 <= window_size.y)
		{
			outSize.x = window_size.x;
			outSize.y = static_cast<float>(h1);
		}
		else
		{
			outSize.x = static_cast<float>(w2);
			outSize.y = window_size.y;
		}
	}
	else
	{
		outSize = ImVec2(0, 0);
	}
	return outSize;
}

cv::Mat resizeKeepAspectRatio(const cv::Mat &input, const cv::Size &dstSize, const cv::Scalar &bgcolor)
{
	cv::Mat output;

	double h1 = dstSize.width * (input.rows / (double)input.cols);
	double w2 = dstSize.height * (input.cols / (double)input.rows);
	if (h1 <= dstSize.height)
	{
		cv::resize(input, output, cv::Size(dstSize.width, h1), 0, 0, cv::INTER_CUBIC);
	}
	else
	{
		cv::resize(input, output, cv::Size(w2, dstSize.height), 0, 0, cv::INTER_CUBIC);
	}

	int top = (dstSize.height - output.rows) / 2;
	int down = (dstSize.height - output.rows + 1) / 2;
	int left = (dstSize.width - output.cols) / 2;
	int right = (dstSize.width - output.cols + 1) / 2;

	cv::copyMakeBorder(output, output, top, down, left, right, cv::BORDER_CONSTANT, bgcolor);

	return std::move(output);
}
#endif