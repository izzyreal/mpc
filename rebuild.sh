rm -rf ~/git/mpc/build/Release/*
rm -rf ~/git/mpc/build/Debug/*

mkdir -p ~/git/mpc/build/Release
mkdir -p ~/git/mpc/build/Debug

cd ~/git/mpc/build/Debug
conan install -s build_type=Debug ../../ mpc/0.1@izmar/dev --build missing
conan build ../../

cd ~/git/mpc/build/Release
conan install -s build_type=Release ../../ mpc/0.1@izmar/dev --build missing
conan build ../../
