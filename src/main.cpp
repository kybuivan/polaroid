#include <iostream>
#include <string>
#include <vector>
#include "window.h"
#include "nfd.h"
#include "utils.h"
#include "ref.h"
#include "opencv2/highgui.hpp"
#include <iostream>
#include <filesystem>
using namespace std::filesystem;

struct Texture2D
{
	int width = 0;
	int height = 0;
	uint32_t id = 0;
};

class ImageInfo
{
public:
	ImageInfo(std::string _path)
		: mPath(_path)
	{
		cv::Mat img = cv::imread(mPath);
		mWidth = img.cols;
		mHeight = img.rows;
		cv::resize(img, img, cv::Size(100, 150));
		cv::resize(img, img, cv::Size(100, 150));
		cv::cvtColor(img, img, cv::COLOR_RGB2BGR);
		mTexture = CreateTexture(img);
		img.release();
	}

	void Release()
	{
		if (mTexture.id)
		{
			glDeleteTextures(1, &mTexture.id);
			mTexture.width = 0;
			mTexture.height = 0;
		}
	}

	static Texture2D CreateTexture(cv::Mat img, int format = GL_RGB)
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
		glTexImage2D(GL_TEXTURE_2D, 0, format, text.width, text.height, 0, format, GL_UNSIGNED_BYTE, img.data);
		return text;
	}

public:
	std::string GetPath() { return mPath; }
	const Texture2D &GetTexture() { return mTexture; }
	int GetWidth() { return mWidth; }
	int GetHeight() { return mHeight; }
	std::string GetName()
	{
		std::string name = mPath;
		return name.substr(name.find_last_of("\\/") + 1);
	}

private:
	std::string mPath = "";
	int mWidth;
	int mHeight;
	Texture2D mTexture;
};

class Application
{
public:
	Application() : mTexture{}
	{
		mTexture = ImageInfo::CreateTexture(cv::Mat::zeros(cv::Size(cm2pixel(mWidth), cm2pixel(mHeight)), CV_8UC3));
	}

	void MenuBarFunction()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New File...", "Ctrl+N"))
				{
					Reset();
				}
				if (ImGui::MenuItem("Open File...", "Ctrl+O"))
				{
					nfdpathset_t outPaths;
					// nfdchar_t *outPath = NULL;
					nfdresult_t result = NFD_OpenDialogMultiple("png,jpg", NULL, &outPaths);
					if (result == NFD_OKAY)
					{
						puts("Success!");
						mImageList.clear();
						mPreviousIdex = mCurrentIdex = 0;
						for (size_t i = 0; i < NFD_PathSet_GetCount(&outPaths); ++i)
						{
							nfdchar_t *outPath = NFD_PathSet_GetPath(&outPaths, i);
							mImageList.push_back(CreateRef<ImageInfo>(outPath));
						}
					}
					else if (result == NFD_CANCEL)
					{
						puts("User pressed cancel.");
					}
					else
					{
						// log("Error: %s\n", NFD_GetError() );
					}
				}

