rmdir C:\Users\Izmar\git\mpc\build\32bit /s /q
rmdir C:\Users\Izmar\git\mpc\build\64bit /s /q

md C:\Users\Izmar\git\mpc\build\32bit
md C:\Users\Izmar\git\mpc\build\64bit

cd C:\Users\Izmar\git\mpc\build\32bit
conan install -s build_type=Debug -s arch=x86 -g cmake_multi ../../ mpc/0.1@izmar/dev --build missing
conan install -s build_type=Release -s arch=x86 -g cmake_multi ../../ mpc/0.1@izmar/dev --build missing
conan build ../../
conan install -s build_type=Debug -s arch=x86 -g cmake_multi ../../ mpc/0.1@izmar/dev
conan build ../../

cd C:\Users\Izmar\git\mpc\build\64bit
conan install -s build_type=Debug -g cmake_multi ../../ mpc/0.1@izmar/dev --build missing
conan install -s build_type=Release -g cmake_multi ../../ mpc/0.1@izmar/dev --build missing
conan build ../../
conan install -s build_type=Debug -g cmake_multi ../../ mpc/0.1@izmar/dev
conan build ../../
