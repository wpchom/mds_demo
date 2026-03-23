includes("../board")

add_requires("stm32f1xx_device", { system = false })

target("stm32f103_apps", function()
    set_kind("binary")
    set_version("0.0.1")

    add_files("apps_main.c")
    add_files("STM32F103XB_FLASH_APPS.ld")

    add_deps("board")

    after_build(function(target)
        local objcopy = target:tool("objcopy")
        local objdump = target:tool("objdump")
        local logstr = ".logstr"
        local elffile = target:targetfile()

        -- bin
        local binfile = path.join(target:targetdir(), path.basename(elffile) .. ".bin")
        os.vrunv(objcopy, { "-Obinary", "-R" .. logstr, elffile, binfile })

        -- hex
        local hexfile = path.join(target:targetdir(), path.basename(elffile) .. ".hex")
        os.vrunv(objcopy, { "-Oihex", "-R" .. logstr, elffile, hexfile })

        -- dis
        local disfile = path.join(target:targetdir(), path.basename(elffile) .. ".dis")
        os.vrunv(objdump, { "-SFhlx", elffile, ">", disfile })

        -- logstr
        local logfile = path.join(target:targetdir(), path.basename(elffile) .. ".logstr")
        os.vrunv(objcopy, { "-Obinary", "-j" .. logstr, elffile, logfile })
    end)
end)
