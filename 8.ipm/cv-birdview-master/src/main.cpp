#include "configuration.hpp"
#include "birdview.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    // std::cout << "Using OpenCV: " << cv::getBuildInformation() << std::endl;
    // std::cout << "Using Boost: " << BOOST_LIB_VERSION << std::endl;

    Configuration config;
    config.debug();

    std::string path_input = config.get(Configuration::INPUT) + "/" + config.get(Configuration::FILENAME);
    std::string path_output = config.get(Configuration::OUTPUT) + "/" + config.get(Configuration::FILENAME);
    std::string path_transform = boost::replace_all_copy(path_output, ".jpg", "_transform.jpg");
    std::string text;

    Birdview image;
    image.load(path_input);
    image.preprocess();
    image.contours();
    image.boundingbox();
    image.viewpoints();
    image.transform();
    image.debug(Birdview::TRANSFORM);
    // image.ocr(text);
    image.save(path_transform, Birdview::TRANSFORM);

    std::cout << text << std::endl;

    return 0;
}