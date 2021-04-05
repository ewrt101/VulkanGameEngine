#include "eve_app_base.hpp"

// std
#include <array>
#include <stdexcept>

namespace eve {

    EveAppBase::EveAppBase() {
        loadModels();
        createPipelineLayout();
        createPipeline();
        createCommandBuffers();
    }

    EveAppBase::~EveAppBase() { vkDestroyPipelineLayout(eveDevice.device(), pipelineLayout, nullptr); }

    void EveAppBase::run() {
        while (!eveWindow.shouldClose()) {
            glfwPollEvents();
            drawFrame();
        }

        vkDeviceWaitIdle(eveDevice.device());
    }

    void EveAppBase::sierpinski(
        std::vector<EveModel::Vertex>& vertices,
        int depth,
        glm::vec2 left,
        glm::vec2 right,
        glm::vec2 top) {
        if (depth <= 0) {
            vertices.push_back({ top });
            vertices.push_back({ right });
            vertices.push_back({ left });
        }
        else {
            auto leftTop = 0.5f * (left + top);
            auto rightTop = 0.5f * (right + top);
            auto leftRight = 0.5f * (left + right);
            sierpinski(vertices, depth - 1, left, leftRight, leftTop);
            sierpinski(vertices, depth - 1, leftRight, right, rightTop);
            sierpinski(vertices, depth - 1, leftTop, rightTop, top);
        }
    }


    void EveAppBase::loadModels() {
        std::vector<EveModel::Vertex> vertices{};
        sierpinski(vertices, 7, { -0.5f, 0.5f }, { 0.5f, 0.5f }, { 0.0f, -0.5f });
        eveModel = std::make_unique<EveModel>(eveDevice, vertices);
    }

    void EveAppBase::createPipelineLayout() {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;
        if (vkCreatePipelineLayout(eveDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }

    void EveAppBase::createPipeline() {
        PipelineConfigInfo pipelineConfig{};
        EvePipeline::defaultPipelineConfigInfo(
            pipelineConfig,
            eveSwapChain.width(),
            eveSwapChain.height());
        pipelineConfig.renderPass = eveSwapChain.getRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;
        evePipeline = std::make_unique<EvePipeline>(
            eveDevice,
            "shaders/simple_shader.vert.spv",
            "shaders/simple_shader.frag.spv",
            pipelineConfig);
    }

    void EveAppBase::createCommandBuffers() {
        commandBuffers.resize(eveSwapChain.imageCount());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = eveDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if (vkAllocateCommandBuffers(eveDevice.device(), &allocInfo, commandBuffers.data()) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }

        for (int i = 0; i < commandBuffers.size(); i++) {
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

            if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
                throw std::runtime_error("failed to begin recording command buffer!");
            }

            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = eveSwapChain.getRenderPass();
            renderPassInfo.framebuffer = eveSwapChain.getFrameBuffer(i);

            renderPassInfo.renderArea.offset = { 0, 0 };
            renderPassInfo.renderArea.extent = eveSwapChain.getSwapChainExtent();

            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
            clearValues[1].depthStencil = { 1.0f, 0 };
            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();

            vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            evePipeline->bind(commandBuffers[i]);
            eveModel->bind(commandBuffers[i]);
            eveModel->draw(commandBuffers[i]);

            vkCmdEndRenderPass(commandBuffers[i]);
            if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to record command buffer!");
            }
        }
    }
    void EveAppBase::drawFrame() {
        uint32_t imageIndex;
        auto result = eveSwapChain.acquireNextImage(&imageIndex);
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        result = eveSwapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }
    }

}  // namespace lve