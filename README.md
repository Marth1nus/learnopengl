# [LearnOpenGL](https://learnopengl.com/)

This repo covers [LearnOpenGL](https://learnopengl.com/) with support for web-builds.

View the public web-build [here](https://marth1nus.github.io/learnopengl)

# Compile

Start but cloning the repo

```sh
git clone https://github.com/Marth1nus/learnopengl
cd learnopengl
```

For a Current-Platform build

```sh
mkdir build
pushd build
cmake ..
cmake --build .
popd
```

For a web-build

```sh
mkdir build-emcc
pushd build-emcc
emcmake cmake ..
cmake --build .
popd
# Resulting Webpage will be in the ./pages/ directory
```

# Support Goals

- **OpenGL API versions**:
  - OpenGL ES3
  - WebGL2
- **Platforms**:
  - Windows
  - Linux
  - Emscripten (web)
