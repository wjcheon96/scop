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
            std::cout << val[0] << " " << val[1] << " " << val[2] << std::endl;
            m_material->diffuse = Texture::CreateFromImage(Image::CreateSingleColorImage(4, 4,
                glm::vec4(toFloat(val[0]), toFloat(val[1]), toFloat(val[2]), 1.0f)).get());
        }
        else if (key == "Ks") {
            m_material->specular = Texture::CreateFromImage(Image::CreateSingleColorImage(4, 4,
                glm::vec4(toFloat(val[0]), toFloat(val[1]), toFloat(val[2]), 1.0f)).get());
        }
    }
}

glm::vec3 Model::MakeNormal(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) {
    glm::vec3 normal = glm::normalize(glm::cross(v2 - v1, v3 - v1));
    return normal;
}

void Model::MakeCorner(std::string val1, std::string val2, std::string val3, std::vector<glm::vec3> v) {
    glm::vec3 vn = MakeNormal(v[toInt(val1) - 1], v[toInt(val2) - 1], v[toInt(val3) - 1]);
    count += 9;
    Vertex vt;

    vt.position = v[toInt(val1) - 1];
    vt.normal = vn;
    vertices.push_back(vt);
    vt.position = v[toInt(val2) - 1];
    vt.normal = vn;
    vertices.push_back(vt);
    vt.position = v[toInt(val3) - 1];
    vt.normal = vn;
    vertices.push_back(vt);
}

void Model::ReadFace(std::vector<std::string> value, std::vector<glm::vec3> v) {
    int triangleCount = value.size() - 2;
    if (triangleCount == 2) {
        MakeCorner(value[0], value[1], value[2], v);
        MakeCorner(value[0], value[2], value[3], v);
    } else if (triangleCount == 1) {
        MakeCorner(value[0], value[1], value[2], v);
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
            ReadFace(value, v);
        } else if (key == "usemtl" || key == "s") {
            continue ;
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