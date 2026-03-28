package("stm32f1xx_device", function()
    set_policy("package.install_always", true)

    set_kind("library")

    set_description("STM32CubeF1 CMSIS Device MCU Component")
    set_homepage("https://github.com/STMicroelectronics/cmsis-device-f1")

    set_urls("https://github.com/STMicroelectronics/cmsis-device-f1/archive/refs/tags/v$(version).tar.gz")

    add_versions("4.3.5", "2994ffe58af1f819928f11840359565e0293e91dfb840661da77c2e02de24ca3")

    add_deps("cmsis")

    add_configs("chip", {
        default = "STM32F103xB"
    })

    on_install(function(package)
        local configs = package:configs()

        package:add("defines", configs["chip"])

        os.cp("*", package:installdir("source"))
        package:add("includedirs", "source/Include")

        os.cp(path.join(os.scriptdir(), "startup"), "source/startup")
        os.cp(path.join(os.scriptdir(), "stm32f1xx_device.lua"), "xmake.lua")

        local xmake = import("package.tools.xmake")
        xmake.install(package)
    end)
end)
