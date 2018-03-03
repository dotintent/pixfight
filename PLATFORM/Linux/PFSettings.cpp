#include "PFSettings.h"

PFSettings::PFSettings(const std::string & path) : _path(path + "settings.psf") {

    _settings.mute = false;
    _settings.hardai = false;

    if (!load()) {
        save();
    }
}

void PFSettings::save() {

    std::fstream m_file;

    m_file.open(_path.c_str(), std::fstream::out|std::fstream::binary);

    if(!m_file){
        std::cerr << "Error Durning opening file: " << _path << " propably no disk space" << std::endl;
        m_file.close();
        return;
    }

    m_file.write((const char*)&_settings, sizeof(_settings));

    m_file.close();
}

bool PFSettings::load() {

    std::fstream m_file;

    m_file.open(_path.c_str(), std::fstream::in|std::fstream::binary);

    if (!m_file) {

        std::cerr << "Error Durning loading file: " << _path << " File not found or data corrupted" <<std::endl;
        m_file.close();

        return false;
    }

    m_file.read((char*)&_settings, sizeof(_settings));

    m_file.close();

    return true;
}
