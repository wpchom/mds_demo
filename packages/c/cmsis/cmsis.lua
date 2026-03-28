target("cmsis", function()
    set_kind("headeronly")

    add_includedirs("CMSIS/Core/Include", { public = true })
end)
