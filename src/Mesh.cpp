#include "Mesh.h"
#include <iostream>

MeshUPtr Mesh::Create(const std::vector<Vertex>& vertices, int count, uint32_t primitiveType) {
    auto mesh = MeshUPtr(new Mesh());
    mesh->Init(vertices, count, primitiveType);
    return std::move(mesh);
}

MeshUPtr Mesh::Create(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, uint32_t primitiveType) {
    auto mesh = MeshUPtr(new Mesh());
    mesh->Init(vertices, indices, primitiveType);
    return std::move(mesh);
}

void Mesh::Init(const std::vector<Vertex>& vertices, int count, uint32_t primitiveType) {
    m_count = count;
    m_vertexLayout = VertexLayout::Create();
    m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW,
        vertices.data(), sizeof(Vertex), vertices.size());

    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, false, sizeof(Vertex), 0);
    m_vertexLayout->SetAttrib(1, 3, GL_FLOAT, false, sizeof(Vertex), offsetof(Vertex, normal));
    m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, false, sizeof(Vertex), offsetof(Vertex, texCoord));
}

void Mesh::Init(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, uint32_t primitiveType) {
    m_vertexLayout = VertexLayout::Create();
    m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW,
        vertices.data(), sizeof(Vertex), vertices.size());
    m_indexBuffer = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW,
        indices.data(), sizeof(uint32_t), indices.size());
    // position
    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, false,
        sizeof(Vertex), 0);
    // normal
    m_vertexLayout->SetAttrib(1, 3, GL_FLOAT, false,
        sizeof(Vertex), offsetof(Vertex, normal));
    // texcoordinate
    // offsetof operator는 structure안의 인자로 들어온 member가 얼마만큼 건너 뛰었는지를 계산해준다.
    m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, false,
        sizeof(Vertex), offsetof(Vertex, texCoord));
    m_materials[0]->diffuse = Texture::CreateFromImage(
        Image::CreateSingleColorImage(4, 4,
            ft::vec4(0.5f, 0.1f, 0.1f, 1.0f)).get());

    m_materials[0]->specular = Texture::CreateFromImage(
        Image::CreateSingleColorImage(4, 4,
            ft::vec4(0.5f, 0.1f, 0.1f, 1.0f)).get());
}

void Mesh::Draw(const Program* program, int idx) const {
    m_vertexLayout->Bind();
    if (m_materials[idx]) {
        m_materials[idx]->SetToProgram(program);
    }
    if (m_indexBuffer)
        glDrawElements(m_primitiveType, m_indexBuffer->GetCount(), GL_UNSIGNED_INT, 0);
    else {
        glDrawArrays(m_primitiveType, 0, m_count);
    }
}


