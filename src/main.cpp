#include <iostream>
#include <string>
#include <vector>
#include "texture.h"
#include "window.h"
#include "imconfig.h"
#include "imgui.h"
#include "nfd.h"

#define PPI 300
#define CM2INCH 1/2.54

float cm2pixel(float d)
{
	return d*PPI*CM2INCH;
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

class Application
{
public:
	void Application::MenuBarFunction()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Open File...", "Ctrl+O")) {
					nfdchar_t *outPath = NULL;
					nfdresult_t result = NFD_OpenDialog( "png,jpg", NULL, &outPath );
					if ( result == NFD_OKAY )
					{
						puts("Success!");
						puts(outPath);
						mText = Texture2D(outPath);
						free(outPath);
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
			ImGui::Text("pointer = %p", mText.mRenderID);
			ImGui::Text("size = %d x %d", mText.mSize.width, mText.mSize.height);
			ImGui::End();
		}

		{
			ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(20, 20, 20, 255));
			ImGui::Begin("Viewer");
			//image_size = ImVec2(text.mWidth, text.mHeight);
			ImVec2 newSize = GetScaleImageSize(ImVec2(static_cast<float>(mText.mSize.width), static_cast<float>(mText.mSize.height)), ImGui::GetWindowSize());
			ImGui::SetCursorPos(ImVec2((ImGui::GetWindowSize().x - newSize.x) * 0.5f, (ImGui::GetWindowSize().y - newSize.y) * 0.5f));
			ImGui::Image((void*)(intptr_t)mText.mRenderID, newSize);
			ImGui::End();
			ImGui::PopStyleColor();
		}
	}
	void inspection(std::string imagePath)
	{
		std::cout << "inspection: " << imagePath << std::endl;
		cv::Mat im = cv::imread(imagePath, cv::IMREAD_ANYCOLOR );

		cv::Rect borderRect;
		float borderOfsetPixel = cm2pixel(mBorderOfset);
		cv::Size size = cv::Size(cm2pixel(mWidth),cm2pixel(mHeight));
		if(im.cols > im.rows)
		{
			cv::rotate(im, im, cv::ROTATE_90_CLOCKWISE);
			borderRect = cv::Rect(borderOfsetPixel*2, borderOfsetPixel, size.width - borderOfsetPixel*3, size.height - borderOfsetPixel*2);
		}
		else
		{
			borderRect = cv::Rect(borderOfsetPixel, borderOfsetPixel, size.width - borderOfsetPixel*2, size.height - borderOfsetPixel*5);
		}

		cv::Mat resizeImg = resizeKeepAspectRatio(im, borderRect.size(), mBgColor);
		cv::Mat ouput = cv::Mat(size, im.type(), mBorderColor);
		resizeImg.copyTo(ouput(borderRect));
		cv::imwrite(imagePath+".png",ouput);
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

		return output;
	}
public:
	bool exit_app = false;
private:
	std::string folderPath = "E:/IN ANH/2023/thang 3/03/6x9 polaroid/*.jpg";
	float mWidth = 6;
	float mHeight = 9;
	float mBorderOfset = 0.25;
	cv::Scalar mBgColor = cv::Scalar(255, 255, 255);
	cv::Scalar mBorderColor = cv::Scalar(255, 255, 255);
	std::vector<std::string> filenames;
	Texture2D mText, mBorderText;
};

int main()
{
	Application app;

	Window window("Image App", 1080, 720, true);

    window.set_key_callback([&](int key, int action) noexcept {
        if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
            window.set_should_close();
        }
    });
	
	window.run([&] {
		app.MenuBarFunction();
        if(app.exit_app)
            window.set_should_close();
        app.ViewFunction();
    });
    return 0;
}