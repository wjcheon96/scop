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
        m_cameraPos += Vector3::Mul(cameraSpeed, m_cameraFront);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        m_cameraPos -= Vector3::Mul(cameraSpeed, m_cameraFront);

    auto cameraLeft = Vector3::Cross(m_cameraUp, m_cameraFront).GetNormalized();
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) 
        m_cameraPos -= Vector3::Mul(cameraSpeed, cameraLeft);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        m_cameraPos += Vector3::Mul(cameraSpeed, cameraLeft);

    auto cameraUp = Vector3::Cross(m_cameraFront, cameraLeft).GetNormalized();
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        m_cameraPos += Vector3::Mul(cameraSpeed, cameraUp);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        m_cameraPos -= Vector3::Mul(cameraSpeed, cameraUp);
}

void Context::MouseMove(double x, double y) {
    // 마우스 클릭이 안 됐으면 해당 함수를 실행시키지 않는다.
    if (!m_cameraControl)
        return;
    auto pos = Vector2((float)x, (float)y);
    auto deltaPos = pos - m_prevMousePos;
    static Vector2 prevPos = Vector2((float)x, (float)y);

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
            m_prevMousePos = Vector2((float)x, (float)y);
            m_cameraControl = true;
        }
        else if (action == GLFW_RELEASE) {
            m_cameraControl = false;
        }
    }
}

