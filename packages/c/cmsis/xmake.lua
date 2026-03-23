package("cmsis", function()
    set_kind("library", { headeronly = true })

    set_description("CMSIS Version 6")
    set_homepage("https://github.com/ARM-software/CMSIS_6")

    set_urls("https://github.com/ARM-software/CMSIS_6/archive/refs/tags/v$(version).tar.gz")
    -- set_urls("https://api.github.com/repos/ARM-software/CMSIS_6/tarball/v$(version)")

    add_versions("6.3.0", "331df74000876b8fb07933658cbf402c4d9f831b429258792d2c34ab5a6c5bf7")
    add_versions("6.2.0", "35a6bbb89c6b8afe616861b4a8f640f385b6510ff977aeb056e1fd4cb9930529")
    add_versions("6.1.0", "d8a044e4b50b7112476d6855a12c729ae2b70b3f77a2a038c23890e9a3515973")

    on_install(function(package)
        os.vcp("*", package:installdir())
        package:add("includedirs", package:installdir("CMSIS/Core/Include"))
    end)
end)
