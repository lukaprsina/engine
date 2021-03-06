#include "vulkan_api/render_target.h"

#include "vulkan_api/device.h"

#include <set>

namespace engine
{
    namespace
    {
        auto CompareExtend2D = [](const VkExtent2D &lhs, const VkExtent2D &rhs)
        {
            return !(lhs.width == rhs.width && lhs.height == rhs.height) &&
                   (lhs.width < rhs.width && lhs.height < rhs.height);
        };
    }

    Attachment::Attachment(VkFormat format, VkSampleCountFlagBits samples,
                           VkImageUsageFlags usage)
        : format(format), samples(samples), usage(usage)
    {
    }

    const RenderTarget::CreateFunc RenderTarget::s_DefaultCreateFunction = [](core::Image &&swapchain_image) -> std::unique_ptr<RenderTarget>
    {
        VkFormat depth_format = GetSuitableDepthFormat(swapchain_image.GetDevice().GetGPU().GetHandle());

        core::Image depth_image(swapchain_image.GetDevice(), swapchain_image.GetExtent(),
                                depth_format,
                                VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT,
                                VMA_MEMORY_USAGE_GPU_ONLY);

        std::vector<core::Image> images;
        images.push_back(std::move(swapchain_image));
        images.push_back(std::move(depth_image));

        return std::make_unique<RenderTarget>(std::move(images));
    };

    RenderTarget::RenderTarget(std::vector<core::Image> &&images)
        : m_Device(images.back().GetDevice()),
          m_Images(std::move(images))
    {
        ENG_ASSERT(!m_Images.empty(), "Render target should have at least one image");

        auto CompareExtend2D = [](const VkExtent2D &lhs, const VkExtent2D &rhs)
        {
            return !(lhs.width == rhs.width && lhs.height == rhs.height) &&
                   (lhs.width < rhs.width && lhs.height < rhs.height);
        };
        std::set<VkExtent2D, decltype(CompareExtend2D)> unique_extent(CompareExtend2D);

        auto get_image_extent = [](const core::Image &image)
        {
            return VkExtent2D{image.GetExtent().width, image.GetExtent().height};
        };
        std::transform(m_Images.begin(), m_Images.end(), std::inserter(unique_extent, unique_extent.end()), get_image_extent);

        if (unique_extent.size() != 1)
        {
            throw VulkanException{VK_ERROR_INITIALIZATION_FAILED, "Extent size is not unique"};
        }

        m_Extent = *unique_extent.begin();

        for (auto &image : m_Images)
        {
            if (image.GetType() != VK_IMAGE_TYPE_2D)
            {
                throw VulkanException{VK_ERROR_INITIALIZATION_FAILED, "Image type is not 2D"};
            }

            m_ImageViews.emplace_back(image, VK_IMAGE_VIEW_TYPE_2D);

            m_Attachments.emplace_back(Attachment(image.GetFormat(),
                                                  image.GetSampleCount(),
                                                  image.GetUsage()));
        }
    }

    RenderTarget::~RenderTarget()
    {
    }
}
