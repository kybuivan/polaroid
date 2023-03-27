#ifndef _TEXTURE_2D_H_
#define _TEXTURE_2D_H_
#include <string>
#include <iostream>
#include <glad/gl.h>
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

class Texture2D
{
public:
    Texture2D(){}
    Texture2D(std::string path)
    {
        // Load from file
        cv::Mat img = cv::imread(path);
        if(!img.empty())
        {
            cv::cvtColor(img, img, cv::COLOR_BGR2RGBA);
            // Create a OpenGL texture identifier
            glGenTextures(1, &mRenderID);
            glBindTexture(GL_TEXTURE_2D, mRenderID);

            // Setup filtering parameters for display
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

            // Upload pixels into texture
        #if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
            glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        #endif
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.cols, img.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.data);
            mSize = img.size();
            img.release();
        }
        else
        {
            std::cout << "Load texture fault!" << std::endl;
        }
    }
    // ~Texture2D()
    // {
    //     glDeleteTextures(1, &mRenderID);
    // }
public:
    cv::Size mSize{};
    uint32_t mRenderID = 0;
};

#endif