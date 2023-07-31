#pragma once
#include <string>
#include <glm/glm.hpp>

class Shader {
    private:
	unsigned int ID;
    public:
	Shader(const char* vertexPath, const char* fragmentPath);
	~Shader();
    
	void use();
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setVec2f(const std::string& name, float x, float y) const;
    void setVec3f(const std::string& name, float x, float y, float z) const;
	void setMatrix4fv(const std::string& name, glm::mat4 value) const;
};
