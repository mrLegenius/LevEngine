#pragma once

namespace LevEngine::Editor
{
class ModalPopup
{
public:
    static void Render() { return Get().RenderPopup(); }

    static void Show(
        String title,
        String description,
        String buttonText,
        const std::function<void()>& buttonAction);

    static void Show(
        String title,
        String description,
        String leftButtonText,
        String rightButtonText,
        const std::function<void()>& leftButtonAction = nullptr,
        const std::function<void()>& rightButtonAction = nullptr);

private:

    void RenderPopup() const;

    static ModalPopup& Get()
    {
        static ModalPopup popup;
        return popup;
    }

    inline static bool m_PopupRequested;
    
    String m_Title;
    String m_Description;
    String m_LeftButtonText;
    String m_RightButtonText;

    std::function<void()> m_LeftButtonAction;
    std::function<void()> m_RightButtonAction;
};
}