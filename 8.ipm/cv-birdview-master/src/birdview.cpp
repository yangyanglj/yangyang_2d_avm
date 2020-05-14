#include "birdview.hpp"

Birdview::Birdview()
{
    // Initialize random number generator with time seed
    m_rng = cv::RNG(time(NULL));
}

Birdview::~Birdview()
{
}

void Birdview::load(const std::string &path)
{
    // Load source image from file
    m_imgInput = cv::imread(path);
}

void Birdview::save(const std::string &path, const modes &level)
{
    cv::Mat image;

    // Select image to save
    switch (level)
    {
        case INPUT: image = m_imgInput; break;
        case SMOOTH: image = m_imgSmooth; break;
        case THRESHOLD: image = m_imgThreshold; break;
        case CANNY: image = m_imgCanny; break;
        case CONTOURS: image = m_imgContours; break;
        case TRANSFORM: image = m_imgTransform; break;
        case OCR: image = m_imgCharacter; break;
    }

    // Save transformed image to file
    std::vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(5);
    cv::imwrite(path, image, compression_params);
}

void Birdview::debug(const modes &level)
{
    // Visualize image processing results
    if (!m_imgInput.empty())
    {
        switch (level)
        {
            case INPUT: cv::imshow("m_imgInput", m_imgInput); break;
            case SMOOTH: cv::imshow("m_imgSmooth", m_imgSmooth); break;
            case THRESHOLD: cv::imshow("m_imgThreshold", m_imgThreshold); break;
            case CANNY: cv::imshow("m_imgCanny", m_imgCanny); break;
            case CONTOURS: cv::imshow("m_imgContours", m_imgContours); break;
            case TRANSFORM: cv::imshow("m_imgTransform", m_imgTransform); break;
            case OCR: cv::imshow("m_imgCharacter", m_imgCharacter); break;
            default: cv::imshow("m_imgInput", m_imgInput); break;
        }
        cv::waitKey(0);
    }
    else
    {
        std::cout << "No image initialized!" << std::endl;
    }
}

void Birdview::preprocess()
{
    // Reduce image resolution
    cv::resize(m_imgInput, m_imgInput, cv::Size(m_imgInput.cols / SCALE, m_imgInput.rows / SCALE));
    // Convert to grey scale
    cv::cvtColor(m_imgInput, m_imgGrey, cv::COLOR_BGR2GRAY);
    // Apply threshold
    cv::threshold(m_imgGrey, m_imgThreshold, 165, 255, cv::THRESH_TOZERO);
    // Apply smoothing filter
    cv::bilateralFilter(m_imgGrey, m_imgSmooth, 40, 27, 27);
    // Apply canny edge detection
    cv::Canny(m_imgThreshold, m_imgCanny, 85, 255);
    // Create a copy of the source image
    m_imgInputClone = m_imgInput.clone();
}

