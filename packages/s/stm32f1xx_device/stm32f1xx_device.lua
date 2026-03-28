add_requires("cmsis", { private = true, system = false })

target("stm32f1xx_device", function()
    set_kind("static")

    add_packages("cmsis")
    add_includedirs("Include", { public = true })

    on_load(function(target)
    --     target:add("files", "Source/Templates/" .. string.
    --     lower(get_config("system")) .. ".c")

    --     target:add("files", path.join("startup", "startup_" .. string.lower(get_config("chip")) .. ".c"))
    --     -- target:add("defines", get_config("chip"), { public = true })
    end)
end)
