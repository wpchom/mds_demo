add_repositories("local-repo " .. path.join(os.scriptdir(), "../../../xbuild"))

add_requires("arm-none-eabi-gcc", {
    system = false
})

toolchain("arm_none_eabi_toolchain", function()
    set_kind("cross")

    add_asflags("-mcpu=cortex-m3", "-mthumb", "-mfloat-abi=soft")
    add_asflags("-fno-common", "-x", "assembler-with-cpp")

    add_cxflags("-mcpu=cortex-m3", "-mthumb", "-mfloat-abi=soft")
    add_cxflags("-fno-common", "-ffreestanding", "-ffunction-sections", "-fdata-sections")
    add_cxflags("-Wall", "-Wextra", "-Wshadow")
    add_cxflags("-Wnull-dereference", "-Wstringop-overflow", "-Wdouble-promotion", "-Wlogical-op", "-Wredundant-decls")
    add_cxflags("-g", "-Os")

    add_ldflags("-mcpu=cortex-m3", "-mthumb", "-mfloat-abi=soft")
    add_ldflags("-nostartfiles", "--specs=nano.specs")
    add_ldflags("-Wl,--gc-sections", "-Wl,--print-memory-usage", "-Wl,--no-warn-rwx-segments")

    set_toolset("objdump", "arm-none-eabi-objdump")
    set_toolset("objcopy", "arm-none-eabi-objcopy")
    set_toolset("size", "arm-none-eabi-size")
end)

set_defaultplat("cross")
set_arch("cortex-m/thumbv7em")
set_toolchains("arm_none_eabi_toolchain@arm-none-eabi-gcc")

add_requires("mds_device", "mds_kernel", "drv_chip_stm32f1xx", "stm32f1xx_device")
add_requireconfs("**mds_kernel", {
    configs = {
        assert = true
    }
})

add_requireconfs("**stm32f1xx_device", {
    configs = {
        device = "STM32F103xB"
    }
})

set_policy("build.intermediate_directory", false)
set_config("buildir", "build/demo_stm32f103/app")
target("demo_stm32f103", function()
    set_kind("binary")

    add_packages("drv_chip_stm32f1xx", "stm32f1xx_device")

    add_files("../src/application/app_main.c")
    add_files("../src/application/STM32F103XB_FLASH_APP.ld")

    on_load(function(target)
        -- import("core.project.project")
        -- import("core.project.config")
        target:set("filename", target:name() .. ".elf")

        target:set("ldflags", "-Wl,--Map=" .. path.join("$(buildir)", target:name()) .. ".map", {
            force = true
        })
        -- config.set("buildir", path.join("build", project.name(), target:name()))
        -- target:set("targetdir", path.join("$(buildir)", project.name(), target:name()))
        -- target:set("objectdir", path.join("$(buildir)", target:name(), "app", ".objs"))
        -- target:set("dependir", path.join("$(buildir)", target:name(), "app", ".deps"))
    end)

    -- after_build(function(target)
    -- os.vrunv(target:tool("objcopy"), {"--version"})
    -- end)

end)