void Context::Render() {
    if (ImGui::Begin("ui window")) {
        if (ImGui::ColorEdit4("clear color", Vector4::GetValue(m_clearColor))) {
            glClearColor(m_clearColor.x, m_clearColor.y, m_clearColor.z, m_clearColor.w);
        }
        ImGui::Separator();
        if (ImGui::CollapsingHeader("light", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::DragFloat3("l.position", Vector3::GetValue(m_light.position), 0.01f);
            ImGui::DragFloat3("l.direction", Vector3::GetValue(m_light.direction), 0.01f);
            ImGui::DragFloat2("l.cutoff", Vector2::GetValue(m_light.cutoff), 0.1f, 0.0f, 180.0f);
            ImGui::DragFloat("l.distance", &m_light.distance, 0.1f, 0.0f, 1000.0f);
            ImGui::ColorEdit3("l.ambient", Vector3::GetValue(m_light.ambient));
            ImGui::ColorEdit3("l.diffuse", Vector3::GetValue(m_light.diffuse));
            ImGui::ColorEdit3("l.specular", Vector3::GetValue(m_light.specular));
            ImGui::Checkbox("flash light", &m_flashLightMode);
            ImGui::Checkbox("l.blinn", &m_blinn);
        }
        ImGui::Separator();
        ImGui::DragFloat3("camera pos", Vector3::GetValue(m_cameraPos), 0.01f);
        ImGui::DragFloat("camera yaw", &m_cameraYaw, 0.5f);
        ImGui::DragFloat("camera pitch", &m_cameraPitch, 0.5f, -89.0f, 89.0f);
        ImGui::Separator();
        if (ImGui::Button("reset camera")) {
            m_cameraYaw = 0.0f;
            m_cameraPitch = 0.0f;
            m_cameraPos = Vector3(0.0f, 0.0f, 0.0f);
        }

        if (ImGui::CollapsingHeader("material", ImGuiTreeNodeFlags_DefaultOpen)) {
            // prev 버튼 안 먹힘
            if (ImGui::Button("prev")) {
                texNum = (texNum - 2 + 5) % 5 + 1; // -2를 추가하여 이전 숫자로 이동
                if (texNum == 0) {
                    texNum = 5;
                }
            }   
            ImGui::SameLine();
            ImGui::Text("texture number: %d", texNum);
            ImGui::SameLine();
            if (ImGui::Button("next")) texNum = (texNum % 5) + 1;
            ImGui::SameLine();
            if (ImGui::Button("default texture")) texNum = 1;
        }
        ImGui::Checkbox("animation", &m_animation);
    }
    ImGui::End();
    // 실제 framebuffer를 clear함.
    // depth초기화
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // depth test를 켜서, z 버퍼상 뒤에 있는 그림(1에 가까운쪽)을 안 그리게끔 한다.
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);


    Matrix m1 = Matrix(1.0f);
    Matrix m2 = Matrix(1.0f);
    m1.RotateY(m1, radians(m_cameraYaw));
    m2.RotateX(m2, radians(m_cameraPitch));

    Matrix m =  m1 * m2;
    Vector4 zVec = Vector4(0.0f, 0.0f, -1.0f, 0.0f);

    // m_cameraFront를 yaw/pitch에 따라 방향 결정.(0, 0, -1)방향을 x축, y축에 따라 회전시킨다.
    m_cameraFront = Vector3(m[8] * -1, m[9] * -1, m[10] * -1);

    Vector3 target = m_cameraPos + m_cameraFront;

    // 카메라는 카메라 벡터에 대해 설정된 값에 따라서 보이는 위치를 잡아준다.
    auto view = Matrix::LookAt(
        m_cameraPos,
        target,
        m_cameraUp
    );

    // zNear, zFar 파라미터가 어느 지점까지 보이는지를 확인 가능하게 한다.
    // // 종횡비 4:3, 세로화각 45도의 원근 투영
    auto projection = Matrix::GetPerspective(radians(45.0f),(float)m_width / (float)m_height, 0.01f, 50.0f);

    Vector3 lightPos = m_light.position;
    Vector3 lightDir = m_light.direction;
    if (m_flashLightMode) {
        lightPos = m_cameraPos;
        lightDir = m_cameraFront;
    } else {
        Matrix mat(1.0f);
        mat.Translate(mat, m_light.position.x, m_light.position.y, m_light.position.z);
        Matrix mat2(1.0f);
        Vector3 scaleVec(0.1);
        Matrix::Scale(mat2, scaleVec);
        auto lightModelTransform = mat * mat2;

        Vector4 colorVec((m_light.ambient + m_light.diffuse).x, (m_light.ambient + m_light.diffuse).y, (m_light.ambient + m_light.diffuse).z, 1.0f);

        m_simpleProgram->Use();
        m_simpleProgram->SetUniform("color", colorVec);
        m_simpleProgram->SetUniform("transform", projection * view * lightModelTransform);
    }

    m_program->Use();
    m_program->SetUniform("viewPos", m_cameraPos);
    m_program->SetUniform("light.position", lightPos);
    m_program->SetUniform("light.direction", lightDir);
    float val1 = cosf(radians(m_light.cutoff.x));
    float val2 = cosf(radians(m_light.cutoff.x + m_light.cutoff.y));
    Vector2 cutoffVector = Vector2(val1, val2);
    m_program->SetUniform("light.cutoff", cutoffVector);
    m_program->SetUniform("light.attenuation", GetAttenuationCoeff(m_light.distance));
    m_program->SetUniform("light.ambient", m_light.ambient);
    m_program->SetUniform("light.diffuse", m_light.diffuse);
    m_program->SetUniform("light.specular", m_light.specular);
    m_program->SetUniform("material.diffuse", 0);
    m_program->SetUniform("material.specular", 1);
    m_program->SetUniform("blinn", (m_blinn ? 1 : 0));

    Vector3 modelPosition(0.0f, 0.0f, 0.0f);
    Matrix modelTransform = Matrix(1.0f);

    // 초기 위치로 이동 변환을 적용
    Matrix::Translate(modelTransform, modelPosition.x, modelPosition.y, modelPosition.z);

    if (m_animation)
        Matrix::RotateY(modelTransform, radians((float)glfwGetTime() * 120.0f + 20.0f));
    auto transform = projection * view * modelTransform;

    m_program->SetUniform("transform", transform);
    m_program->SetUniform("modelTransform", modelTransform);

    m_model->Draw(m_program.get(), texNum - 1);
}

bool Context::Init() {
    glClearColor(0.0f, 0.1f, 0.2f, 0.0f);

    m_model = Model::Load("./resources/teapot2.obj");
    if (!m_model)
        return false;

    m_simpleProgram = Program::Create("./shader/simple.vs", "./shader/simple.fs");
    if (!m_simpleProgram)
        return false;
    SPDLOG_INFO("program id: {}", m_simpleProgram->Get());

    m_program = Program::Create("./shader/lighting.vs", "./shader/lighting.fs");
    if (!m_program)
        return false;
    SPDLOG_INFO("program id: {}", m_program->Get());

    return true;
}
