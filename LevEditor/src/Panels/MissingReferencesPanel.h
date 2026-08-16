#pragma once
#include "Panel.h"

#include "Assets/MissingReferences.h"

namespace LevEngine::Editor
{
    // Lists every asset reference that points at nothing.
    //
    // A deleted asset leaves its UUID behind in whatever pointed at it, and the only sign of that
    // used to be a warning naming the UUID and nothing else. Here the same reference comes with the
    // file it lives in and the place inside that file, and a scan finds the ones in assets the
    // editor has not loaded yet.
    class MissingReferencesPanel final : public Panel
    {
    public:
        MissingReferencesPanel();

    protected:
        String GetName() override;
        void DrawContent() override;

    private:
        void DrawToolbar();
        void DrawTable();
        void DrawDanglingAddresses();

        [[nodiscard]] bool Matches(const MissingReference& reference) const;

        //<--- Selects the asset holding the reference in the inspector, if it is still there ---<<
        static void SelectSource(const String& source);

        static void RemoveAddress(const String& address);

        Vector<MissingReference> m_References;
        Vector<Pair<String, UUID>> m_DanglingAddresses;

        String m_Filter;
        char m_FilterBuffer[128]{};

        bool m_IsRefreshRequested = true;

        //<--- What the registry looked like when the lists above were read ---<<
        uint32_t m_Version = 0;
    };
}
