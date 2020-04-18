rmdir C:\Users\Izmar\git\mpc\build\32bit /s /q

md C:\Users\Izmar\git\mpc\build\32bit

cd C:\Users\Izmar\git\mpc\build\32bit
conan install -s build_type=Debug -s arch=x86 -g cmake_multi ../../ mpc/0.1@izmar/dev --build missing
conan install -s build_type=Release -s arch=x86 -g cmake_multi ../../ mpc/0.1@izmar/dev --build missing
conan build ../../
conan install -s build_type=Debug -s arch=x86 -g cmake_multi ../../ mpc/0.1@izmar/dev
conan build ../../
