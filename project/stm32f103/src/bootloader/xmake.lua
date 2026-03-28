includes("../board")

add_requires("stm32f1xx_device")

target("stm32f103_boot", function()
    set_kind("binary")
    set_version("0.0.1")

    add_files("./boot_main.c")
    add_files("STM32F103XB_FLASH_BOOT.ld")

    -- add_deps("board")
    add_packages("stm32f1xx_device")

    add_ldflags(
        "-nostartfiles",
        "-Wl,--gc-sections",
        "-Wl,--print-memory-usage",
        "-Wl,--no-warn-rwx-segments", { force = true })

    after_build(function(target)
        local objcopy = target:tool("objcopy")
        local objdump = target:tool("objdump")
        local elffile = target:targetfile()
        local logstr = ".logstr"

        -- bin
        local binfile = path.join(target:targetdir(), path.basename(elffile) .. ".bin")
        os.runv(objcopy, { "-O", "binary", "-R", logstr, elffile, binfile })

        -- hex
        local hexfile = path.join(target:targetdir(), path.basename(elffile) .. ".hex")
        os.runv(objcopy, { "-O", "ihex", "-R", logstr, elffile, hexfile })

        -- logstr
        local logfile = path.join(target:targetdir(), path.basename(elffile) .. ".logstr")
        os.runv(objcopy, { "-O", "binary", "-j", logstr, elffile, logfile })

        -- dis
        local disfile = path.join(target:targetdir(), path.basename(elffile) .. ".dis")
        local context, err = os.iorunv(objdump, { "-SFhlx", elffile })
        io.writefile(disfile, context)
    end)
end)
