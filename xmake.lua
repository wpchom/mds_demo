-- repository
add_repositories("local-repo ./", { rootdir = os.scriptdir() })
set_defaultplat("cross")
set_defaultarchs("arm")

-- policy
set_policy("package.install_locally", true)
set_policy("build.intermediate_directory", false)

-- include
includes("project")
