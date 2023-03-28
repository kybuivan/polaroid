#include <iostream>
#include <string>
#include <vector>
#include "window.h"
#include "imconfig.h"
#include "imgui.h"
#include <imgui_internal.h>
#include "nfd.h"
#include <glad/gl.h>
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#define PPI 300
#define CM2INCH 1/2.54

float cm2pixel(float d)
{
    return d * PPI * CM2INCH;
}

cv::Scalar vec2scalar(ImVec4 vec)
{
    return cv::Scalar(vec.z * 255, vec.y * 255, vec.x * 255);
}

bool RoiRefine(cv::Rect& roi, cv::Size size)
{
	roi = roi & cv::Rect(cv::Point(0,0), size);
	return roi.area() > 0;
}

ImVec2 GetScaleImageSize(ImVec2 img_size, ImVec2 window_size)
{
    ImVec2 outSize{};
    if(img_size.x != 0 && img_size.y != 0)
    {
        double h1 = window_size.x * (img_size.y / (double)img_size.x);
        double w2 = window_size.y * (img_size.x / (double)img_size.y);
        if (h1 <= window_size.y) {
            outSize.x = window_size.x;
            outSize.y = static_cast<float>(h1);
        }
        else {
            outSize.x = static_cast<float>(w2);
            outSize.y = window_size.y;
        }
    }
    else
    {
        outSize = ImVec2(0,0);
    }
    return outSize;
}

struct Texture2D
{
	int width = 0;
	int height = 0;
	uint32_t id = 0;
};

Texture2D CreateTexture(cv::Mat img)
{
	Texture2D text;
	// Create a OpenGL texture identifier
	glGenTextures(1, &text.id);
	glBindTexture(GL_TEXTURE_2D, text.id);
	text.width = img.cols;
	text.height = img.rows;

	// Setup filtering parameters for display
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, text.width, text.height, 0, GL_RGB, GL_UNSIGNED_BYTE, img.data);
	return text;
}

struct ImageInfo
{
	std::string path = "";
	cv::Mat src {};

	ImageInfo() {}
	ImageInfo(std::string _path, cv::Mat _src) : path(_path), src (_src) {}
};

class Application
{
public:
	Application() : mTexture{}
	{
		mTexture = CreateTexture(cv::Mat::zeros(cv::Size(cm2pixel(mWidth), cm2pixel(mHeight)), CV_8UC3));
	}

