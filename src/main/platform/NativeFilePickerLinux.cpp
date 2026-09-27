#if defined(__linux__) && !defined(__ANDROID__)
#include "NativeFilePicker.hpp"
#include <gio/gio.h>
#include <chrono>
#include <thread>
using namespace mpc::platform;
namespace
{
    struct PortalResult
    {
        bool ready = false;
        FileSelection selection;
    };
    void response(GDBusConnection *, const gchar *, const gchar *,
                  const gchar *, const gchar *, GVariant *parameters,
                  gpointer data)
    {
        auto &result = *static_cast<PortalResult *>(data);
        guint32 status;
        GVariant *values;
        g_variant_get(parameters, "(u@a{sv})", &status, &values);
        if (status == 0)
        {
            auto *uris =
                g_variant_lookup_value(values, "uris", G_VARIANT_TYPE("as"));
            if (uris && g_variant_n_children(uris))
            {
                auto *first = g_variant_get_child_value(uris, 0);
                const auto *uri = g_variant_get_string(first, nullptr);
                GError *error = nullptr;
                gchar *path = g_filename_from_uri(uri, nullptr, &error);
                if (path)
                {
                    result.selection.image = selectedImage(path);
                    g_free(path);
                }
                else
                {
                    result.selection.error =
                        "Select an image accessible as a local file";
                }
                g_clear_error(&error);
                g_variant_unref(first);
            }
            else
            {
                result.selection.error = "File picker returned no image";
            }
            if (uris)
            {
                g_variant_unref(uris);
            }
        }
        else if (status != 1)
        {
            result.selection.error = "Native file picker failed";
        }
        g_variant_unref(values);
        result.ready = true;
    }
} // namespace
void mpc::platform::showNativeFilePicker(std::shared_ptr<PickerRequest> request,
                                         void *, const std::string &parent)
{
    request->worker = std::thread(
        [request, parent]
        {
            auto *context = g_main_context_new();
            g_main_context_push_thread_default(context);
            auto *cancel = g_cancellable_new();
            auto keepCancel =
                std::shared_ptr<GCancellable>(cancel,
                                              [](GCancellable *p)
                                              {
                                                  g_object_unref(p);
                                              });
            request->setDismiss(
                [keepCancel]
                {
                    g_cancellable_cancel(keepCancel.get());
                });
            GError *error = nullptr;
            auto *bus = g_bus_get_sync(G_BUS_TYPE_SESSION, cancel, &error);
            PortalResult result;
            guint subscription = 0;
            std::string handle;
            if (bus)
            {
                // Subscribe before sending the request so an immediate response
                // cannot be lost. A unique token routes only this instance's
                // result.
                std::string token = selectedImage("unused").volumeUUID;
                for (auto &c : token)
                {
                    if (c == '-')
                    {
                        c = '_';
                    }
                }
                std::string sender = g_dbus_connection_get_unique_name(bus);
                if (!sender.empty() && sender[0] == ':')
                {
                    sender.erase(0, 1);
                }
                for (auto &c : sender)
                {
                    if (c == '.')
                    {
                        c = '_';
                    }
                }
                handle = "/org/freedesktop/portal/desktop/request/" + sender +
                         "/" + token;
                subscription = g_dbus_connection_signal_subscribe(
                    bus, "org.freedesktop.portal.Desktop",
                    "org.freedesktop.portal.Request", "Response",
                    handle.c_str(), nullptr, G_DBUS_SIGNAL_FLAGS_NONE, response,
                    &result, nullptr);
                GVariantBuilder options;
                g_variant_builder_init(&options, G_VARIANT_TYPE_VARDICT);
                g_variant_builder_add(&options, "{sv}", "handle_token",
                                      g_variant_new_string(token.c_str()));
                g_variant_builder_add(&options, "{sv}", "multiple",
                                      g_variant_new_boolean(FALSE));
                g_variant_builder_add(&options, "{sv}", "modal",
                                      g_variant_new_boolean(!parent.empty()));
                auto *reply = g_dbus_connection_call_sync(
                    bus, "org.freedesktop.portal.Desktop",
                    "/org/freedesktop/portal/desktop",
                    "org.freedesktop.portal.FileChooser", "OpenFile",
                    g_variant_new("(ss@a{sv})", parent.c_str(),
                                  "Select a FAT disk image",
                                  g_variant_builder_end(&options)),
                    G_VARIANT_TYPE("(o)"), G_DBUS_CALL_FLAGS_NONE, 10000,
                    cancel, &error);
                if (reply)
                {
                    const gchar *returnedHandle;
                    g_variant_get(reply, "(&o)", &returnedHandle);
                    if (handle != returnedHandle)
                    {
                        g_dbus_connection_signal_unsubscribe(bus, subscription);
                        handle = returnedHandle;
                        subscription = g_dbus_connection_signal_subscribe(
                            bus, "org.freedesktop.portal.Desktop",
                            "org.freedesktop.portal.Request", "Response",
                            handle.c_str(), nullptr, G_DBUS_SIGNAL_FLAGS_NONE,
                            response, &result, nullptr);
                    }
                    g_variant_unref(reply);
                    while (!result.ready && !request->cancelled &&
                           !g_dbus_connection_is_closed(bus))
                    {
                        while (g_main_context_iteration(context, FALSE))
                        {
                        }
                        if (!result.ready)
                        {
                            std::this_thread::sleep_for(
                                std::chrono::milliseconds(20));
                        }
                    }
                    if (!request->cancelled && !result.ready)
                    {
                        result.selection.error =
                            "File picker connection closed";
                    }
                }
                else
                {
                    result.selection.error =
                        "Native file picker unavailable; install an XDG "
                        "FileChooser portal backend";
                }
                if (request->cancelled)
                {
                    // OpenFile may have reached the portal even if cancellation
                    // prevented its reply from reaching us.
                    g_dbus_connection_call(
                        bus, "org.freedesktop.portal.Desktop", handle.c_str(),
                        "org.freedesktop.portal.Request", "Close", nullptr,
                        nullptr, G_DBUS_CALL_FLAGS_NONE, 1000, nullptr,
                        nullptr, nullptr);
                }
                if (subscription)
                {
                    g_dbus_connection_signal_unsubscribe(bus, subscription);
                }
                g_object_unref(bus);
            }
            else
            {
                result.selection.error =
                    "Native file picker requires a desktop session bus";
            }
            g_clear_error(&error);
            g_main_context_pop_thread_default(context);
            g_main_context_unref(context);
            request->finish(std::move(result.selection));
        });
}
#endif
