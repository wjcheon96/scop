#include "Common.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

std::optional<std::string> LoadTextFile(const std::string& filename) {
    std::ifstream fin(filename);
    if (!fin.is_open()) {
        SPDLOG_ERROR("failed to open file: {}", filename);
        return {};
    }
    std::stringstream text;
    text << fin.rdbuf();
    return text.str();
}

std::vector<std::pair<std::string, std::vector<std::string>>> tokenize(std::istringstream& text) {
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(text, line)) {
        lines.push_back(line);
    }

    std::vector<std::pair<std::string, std::vector<std::string>>> token;
    for (const std::string& line : lines) {
        std::istringstream lineStream(line);
        std::string key, word;
        std::vector<std::string> vertex;
        // 남은 부분을 공백 단위로 분할하여 벡터에 추가
        if (lineStream >> key && key[0] != '#') {
            while (lineStream >> word) {
                vertex.push_back(word);
            }
            token.push_back(std::make_pair(key, vertex));
        }
    }
    return token;
}

Vector3 GetAttenuationCoeff(float distance) {
    const auto linear_coeff = Vector4(
        8.4523112e-05, 4.4712582e+00, -1.8516388e+00, 3.3955811e+01
    );
    const auto quad_coeff = Vector4(
        -7.6103583e-04, 9.0120201e+00, -1.1618500e+01, 1.0000464e+02
    );

    float kc = 1.0f;
    float d = 1.0f / distance;
    auto dvec = Vector4(1.0f, d, d*d, d*d*d);
    float kl = Vector4::Dot(linear_coeff, dvec);
    float kq = Vector4::Dot(quad_coeff, dvec);
    
    float f1, f2;
    f1 = kl > 0.0f ? kl : 0.0f;
    f2 = kq * kq > 0.0f ? kq * kq : 0.0f;
    return Vector3(kc, f1, f2);
}

float radians(float degrees) {
    return degrees * (M_PI / 180.0f);
}