-- repository
add_repositories("local-repo ~/code-workspace/gnb", { rootdir = os.scriptdir() })

-- policy
set_policy("build.intermediate_directory", false)
set_policy("package.install_locally", true)

-- toolchain
add_requires("arm-none-eabi-gcc 14.3.rel1", { system = false })
set_defaultplat("cross")
set_defaultarchs("cortex-m3")

toolchain("stm32f103_toolchain", function()
    set_kind("cross")

    on_load(function(toolchain)
        toolchain:load_cross_toolchain()
        toolchain:add("cxflags", "-mcpu=cortex-m3", "-mthumb", "-msoft-float", "-Og", { force = true })
        toolchain:add("ldflags", "-mcpu=cortex-m3", "-mthumb", "-msoft-float", { force = true })
        toolchain:add("shflags", "-mcpu=cortex-m3", "-mthumb", "-msoft-float", { force = true })
    end)

    set_toolset("objcopy", "arm-none-eabi-objcopy")
    set_toolset("objdump", "arm-none-eabi-objdump")
end)

set_toolchains("stm32f103_toolchain@arm-none-eabi-gcc")

includes("../src/application")
