rm -rf ~/.conan/data/mpc
rm -rf ~/git/mpc/build/Release/*

cd ~/git/mpc/build/Release

conan install -s build_type=Release ../../ mpc/0.1@izmar/dev --build missing
conan build ../../
