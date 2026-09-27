#ifdef _WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <shobjidl.h>
#include "NativeFilePicker.hpp"
#include <mpc_fs.hpp>
using namespace mpc::platform;
namespace
{
    thread_local IFileOpenDialog *activeDialog = nullptr;
    thread_local PickerRequest *activeRequest = nullptr;
    void CALLBACK checkCancel(HWND, UINT, UINT_PTR, DWORD)
    {
        if (activeRequest && activeRequest->cancelled && activeDialog)
        {
            activeDialog->Close(HRESULT_FROM_WIN32(ERROR_CANCELLED));
        }
    }
} // namespace
void mpc::platform::showNativeFilePicker(std::shared_ptr<PickerRequest> request,
                                         void *parent, const std::string &)
{
    request->worker = std::thread(
        [request, parent]
        {
            if (request->cancelled)
            {
                return;
            }
            const auto initialized = CoInitializeEx(
                nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
            if (FAILED(initialized))
            {
                request->finish({{}, "Cannot initialize native file picker"});
                return;
            }
            IFileOpenDialog *dialog = nullptr;
            auto result =
                CoCreateInstance(CLSID_FileOpenDialog, nullptr,
                                 CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&dialog));
            FileSelection selection;
            if (SUCCEEDED(result))
            {
                dialog->SetTitle(L"Select a FAT disk image");
                dialog->SetOptions(FOS_FILEMUSTEXIST | FOS_PATHMUSTEXIST |
                                   FOS_FORCEFILESYSTEM | FOS_NOCHANGEDIR);
                activeDialog = dialog;
                activeRequest = request.get();
                const auto timer = SetTimer(nullptr, 0, 50, checkCancel);
                if (!timer)
                {
                    result = E_FAIL;
                }
                else if (request->cancelled)
                {
                    result = HRESULT_FROM_WIN32(ERROR_CANCELLED);
                }
                else
                {
                    result = dialog->Show(parent && IsWindow((HWND)parent)
                                              ? (HWND)parent
                                              : nullptr);
                }
                if (timer)
                {
                    KillTimer(nullptr, timer);
                }
                activeDialog = nullptr;
                activeRequest = nullptr;
                if (SUCCEEDED(result))
                {
                    IShellItem *item = nullptr;
                    result = dialog->GetResult(&item);
                    if (SUCCEEDED(result))
                    {
                        PWSTR path = nullptr;
                        result = item->GetDisplayName(SIGDN_FILESYSPATH, &path);
                        if (SUCCEEDED(result))
                        {
                            selection.image =
                                selectedImage(mpc_fs::path(path).string());
                            CoTaskMemFree(path);
                        }
                        item->Release();
                    }
                }
                dialog->Release();
            }
            if (FAILED(result) && result != HRESULT_FROM_WIN32(ERROR_CANCELLED))
            {
                selection.error = "Native file picker failed";
            }
            CoUninitialize();
            request->finish(std::move(selection));
        });
}
#endif
