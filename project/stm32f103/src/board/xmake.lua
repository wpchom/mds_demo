target("board", function()
    set_kind("object")
    set_default(false)

    add_files("brd_clock.c")

    add_includedirs(".", { public = true })
end)
