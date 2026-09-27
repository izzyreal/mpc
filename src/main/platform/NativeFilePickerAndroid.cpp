#ifdef __ANDROID__
#include "NativeFilePicker.hpp"
#include <disk/ImageFileDevice.hpp>
#include <jni.h>
#include <map>
#include <stdexcept>
using namespace mpc::platform;
namespace
{
    std::mutex bridgeMutex;
    JavaVM *javaVm = nullptr;
    jclass helper = nullptr;
    std::map<jlong, std::weak_ptr<PickerRequest>> requests;
    jlong nextRequest = 1;
    struct Environment
    {
        JavaVM *vm = nullptr;
        JNIEnv *env = nullptr;
        bool attached = false;
        Environment()
        {
            {
                std::lock_guard lock(bridgeMutex);
                vm = javaVm;
            }
            if (!vm)
            {
                throw std::runtime_error(
                    "Attach the Android Activity before opening a document");
            }
            if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) ==
                JNI_EDETACHED)
            {
                if (vm->AttachCurrentThread(&env, nullptr) != JNI_OK)
                {
                    throw std::runtime_error(
                        "Unable to attach image access thread");
                }
                attached = true;
            }
            if (!env)
            {
                throw std::runtime_error("JNI environment unavailable");
            }
        }
        ~Environment()
        {
            if (attached)
            {
                vm->DetachCurrentThread();
            }
        }
        void check()
        {
            if (env->ExceptionCheck())
            {
                env->ExceptionClear();
                throw std::runtime_error(
                    "Image provider denied access; replace the binding");
            }
        }
    };
    std::string stringFromJava(JNIEnv *env, jstring value)
    {
        if (!value)
        {
            return {};
        }
        const char *text = env->GetStringUTFChars(value, nullptr);
        if (!text)
        {
            return {};
        }
        std::string result(text);
        env->ReleaseStringUTFChars(value, text);
        return result;
    }
    void call(const char *method, jlong request)
    {
        Environment e;
        jclass type;
        {
            std::lock_guard lock(bridgeMutex);
            type = static_cast<jclass>(e.env->NewLocalRef(helper));
        }
        const auto id = e.env->GetStaticMethodID(type, method, "(J)V");
        e.env->CallStaticVoidMethod(type, id, request);
        e.env->DeleteLocalRef(type);
        e.check();
    }
} // namespace
extern "C" JNIEXPORT void JNICALL
Java_org_vmpc_platform_NativeImagePicker_initialize(JNIEnv *env, jclass type)
{
    std::lock_guard lock(bridgeMutex);
    env->GetJavaVM(&javaVm);
    if (!helper)
    {
        helper = static_cast<jclass>(env->NewGlobalRef(type));
    }
}
extern "C" JNIEXPORT void JNICALL
Java_org_vmpc_platform_NativeImagePicker_complete(JNIEnv *env, jclass, jlong id,
                                                  jstring uri, jstring label,
                                                  jstring error)
{
    std::shared_ptr<PickerRequest> request;
    {
        std::lock_guard lock(bridgeMutex);
        auto it = requests.find(id);
        if (it == requests.end())
        {
            return;
        }
        request = it->second.lock();
        requests.erase(it);
    }
    if (!request)
    {
        return;
    }
    FileSelection result;
    result.error = stringFromJava(env, error);
    if (uri)
    {
        result.image =
            selectedImage(stringFromJava(env, uri), stringFromJava(env, label));
    }
    request->finish(std::move(result));
}
void mpc::platform::showNativeFilePicker(std::shared_ptr<PickerRequest> request,
                                         void *, const std::string &)
{
    jlong id;
    {
        std::lock_guard lock(bridgeMutex);
        id = nextRequest++;
        requests[id] = request;
    }
    request->setDismiss(
        [id]
        {
            {
                std::lock_guard lock(bridgeMutex);
                requests.erase(id);
            }
            try
            {
                call("cancel", id);
            }
            catch (...)
            {
            }
        });
    try
    {
        call("pick", id);
    }
    catch (...)
    {
        {
            std::lock_guard lock(bridgeMutex);
            requests.erase(id);
        }
        throw;
    }
}
std::shared_ptr<akaifat::BlockDevice>
mpc::platform::openNativeImage(const disk::Volume &v, bool ro)
{
    if (v.diskImagePath.rfind("content:", 0) != 0)
    {
        return disk::ImageFileDevice::open(v.diskImagePath, ro);
    }
    Environment e;
    jclass type;
    {
        std::lock_guard lock(bridgeMutex);
        type = static_cast<jclass>(e.env->NewLocalRef(helper));
    }
    auto location = e.env->NewStringUTF(v.diskImagePath.c_str());
    const auto open =
        e.env->GetStaticMethodID(type, "openImage", "(Ljava/lang/String;Z)I");
    const int fd = e.env->CallStaticIntMethod(type, open, location,
                                              ro ? JNI_TRUE : JNI_FALSE);
    e.env->DeleteLocalRef(location);
    e.env->DeleteLocalRef(type);
    e.check();
    return disk::ImageFileDevice::adoptDescriptor(fd, ro);
}
#endif
