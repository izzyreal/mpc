#define MPC_NON_COPYABLE(ClassName)                                            \
    ClassName(const ClassName &) = delete;                                     \
    ClassName &operator=(const ClassName &) = delete;                          \
    ClassName(ClassName &&) noexcept = default;                                \
    ClassName &operator=(ClassName &&) noexcept = default;
