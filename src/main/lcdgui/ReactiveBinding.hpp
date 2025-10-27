#pragma once

#include <functional>
#include <memory>
#include <utility>
#include <type_traits>

namespace mpc::lcdgui {

    template <typename T>
    using PropertyGetter = std::function<T()>;

    template <typename T>
    using GuiUpdater = std::function<void(const T&)>;

    struct ReactiveBinding
    {
        struct Concept {
            virtual ~Concept() = default;
            virtual void refresh() = 0;
        };

        template <typename T>
        struct Model final : Concept {
            PropertyGetter<T> get;
            GuiUpdater<T> update;
            T lastValue{};
            void refresh() override {
                const T current = get();
                if (current != lastValue) {
                    lastValue = current;
                    update(current);
                }
            }
        };

        std::unique_ptr<Concept> self;

        template <typename Getter, typename Updater>
        ReactiveBinding(Getter getter, Updater updater)
        {
            using T = std::invoke_result_t<Getter>;
            auto model = std::make_unique<Model<T>>();
            model->get = getter;
            model->update = updater;
            self = std::move(model);
        }

        void refresh() { if (self) self->refresh(); }
    };

} // namespace mpc::lcdgui

