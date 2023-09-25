#include "Context.h"
#include "Mesh.h"

ContextUPtr Context::Create() {
    auto context = ContextUPtr(new Context());
    if (!context->Init())
        return nullptr;
    return std::move(context);
}

// 창 크기 변경시 해당 함수가 호출되며, resizing을 진행한다.
void Context::Reshape(int width, int height) {
    m_width = width;
    m_height = height;
    glViewport(0, 0, m_width, m_height);
}

// 키 입력에 따라상 하 좌 우 에 대해 이동을 하게끔 한다.
void Context::KeyInput(GLFWwindow* window) {

    const float cameraSpeed = 0.05f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * m_cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * m_cameraFront;

    auto cameraLeft = glm::normalize(glm::cross(m_cameraUp, m_cameraFront));
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * cameraLeft;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * cameraLeft;

    auto cameraUp = glm::normalize(glm::cross(m_cameraFront, cameraLeft));
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * cameraUp;
}

void Context::MouseMove(double x, double y) {
    // 마우스 클릭이 안 됐으면 해당 함수를 실행시키지 않는다.
    if (!m_cameraControl)
        return;
    auto pos = glm::vec2((float)x, (float)y);
    auto deltaPos = pos - m_prevMousePos;
    static glm::vec2 prevPos = glm::vec2((float)x, (float)y);

    const float cameraRotSpeed = 0.8f;
    // yaw와 pitch를 계산한다. rotspeed를 조절하여 너무 빠른 전환이 안 일어나게끔 한다.
    m_cameraYaw -= deltaPos.x * cameraRotSpeed;
    m_cameraPitch -= deltaPos.y * cameraRotSpeed;

    // 사이즈가 각도를 초과하면 0 ~ 360 사이의 값으로 다시 바꿔줌.
    if (m_cameraYaw < 0.0f)   m_cameraYaw += 360.0f;
    if (m_cameraYaw > 360.0f) m_cameraYaw -= 360.0f;

    if (m_cameraPitch > 89.0f)  m_cameraPitch = 89.0f;
    if (m_cameraPitch < -89.0f) m_cameraPitch = -89.0f;

    m_prevMousePos = pos;
}

void Context::MouseButton(int button, int action, double x, double y) {
    if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
        if (action == GLFW_PRESS) {
        // 마우스 조작 시작 시점에 현재 마우스 커서 위치 저장
            m_prevMousePos = glm::vec2((float)x, (float)y);
            m_cameraControl = true;
        }
        else if (action == GLFW_RELEASE) {
            m_cameraControl = false;
        }
    }
}

