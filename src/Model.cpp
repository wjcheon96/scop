#include "Model.h"
#include "Common.h"
#include "Mesh.h"
#include "VertexLayout.h"
#include "glm/fwd.hpp"
#include <utility>
#include <vector>
#include <iostream>

ModelUPtr Model::Load(const std::string& filename) {
    auto model = ModelUPtr(new Model());
    if (!model->LoadObj(filename))
        return nullptr;
    return std::move(model);
}

void Model::SetMinMax(Vector3 position) {
    position.x > max.x ? max.x = position.x : max.x;
    position.y > max.y ? max.y = position.y : max.y;
    position.z > max.z ? max.z = position.z : max.z;

    min.x > position.x ? min.x = position.x : min.x;
    min.y > position.y ? min.y = position.y : min.y;
    min.z > position.z ? min.z = position.z : min.z;
}

bool Model::LoadMtl(const std::string& filename) {
    auto result = LoadTextFile("./resources/" + filename);
    if (!result) {
        return false;
    }

    std::istringstream iss(*result);
    // tokenize(iss);

    mtl = tokenize(iss);
    return true;
}


void Model::SetMaterial() {
    auto material = Material::Create();
    if (mtl.empty()) {
        material->diffuse = Texture::CreateFromImage(
            Image::CreateSingleColorImage(4, 4,
                Vector4(0.5f, 0.1f, 0.1f, 1.0f)).get());

        material->specular = Texture::CreateFromImage(
            Image::CreateSingleColorImage(4, 4,
                Vector4(0.5f, 0.1f, 0.1f, 1.0f)).get());
    }
    else {
        for (int i = 0; i < mtl.size(); i++) {
            std::string key = mtl[i].first;
            std::vector<std::string> val = mtl[i].second;
            if (key == "Kd") {
                material->diffuse = Texture::CreateFromImage(Image::CreateSingleColorImage(4, 4,
                    Vector4(toFloat(val[0]), toFloat(val[1]), toFloat(val[2]), 1.0f)).get());
            }
            else if (key == "Ks") {
                material->specular = Texture::CreateFromImage(Image::CreateSingleColorImage(4, 4,
                    Vector4(toFloat(val[0]), toFloat(val[1]), toFloat(val[2]), 1.0f)).get());
            }
        }
    }
    m_materials.push_back(std::move(material));
    ImageUPtr image;
    image = Image::Load("./resources/slime.png");
    auto material1 = Material::Create();
    material1->diffuse = Texture::CreateFromImage(image.get());
    material1->specular = Texture::CreateFromImage(image.get());
    m_materials.push_back(std::move(material1));
    
    auto material2 = Material::Create();
    image = Image::Load("./resources/galaxy.jpeg");
    material2->diffuse = Texture::CreateFromImage(image.get());
    material2->specular = Texture::CreateFromImage(image.get());
    m_materials.push_back(std::move(material2));

    auto material3 = Material::Create();
    image = Image::Load("./resources/opengl.png");
    material3->diffuse = Texture::CreateFromImage(image.get());
    material3->specular = Texture::CreateFromImage(image.get());
    m_materials.push_back(std::move(material3));

    auto material4 = Material::Create();
    image = Image::Load("./resources/pinkbean.jpeg");
    material4->diffuse = Texture::CreateFromImage(image.get());
    material4->specular = Texture::CreateFromImage(image.get());
    m_materials.push_back(std::move(material4));

}

// make vertex normal
void Model::MakeNormal(uint32_t v1, uint32_t v2, uint32_t v3) {
    Vector3 normal = Vector3::Cross(v[v2 - 1] - v[v1 - 1], v[v3 - 1] - v[v1 - 1]).GetNormalized();    // face normal
    vn[v1] = normal + vn[v1].GetNormalized();
    vn[v2] = normal + vn[v2].GetNormalized();
    vn[v3] = normal + vn[v3].GetNormalized();
}

