#pragma once

#include "core/layer.h"

namespace engine
{
	class LayerStack
	{
	public:
		void PushLayer(Layer *layer);
		void PopLayer(Layer *layer);

		void BringLayerToFront(Layer *layer);
		void SendLayerToBack(Layer *layer);

		void BringLayerForward(Layer *layer, unsigned int count = 1);
		void SendLayerBackward(Layer *layer, unsigned int count = 1);

		std::vector<Layer *> &GetLayers() { return m_Layers; }

	private:
		std::vector<Layer *> m_Layers;
	};
}