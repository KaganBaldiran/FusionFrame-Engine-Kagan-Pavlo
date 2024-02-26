#pragma once
#include <glew.h>
#include <glfw3.h>
#include "../FusionUtility/Log.h"
#include "../FusionUtility/VectorMath.h"
#include "Buffer.h"
#include "Camera.h"
#include "Texture.h"
#include <functional>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include "Mesh.h"
#include <functional>
#include <map>
#include "Object.hpp"
#include "Cubemap.h"

namespace FUSIONOPENGL
{
    //forward declaration of the omnishadow map class
    class OmniShadowMap;

    class PreMeshData
    {
    public:
        std::vector<FUSIONOPENGL::Texture2D> textures;
        std::vector<unsigned int> indices;
        std::vector<FUSIONOPENGL::Vertex> vertices;

        PreMeshData(std::vector<FUSIONOPENGL::Texture2D> textures, std::vector<unsigned int> indices, std::vector<FUSIONOPENGL::Vertex> vertices)
        {
            this->textures = textures;
            this->indices = indices;
            this->vertices = vertices;
        }

    };

    class Model : public Object
    {
    public:
        Model(std::string const& filePath, bool Async = false, bool AnimationModel = false)
        {
            this->AnimationEnabled = AnimationModel;
            FinalAnimationMatrices = nullptr;
            this->loadModel(filePath,Async,AnimationModel);
            FindGlobalMeshScales();
            static unsigned int counter = 0;
            this->ModelID = counter;
            counter++;
        }

        inline unsigned int GetModelID() { return this->ModelID; };
        void Draw(Camera3D& camera, Shader& shader, std::function<void()>& ShaderPreperations);
        void Draw(Camera3D& camera, Shader& shader, Material material, std::function<void()>& ShaderPreperations);
        void Draw(Camera3D& camera, Shader& shader, std::function<void()>& ShaderPreperations, CubeMap& cubemap,Material material,float EnvironmentAmbientAmount = 0.2f);
        void Draw(Camera3D& camera, Shader& shader, std::function<void()>& ShaderPreperations, CubeMap& cubemap, Material material, std::vector<OmniShadowMap*> ShadowMaps, float EnvironmentAmbientAmount = 0.2f);
        void Draw(Camera3D& camera, Shader& shader, std::function<void()>& ShaderPreperations, CubeMap& cubemap, Material material, std::vector<OmniShadowMap*> ShadowMaps , std::vector<glm::mat4>& AnimationBoneMatrices, float EnvironmentAmbientAmount = 0.2f);
        void DrawDeferred(Camera3D& camera, Shader& shader, std::function<void()>& ShaderPreperations, CubeMap& cubemap, Material material, std::vector<OmniShadowMap*> ShadowMaps, std::vector<glm::mat4>& AnimationBoneMatrices, float EnvironmentAmbientAmount = 0.2f);

        void Draw(Camera3D& camera, Shader& shader, std::vector<Material> materials, std::function<void()>& ShaderPreperations,CubeMap& cubemap, float EnvironmentAmbientAmount = 0.2f);
        void Draw(Camera3D& camera, Shader& shader, std::vector<Material> materials, std::function<void()>& ShaderPreperations, CubeMap& cubemap , std::vector<OmniShadowMap*> ShadowMaps, float EnvironmentAmbientAmount = 0.2f);
        void Draw(Camera3D& camera, Shader& shader, std::vector<Material> materials, std::function<void()>& ShaderPreperations, CubeMap& cubemap, std::vector<OmniShadowMap*> ShadowMaps , std::vector<glm::mat4> &AnimationBoneMatrices, float EnvironmentAmbientAmount = 0.2f);
        void DrawImportedMaterial(Camera3D& camera, Shader& shader, std::function<void()>& ShaderPreperations, CubeMap& cubemap, float EnvironmentAmbientAmount = 0.2f);

        void FindGlobalMeshScales();
        
        std::vector<Mesh3D> Meshes;
        std::vector<Texture2D> textures_loaded;
        std::vector<PreMeshData> PreMeshDatas;

        ~Model()
        {
            for (size_t i = 0; i < this->Meshes.size(); i++)
            {
                Meshes[i].Clean();
                LOG_INF("Model" << this->ModelID << " buffers cleaned!");
            }
        }


        inline void SetVertexBoneDataDefault(Vertex& vertex)
        {
            for (size_t i = 0; i < MAX_BONE_INFLUENCE; i++)
            {
                vertex.m_BoneIDs[i] = -1;
                vertex.m_Weights[i] = 0.0f;
            }
        }
        
        void loadModel(std::string const& path , bool Async = false, bool AnimationModel = false);
        void processNode(aiNode* node, const aiScene* scene);
        Mesh3D processMesh(aiMesh* mesh, const aiScene* scene);
        void ExtractBones(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);
        void loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, std::vector<FUSIONOPENGL::Texture2D>& Destination);
        
        PreMeshData processMeshAsync(aiMesh* mesh, const aiScene* scene);
        void processNodeAsync(aiNode* node, const aiScene* scene);
        void ConstructMeshes(std::vector<PreMeshData> PreMeshDatas);
        inline std::vector<glm::mat4>*& GetAnimationMatricesPointer() { return this->FinalAnimationMatrices; };

        inline std::map<std::string, BoneInfo>& GetBones() { return Bones; };
        inline int& GetBoneCounter() { return this->boneCounter; };
        inline bool IsAnimationEnabled() { return this->AnimationEnabled; };

        static inline glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from)
        {
            glm::mat4 to;
            
            to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
            to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
            to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
            to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
            return to;
        }

    private:

            std::string path;
            std::vector<Texture2D> Textures;
            unsigned int ModelID;
            std::string directory;
            glm::vec3 originpoint;
            glm::vec3 dynamic_origin;
            bool AnimationEnabled;
            std::vector<glm::mat4>* FinalAnimationMatrices;

            std::map<std::string, BoneInfo> Bones;
            int boneCounter = 0;

            aiScene* scene;
    };

}


