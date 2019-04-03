from conans import ConanFile, CMake

class Pkg(ConanFile):
    name = "mpc"
    description = "static library for emulating the Akai MPC2000XL sequencer sampler"
    version = "0.1"
    settings = "os", "compiler", "arch", "build_type"
    generators = "cmake"
    exports_sources = "src/*"
    requires = ("ctoot/0.1@izmar/dev", "moduru/0.1@izmar/dev", "rapidjson/1.1.0@bincrafters/stable", "Catch2/2.7.0@catchorg/stable")
    url = "https://github.com/izzyreal/mpc.git"
	
    def build(self):
        cmake = CMake(self)
        cmake.configure(source_folder="src")
        cmake.build()

    def package(self):
        self.copy("*.h", src="src/main", dst="include")
        self.copy("*.hpp", src="src/main", dst="include")
        self.copy("*.lib", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.libs=["mpc"]
        if not self.settings.os == "Windows":
            self.cpp_info.cxxflags = ["-pthread"]
            self.cpp_info.sharedlinkflags = ["-lpthread"]