void Birdview::contours()
{
    // Find contours in the image
    m_imgContours = cv::Mat::zeros(m_imgCanny.size(), CV_8UC3);
    cv::findContours(m_imgCanny, m_contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // Draw each contour in a unique random color
    for(int i = 0; i < static_cast<int>(m_contours.size()); i++)
    {
        cv::Scalar color = cv::Scalar(m_rng(255), m_rng(255), m_rng(255));
        cv::drawContours(m_imgContours, m_contours, i, color, 2, 8, m_hierarchy, 0, cv::Point());
    }
}

void Birdview::boundingbox()
{
    double areaMax, area, perimeter = 0.0;
    int areaMaxIndex, areaMaxEdges = 0;

    // Loop over all contours and find the bounding box with the largest area
    for (int i = 0; i < static_cast<int>(m_contours.size()); i++)
    {
        area = cv::contourArea(m_contours[i]);
        if (area > areaMax)
        {
            // Approximate a polygonal curve with the specified precision
            perimeter = cv::arcLength(m_contours[i], true);
            cv::approxPolyDP(m_contours[i], m_boundBox, 0.02 * perimeter, true);
            // Bounding box needs to either consist of four line segments or a specific area
            if ((m_boundBox.size() == 4) || (area > 80000.0))
            {
                areaMax = area;
                areaMaxIndex = i;
                areaMaxEdges = m_boundBox.size();
            }
            else
            {
                continue;
            }
        }
    }

    // Draw bounding box in green color
    cv::drawContours(m_imgInput, m_contours, areaMaxIndex, cv::Scalar(0, 255, 0), 2, 8, m_hierarchy, 0, cv::Point());

    // Output contour index and number of edges found which represent the bounding box
    std::cout << "Box Index:\t" << areaMaxIndex << std::endl;
    std::cout << "Box Area:\t" << areaMax << std::endl;
    std::cout << "Box Edges:\t" << areaMaxEdges << std::endl;
    std::cout << "Box Vertex 0:\t" << m_boundBox[0].x << " " << m_boundBox[0].y << std::endl;
    std::cout << "Box Vertex 1:\t" << m_boundBox[1].x << " " << m_boundBox[1].y << std::endl;
    std::cout << "Box Vertex 2:\t" << m_boundBox[2].x << " " << m_boundBox[2].y << std::endl;
    std::cout << "Box Vertex 3:\t" << m_boundBox[3].x << " " << m_boundBox[3].y << std::endl;
}

void Birdview::viewpoints()
{
    std::vector<int> pointOfView;
    int index;

    // Determine top-left and bottom-right vertex of the bounding box
    m_boundBoxSort.resize(4);
    for (int i = 0; i < static_cast<int>(m_boundBox.size()); i++)
    {
        pointOfView.push_back(m_boundBox[i].x + m_boundBox[i].y);
    }
    index = std::min_element(std::begin(pointOfView), std::end(pointOfView)) - pointOfView.begin();
    m_boundBoxSort[0] = m_boundBox[index];
    index = std::max_element(std::begin(pointOfView), std::end(pointOfView)) - pointOfView.begin();
    m_boundBoxSort[2] = m_boundBox[index];

    // Determine top-right and bottom-left vertex of the bounding box
    pointOfView.clear();
    for (int i = 0; i < static_cast<int>(m_boundBox.size()); i++)
    {
        pointOfView.push_back(m_boundBox[i].x - m_boundBox[i].y);
    }
    index = std::max_element(std::begin(pointOfView), std::end(pointOfView)) - pointOfView.begin();
    m_boundBoxSort[1] = m_boundBox[index];
    index = std::min_element(std::begin(pointOfView), std::end(pointOfView)) - pointOfView.begin();
    m_boundBoxSort[3] = m_boundBox[index];

    // Draw perspective labels with red color
    cv::putText(m_imgInput, "TL", m_boundBoxSort[0], 5, 1, cv::Scalar(0, 0, 255), 2);
    cv::putText(m_imgInput, "TR", m_boundBoxSort[1], 5, 1, cv::Scalar(0, 0, 255), 2);
    cv::putText(m_imgInput, "BR", m_boundBoxSort[2], 5, 1, cv::Scalar(0, 0, 255), 2);
    cv::putText(m_imgInput, "BL", m_boundBoxSort[3], 5, 1, cv::Scalar(0, 0, 255), 2);
}

void Birdview::transform()
{
    cv::Point2f sourceVertex[4], destVertex[4];
    cv::Mat transMatrix;
    int aWidth, bWidth, maxWidth;
    int aHeight, bHeight, maxHeight;
    double distance;

    // Determine bounding box edge with the largest width
    distance = std::sqrt(std::pow(m_boundBoxSort[2].x - m_boundBoxSort[3].x, 2) + std::pow(m_boundBoxSort[2].y - m_boundBoxSort[3].y, 2));
    aWidth = static_cast<int>(distance);
    distance = std::sqrt(std::pow(m_boundBoxSort[1].x - m_boundBoxSort[0].x, 2) + std::pow(m_boundBoxSort[1].y - m_boundBoxSort[0].y, 2));
    bWidth = static_cast<int>(distance);
    maxWidth = (aWidth > bWidth) ? aWidth : bWidth;

    // Determine bounding box edge with the largest height
    distance = std::sqrt(std::pow(m_boundBoxSort[1].x - m_boundBoxSort[2].x, 2) + std::pow(m_boundBoxSort[1].y - m_boundBoxSort[2].y, 2));
    aHeight = static_cast<int>(distance);
    distance = std::sqrt(std::pow(m_boundBoxSort[0].x - m_boundBoxSort[3].x, 2) + std::pow(m_boundBoxSort[0].y - m_boundBoxSort[3].y, 2));
    bHeight = static_cast<int>(distance);
    maxHeight = (aHeight > bHeight) ? aHeight : bHeight;

    // Determine source vertices
    sourceVertex[0] = cv::Point2f(m_boundBoxSort[0].x, m_boundBoxSort[0].y);
    sourceVertex[1] = cv::Point2f(m_boundBoxSort[1].x, m_boundBoxSort[1].y);
    sourceVertex[2] = cv::Point2f(m_boundBoxSort[2].x, m_boundBoxSort[2].y);
    sourceVertex[3] = cv::Point2f(m_boundBoxSort[3].x, m_boundBoxSort[3].y);

    // Determine destination vertices
    destVertex[0] = cv::Point2f(0, 0);
    destVertex[1] = cv::Point2f(maxWidth - 1, 0);
    destVertex[2] = cv::Point2f(maxWidth - 1, maxHeight - 1);
    destVertex[3] = cv::Point2f(0, maxHeight - 1);

    // Calculate perspective transform matrix
    transMatrix = cv::getPerspectiveTransform(sourceVertex, destVertex);
    // Apply perspective transformation to source image
    cv::warpPerspective(m_imgInputClone, m_imgTransform, transMatrix, cv::Size(maxWidth, maxHeight));
}

void Birdview::ocr(std::string &text)
{
    int threshold_value = 105;

    // Create a window to display results 
    cvNamedWindow("Result", CV_WINDOW_NORMAL); 
    cv::createTrackbar("Threshold", "Result", &threshold_value, 150);

    tesseract::TessBaseAPI *ocr = new tesseract::TessBaseAPI();

    while (1)  
    {
        // Display current threshold value
        std::cout << "Threshold: " << threshold_value << std::endl;

        // Convert to grey scale and apply basic thresholding operation
        cv::cvtColor(m_imgTransform, m_imgCharacter, cv::COLOR_BGR2GRAY);
        cv::threshold(m_imgCharacter, m_imgCharacter, threshold_value, 255, cv::THRESH_BINARY);

        // Crop transformed image to region-of-interest
        cv::Rect roi(30, 90, 540, 230);
        m_imgCharacter = cv::Mat(m_imgCharacter, roi);

        // Perform optical character recognition
        ocr->Init(NULL, "eng", tesseract::OEM_LSTM_ONLY);
        ocr->SetPageSegMode(tesseract::PSM_AUTO);
        ocr->SetImage(m_imgCharacter.data, m_imgCharacter.cols, m_imgCharacter.rows, 1, m_imgCharacter.step);
        text = ocr->GetUTF8Text();
        std::cout << text << std::endl;
        imshow("Result", m_imgCharacter); 
        cv::waitKey(0); 
    }
    ocr->End();
}