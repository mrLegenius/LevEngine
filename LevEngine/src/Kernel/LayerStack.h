#pragma once
#include <vector>
#include "Layer.h"
#include "DataTypes/Vector.h"

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

	Vector<Layer*>::iterator begin() { return m_Layers.begin(); }
	Vector<Layer*>::iterator end() { return m_Layers.end(); }

private:
	Vector<Layer*> m_Layers;
	unsigned int m_LayerInsertIndex = 0;
};
}
