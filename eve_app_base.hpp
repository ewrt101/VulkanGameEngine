#pragma once

#include "eve_window.hpp"
#include "eve_pipeline.hpp"
#include "eve_device.hpp"
#include "eve_swap_chain.hpp"

// std
#include <memory>
#include <vector>

namespace eve {

class EveAppBase {
 public:
  static constexpr int WIDTH = 800;
  static constexpr int HEIGHT = 600;

  EveAppBase();
  ~EveAppBase();

  EveAppBase(const EveAppBase&) = delete;
  EveAppBase& operator=(const EveAppBase&) = delete;

  void run();

 private:
	 void createPipelineLayout();
	 void createPipeline();
	 void createCommandBuffers();
	 void drawFrame();

	EveWindow eveWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
	EveDevice eveDevice{ eveWindow };
	EveSwapChain eveSwapChain{ eveDevice, eveWindow.getExtent() };
	std::unique_ptr<EvePipeline> evePipeline;
	VkPipelineLayout pipelineLayout;
	std::vector<VkCommandBuffer> commandBuffers;
};
}  // namespace eve
