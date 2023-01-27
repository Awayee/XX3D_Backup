#include "RenderScene.h"
#include "Resource/Config/Config.h"
#include "RenderSystem.h"
#include "Core/Concurrency/Concurrency.h"

namespace Engine {
    RenderObject::RenderObject(RenderScene* scene) : Scene(scene)
    {
        if (nullptr == Scene)Scene = RenderScene::GetDefaultScene();
        Scene->AddRenderObject(this);
    }
    RenderObject::~RenderObject()
    {
        Scene->RemoveRenderObject(this);
    }

    TUniquePtr<RenderScene> RenderScene::s_Default;

    struct SceneUBO {
        Math::FVector4 LightDir;
        Math::FVector4 LightColor;
    };

    struct CameraUBO {
        Math::FMatrix4x4 View;
        Math::FMatrix4x4 Proj;
        Math::FMatrix4x4 VP;
    };
    void RenderScene::UpdateUniform()
    {
        void* pData;
        m_SceneUniform.Map(&pData);
        SceneUBO sceneUbo;
        sceneUbo.LightDir = { m_DirectionalLight->GetLightDir(), 0.0f };
        sceneUbo.LightColor = { m_DirectionalLight->GetLightColor(), 0.0f };
        memcpy(pData, &sceneUbo, sizeof(sceneUbo));
        m_SceneUniform.Unmap();

        void* pData0;
        m_CameraUniform.Map(&pData0);
        CameraUBO cameraUbo;
        cameraUbo.View = m_Camera->GetViewMatrix();
        cameraUbo.Proj = m_Camera->GetProjectMatrix();
        cameraUbo.VP = m_Camera->GetViewProjectMatrix();
        memcpy(pData0, &cameraUbo, sizeof(cameraUbo));
        m_CameraUniform.Unmap();
    }

    void RenderScene::CreateResources()
    {
        m_DirectionalLight.reset(new DirectionalLight);
        m_DirectionalLight->SetDir({-1, -1, -1});
        auto& ext = RHI_INSTANCE->GetSwapchainExtent();
        m_Camera.reset(new Camera(PROJECT_PERSPECTIVE, (float)ext.width / ext.height, 1.0f, 1000.0f, 0.78f));
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

        GET_RHI(rhi);

        m_SceneDescs = rhi->AllocateDescriptorSet(DescsMgr::GetLayout(DESCS_SCENE));
        uint32 bufferSize = sizeof(SceneUBO);
        m_SceneUniform.Create(bufferSize, RHI::BUFFER_USAGE_UNIFORM_BUFFER_BIT, RHI::MEMORY_PROPERTY_DEVICE_LOCAL_BIT, nullptr);
        RHI::RDescriptorInfo lightInfo;
        lightInfo.buffer = m_SceneUniform.Buffer;
        lightInfo.offset = 0;
        lightInfo.range = sizeof(SceneUBO);
        rhi->UpdateDescriptorSet(m_SceneDescs, 0, 0, 1, RHI::DESCRIPTOR_TYPE_UNIFORM_BUFFER, lightInfo);

        bufferSize = sizeof(CameraUBO);
        m_CameraUniform.Create(bufferSize, RHI::BUFFER_USAGE_UNIFORM_BUFFER_BIT, RHI::MEMORY_PROPERTY_DEVICE_LOCAL_BIT, nullptr);
        RHI::RDescriptorInfo cameraInfo;
        cameraInfo.buffer = m_CameraUniform.Buffer;
        cameraInfo.offset = 0;
        cameraInfo.range = sizeof(CameraUBO);
        rhi->UpdateDescriptorSet(m_SceneDescs, 1, 0, 1, RHI::DESCRIPTOR_TYPE_UNIFORM_BUFFER, cameraInfo);
    }

    RenderScene::~RenderScene() {
        m_SceneUniform.Release();
        RHI_INSTANCE->FreeDescriptorSet(m_SceneDescs);
    }

    void RenderScene::AddRenderObject(RenderObject* obj)
    {
        obj->Index = static_cast<int32>(m_RenderObjects.size());
        m_RenderObjects.push_back(obj);
    }

    void RenderScene::RemoveRenderObject(RenderObject* obj)
    {
        if(obj->Index < 0 || obj->Index >= m_RenderObjects.size()) {
            return;
        }
        Swap(m_RenderObjects[obj->Index], m_RenderObjects.back());
        m_RenderObjects.pop_back();
        m_RenderObjects[obj->Index]->Index = obj->Index;
    }

    void RenderScene::RenderGBuffer(RHI::RCommandBuffer* cmd, RHI::RPipelineLayout* layout)
    {
        RHI_INSTANCE->CmdBindDescriptorSet(cmd, RHI::PIPELINE_GRAPHICS, layout, m_SceneDescs, 0);

        for(RenderObject* obj: m_RenderObjects) {
            obj->DrawCall(cmd, layout);
        }
    }
    void RenderScene::RenderLight(RHI::RCommandBuffer* cmd)
    {
    }
}