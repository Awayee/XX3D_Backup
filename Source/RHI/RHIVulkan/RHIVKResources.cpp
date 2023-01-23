#include "RHIVKResources.h"
#include "VulkanUtil.h"
namespace RHI {
    void RRenderPassVk::SetAttachment(uint32 idx, RImageView* imageView)
    {
        if(!(idx < m_Attachments.size())) {
	        for(int i= m_Attachments.size()-1; i<idx; ++i) {
                m_Attachments.push_back(VK_NULL_HANDLE);
	        }
        }
        m_Attachments[idx] = ((RImageViewVk*)imageView)->handle;
    }
    void RRenderPassVk::SetClearValue(uint32 idx, const RSClear& clear)
    {
        if(!(idx < m_Clears.size())) {
	        for(int i=m_Clears.size()-1;i<idx; ++i) {
                m_Clears.emplace_back();
	        }
        }
        m_Clears[idx] = ResolveClearValue(clear);
    }
}