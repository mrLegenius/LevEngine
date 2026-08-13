#pragma once

#include "Kernel/Core.h"

namespace LevEngine
{
	class Layer;

	class LEV_API LayerStack
{
public:
	LayerStack();
	~LayerStack();

	void PushLayer(Layer* layer);
	void PushOverlay(Layer* overlay);
	void PopLayer(const Layer* layer);
	void PopOverlay(const Layer* overlay);

	//Detaches and deletes every layer. Called explicitly on shutdown, so the layers release
	//their GPU resources while the render device is still alive
	void Clear();

	Vector<Layer*>::iterator begin() { return m_Layers.begin(); }
	Vector<Layer*>::iterator end() { return m_Layers.end(); }

private:
	Vector<Layer*> m_Layers;
	unsigned int m_LayerInsertIndex = 0;
};
}
