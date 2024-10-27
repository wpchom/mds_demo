#!/usr/bin/python3

import os
import shutil
import subprocess
import argparse

MDS_BUILD_GIT = "https://gitee.com/pchom/mds_build.git"
MDS_BUILD_DIR = os.path.join(
    os.path.dirname(os.path.realpath(__file__)), "..", "mds_build")


def error(message):
    print("\033[31m>>> {}\033[0m".format(message), flush=True)
    exit(-1)


def check_git():
    git_bin = shutil.which("git")

    ret = subprocess.run([git_bin, "--version"], stdout=subprocess.DEVNULL)
    if ret.returncode != 0:
        error("'git' command is erorr")

    return (git_bin)


def check_build(git_bin):
    if not os.path.exists(MDS_BUILD_DIR):
        ret = subprocess.run([git_bin, "clone", MDS_BUILD_GIT, MDS_BUILD_DIR])
        if ret.returncode != 0:
            error("Failed to clone mds_build")


def build_argparse(project_dir):
    parser = argparse.ArgumentParser(
        description="./build.py <project> [profile]")

    parser.add_argument("project", type=str, choices=os.listdir(os.path.join(project_dir, "src", "project")),
                        help="name for dir where in project for build")
    parser.add_argument("-f", "--profile", type=str, required=False,
                        help="default all profile in project")

    parser.add_argument("-k", "--update", action="store_true", default=False,
                        help="update mds_build from git")
    parser.add_argument("-r", "--rebuild", action="store_true", default=False,
                        help="clean outdir before build")
    parser.add_argument("-v", "--verbose", action="store_true", default=False,
                        help="show build verbose output")
    parser.add_argument("-x", "--proxy", type=str, default=os.getenv("MDS_BUILD_PROXY"),
                        help="proxy server, default getenv MDS_BUILD_PROXY")

    parser.add_argument("--args", type=str, default=None,
                        help="gn gen with args")

    args = parser.parse_args()

    return (args)


def main():
    project_dir = os.path.dirname(os.path.realpath(__file__))

    args = build_argparse(project_dir)

    buildir = os.path.join(project_dir, "src", "project", args.project)

    profiles = [os.path.join(buildir, "profile", "{}.gn".format(args.profile))] if args.profile != None else [
        os.path.join(buildir, "profile", f) for f in os.listdir(os.path.join(buildir, "profile")) if f.endswith(".gn")]

    for f in profiles:
        outdir = os.path.join(project_dir, "out", args.project,
                              os.path.basename(f).split(".")[0])

        cmd_build = ([os.path.join(MDS_BUILD_DIR, "scripts/build.py"),
                     "-b", buildir, "-o", outdir, "-f", f] +
                     (["-x=%s" % args.proxy] if args.proxy else []) +
                     (["--args=%s" % args.args] if args.args else []) +
                     (["-v"] if args.verbose else []) +
                     (["-r"] if args.rebuild else []) +
                     (["-k"] if args.update else []))

        if args.verbose:
            print(" ".join(cmd_build))

        res = os.system(" ".join(cmd_build))
        if res != 0:
            error("build '{}' failed".format(f))


if __name__ == "__main__":
    main()
