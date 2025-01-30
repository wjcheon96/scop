#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include <imgui.h>
#include "Common.h"
#include "Program.h"
#include "Texture.h"
#include "Mesh.h"
#include "Model.h"

CLASS_PTR(Context)
class Context{
    public:
        static ContextUPtr Create();
        void Render();
        void KeyInput(GLFWwindow* window);
        void Reshape(int width, int height);
        void MouseMove(double x, double y);
        void MouseButton(int button, int action, double x, double y);
        void MouseScroll(double yoffset);
        void DrawScene(const ft::mat4& view, const ft::mat4& projection, const Program* program);

    private:
        Context() {}
        bool Init();

        // 카메라 세팅
        ft::vec3 m_cameraPos { ft::vec3(0.0f, 0.0f, 10.0f) };
        // 바라보는 방향 세팅.
        ft::vec3 m_cameraFront { ft::vec3(0.0f, 0.0f, -1.0f) };
        // 카메라가 특정 지점을 바라보게 하는 법.
        ft::vec3 m_cameraUp { ft::vec3(0.0f, 1.0f, 0.0f) };

        // camera parameter.
        // pitch 와 yaw에 대한 값 세팅.
        float m_cameraPitch { 0.0f };
        float m_cameraYaw { 0.0f };
        bool m_cameraControl { false };

        // window 사이즈 세팅
        int m_width { WINDOW_WIDTH };
        int m_height { WINDOW_HEIGHT };
        
        ProgramUPtr m_program;
        ProgramUPtr m_simpleProgram;

        // 빛 세팅
        struct Light {
            ft::vec3 position { ft::vec3(2.0f, 2.0f, 2.0f) };
            ft::vec3 direction { ft::vec3(-1.0f, -1.0f, -1.0f) };
            ft::vec2 cutoff { ft::vec2(20.0f, 5.0f) };
            float distance { 32.0f };
            ft::vec3 ambient { ft::vec3(0.1f, 0.1f, 0.1f) };
            ft::vec3 diffuse { ft::vec3(0.5f, 0.5f, 0.5f) };
            ft::vec3 specular { ft::vec3(1.0f, 1.0f, 1.0f) };
        };
        Light m_light;

        // 재질 세팅
        struct Material {
            TextureUPtr diffuse;
            TextureUPtr specular;
            float shininess { 32.0f };
        };
        Material m_material;

        // imgui에서 사용할 여러 모드들 세팅.
        bool m_animation { true };
        bool m_flashLightMode { false };
        bool m_blinn { false };

        ft::vec2 m_prevMousePos { ft::vec2(0.0f) };

        MeshUPtr m_box;
        ModelUPtr m_model;

        int texNum { 1 };

        ft::vec4 m_clearColor { ft::vec4(0.1f, 0.1f, 0.1f, 0.0f) };
};

#endif