void Context::Render() {
    if (ImGui::Begin("ui window")) {
        if (ImGui::CollapsingHeader("light", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::DragFloat3("l.position", glm::value_ptr(m_light.position), 0.01f);
            ImGui::DragFloat3("l.direction", glm::value_ptr(m_light.direction), 0.01f);
            ImGui::DragFloat2("l.cutoff", glm::value_ptr(m_light.cutoff), 0.1f, 0.0f, 180.0f);
            ImGui::DragFloat("l.distance", &m_light.distance, 0.1f, 0.0f, 1000.0f);
            ImGui::ColorEdit3("l.ambient", glm::value_ptr(m_light.ambient));
            ImGui::ColorEdit3("l.diffuse", glm::value_ptr(m_light.diffuse));
            ImGui::ColorEdit3("l.specular", glm::value_ptr(m_light.specular));
            ImGui::Checkbox("flash light", &m_flashLightMode);
            ImGui::Checkbox("l.blinn", &m_blinn);
        }

        if (ImGui::ColorEdit4("clear color", glm::value_ptr(m_clearColor))) {
            glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);
        }
        ImGui::Separator();
        ImGui::DragFloat3("camera pos", glm::value_ptr(m_cameraPos), 0.01f);
        ImGui::DragFloat("camera yaw", &m_cameraYaw, 0.5f);
        ImGui::DragFloat("camera pitch", &m_cameraPitch, 0.5f, -89.0f, 89.0f);
        ImGui::Separator();
        if (ImGui::Button("reset camera")) {
            m_cameraYaw = 0.0f;
            m_cameraPitch = 0.0f;
            m_cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
        }
    }
    ImGui::End();
    // 실제 framebuffer를 clear함.
    // depth초기화
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // depth test를 켜서, z 버퍼상 뒤에 있는 그림(1에 가까운쪽)을 안 그리게끔 한다.
    glEnable(GL_DEPTH_TEST);

    // m_cameraFront를 yaw/pitch에 따라 방향 결정.(0, 0, -1)방향을 x축, y축에 따라 회전시킨다.
    m_cameraFront = glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraYaw), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f),
        glm::radians(m_cameraPitch), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
    
    // 손전등 효과를 주는 방법
    // m_light.position = m_cameraPos;
    // m_light.direction = m_cameraFront;

    auto cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    auto cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    auto cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    // 카메라는 카메라 벡터에 대해 설정된 값에 따라서 보이는 위치를 잡아준다.
    auto view = glm::lookAt(
      m_cameraPos,
      m_cameraPos + m_cameraFront,
      m_cameraUp);

    // zNear, zFar 파라미터가 어느 지점까지 보이는지를 확인 가능하게 한다.
    // // 종횡비 4:3, 세로화각 45도의 원근 투영
    auto projection = glm::perspective(glm::radians(45.0f),(float)m_width / (float)m_height, 0.01f, 50.0f);

    glm::vec3 lightPos = m_light.position;
    glm::vec3 lightDir = m_light.direction;
    if (m_flashLightMode) {
        lightPos = m_cameraPos;
        lightDir = m_cameraFront;
    } else {
        auto lightModelTransform = glm::translate(glm::mat4(1.0), m_light.position) *
            glm::scale(glm::mat4(1.0), glm::vec3(0.1f));

        m_simpleProgram->Use();
        m_simpleProgram->SetUniform("color", glm::vec4(m_light.ambient + m_light.diffuse, 1.0f));
        m_simpleProgram->SetUniform("transform", projection * view * lightModelTransform);
        m_box->Draw(m_program.get());
    }

    m_program->Use();
    m_program->SetUniform("viewPos", m_cameraPos);
    m_program->SetUniform("light.position", lightPos);
    m_program->SetUniform("light.direction", lightDir);
    m_program->SetUniform("light.cutoff", glm::vec2(
        cosf(glm::radians(m_light.cutoff[0])), cosf(glm::radians(m_light.cutoff[0] + m_light.cutoff[1]))));
    m_program->SetUniform("light.attenuation", GetAttenuationCoeff(m_light.distance));
    m_program->SetUniform("light.ambient", m_light.ambient);
    m_program->SetUniform("light.diffuse", m_light.diffuse);
    m_program->SetUniform("light.specular", m_light.specular);

    m_program->SetUniform("material.diffuse", 0);
    m_program->SetUniform("material.specular", 1);
    m_program->SetUniform("material.shininess", m_material.shininess);
    m_program->SetUniform("blinn", (m_blinn ? 1 : 0));
    // glActiveTexture(GL_TEXTURE0);
    // m_material.diffuse->Bind();
    // glActiveTexture(GL_TEXTURE1);
    // m_material.specular->Bind();


    auto modelTransform = glm::mat4(1.0f);
    auto transform = projection * view * modelTransform;
    m_program->SetUniform("transform", transform);
    m_program->SetUniform("modelTransform", modelTransform);
    m_model->Draw(m_program.get());

}

bool Context::Init() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);

    m_box = Mesh::CreateBox();
    m_model = Model::Load("./resources/42.obj");

    m_simpleProgram = Program::Create("./shader/simple.vs", "./shader/simple.fs");
    if (!m_simpleProgram)
        return false;
    SPDLOG_INFO("program id: {}", m_simpleProgram->Get());

    m_program = Program::Create("./shader/lighting.vs", "./shader/lighting.fs");
    if (!m_program)
        return false;
    SPDLOG_INFO("program id: {}", m_program->Get());

    m_material.diffuse = Texture::CreateFromImage(
    Image::CreateSingleColorImage(4, 4, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)).get());

    m_material.specular = Texture::CreateFromImage(
    Image::CreateSingleColorImage(4, 4, glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)).get());


    m_program->Use();
	m_program->SetUniform("tex", 0);
    m_program->SetUniform("tex2", 1);

    return true;    
}
