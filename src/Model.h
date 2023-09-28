#ifndef __MODEL_H__
#define __MODEL_H__

#include <sstream>
#include <iostream>
#include "Common.h"
#include "Mesh.h"


CLASS_PTR(Model);
class Model {
public:
    static ModelUPtr Load(const std::string& filename);

    void Draw(const Program* program) const;
    bool LoadMtl(const std::string& filename);
    MaterialUPtr SetTexture();

private:
    Model() {}
    bool LoadObj(const std::string& filename);
    GLfloat toFloat(std::string token);
    uint32_t toInt(std::string token);
    
    void TokenizeObj(std::istringstream& text);
    void TokenizeMtl(std::istringstream& text);
    void MakeNormal(uint32_t v1, uint32_t v2, uint32_t v3);
    void FaceToNormal(std::vector<std::string> value);

    void MakeCorner(uint32_t val1, uint32_t val2, uint32_t val3, glm::vec3 center); 

    void ReadFace(std::vector<std::string> value, glm::vec3 center);

    void MakeCorner(std::string val);

    MaterialUPtr SetMaterial();

    void SetMinMax(glm::vec3 position);

    std::vector<MeshPtr> m_meshes;
    MaterialPtr m_material;
    std::vector<std::pair<std::string, std::vector<std::string>>> mtl;

    std::vector<Vertex> vertices;

    std::string MtlPath;
    std::vector<glm::vec3> v;
    std::vector<glm::vec2> vt;
    std::vector<glm::vec3> vn;
    std::vector<std::vector<std::string>> f;
    glm::vec3 min {0};
    glm::vec3 max {0};
    int faceSize {0};

    int count {0};
};

#endif