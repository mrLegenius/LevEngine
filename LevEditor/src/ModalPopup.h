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
        const Action<>& buttonAction);

    static void Show(
        String title,
        String description,
        String leftButtonText,
        String rightButtonText,
        const Action<>& leftButtonAction = nullptr,
        const Action<>& rightButtonAction = nullptr);

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

    Action<> m_LeftButtonAction;
    Action<> m_RightButtonAction;
};
}