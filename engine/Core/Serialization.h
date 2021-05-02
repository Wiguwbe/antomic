/*
   Copyright 2020 Alexandre Pires (c.alexandre.pires@gmail.com)

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
#pragma once
#include "Core/Base.h"
#include "nlohmann/json.hpp"
#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>

namespace Antomic
{

    class Serialization
    {
    public:
        static void Save(const Ref<Scene>& scene, const std::string& file);
        static Ref<Scene> Load(const std::string& file);

        static void Serialize(nlohmann::json &json, const glm::vec2 &in);
        static void Serialize(nlohmann::json &json, const glm::vec3 &in);
        static void Serialize(nlohmann::json &json, const glm::vec4 &in);
        static void Serialize(nlohmann::json &json, const glm::mat3 &in);
        static void Serialize(nlohmann::json &json, const glm::mat4 &in);

        static void Deserialize(const nlohmann::json &json, glm::vec2 &out);
        static void Deserialize(const nlohmann::json &json, glm::vec3 &out);
        static void Deserialize(const nlohmann::json &json, glm::vec4 &out);
        static void Deserialize(const nlohmann::json &json, glm::mat3 &out);
        static void Deserialize(const nlohmann::json &json, glm::mat4 &out);

    };

    inline void Serialization::Serialize(nlohmann::json &json, const glm::vec2 &in)
    {
        json.push_back(in.x);
        json.push_back(in.y);
    }

    inline void Serialization::Serialize(nlohmann::json &json, const glm::vec3 &in)
    {
        json.push_back(in.x);
        json.push_back(in.y);
        json.push_back(in.z);
    }

    inline void Serialization::Serialize(nlohmann::json &json, const glm::vec4 &in)
    {
        json.push_back(in.x);
        json.push_back(in.y);
        json.push_back(in.z);
        json.push_back(in.w);
    }

    inline void Serialization::Serialize(nlohmann::json &json, const glm::mat3 &in)
    {
        const float *mat = (const float *)glm::value_ptr(in);
        for (int i = 0; i < 9; ++i)
            json.push_back(mat[i]);
    }

    inline void Serialization::Serialize(nlohmann::json &json, const glm::mat4 &in)
    {
        const float *mat = (const float *)glm::value_ptr(in);
        for (int i = 0; i < 16; ++i)
            json.push_back(mat[i]);
    }

    inline void Serialization::Deserialize(const nlohmann::json &json, glm::vec2 &out)
    {
        out.x = json.at(0).get<float>();
        out.y = json.at(1).get<float>();
    }

    inline void Serialization::Deserialize(const nlohmann::json &json, glm::vec3 &out)
    {
        out.x = json.at(0).get<float>();
        out.y = json.at(1).get<float>();
        out.z = json.at(2).get<float>();
    }

    inline void Serialization::Deserialize(const nlohmann::json &json, glm::vec4 &out)
    {
        out.x = json.at(0).get<float>();
        out.y = json.at(1).get<float>();
        out.z = json.at(2).get<float>();
        out.w = json.at(3).get<float>();
    }

    inline void Serialization::Deserialize(const nlohmann::json &json, glm::mat3 &out)
    {
        float *mat = (float *)glm::value_ptr(out);
        for (int i = 0; i < 9; ++i)
            mat[i] = json.at(i).get<float>();
    }

    inline void Serialization::Deserialize(const nlohmann::json &json, glm::mat4 &out)
    {
        float *mat = (float *)glm::value_ptr(out);
        for (int i = 0; i < 16; ++i)
            mat[i] = json.at(i).get<float>();
    }
}