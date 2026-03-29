includes("../board")

add_requireconfs("**stm32f1xx_device", { configs = { device = "STM32F103xB" } })
add_requireconfs("**stm32f1xx_hal_driver", { configs = { confdir = path.join(os.scriptdir(), "../board") } })

add_requires("stm32f1xx_hal_driver")

target("apps", function()
    set_version("0.0.1")

    set_kind("binary")
    set_basename("stm32f103_apps")
    set_extension(".elf")

    add_files("apps_main.c")
    add_files("STM32F103XB_FLASH_APPS.ld")

    -- add_deps("board")
    add_packages("stm32f1xx_hal_driver")

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
