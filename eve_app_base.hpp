#pragma once

#include "eve_window.hpp"
#include "eve_pipeline.hpp"
#include "eve_device.hpp"
#include "eve_swap_chain.hpp"
#include "eve_model.hpp"

// std
#include <memory>
#include <vector>

namespace eve {

class EveAppBase {
 public:
  static constexpr int WIDTH = 1920;
  static constexpr int HEIGHT = 1080;

  EveAppBase();
  ~EveAppBase();

  EveAppBase(const EveAppBase&) = delete;
  EveAppBase& operator=(const EveAppBase&) = delete;

  void run();

 private:
	 void sierpinski(std::vector<EveModel::Vertex>& vertices,
		 int depth,
		 glm::vec2 left,
		 glm::vec2 right,
		 glm::vec2 top);
	 void loadModels();
	 void createPipelineLayout();
	 void createPipeline();
	 void createCommandBuffers();
	 void drawFrame();

	EveWindow eveWindow{WIDTH, HEIGHT, "Ewrts Vulkan thing"};
	EveDevice eveDevice{ eveWindow };
	EveSwapChain eveSwapChain{ eveDevice, eveWindow.getExtent() };
	std::unique_ptr<EvePipeline> evePipeline;
	VkPipelineLayout pipelineLayout;
	std::vector<VkCommandBuffer> commandBuffers;
	std::unique_ptr<EveModel> eveModel;
};
}  // namespace eve
