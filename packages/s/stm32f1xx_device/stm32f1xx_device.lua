add_requires("cmsis", { system = false })

target("stm32f1xx_device", function()
    set_kind("static")

    add_files("Source/Templates/system_stm32f1xx.c")
    add_includedirs("Include", { public = true })

    add_headerfiles("Include/(**.h)")
end)