				if (ImGui::MenuItem("Open Folder...", "Ctrl+Shift+O"))
				{
					nfdchar_t *outPath = NULL;
					nfdresult_t result = NFD_PickFolder( NULL, &outPath );
					if ( result == NFD_OKAY )
					{
						puts("Success!");
						puts(outPath);
						mImageList.clear();
						mPreviousIdex = mCurrentIdex = 0;
						std::vector<std::string> extensions = { ".jpg", ".JPG", ".png", ".PNG" };
						for (const auto& entry : std::filesystem::directory_iterator(outPath)) {
							if (entry.is_regular_file()) {
								std::string file_path = entry.path().string();
								for (const auto& ext : extensions) {
									if (file_path.size() >= ext.size() && file_path.compare(file_path.size() - ext.size(), ext.size(), ext) == 0) {
										mImageList.push_back(CreateRef<ImageInfo>(file_path));
										break;
									}
								}
							}
						}
						free(outPath);
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

				if (ImGui::MenuItem("Save As...", "Ctrl+S"))
				{
					nfdchar_t *savePath = NULL;
					nfdresult_t result = NFD_SaveDialog("png;jpg;tif", mCurrentImagePath.c_str(), &savePath);
					if (result == NFD_OKAY)
					{
						puts("Success!");
						puts(savePath);
						SaveFile(savePath);
						free(savePath);
					}
					else if (result == NFD_CANCEL)
					{
						puts("User pressed cancel.");
					}
					else
					{
						printf("Error: %s\n", NFD_GetError());
					}
				}

				if (ImGui::MenuItem("Save All", "Ctrl+Shift+S"))
				{
					nfdchar_t *outPath = NULL;
					nfdresult_t result = NFD_PickFolder( NULL, &outPath );
					if ( result == NFD_OKAY )
					{
						puts("Success!");
						puts(outPath);
						SaveFolder(outPath);
						free(outPath);
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

				if (ImGui::MenuItem("Exit"))
				{
					exit_app = true;
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Undo", "Ctrl+Z"))
				{
				}
				if (ImGui::MenuItem("Redo", "Ctrl+Y"))
				{
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Help"))
			{
				if (ImGui::MenuItem("About"))
				{
					ImGui::OpenPopup("AboutMe");

					if (ImGui::BeginPopupModal("AboutMe"))
					{
						ImGui::Text("This is about.");
						ImGui::EndPopup();
					}
				}

				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}

	void ViewFunction()
	{
		ImGuiIO& io = ImGui::GetIO();
		ImVec2 screen_size = ImVec2(io.DisplaySize.x, io.DisplaySize.y);
		{
			// Set the next window position to the left side of the screen
			//ImGui::SetNextWindowPos(ImVec2(screen_size.x / 4, 0));
			ImGui::SetNextWindowSize(ImVec2(screen_size.x / 4, screen_size.y));
			//ImGui::SetNextWindowPos(ImVec2(io.DisplacP, 0));
			ImGui::Begin("Setting", nullptr, ImGuiWindowFlags_NoCollapse); // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Text("texture pos = %d", mTexture.id);
			cv::Size currentSize = {0, 0};

			if (!mImageList.empty())
			{
				currentSize = cv::Size(mImageList[mCurrentIdex]->GetWidth(),mImageList[mCurrentIdex]->GetHeight());
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
				ImGui::PopItemWidth();
				ImGui::PopID();
				ImGui::PushID("border");
				ImGui::TextUnformatted("border offset");
				ImGui::SameLine();
				ImGui::DragFloat("##hidelabel", &mBorderOfset, 0.01f, 0.00f, 50.00f);
				ImGui::PopID();
				ImGui::PushID("bottom");
				ImGui::TextUnformatted("bottom offset");
				ImGui::SameLine();
				ImGui::DragFloat("##hidelabel", &mBottomOfset, 0.01f, 0.00f, 50.00f);
				ImGui::PopItemWidth();
				ImGui::PopID();
			}

			//{
			//	ImGui::BeginGroup();
			//	ImGui::TextUnformatted("width"); ImGui::SameLine(); ImGui::DragFloat("##hidelabel", &mWidth, 0.1f);
			//	ImGui::EndGroup();
			//}
			// ImGui::TextUnformatted("height"); ImGui::SameLine(); ImGui::DragFloat("##hidelabel", &mHeight, 0.1f);
			ImGui::PushID("background");
			ImGui::TextUnformatted("background");
			ImGui::SameLine();
			ImGui::ColorEdit3("##hidelabel", (float *)&mBgColor);
			ImGui::PopID();
			ImGui::TextUnformatted("border");
			ImGui::SameLine();
			ImGui::ColorEdit3("##hidelabel", (float *)&mBorderColor);
			ImGui::End();
		}

		{
			ImGui::SetNextWindowSize(ImVec2(screen_size.x * 3 / 4, screen_size.y * 3 / 4));
			ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(20, 20, 20, 255));
			ImGui::Begin("View", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
			// image_size = ImVec2(text.mWidth, text.mHeight);
			int border = 20;
			ImVec2 currentWindowSize = ImGui::GetWindowSize();
			ImVec2 windowSize(ImGui::GetWindowSize().x - border * 2, ImGui::GetWindowSize().y - border * 2);
			ImVec2 newSize = GetScaleImageSize(ImVec2(static_cast<float>(cm2pixel(mWidth)), static_cast<float>(mTexture.height)), windowSize);
			ImGui::SetCursorPos(ImVec2((ImGui::GetWindowSize().x - newSize.x) * 0.5f, (ImGui::GetWindowSize().y - newSize.y) * 0.5f + border));
			ImGui::Image((void *)(intptr_t)mTexture.id, newSize);
			ImGui::End();
			ImGui::PopStyleColor();
		}

		{
			ImGui::SetNextWindowSize(ImVec2(screen_size.x / 4, screen_size.y / 4));
			ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(20, 20, 20, 255));
			ImGui::Begin("Image List", NULL, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
			int border = 30;
			ImVec2 currentWindowSize = ImGui::GetWindowSize();
			int currentCursorPosX = border / 2;
			for (int i = 0; i < mImageList.size(); i++)
			{
				auto tex = mImageList[i]->GetTexture();
				ImVec2 windowSize(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y - border);
				ImVec2 image_size = GetScaleImageSize(ImVec2(static_cast<float>(tex.width), static_cast<float>(tex.height)), windowSize);
				ImVec2 image_pos = ImVec2(currentCursorPosX, border);
				ImGui::SetCursorPos(image_pos);
				ImGui::Image((void *)(intptr_t)tex.id, image_size);

				// Check if the imgui::image was double-clicked
				if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && i != mCurrentIdex)
				{
					mPreviousIdex = mCurrentIdex;
					mCurrentIdex = i;
				}

				if(i == mCurrentIdex)
				{
					// Get the position and size of the imgui::image
					// Draw a border around the imgui::image
					ImDrawList* draw_list = ImGui::GetWindowDrawList();
					ImVec2 border_min = ImVec2(image_pos.x - 1, image_pos.y - 1);
					ImVec2 border_max = ImVec2(image_pos.x + image_size.x + 1, image_pos.y + image_size.y + 1);
					draw_list->AddRect(border_min, border_max, IM_COL32(0, 255, 255, 255));
				}

				currentCursorPosX += image_size.x + border / 2;
			}
			// Get the maximum horizontal scrolling position
			//float max_scroll_x = ImGui::GetScrollMaxX();

			// Set the horizontal scrolling position
			//ImGui::SetScrollX(max_scroll_x);
			ImGui::End();
			ImGui::PopStyleColor();
		}
	}

	void SaveFile(std::string path)
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
		// cv::Mat bgrImage;
		// cv::cvtColor(img, bgrImage, cv::COLOR_RGB2RGB);

		// write image to file using imwrite
		cv::imwrite(path, img);
	}

	void SaveFolder(std::string folderPath)
	{
		cv::Scalar bgColor = vec2scalar(mBgColor);
		cv::Scalar borderColor = vec2scalar(mBorderColor);
		cv::Size size = cv::Size(cm2pixel(mWidth), cm2pixel(mHeight));
		float borderOfsetPixel = cm2pixel(mBorderOfset);
		float bottomOfsetPixel = cm2pixel(mBottomOfset);

		// crash when input width, height
		if (!size.empty())
		{
			cv::Mat mBorderText = cv::Mat(size, CV_8UC3, borderColor);
			cv::Rect borderRect = cv::Rect(borderOfsetPixel, borderOfsetPixel, size.width - borderOfsetPixel * 2, size.height - borderOfsetPixel * 2 - bottomOfsetPixel);
			if (RoiRefine(borderRect, size))
			{
				cv::Mat resizeImg = cv::Mat::zeros(borderRect.size(), CV_8UC3);
				if (!mImageList.empty())
				{
					cv::Mat mCurrentMat;
					for(auto& img : mImageList)
					{
						mCurrentMat = cv::imread(img->GetPath());
						resizeImg = resizeKeepAspectRatio(mCurrentMat, borderRect.size(), bgColor);
						resizeImg.copyTo(mBorderText(borderRect));
						std::string filePath = folderPath + "\\" + img->GetName();
						cv::imwrite(filePath, mBorderText);
					}
				}
				resizeImg.release();
			}
			mBorderText.release();
		}
		
	}

	void Inspection()
	{
		cv::Scalar bgColor = vec2scalar(mBgColor);
		cv::Scalar borderColor = vec2scalar(mBorderColor);
		cv::Size size = cv::Size(cm2pixel(mWidth), cm2pixel(mHeight));
		float borderOfsetPixel = cm2pixel(mBorderOfset);
		float bottomOfsetPixel = cm2pixel(mBottomOfset);
		// crash when input width, height
		if (!size.empty())
		{
			cv::Mat mBorderText = cv::Mat(size, CV_8UC3, borderColor);
			cv::Rect borderRect = cv::Rect(borderOfsetPixel, borderOfsetPixel, size.width - borderOfsetPixel * 2, size.height - borderOfsetPixel * 2 - bottomOfsetPixel);
			if (RoiRefine(borderRect, size))
			{
				cv::Mat resizeImg = cv::Mat::zeros(borderRect.size(), CV_8UC3);
				if (!mImageList.empty())
				{
					// if(mText.cols > mText.rows)
					// {
					// 	cv::rotate(mText, mText, cv::ROTATE_90_CLOCKWISE);
					// 	borderRect = cv::Rect(borderOfsetPixel*2, borderOfsetPixel, size.width - borderOfsetPixel*3, size.height - borderOfsetPixel*2);
					// }
					if(mPreviousIdex != mCurrentIdex || mCurrentMat.empty()) mCurrentMat = cv::imread(mImageList[mCurrentIdex]->GetPath());
					resizeImg = resizeKeepAspectRatio(mCurrentMat, borderRect.size(), bgColor);
				}
				resizeImg.copyTo(mBorderText(borderRect));
				resizeImg.release();
			}
			// update OpenGL texture if size has changed
			if (size.width != mTexture.width || size.height != mTexture.height)
			{
				ImageRelease(mTexture);
				mTexture = ImageInfo::CreateTexture(cv::Mat::zeros(size, CV_8UC3));
				mTexture.width = size.width;
				mTexture.height = size.height;
				// glTexImage2D(GL_TEXTURE_2D, 0, GL_BGR, mTexture.width, mTexture.height, 0, GL_BGR, GL_UNSIGNED_BYTE, NULL);
			}
			glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)mTexture.id);

			// set alignment explicitly to 1
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, mBorderText.cols, mBorderText.rows, GL_BGR, GL_UNSIGNED_BYTE, mBorderText.data);
			mBorderText.release();
		}
	}

	void ImageRelease(Texture2D &text)
	{
		if (text.id)
		{
			glDeleteTextures(1, &text.id);
			text.width = 0;
			text.height = 0;
		}
	}

	void Reset()
	{
		for (auto &image : mImageList)
			image->Release();
		mImageList.clear();
		mCurrentIdex = mPreviousIdex = 0;
		mCurrentMat = {};
		mWidth = 6;
		mHeight = 9;
		mBorderOfset = 0.25;
		mBgColor = {1.0f, 1.0f, 1.0f, 1.0f};
		mBorderColor = {1.0f, 1.0f, 1.0f, 1.0f};
		ImageRelease(mTexture);
		mTexture = ImageInfo::CreateTexture(cv::Mat::zeros(cv::Size(cm2pixel(mWidth), cm2pixel(mHeight)), CV_8UC3));
	}

	~Application()
	{
		ImageRelease(mTexture);
		for (auto &image : mImageList)
			image->Release();
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
	float mBottomOfset = 0.75;
	ImVec4 mBgColor = {1.0f, 1.0f, 1.0f, 1.0f};
	ImVec4 mBorderColor = {1.0f, 1.0f, 1.0f, 1.0f};
	std::vector<Ref<ImageInfo>> mImageList{};
	int mCurrentIdex;
	int mPreviousIdex;
	cv::Mat mCurrentMat;
	Texture2D mTexture;
};

int main()
{
	Window window("Polaroid", 1080, 720, true);

	window.set_key_callback([&](int key, int action) noexcept
							{
        if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
            window.set_should_close();
        } });

	Application app;
	window.run([&]
			   {
		app.MenuBarFunction();
		app.Inspection();
        if(app.exit_app)
            window.set_should_close();
        app.ViewFunction(); });
	return 0;
}