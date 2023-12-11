#pragma once
#include "EntitySelection.h"

namespace LevEngine::Editor
{
    class IComponentDebugRenderer
    {
    public:
        virtual ~IComponentDebugRenderer() = default;
        virtual void Draw(Entity entity) = 0;
    };

    template <typename TComponent, class TDebugRenderer>
    class ComponentDebugRenderer : public IComponentDebugRenderer
    {
    public:
        ~ComponentDebugRenderer() override = default;

        void Draw(const Entity entity) override
        {
            if (!entity.HasComponent<TComponent>()) return;

            if (IsRenderedOnlyWhenSelected())
            {
                const auto selectedEntity = Selection::CurrentAs<EntitySelection>();
                if (!selectedEntity || selectedEntity->Get() != entity) return;
            }
            
            auto& component = entity.GetComponent<TComponent>();
            DrawContent(component, entity);
        }

    protected:
        virtual void DrawContent(TComponent& component, Entity entity) = 0;
        virtual bool IsRenderedOnlyWhenSelected() { return false; }

    private:
        static inline ClassRegister<IComponentDebugRenderer, TDebugRenderer> s_ClassRegister;
    };
}
