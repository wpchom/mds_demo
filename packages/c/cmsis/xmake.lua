package("cmsis", function()
    set_kind("library", { headeronly = true })

    set_description("CMSIS Version 6")
    set_homepage("https://github.com/ARM-software/CMSIS_6")

    add_urls("https://github.com/ARM-software/CMSIS_6/archive/refs/tags/v$(version).tar.gz", { alias = "tarurl" })
    add_urls("https://github.com/ARM-software/CMSIS_6/archive/refs/tags/v$(version).zip", { alias = "zipurl" })

    add_versions("tarurl:6.3.0", "331df74000876b8fb07933658cbf402c4d9f831b429258792d2c34ab5a6c5bf7")
    add_versions("tarurl:6.2.0", "35a6bbb89c6b8afe616861b4a8f640f385b6510ff977aeb056e1fd4cb9930529")
    add_versions("tarurl:6.1.0", "d8a044e4b50b7112476d6855a12c729ae2b70b3f77a2a038c23890e9a3515973")

    add_configs("srcdep", {
        default = nil
    })

    on_load(function(package)
        local sourcedir = package:installdir("source")
        local srcdepfile = path.join(get_config("builddir"), "packages", "cmsis", "xmake.lua")

        io.writefile(srcdepfile, [[
        target("cmsis", funtion()
        end)
        ]])
    end)

    on_install(function(package)
        local sourcedir = package:installdir("source")

        os.cp("*", sourcedir)

        package:add("includedirs", path.join(sourcedir, "CMSIS/Core/Include"))
    end)
end)
