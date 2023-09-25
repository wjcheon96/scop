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
            m_material->diffuse = Texture::CreateFromImage(Image::CreateSingleColorImage(4, 4,
                glm::vec4(toFloat(val[0]), toFloat(val[1]), toFloat(val[2]), 1.0f)).get());
        }
        else if (key == "ks") {
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
    Vertex vert;

    vert.position = v[toInt(val1) - 1];
    vert.normal = MakeNormal(v[toInt(val1) - 1], v[toInt(val2) - 1], v[toInt(val3) - 1]);
    vertices.push_back(vert);
    indices.push_back(toInt(val1) - 1);

    vert.position = v[toInt(val2) - 1];
    vert.normal = MakeNormal(v[toInt(val1) - 1], v[toInt(val2) - 1], v[toInt(val3) - 1]);
    vertices.push_back(vert);
    indices.push_back(toInt(val2) - 1);

    vert.position = v[toInt(val3) - 1];
    vert.normal = MakeNormal(v[toInt(val1) - 1], v[toInt(val2) - 1], v[toInt(val3) - 1]);
    vertices.push_back(vert);
    indices.push_back(toInt(val3) - 1);
}

void Model::MakeCorner(std::string val1, std::string val2, std::string val3, std::vector<glm::vec3> v, std::vector<glm::vec2> vt) {
    std::vector<std::string> v_vt_vn_tokens;
    std::istringstream ss(val1);
    std::string token;
    while (std::getline(ss, token, '/')) {
        v_vt_vn_tokens.push_back(token);
    }
    Vertex vert;
    vert.position = v[toInt(v_vt_vn_tokens[0]) - 1];
    vert.normal = v[toInt(v_vt_vn_tokens[1]) - 1];
    vertices.push_back(vert);
    indices.push_back(toInt(v_vt_vn_tokens[0]) - 1);
}

void Model::MakeCorner(std::string val, std::vector<glm::vec3> v, std::vector<glm::vec3> vn) {
    std::vector<std::string> v_vt_vn_tokens;
    std::istringstream ss(val);
    std::string token;
    while (std::getline(ss, token, '/')) {
        v_vt_vn_tokens.push_back(token);
    }
    Vertex vert;
    vert.position = v[toInt(v_vt_vn_tokens[0]) - 1];
    vert.normal = vn[toInt(v_vt_vn_tokens[1]) - 1];
    vertices.push_back(vert);
    indices.push_back(toInt(v_vt_vn_tokens[0]) - 1);
}

void Model::MakeCorner(std::string val, std::vector<glm::vec3> v, std::vector<glm::vec2> vt, std::vector<glm::vec3> vn) {
    std::vector<std::string> v_vt_vn_tokens;
    std::istringstream ss(val);
    std::string token;
    while (std::getline(ss, token, '/')) {
        v_vt_vn_tokens.push_back(token);
    }
    Vertex vert;
    vert.position = v[toInt(v_vt_vn_tokens[0]) - 1];
    vert.texCoord = vt[toInt(v_vt_vn_tokens[1]) - 1];
    vert.normal = vn[toInt(v_vt_vn_tokens[2]) - 1];
    vertices.push_back(vert);
    indices.push_back(toInt(v_vt_vn_tokens[0]) - 1);
}

// texture가 없을땐 대충 아무거나 만들어서 던져주기
void Model::ReadFace(std::vector<std::string> value, std::vector<glm::vec3> v, std::vector<glm::vec2> vt, std::vector<glm::vec3> vn) {
    if (vt.empty() || vn.empty()) {
        int triangleCount = value.size() - 2;
        for (int i = 0; i < triangleCount; i++) {
            MakeCorner(value[i], value[i + 1], value[i + 2], v);
        }
    } else if (vt.empty()) {
        int triangleCount = value.size() - 2;
        for (int i = 0; i < triangleCount; i++) {
            MakeCorner(value[i], v, vn);
            MakeCorner(value[i + 1], v, vn);
            MakeCorner(value[i + 2], v, vn);
        }
    } else if (vn.empty()) {
        int triangleCount = value.size() - 2;
        for (int i = 0; i < triangleCount; i++) {
            MakeCorner(value[i], value[i + 1], value[i + 2], v, vt);
        }
    } else {
        int triangleCount = value.size() - 2;
        for (int i = 0; i < triangleCount; i++) {
            MakeCorner(value[i], v, vt, vn);
            MakeCorner(value[i + 1], v, vt, vn);
            MakeCorner(value[i + 2], v, vt, vn);
        }
    }
}

bool Model::LoadObj(const std::string& filename) {
    auto result = LoadTextFile(filename);
    if (!result) {
        return false;
    }

    std::vector<glm::vec3> v;
    std::vector<glm::vec2> vt;
    std::vector<glm::vec3> vn;

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
        } else if (key == "vt") {
            vt.push_back(glm::vec2(toFloat(value[0]), toFloat(value[1])));
        } else if (key == "vn") {
            vn.push_back(glm::vec3(toFloat(value[0]), toFloat(value[1]), toFloat(value[2])));
        } else if (key == "f") {
            ReadFace(value, v, vt, vn);
        } else if (key == "usemtl" || key == "s") {
            continue ;
        }
    }

    std::cout << vertices.size() << " " << indices.size() << std::endl;
    
    for (int i = 0; i <= indices.size(); i++) {
        std::cout << indices[i] << " ";
    }
    
    auto glMesh = Mesh::Create(vertices, indices, GL_TRIANGLES);
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