MeshUPtr Mesh::CreateBox() {
    std::vector<Vertex> vertices = {
        Vertex { ft::vec3(-0.5f, -0.5f, -0.5f), ft::vec3( 0.0f,  0.0f, -1.0f), ft::vec2(0.0f, 0.0f) },
        Vertex { ft::vec3( 0.5f, -0.5f, -0.5f), ft::vec3( 0.0f,  0.0f, -1.0f), ft::vec2(1.0f, 0.0f) },
        Vertex { ft::vec3( 0.5f,  0.5f, -0.5f), ft::vec3( 0.0f,  0.0f, -1.0f), ft::vec2(1.0f, 1.0f) },
        Vertex { ft::vec3(-0.5f,  0.5f, -0.5f), ft::vec3( 0.0f,  0.0f, -1.0f), ft::vec2(0.0f, 1.0f) },

        Vertex { ft::vec3(-0.5f, -0.5f,  0.5f), ft::vec3( 0.0f,  0.0f,  1.0f), ft::vec2(0.0f, 0.0f) },
        Vertex { ft::vec3( 0.5f, -0.5f,  0.5f), ft::vec3( 0.0f,  0.0f,  1.0f), ft::vec2(1.0f, 0.0f) },
        Vertex { ft::vec3( 0.5f,  0.5f,  0.5f), ft::vec3( 0.0f,  0.0f,  1.0f), ft::vec2(1.0f, 1.0f) },
        Vertex { ft::vec3(-0.5f,  0.5f,  0.5f), ft::vec3( 0.0f,  0.0f,  1.0f), ft::vec2(0.0f, 1.0f) },

        Vertex { ft::vec3(-0.5f,  0.5f,  0.5f), ft::vec3(-1.0f,  0.0f,  0.0f), ft::vec2(1.0f, 0.0f) },
        Vertex { ft::vec3(-0.5f,  0.5f, -0.5f), ft::vec3(-1.0f,  0.0f,  0.0f), ft::vec2(1.0f, 1.0f) },
        Vertex { ft::vec3(-0.5f, -0.5f, -0.5f), ft::vec3(-1.0f,  0.0f,  0.0f), ft::vec2(0.0f, 1.0f) },
        Vertex { ft::vec3(-0.5f, -0.5f,  0.5f), ft::vec3(-1.0f,  0.0f,  0.0f), ft::vec2(0.0f, 0.0f) },

        Vertex { ft::vec3( 0.5f,  0.5f,  0.5f), ft::vec3( 1.0f,  0.0f,  0.0f), ft::vec2(1.0f, 0.0f) },
        Vertex { ft::vec3( 0.5f,  0.5f, -0.5f), ft::vec3( 1.0f,  0.0f,  0.0f), ft::vec2(1.0f, 1.0f) },
        Vertex { ft::vec3( 0.5f, -0.5f, -0.5f), ft::vec3( 1.0f,  0.0f,  0.0f), ft::vec2(0.0f, 1.0f) },
        Vertex { ft::vec3( 0.5f, -0.5f,  0.5f), ft::vec3( 1.0f,  0.0f,  0.0f), ft::vec2(0.0f, 0.0f) },

        Vertex { ft::vec3(-0.5f, -0.5f, -0.5f), ft::vec3( 0.0f, -1.0f,  0.0f), ft::vec2(0.0f, 1.0f) },
        Vertex { ft::vec3( 0.5f, -0.5f, -0.5f), ft::vec3( 0.0f, -1.0f,  0.0f), ft::vec2(1.0f, 1.0f) },
        Vertex { ft::vec3( 0.5f, -0.5f,  0.5f), ft::vec3( 0.0f, -1.0f,  0.0f), ft::vec2(1.0f, 0.0f) },
        Vertex { ft::vec3(-0.5f, -0.5f,  0.5f), ft::vec3( 0.0f, -1.0f,  0.0f), ft::vec2(0.0f, 0.0f) },

        Vertex { ft::vec3(-0.5f,  0.5f, -0.5f), ft::vec3( 0.0f,  1.0f,  0.0f), ft::vec2(0.0f, 1.0f) },
        Vertex { ft::vec3( 0.5f,  0.5f, -0.5f), ft::vec3( 0.0f,  1.0f,  0.0f), ft::vec2(1.0f, 1.0f) },
        Vertex { ft::vec3( 0.5f,  0.5f,  0.5f), ft::vec3( 0.0f,  1.0f,  0.0f), ft::vec2(1.0f, 0.0f) },
        Vertex { ft::vec3(-0.5f,  0.5f,  0.5f), ft::vec3( 0.0f,  1.0f,  0.0f), ft::vec2(0.0f, 0.0f) },
    };

    std::vector<uint32_t> indices = {
         0,  2,  1,  2,  0,  3,
         4,  5,  6,  6,  7,  4,
         8,  9, 10, 10, 11,  8,
        12, 14, 13, 14, 12, 15,
        16, 17, 18, 18, 19, 16,
        20, 22, 21, 22, 20, 23,
    };

    return Create(vertices, indices, GL_TRIANGLES);
}

void Material::SetToProgram(const Program* program) const {
    int textureCount = 0;
    // material의 diffuse가 있으면, 0번 슬롯에 바인딩을 시킨다.
    if (diffuse) {
        glActiveTexture(GL_TEXTURE0 + textureCount);
        program->SetUniform("material.diffuse", textureCount);
        diffuse->Bind();
        textureCount++;
    }
    // specula가 있을시, 동일하게 진행.
    if (specular) {
        glActiveTexture(GL_TEXTURE0 + textureCount);
        program->SetUniform("material.specular", textureCount);
        specular->Bind();
        textureCount++;
    }
    // texture 다시 0번으로 초기화시켜서, shininess 추가.
    glActiveTexture(GL_TEXTURE0);
    program->SetUniform("material.shininess", shininess);
}


void Mesh::Draw(const Program* program) const {
    m_vertexLayout->Bind();
    if (m_material) {
        m_material->SetToProgram(program);
    }
    glDrawElements(m_primitiveType, m_indexBuffer->GetCount(), GL_UNSIGNED_INT, 0);
}