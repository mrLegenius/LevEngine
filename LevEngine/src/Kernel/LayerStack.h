#pragma once
#include <vector>
#include "Layer.h"
namespace LevEngine
{
class LayerStack
{
public:
	LayerStack();
	~LayerStack();

	void PushLayer(Layer* layer);
	void PushOverlay(Layer* overlay);
	void PopLayer(const Layer* layer);
	void PopOverlay(const Layer* overlay);

	std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
	std::vector<Layer*>::iterator end() { return m_Layers.end(); }

private:
	std::vector<Layer*> m_Layers;
	unsigned int m_LayerInsertIndex = 0;
};
}