void Model::MakeCorner(uint32_t val1, uint32_t val2, uint32_t val3, Vector3 center) {
    count += 6;
    Vertex vert;

    vert.position = v[val1 - 1] - center;
    vert.texCoord = Vector2((vn[val1].x + 1.0f) / 2.0f, (vn[val1].y + 1.0f) / 2.0f);
    vert.normal = vn[val1];
    vertices.push_back(vert);
    vert.position = v[val2 - 1] - center;
    vert.texCoord = Vector2((vn[val2].x + 1.0f) / 2.0f, (vn[val2].y + 1.0f) / 2.0f);
    vert.normal = vn[val2];
    vertices.push_back(vert);
    vert.position = v[val3 - 1] - center;
    vert.texCoord = Vector2((vn[val3].x + 1.0f) / 2.0f, (vn[val3].y + 1.0f) / 2.0f);
    vert.normal = vn[val3];
    vertices.push_back(vert);
}

void Model::ReadFace(std::vector<std::string> value, Vector3 center) {
    int triangleCount = value.size() - 2;
    if (triangleCount == 2) {
        MakeCorner(toInt(value[0]), toInt(value[1]), toInt(value[2]), center);
        MakeCorner(toInt(value[0]), toInt(value[2]), toInt(value[3]), center);
    } else if (triangleCount == 1) {
        MakeCorner(toInt(value[0]), toInt(value[1]), toInt(value[2]), center);
    }
}

void Model::FaceToNormal(std::vector<std::string> value) {
    int triangleCount = value.size() - 2;
    if (triangleCount == 2) {
        MakeNormal(toInt(value[0]), toInt(value[1]), toInt(value[2]));
        MakeNormal(toInt(value[0]), toInt(value[2]), toInt(value[3]));
    } else if (triangleCount == 1) {
        MakeNormal(toInt(value[0]), toInt(value[1]), toInt(value[2]));
    }
}

void Model::TokenizeObj(std::istringstream& text) {
    std::vector<std::string> lines;
    std::string line;
    int cnt;
    while (std::getline(text, line)) {
        std::istringstream lineStream(line);
        std::string key, word;
        GLfloat coord;
        Vector3 position, normal;
        Vector2 texCoord;
        if (lineStream >> key && key[0] != '#') {
            if (key == "mtllib") {
                if (lineStream >> word) {
                    LoadMtl(word);
                }
            } else if (key == "v") {
                if (lineStream >> position.x >> position.y >> position.z) {
                    SetMinMax(position);
                    v.push_back(position);
                }
            } else if (key == "f") {
                if (v.empty()) {
                    SPDLOG_ERROR("Obj file invalid error");
                    exit(1);
                } else {
                    std::vector<std::string> face;
                    while (lineStream >> word) {
                        face.push_back(word);
                    }
                    f.push_back(face);
                    faceSize += face.size() - 2;
                }
            } else {
                continue ;
            }
        }
    }
}

bool Model::LoadObj(const std::string& filename) {
    auto result = LoadTextFile(filename);
    if (!result) {
        return false;
    }
    std::istringstream iss(*result);
    TokenizeObj(iss);
    vn.resize(faceSize);

    for (std::vector line : f) {
        FaceToNormal(line);
    }
    
    Vector3 center = Vector3(((max.x + min.x) / 2), ((max.y + min.y) / 2), ((max.z + min.z) / 2));

    for (std::vector line : f) {
        ReadFace(line, center);
    }

    auto glMesh = Mesh::Create(vertices, count, GL_TRIANGLES);
    SetMaterial();
    glMesh->SetMaterial(m_materials);
    m_mesh = std::move(glMesh);
    return true;
}

void Model::Draw(const Program* program, int idx) const {
    m_mesh->Draw(program, idx);
}

GLfloat Model::toFloat(std::string token) {
	GLfloat num = 0.0f;

	std::stringstream ss(token);
	ss >> num;
	return num;
}

uint32_t Model::toInt(std::string token) {
    uint32_t num = 0;

	std::stringstream ss(token);
	ss >> num;
	return num;
}
