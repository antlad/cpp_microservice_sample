#!/bin/bash
conan profile new default --detect || true
conan profile update settings.compiler.libcxx=libstdc++11 default || true
conan remote add inexorgame https://api.bintray.com/conan/inexorgame/inexor-conan || true
conan remote add stiffstream https://api.bintray.com/conan/stiffstream/public || true