	void Application::MenuBarFunction()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Open File...", "Ctrl+O")) {
					nfdpathset_t outPaths;
					//nfdchar_t *outPath = NULL;
					nfdresult_t result = NFD_OpenDialogMultiple( "png,jpg", NULL, &outPaths );
					if ( result == NFD_OKAY )
					{
						puts("Success!");
						mImageList.clear();
						mTextureList.clear();
						for (auto& text : mTextureList) ImageRelease(text);
						mCurrentIdex = 0;
						for (size_t i = 0; i < NFD_PathSet_GetCount(&outPaths); ++i) {
							nfdchar_t *outPath = NFD_PathSet_GetPath(&outPaths, i);
							cv::Mat img = cv::imread(outPath);
							mImageList.emplace_back(outPath, img);
							cv::resize(img, img, cv::Size(50, 100));
							mTextureList.emplace_back(CreateTexture(img));
							img.release();
						}
					}
					else if ( result == NFD_CANCEL )
					{
						puts("User pressed cancel.");
					}
					else 
					{
						//log("Error: %s\n", NFD_GetError() );
					}
				}

				if (ImGui::MenuItem("Open Folder...", "Ctrl+Shift+O")) {

				}

				if (ImGui::MenuItem("Save", "Ctrl+S")) {
					nfdchar_t *savePath = NULL;
					nfdresult_t result = NFD_SaveDialog("png;jpg;tif", mCurrentImagePath.c_str(), &savePath );
					if ( result == NFD_OKAY )
					{
						puts("Success!");
						puts(savePath);
						SaveImage(savePath);
						free(savePath);
					}
					else if ( result == NFD_CANCEL )
					{
						puts("User pressed cancel.");
					}
					else 
					{
						printf("Error: %s\n", NFD_GetError() );
					}
				}

				if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) {
				}

				if (ImGui::MenuItem("Exit")) {
					exit_app = true;
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Undo", "Ctrl+Z")) {
				}
				if (ImGui::MenuItem("Redo", "Ctrl+Y")) {
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Help"))
			{
				if (ImGui::MenuItem("About")) {
					ImGui::OpenPopup("AboutMe");

					if (ImGui::BeginPopupModal("AboutMe")) {
						ImGui::Text("This is about.");
						ImGui::EndPopup();
					}
				}
				
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}

	void Application::ViewFunction()
	{
		{
			ImGui::Begin("Setting");   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Text("texture pos = %d", mTexture.id);
			cv::Size currentSize = { 0,0 };

			if (!mImageList.empty())
			{
				currentSize = mImageList[mCurrentIdex].src.size();
			}
			ImGui::Text("size = %d x %d", currentSize.width, currentSize.height);
			{
				ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
				ImGui::PushID("width");
				ImGuiContext &g = *GImGui;
				ImGui::TextUnformatted("width:");
				ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);
				ImGui::DragFloat("##hidelabel", &mWidth, 0.1f, 1.0f, 100.0f);
				ImGui::PopID();
				ImGui::PushID("height");
				ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);
				ImGui::TextUnformatted("height:"); 
				ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);
				ImGui::DragFloat("##hidelabel", &mHeight, 0.1f, 1.0f, 100.0f);
				ImGui::PopID();
				ImGui::PushID("border");
				ImGui::TextUnformatted("border offset"); ImGui::SameLine();
				ImGui::DragFloat("##hidelabel", &mBorderOfset, 0.01f, 0.00f, 50.00f);
				ImGui::PopID();
				ImGui::PopItemWidth();
			}

			//{
			//	ImGui::BeginGroup();
			//	ImGui::TextUnformatted("width"); ImGui::SameLine(); ImGui::DragFloat("##hidelabel", &mWidth, 0.1f);
			//	ImGui::EndGroup();
			//}
			//ImGui::TextUnformatted("height"); ImGui::SameLine(); ImGui::DragFloat("##hidelabel", &mHeight, 0.1f);
			ImGui::PushID("background");
			ImGui::TextUnformatted("background"); ImGui::SameLine(); ImGui::ColorEdit3("##hidelabel", (float*)&mBgColor);
			ImGui::PopID();
			ImGui::TextUnformatted("border"); ImGui::SameLine(); ImGui::ColorEdit3("##hidelabel", (float*)&mBorderColor);
			ImGui::End();
		}

		{
			ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(20, 20, 20, 255));
			ImGui::Begin("Viewer");
			//image_size = ImVec2(text.mWidth, text.mHeight);
			int border = 20;
			ImVec2 currentWindowSize = ImGui::GetWindowSize();
			ImVec2 windowSize(ImGui::GetWindowSize().x - border * 2, ImGui::GetWindowSize().y - border * 2);
			ImVec2 newSize = GetScaleImageSize(ImVec2(static_cast<float>(cm2pixel(mWidth)), static_cast<float>(mTexture.height)), windowSize);
			ImGui::SetCursorPos(ImVec2((ImGui::GetWindowSize().x - newSize.x) * 0.5f, (ImGui::GetWindowSize().y - newSize.y) * 0.5f + border));
			ImGui::Image((void*)(intptr_t)mTexture.id, newSize);
			ImGui::End();
			ImGui::PopStyleColor();
		}

		{
			ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(20, 20, 20, 255));
			ImGui::Begin("Image List");
			int border = 10;
			ImVec2 currentWindowSize = ImGui::GetWindowSize();
			int currentCursorPosX = border;
			for (int i = 0; i < mTextureList.size(); i++)
			{
				const auto& tex = mTextureList[i];
				ImVec2 windowSize(ImGui::GetWindowSize().x/6, ImGui::GetWindowSize().y - border * 3);
				ImVec2 newSize = GetScaleImageSize(ImVec2(static_cast<float>(tex.width), static_cast<float>(tex.height)), windowSize);
				ImGui::SetCursorPos(ImVec2(currentCursorPosX, border));
				ImGui::Image((void*)(intptr_t)tex.id, newSize);
				currentCursorPosX += newSize.x + border;
			}
			
			ImGui::End();
			ImGui::PopStyleColor();
		}
	}

	void SaveImage(std::string path)
	{
		// Bind the texture
		glBindTexture(GL_TEXTURE_2D, mTexture.id);

		// Allocate buffer to store pixel data
		std::vector<unsigned char> buffer(mTexture.width * mTexture.height * 3);

		// Retrieve the pixel data from the texture
		glGetTexImage(GL_TEXTURE_2D, 0, GL_BGR, GL_UNSIGNED_BYTE, buffer.data());

		// Create cv::Mat object with texture data
		cv::Mat img(mTexture.height, mTexture.width, CV_8UC3, buffer.data());

		// convert image data to BGR format
		//cv::Mat bgrImage;
		//cv::cvtColor(img, bgrImage, cv::COLOR_RGB2RGB);

		// write image to file using imwrite
		cv::imwrite(path, img);
	}

	void Inspection()
	{
		cv::Scalar bgColor = vec2scalar(mBgColor);
		cv::Scalar borderColor = vec2scalar(mBorderColor);
		cv::Size size = cv::Size(cm2pixel(mWidth),cm2pixel(mHeight));
		float borderOfsetPixel = cm2pixel(mBorderOfset);
		//crash when input width, height
		if(!size.empty())
		{
			cv::Mat mBorderText = cv::Mat(size, CV_8UC3, borderColor);
			cv::Rect borderRect = cv::Rect(borderOfsetPixel, borderOfsetPixel, size.width - borderOfsetPixel*2, size.height - borderOfsetPixel*5);
			if(RoiRefine(borderRect, mBorderText.size()))
			{
				cv::Mat resizeImg = cv::Mat::zeros(borderRect.size(), CV_8UC3);
				if(!mImageList.empty())
				{
					// if(mText.cols > mText.rows)
					// {
					// 	cv::rotate(mText, mText, cv::ROTATE_90_CLOCKWISE);
					// 	borderRect = cv::Rect(borderOfsetPixel*2, borderOfsetPixel, size.width - borderOfsetPixel*3, size.height - borderOfsetPixel*2);
					// }
					resizeImg = resizeKeepAspectRatio(mImageList[mCurrentIdex].src, borderRect.size(), bgColor);
				}
				resizeImg.copyTo(mBorderText(borderRect));
			}
			// update OpenGL texture if size has changed
			glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)mTexture.id);
			if (size.width != mTexture.width || size.height != mTexture.height) {
				ImageRelease(mTexture);
				mTexture.width = size.width;
				mTexture.height = size.height;
				glTexImage2D(GL_TEXTURE_2D, 0, GL_BGR, mTexture.width, mTexture.height, 0, GL_BGR, GL_UNSIGNED_BYTE, NULL);
			}

			// set alignment explicitly to 1
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, mBorderText.cols, mBorderText.rows, GL_BGR, GL_UNSIGNED_BYTE, mBorderText.data);
		}
	}

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

		return std::move(output);
	}

	void ImageRelease(Texture2D& text)
	{
		if (text.id)
		{
			glDeleteTextures(1, &text.id);
			text.width = 0;
			text.height = 0;
		}
	}

	~Application()
	{
		ImageRelease(mTexture);
		for (auto& image : mImageList) image.src.release();
	}
public:
	bool exit_app = false;
private:
	std::string mCurrentImagePath{};
	std::string mFolderPath{};
	std::string mSaveFolderPath{};
	float mWidth = 6;
	float mHeight = 9;
	float mBorderOfset = 0.25;
	ImVec4 mBgColor = {1.0f, 1.0f, 1.0f, 1.0f};
	ImVec4 mBorderColor = {1.0f, 1.0f, 1.0f, 1.0f};
	std::vector<ImageInfo> mImageList{};
	std::vector<Texture2D> mTextureList{};
	int mCurrentIdex;
	Texture2D mTexture;
};

int main()
{
	Window window("Image App", 1080, 720, true);

    window.set_key_callback([&](int key, int action) noexcept {
        if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
            window.set_should_close();
        }
    });
	
	Application app;
	window.run([&] {
		app.MenuBarFunction();
		app.Inspection();
        if(app.exit_app)
            window.set_should_close();
        app.ViewFunction();
    });
    return 0;
}