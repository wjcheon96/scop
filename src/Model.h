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

    void Draw(const Program* program, int idx) const;
    bool LoadMtl(const std::string& filename);

private:
    Model() {}
    bool LoadObj(const std::string& filename);
    GLfloat toFloat(std::string token);
    uint32_t toInt(std::string token);
    
    void TokenizeObj(std::istringstream& text);
    void TokenizeMtl(std::istringstream& text);
    void MakeNormal(uint32_t v1, uint32_t v2, uint32_t v3);
    void FaceToNormal(std::vector<std::string> value);

    void MakeCorner(uint32_t val1, uint32_t val2, uint32_t val3, ft::vec3 center); 

    void ReadFace(std::vector<std::string> value, ft::vec3 center);

    void MakeCorner(std::string val);

    void SetMaterial();

    void SetMinMax(ft::vec3 position);

    MeshPtr m_mesh;
    std::vector<MaterialPtr> m_materials;
    std::vector<std::pair<std::string, std::vector<std::string>>> mtl;

    std::vector<Vertex> vertices;

    std::string MtlPath;
    std::vector<ft::vec3> v;
    std::vector<ft::vec2> vt;
    std::vector<ft::vec3> vn;
    std::vector<std::vector<std::string>> f;
    ft::vec3 min {0};
    ft::vec3 max {0};
    int faceSize {0};

    int count {0};
};

#endif