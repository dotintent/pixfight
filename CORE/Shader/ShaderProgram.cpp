#include "ShaderProgram.hpp"

//-------------------------------------------------------------------------

void ShaderProgram::createShader(const std::string & filePath, GLenum type) {
    
    assert(filePath.size() > 0);
    assert((type == GL_VERTEX_SHADER || type == GL_FRAGMENT_SHADER));

    std::cout << "[INFO] Shader path: " << filePath << std::endl;

    std::ifstream stream(filePath.c_str());
    
    assert(stream.good());
    
    std::string str((std::istreambuf_iterator<char>(stream)),
                     std::istreambuf_iterator<char>());

    if(str.length() == 0) {
        
        std::cerr << "[ERROR] Failed to load shader:" << filePath << std::endl;
        return;
    }
    
    _shaderId = glCreateShader(type);
    
    const char * source = str.c_str();
    
    glShaderSource(_shaderId, 1, &source, NULL);
    
    glCompileShader(_shaderId);
    
    GLint status = 0;
    
    glGetShaderiv(_shaderId, GL_COMPILE_STATUS, &status);
    
    if (status != 0) {
        return;
    }
    
    GLint logLength = 0;
    
    glGetShaderiv(_shaderId, GL_INFO_LOG_LENGTH, &logLength);
    
    if (logLength > 0) {
        
        GLchar *log = (GLchar *)malloc(logLength);
        glGetShaderInfoLog(_shaderId, logLength, &logLength, log);
        std::cout<<"[COMPILE] Log:" << log << std::endl;
        free(log);
    }
    
    glDeleteShader(_shaderId);
    
    std::cerr <<"[WARNING] Failed to compile " << (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment") << " shader." << std::endl;
}

//-------------------------------------------------------------------------

GLShader::GLShader()
: _shaderProgram(0)
, _vertexProgram(0)
, _fragmentProgram(0) {
    
}

GLShader::~GLShader() {
    
    this->deinitShader();
}

void GLShader::deinitShader() {
    
    std::cout << "[INFO] Deinit Shader: " << _shaderProgram << std::endl;
    
    if (_shaderProgram) {
        
        glDeleteProgram(_shaderProgram);
        _shaderProgram = 0;
    }
}

void GLShader::linkProgram() {
    
    if (_shaderProgram) {
        
        glLinkProgram(_shaderProgram);
        
        GLint linkSuccess;
        
        glGetProgramiv(_shaderProgram, GL_LINK_STATUS, &linkSuccess);
        
        if (linkSuccess == GL_FALSE) {
            GLchar messages[256];
            glGetProgramInfoLog(_shaderProgram, sizeof(messages), 0, &messages[0]);
            
            std::cerr << "[ERROR] Error linking program: " << messages << std::endl;
        }
        
        this->printInfoLog();
    }
    
    glDeleteShader(_vertexProgram);
    _vertexProgram = 0;
    
    glDeleteShader(_fragmentProgram);
    _fragmentProgram = 0;
}

bool GLShader::loadShader(const std::string & path) {
    
    bool loaded = true;
    
    std::size_t last = path.find_last_of("\\/");
    
    if (last != std::string::npos) {
        std::cout<< "[INFO] Loading Shader: " << path.substr(last + 1) << std::endl;
    }

    auto vertShaderPathName = path + ".vsh";
    auto fragShaderPathName = path + ".fsh";
    
    _shaderProgram = glCreateProgram();
    
    //VERTEX
    
    ShaderProgram vertexShader = ShaderProgram();
    vertexShader.createShader(vertShaderPathName, GL_VERTEX_SHADER);

    _vertexProgram = vertexShader.getShaderId();
    
    if (_vertexProgram > 0) {
        
        glAttachShader(_shaderProgram, _vertexProgram);
        
    } else {
        
        loaded = false;
    }
    
    //FRAGMENT
    ShaderProgram fragmentShader = ShaderProgram();
    fragmentShader.createShader(fragShaderPathName, GL_FRAGMENT_SHADER);
    
    _fragmentProgram = fragmentShader.getShaderId();
    
    if (_fragmentProgram > 0) {
        
        glAttachShader(_shaderProgram, _fragmentProgram);
        
    } else {
        
        loaded = false;
    }
    
    return loaded;
}

const GLuint & GLShader::getProgramId() const {
    
    return _shaderProgram;
}

GLint GLShader::uniformLocation(const GLchar * name) const {
    
    return glGetUniformLocation(_shaderProgram, name);
}

void GLShader::begin() const {
    
    glUseProgram(_shaderProgram);
}

void GLShader::end() const {
    
    glUseProgram(0);
}

void GLShader::printInfoLog() const {

    GLint logLength, status;

    glValidateProgram(_shaderProgram);
    glGetProgramiv(_shaderProgram, GL_VALIDATE_STATUS, &status);

    if (status != 0) {
        return;
    }

    //Teoretically if program is not valid should not be used for now we will
    //ignore this info (mostly beacause of iOS) but we need to track the errors

    std::cout<< "[WARNING] Program is not valid! (see the reason below)" << std::endl;

    glGetProgramiv(_shaderProgram, GL_INFO_LOG_LENGTH, &logLength);

    if (logLength == 0) {
        return;
    }

    GLchar *log = (GLchar *)malloc(logLength);
    glGetProgramInfoLog(_shaderProgram, logLength, &logLength, log);

    std::cout<< "[VALIDATION]: " << log << std::endl;

    free(log);
}
