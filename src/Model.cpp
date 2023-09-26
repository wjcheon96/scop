#include "Model.h"
#include "Common.h"
#include "Mesh.h"
#include "VertexLayout.h"
#include <vector>

ModelUPtr Model::Load(const std::string& filename) {
    auto model = ModelUPtr(new Model());
    if (!model->LoadObj(filename))
        return nullptr;
    return std::move(model);
}

bool Model::LoadMtl(const std::string& filename) {
    auto result = LoadTextFile("./resources/" + filename);
    if (!result) {
        return false;
    }

    int count = 0;

    std::istringstream iss(*result);
    mtl = tokenize(iss);
    return true;
}

void Model::SetMaterial() {
    m_material = Material::Create();
    if (mtl.empty()) {
            m_material->diffuse = Texture::CreateFromImage(
                Image::CreateSingleColorImage(4, 4,
                    glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)).get());

            m_material->specular = Texture::CreateFromImage(
                Image::CreateSingleColorImage(4, 4,
                    glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)).get());
        return ;
    }

    for (int i = 0; i < mtl.size(); i++) {
        std::string key = mtl[i].first;
        std::vector<std::string> val = mtl[i].second;
        if (key == "Kd") {
            m_material->diffuse = Texture::CreateFromImage(Image::CreateSingleColorImage(4, 4,
                glm::vec4(toFloat(val[0]), toFloat(val[1]), toFloat(val[2]), 1.0f)).get());
        }
        else if (key == "Ks") {
            m_material->specular = Texture::CreateFromImage(Image::CreateSingleColorImage(4, 4,
                glm::vec4(toFloat(val[0]), toFloat(val[1]), toFloat(val[2]), 1.0f)).get());
        }
    }
}

// make vertex normal
void Model::MakeNormal(uint32_t v1, uint32_t v2, uint32_t v3, std::vector<glm::vec3> v) {
    glm::vec3 vn = glm::normalize(glm::cross(v[v2 - 1] - v[v1 - 1], v[v3 - 1] - v[v1 - 1]));    // face normal
    normal[v1] = glm::normalize(vn + normal[v1]);
    normal[v2] = glm::normalize(vn + normal[v2]);
    normal[v3] = glm::normalize(vn + normal[v3]);
}

void Model::MakeCorner(uint32_t val1, uint32_t val2, uint32_t val3, std::vector<glm::vec3> v) {
    count += 9;
    Vertex vt;

    vt.position = v[val1 - 1];
    vt.normal = normal[val1];
    vertices.push_back(vt);
    vt.position = v[val2 - 1];
    vt.normal = normal[val2];
    vertices.push_back(vt);
    vt.position = v[val3 - 1];
    vt.normal = normal[val3];
    vertices.push_back(vt);
}

void Model::ReadFace(std::vector<std::string> value, std::vector<glm::vec3> v) {
    int triangleCount = value.size() - 2;
    if (triangleCount == 2) {
        MakeCorner(toInt(value[0]), toInt(value[1]), toInt(value[2]), v);
        MakeCorner(toInt(value[0]), toInt(value[2]), toInt(value[3]), v);
    } else if (triangleCount == 1) {
        MakeCorner(toInt(value[0]), toInt(value[1]), toInt(value[2]), v);
    }
}

void Model::FaceToNormal(std::vector<std::string> value, std::vector<glm::vec3> v) {
    int triangleCount = value.size() - 2;
    if (triangleCount == 2) {
        MakeNormal(toInt(value[0]), toInt(value[1]), toInt(value[2]), v);
        MakeNormal(toInt(value[0]), toInt(value[2]), toInt(value[3]), v);
    } else if (triangleCount == 1) {
        MakeNormal(toInt(value[0]), toInt(value[1]), toInt(value[2]), v);
    }
}


bool Model::LoadObj(const std::string& filename) {
    auto result = LoadTextFile(filename);
    if (!result) {
        return false;
    }
    std::vector<glm::vec3> v;
    std::istringstream iss(*result);
    obj = tokenize(iss);
    int size = 0;
    for (std::pair line : obj) {
        if (line.first == "f") {
            int triangleCount = line.second.size() - 2;
            size += triangleCount;
        }
    }
    normal.resize(size);
    for (int i = 0; i < obj.size(); i++) {
        std::string key = obj[i].first;
        std::vector<std::string> value = obj[i].second;
        if (key == "mtllib") {
            LoadMtl(value[0]);
        } else if (key == "o") {
            name = value[0];
        } else if (key == "v") {
            v.push_back(glm::vec3(toFloat(value[0]), toFloat(value[1]), toFloat(value[2])));
        } else if (key == "f") {
            FaceToNormal(obj[i].second, v);
            // ReadFace(value, v);
        } else if (key == "usemtl" || key == "s") {
            continue ;
        }
    }

    for (std::pair line : obj) {
        if (line.first == "f") {
            ReadFace(line.second, v);
        }
    }

    auto glMesh = Mesh::Create(vertices, count, GL_TRIANGLES);
    SetMaterial();
    glMesh->SetMaterial(m_material);
    m_meshes.push_back(std::move(glMesh));
    return true;
}

void Model::Draw(const Program* program) const {
    for (auto& mesh: m_meshes) {
        mesh->Draw(program);
    }
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