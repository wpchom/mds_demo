-- project
-- set_defaultplat("cross")
-- set_defaultarchs("stm32f103")
-- set_plat("cross")

-- toolchain
add_requires("arm_none_eabi_gcc 14.3.rel1", { system = false })
set_toolchains("gnu-rm@arm_none_eabi_gcc")

-- include
namespace("apps", function()
    includes("src/application")
end)

-- namespace("boot", function()
--     includes("src/bootloader", "src/board")
-- end)
