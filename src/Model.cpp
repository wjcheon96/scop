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

void Model::SetMinMax(glm::vec3 position) {
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
    tokenize(iss);

    // mtl = tokenize(iss);
    return true;
}

MaterialUPtr Model::SetTexture() {
    auto material = Material::Create();
    ImageUPtr image;
    image = Image::Load("./resources/slime.png");
    if (!image)
        return nullptr;
    image = Image::Load("./resources/galaxy.jpeg");
    if (!image)
        return nullptr;
    image = Image::Load("./resources/opengl.png");
    if (!image)
        return nullptr;
    image = Image::Load("./resources/slime.png");
    if (!image)
        return nullptr;
    material->diffuse = Texture::CreateFromImage(image.get());
    material->specular = Texture::CreateFromImage(image.get());
    return (std::move(material));
}

MaterialUPtr Model::SetMaterial() {
    auto material = Material::Create();
    if (mtl.empty()) {
        material->diffuse = Texture::CreateFromImage(
            Image::CreateSingleColorImage(4, 4,
                glm::vec4(0.5f, 0.1f, 0.1f, 1.0f)).get());

        material->specular = Texture::CreateFromImage(
            Image::CreateSingleColorImage(4, 4,
                glm::vec4(0.5f, 0.1f, 0.1f, 1.0f)).get());
        return (std::move(material));
    }

    for (int i = 0; i < mtl.size(); i++) {
        std::string key = mtl[i].first;
        std::vector<std::string> val = mtl[i].second;
        if (key == "Kd") {
            material->diffuse = Texture::CreateFromImage(Image::CreateSingleColorImage(4, 4,
                glm::vec4(toFloat(val[0]), toFloat(val[1]), toFloat(val[2]), 1.0f)).get());
        }
        else if (key == "Ks") {
            material->specular = Texture::CreateFromImage(Image::CreateSingleColorImage(4, 4,
                glm::vec4(toFloat(val[0]), toFloat(val[1]), toFloat(val[2]), 1.0f)).get());
        }
    }
    return (std::move(material));
}

// make vertex normal
void Model::MakeNormal(uint32_t v1, uint32_t v2, uint32_t v3) {
    glm::vec3 normal = glm::normalize(glm::cross(v[v2 - 1] - v[v1 - 1], v[v3 - 1] - v[v1 - 1]));    // face normal
    vn[v1] = glm::normalize(normal + vn[v1]);
    vn[v2] = glm::normalize(normal + vn[v2]);
    vn[v3] = glm::normalize(normal + vn[v3]);
}

void Model::MakeCorner(uint32_t val1, uint32_t val2, uint32_t val3, glm::vec3 center) {
    count += 6;
    Vertex vert;

    vert.position = v[val1 - 1] - center;
    vert.texCoord = glm::vec2((vn[val1].x + 1.0f) / 2.0f, (vn[val1].y + 1.0f) / 2.0f);
    vert.normal = vn[val1];
    vertices.push_back(vert);
    vert.position = v[val2 - 1] - center;
    vert.texCoord = glm::vec2((vn[val2].x + 1.0f) / 2.0f, (vn[val2].y + 1.0f) / 2.0f);
    vert.normal = vn[val2];
    vertices.push_back(vert);
    vert.position = v[val3 - 1] - center;
    vert.texCoord = glm::vec2((vn[val3].x + 1.0f) / 2.0f, (vn[val3].y + 1.0f) / 2.0f);
    vert.normal = vn[val3];
    vertices.push_back(vert);
}

void Model::ReadFace(std::vector<std::string> value, glm::vec3 center) {
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
        glm::vec3 position, normal;
        glm::vec2 texCoord;
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
    
    glm::vec3 center = glm::vec3(((max.x + min.x) / 2), ((max.y + min.y) / 2), ((max.z + min.z) / 2));

    for (std::vector line : f) {
        ReadFace(line, center);
    }

    auto glMesh = Mesh::Create(vertices, count, GL_TRIANGLES);
    glMesh->SetMaterial(SetMaterial());
    m_meshes.push_back(std::move(glMesh));
    return true;
}

void Model::Draw(const Program* program) const {
    for (auto& mesh: m_meshes)
        mesh->Draw(program);
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
