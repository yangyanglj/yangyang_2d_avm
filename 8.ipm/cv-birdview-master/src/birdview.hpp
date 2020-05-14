#ifndef BIRDVIEW_HPP
#define BIRDVIEW_HPP

#include "time.h"
#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>

class Birdview
{

public:
    enum modes
    {
        INPUT,
        SMOOTH,
        THRESHOLD,
        CANNY,
        CONTOURS,
        TRANSFORM,
        OCR
    };

    explicit Birdview();
    ~Birdview();

    void load(const std::string &path);
    void save(const std::string &path, const modes &level = TRANSFORM);
    void debug(const modes &level);
    void preprocess();
    void contours();
    void boundingbox();
    void viewpoints();
    void transform();
    void ocr(std::string &text);

private:
    static const unsigned int SCALE = 4;
    cv::RNG m_rng;
    cv::Mat m_imgInput;
    cv::Mat m_imgInputClone;
    cv::Mat m_imgGrey;
    cv::Mat m_imgSmooth;
    cv::Mat m_imgThreshold;
    cv::Mat m_imgCanny;
    cv::Mat m_imgContours;
    cv::Mat m_imgTransform;
    cv::Mat m_imgCharacter;
    std::vector<std::vector<cv::Point> > m_contours;
    std::vector<cv::Vec4i> m_hierarchy;
    std::vector<cv::Point> m_boundBox;
    std::vector<cv::Point> m_boundBoxSort;
};

#endif // BIRDVIEW_HPP