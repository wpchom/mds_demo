# mds_demo

# directory
```
.
├── README.md  # this file
├── project    # project source files
├── public     # public source files
├── builds     # git submodule of [mds_build](https://github.com/wpchom/mds_build)
└── repos      # git submodule of [xmake-embed](https://github.com/wpchom/xmake-embed)
```

# build script

## build with gn/ninja
```
./builds/build.py -h
./builds/build.py <dotfile> [-k] [-r] [-v] [-x PROXY] [--args ARGS]
```

`
./builds/build.py project/stm32f103/profile/boot.gn
`

## build with xmake
```
xmake -F project/stm32f103/profile/boot.lua
```
