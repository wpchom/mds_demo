package("stm32f1xx_device", function()
    set_kind("library")

    set_description("STM32CubeF1 CMSIS Device MCU Component")
    set_homepage("https://github.com/STMicroelectronics/cmsis-device-f1")

    set_urls("https://github.com/STMicroelectronics/cmsis-device-f1/archive/refs/tags/v$(version).tar.gz")

    add_versions("4.3.5", "2994ffe58af1f819928f11840359565e0293e91dfb840661da77c2e02de24ca3")

    on_install(function(package)
        local configs = {}
        local xmake = import("package.tools.xmake")

        os.vcp(path.join(os.scriptdir(), "stm32f1xx_device.lua"), "xmake.lua")
        xmake.install(package, configs)
    end)
end)
