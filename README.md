install connan and after that: 

conan profile update settings.compiler.libcxx=libstdc++11 default

conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan True 
conan remote add conan-community https://api.bintray.com/conan/conan-community/conan True 
conan remote add inexorgame https://api.bintray.com/conan/inexorgame/inexor-conan True 

mkdir build && cd build && conan install .. --build=missing --profile ../gcc8_profile.txt && cmake -GNinja -DCMAKE_BUILD_TYPE=Release .. && cmake --build .  --config Release 
