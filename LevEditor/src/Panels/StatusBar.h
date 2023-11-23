#pragma once

namespace LevEngine::Editor
{
class StatusBar
{
public:
    StatusBar();
    void Render() const;

private:
    float m_Height;
};
}