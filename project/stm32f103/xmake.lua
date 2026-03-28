-- repository
add_repositories("local-repo ../../", { rootdir = os.scriptdir() })

-- policy
set_policy("build.intermediate_directory", false)
set_policy("package.install_locally", true)

-- toolchain
set_defaultplat("cross")
add_requires("arm_none_eabi_gcc 14.3.rel1", { system = false })

toolchain("arm_gnu_cm3", function()
    set_kind("cross")

    on_load(function(toolchain)
        toolchain:load_cross_toolchain()
        toolchain:add("cxflags", "-mcpu=cortex-m3", "-mthumb", "-mfloat-abi=soft", { force = true })
        toolchain:add("ldflags", "-mcpu=cortex-m3", "-mthumb", "-mfloat-abi=soft", { force = true })
    end)

    set_toolset("objcopy", "arm-none-eabi-objcopy")
    set_toolset("objdump", "arm-none-eabi-objdump")
end)

set_toolchains("arm_gnu_cm3@arm_none_eabi_gcc")

-- include
namespace("apps", function()
    includes("src/application")
end)

namespace("boot", function()
    includes("src/bootloader")
end)
