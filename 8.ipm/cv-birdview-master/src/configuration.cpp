#include "configuration.hpp"

Configuration::Configuration() :
    m_ptree(new boost::property_tree::ptree())
{
    // Initialize configuration settings
    std::string path = getenv("HOME") + std::string("/Library/Birdview/config.ini");
    boost::property_tree::ini_parser::read_ini(path, *m_ptree);

    // Load configuration parameters
    this->load();
}

Configuration::~Configuration()
{
    delete m_ptree;
}

void Configuration::load()
{
    load(m_parameters);
}

void Configuration::load(Configuration::settings &parameter)
{
    // Load configuration parameters from file
    parameter.path_input = m_ptree->get<std::string>("Path.input");
    parameter.path_output = m_ptree->get<std::string>("Path.output");
    parameter.file_name = m_ptree->get<std::string>("Path.filename");
}

bool Configuration::valid()
{
    // Check if configuration file is available and valid
    bool check = m_parameters.path_input.empty() || m_parameters.path_output.empty() || m_parameters.file_name.empty();
    return !check;
}

std::string Configuration::get(const parameters &param)
{
    // Get specific configuration parameter
    std::string value;
    switch (param)
    {
        case INPUT: value = m_parameters.path_input; break;
        case OUTPUT: value = m_parameters.path_output; break;
        case FILENAME: value = m_parameters.file_name; break;
    }
    return value;
}

void Configuration::set(const parameters &param, std::string value)
{
    // Set specific configuration parameter
    switch (param)
    {
        case INPUT: m_parameters.path_input = value; break;
        case OUTPUT: m_parameters.path_output = value; break;
        case FILENAME: m_parameters.path_output = value; break;
    }
}

void Configuration::debug()
{
    // Output current configuration parameters
    std::cout << "path_input:\t" << m_parameters.path_input << std::endl;
    std::cout << "path_output:\t" << m_parameters.path_output << std::endl;
    std::cout << "file_name:\t" << m_parameters.file_name << std::endl;
}