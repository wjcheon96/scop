#ifndef __MODEL_H__
#define __MODEL_H__

#include <sstream>
#include <iostream>
#include "Common.h"
#include "Mesh.h"

typedef struct vec3 {
    GLfloat x;
    GLfloat y;
    GLfloat z;
} vec3;

CLASS_PTR(Model);
class Model {
public:
    static ModelUPtr Load(const std::string& filename);

    int GetMeshCount() const { return (int)m_meshes.size(); }
    MeshPtr GetMesh(int index) const { return m_meshes[index]; }
    void Draw(const Program* program) const;
    bool LoadMtl(const std::string& filename);
    std::vector<std::pair<std::string, std::vector<std::string>>> GetMtl() const { return obj; }
    std::vector<std::pair<std::string, std::vector<std::string>>> GetObj() const { return mtl; }

private:
    Model() {}
    bool LoadObj(const std::string& filename);
    GLfloat toFloat(std::string token);
    uint32_t toInt(std::string token);
    glm::vec3 MakeNormal(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3);


    void MakeCorner(std::string val1, std::string val2, std::string val3, std::vector<glm::vec3> v); 

    void ReadFace(std::vector<std::string> value, std::vector<glm::vec3> v);

    void MakeCorner(std::string val);

    void SetMaterial();

    std::vector<MeshPtr> m_meshes;
    MaterialPtr m_material;
    std::vector<std::pair<std::string, std::vector<std::string>>> obj;
    std::vector<std::pair<std::string, std::vector<std::string>>> mtl;
    std::string name;

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<glm::vec3> vertexNormal;
    int count {0};
};

#endif