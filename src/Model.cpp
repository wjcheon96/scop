#include "Model.h"
#include "Common.h"
#include "Mesh.h"
#include "VertexLayout.h"

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


void Model::MakeCorner(std::string val) {
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
    indices.push_back(toInt(v_vt_vn_tokens[0]));
}

void Model::ReadFace(std::vector<std::string> value) {
    int triangleCount = value.size() - 2;
    for (int i = 0; i < triangleCount; i++) {
        MakeCorner(value[i]);
        MakeCorner(value[i + 1]);
        MakeCorner(value[i + 2]);
    }
}

bool Model::LoadObj(const std::string& filename) {
    auto result = LoadTextFile(filename);
    if (!result) {
        return false;
    }

    std::istringstream iss(*result);
    obj = tokenize(iss);
    vertices.resize(obj.size() / 3);

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
            vt.push_back(glm::vec3(toFloat(value[0]), toFloat(value[1]), toFloat(value[2])));
        } else if (key == "vn") {
            vn.push_back(glm::vec3(toFloat(value[0]), toFloat(value[1]), toFloat(value[2])));
        } else if (key == "f") {
            ReadFace(value);
        } else if (key == "usemtl" || key == "s") {
            continue ;
        }
    }

    auto glMesh = Mesh::Create(vertices, indices, GL_TRIANGLES);
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