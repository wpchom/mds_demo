#!/usr/bin/python3

import os
import shutil
import subprocess
import argparse

MDS_BUILD_DIR = os.path.join(os.path.dirname(__file__), "..", "mds_build")
MDS_BUILD_GIT = "git@gitee.com:pchom/mds_build.git"


def error(message):
    print("\033[31m>>> {}\033[0m".format(message), flush=True)
    exit(-1)


def check_git():
    git_cmd = shutil.which("git")

    ret = subprocess.run([git_cmd, "--version"], stdout=subprocess.DEVNULL)
    if ret.returncode != 0:
        error("'git' command is erorr")

    return git_cmd


def check_build(git):
    if os.path.exists(MDS_BUILD_DIR):
        ret = subprocess.run(
            [git, "pull"], cwd=MDS_BUILD_DIR, stdout=subprocess.DEVNULL)
        if ret.returncode != 0:
            error("Failed to update mds_build, please update it manually")
    else:
        ret = subprocess.run(
            [git, "clone", MDS_BUILD_GIT, MDS_BUILD_DIR], stdout=subprocess.DEVNULL)
        if ret.returncode != 0:
            error("Failed to clone mds_build")


def build_argparse():
    parser = argparse.ArgumentParser(
        description="with ./project/<proj_name>/dotfile/<profile>.gn")

    parser.add_argument("project", type=str, nargs="?", const=1,
                        help="name for dir where in project for build")
    parser.add_argument("-f", "--dotfile", type=str, default=None,
                        help="default all dotfile in project")
    parser.add_argument("-x", "--proxy", type=str, default=None,
                        help="proxy server, default: None")
    parser.add_argument("-r", "--rebuild", dest="rebuild",
                        action="store_true", default=False,
                        help="clean output before build")
    parser.add_argument("-v", "--verbose", dest="verbose",
                        action="store_true", default=False,
                        help="show build verbose output")

    args = parser.parse_args()

    if (args.project == None) or (args.project not in os.listdir(os.path.join(os.path.dirname(__file__), "project"))):
        error("project name `{}` is invailed, please check name in project dir".format(
            args.project))

    args.buildir = os.path.abspath(os.path.join(
        os.path.dirname(__file__), "project", args.project))
    args.outdir = os.path.abspath(os.path.join(
        os.path.dirname(__file__), "output", args.project))

    return args


def main():
    check_build(check_git())

    args = build_argparse()

    build_args = ["-b", args.buildir, "-o", args.outdir]
    if (args.dotfile != None):
        build_args += ["-f", args.dotfile]
    if (args.proxy != None):
        build_args += ["-x", args.proxy]
    if (args.rebuild):
        build_args += ["-r"]
    if (args.verbose):
        build_args += ["-v"]
        print(' '.join(build_args))

    result = subprocess.run([os.path.join(
        MDS_BUILD_DIR, "script/build.py")] + build_args + ["--args", "mds_build_dir=\\\"{}\\\"".format(MDS_BUILD_DIR)])

    return (result.returncode)


if __name__ == "__main__":
    main()
