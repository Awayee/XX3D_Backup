#include "RenderScene.h"
#include "Resource/Config/Config.h"
#include "Core/Concurrency/Concurrency.h"

#include "../Camera/Camera.h"
#include "../Light/DirectionalLight.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_LEFT_HANDED
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Engine {
    RenderObject::RenderObject(RenderScene* scene)
    {
        if (nullptr == scene)scene = RenderScene::GetDefaultScene();
        scene->AddRenderObject(this);
    }
    RenderObject::~RenderObject()
    {
        m_Scene->RemoveRenderObject(this);
        m_Scene = nullptr;
        m_Index = 0u;
    }

    TUniquePtr<RenderScene> RenderScene::s_Default;

    struct SceneUBO {
        Math::FVector3 LightDir; float _padding;
        Math::FVector3 LightColor; float _padding1;
    };

    struct CameraUBO {
        Math::FMatrix4x4 View;
        Math::FMatrix4x4 Proj;
        Math::FMatrix4x4 VP;
        //glm::mat4 View;
        //glm::mat4 Proj;
        //glm::mat4 VP;
    };
    void RenderScene::UpdateUniform()
    {
        SceneUBO sceneUbo;
        sceneUbo.LightDir = m_DirectionalLight->GetLightDir();
        sceneUbo.LightColor = m_DirectionalLight->GetLightColor();
        m_LightUniform.UpdateData(&sceneUbo);
        CameraUBO cameraUbo;
        cameraUbo.View = m_Camera->GetViewMatrix();
        cameraUbo.Proj = m_Camera->GetProjectMatrix();
        cameraUbo.VP = m_Camera->GetViewProjectMatrix();
        //cameraUbo.View = glm::lookAt(glm::vec3{ 0, 2, -2 }, { 0, 2, 0 }, { 0, 1, 0 });
        //cameraUbo.Proj = glm::perspective(1.5f, 1.333f, 0.1f, 1000.0f);
        //cameraUbo.VP = cameraUbo.Proj * cameraUbo.View;
        m_CameraUniform.UpdateData(&cameraUbo);
    }

    void RenderScene::CreateResources()
    {
        m_DirectionalLight.reset(new DirectionalLight);
        m_DirectionalLight->SetDir({-1, -1, -1});
        auto& ext = RHI_INSTANCE->GetSwapchainExtent();
        m_Camera.reset(new Camera(CAMERA_PERSPECTIVE, (float)ext.width / ext.height, 0.1f, 1000.0f, Math::PI * 0.49f));
        //m_Camera.reset(new Camera(CAMERA_ORTHO, (float)ext.width / ext.height, 0.1f, 1000.0f, 0.5f * ext.height));
        m_Camera->SetView({ 0, 4, -4 }, { 0, 2, 0}, { 0, 1, 0 });
    }

    void RenderScene::CreateDescriptorSets()
    {
        GET_RHI(rhi);

        m_SceneDescs = rhi->AllocateDescriptorSet(DescsMgr::Get(DESCS_SCENE));

        uint32 bufferSize = sizeof(SceneUBO);
        m_LightUniform.CreateForUniform(bufferSize, nullptr);
        m_SceneDescs->UpdateUniformBuffer(0, m_LightUniform.Buffer);

        bufferSize = sizeof(CameraUBO);
        m_CameraUniform.CreateForUniform(bufferSize, nullptr);
        m_SceneDescs->UpdateUniformBuffer(1, m_CameraUniform.Buffer);
    }

    RenderScene* RenderScene::GetDefaultScene()
    {
        if(nullptr == s_Default.get()) {
            static Mutex m;
            MutexLock lock(m);
            if (nullptr == s_Default.get()) {
                s_Default.reset(new RenderScene);
            }
        }
        return s_Default.get();
    }

    RenderScene::RenderScene()
    {
        CreateResources();
        CreateDescriptorSets();
    }

    RenderScene::~RenderScene() {
        m_LightUniform.Release();
        m_CameraUniform.Release();
        //RHI_INSTANCE->FreeDescriptorSet(m_SceneDescs);
    }

    void RenderScene::AddRenderObject(RenderObject* obj)
    {
        m_RenderObjects.push_back(obj);
        obj->m_Scene = this;
        obj->m_Index = m_RenderObjects.size();
    }

    void RenderScene::RemoveRenderObject(RenderObject* obj)
    {
        if (obj->m_Scene != this) return;
        if (0 == obj->m_Index || m_RenderObjects.size() < obj->m_Index)return;

        Swap(m_RenderObjects[obj->m_Index-1], m_RenderObjects.back());
        m_RenderObjects.pop_back();
        if(!m_RenderObjects.empty()) {
            m_RenderObjects[obj->m_Index-1]->m_Index = obj->m_Index;
        }
    }

    void RenderScene::Update()
    {
        UpdateUniform();
    }

    void RenderScene::RenderGBuffer(RHI::RCommandBuffer* cmd, RHI::RPipelineLayout* layout)
    {
        cmd->BindDescriptorSet(layout, m_SceneDescs, 0, RHI::PIPELINE_GRAPHICS);
        for(RenderObject* obj: m_RenderObjects) {
            obj->DrawCall(cmd, layout);
        }
    }
}