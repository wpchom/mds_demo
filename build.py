#!/usr/bin/python3

import os
import sys
import shutil
import argparse
import subprocess

sys.dont_write_bytecode = True


def repo_check():
    GNB_REPO_GIT = "https://github.com/wpchom/gnb.git"
    GNB_REPO_DIR = os.environ.get("GNB_REPO_DIR")
    GNB_REPO_DIR = (
        os.path.join(f"{GNB_REPO_DIR}".replace("~", os.path.expanduser("~")))
        if GNB_REPO_DIR != None
        else os.path.join(os.path.expanduser("~"), ".gnb")
    )

    if not os.path.exists(GNB_REPO_DIR):
        try:
            os.system(f"git clone {GNB_REPO_GIT} {GNB_REPO_DIR}")
        except:
            print(
                f"not exist `gnb`, prepare with: `git clone {GNB_REPO_GIT} ~/.gnb`",
                flush=True,
            )
            exit(1)

    return GNB_REPO_DIR


def gnb_build(repo_dir, args, extra_args):
    profile_list = []
    profile_path = os.path.join(
        os.path.dirname(__file__), "project", args.project, "profiles"
    )

    if args.profile == None:
        for pf in os.listdir(profile_path):
            if pf.endswith(".gn"):
                profile_list.append(pf[:-3])
    else:
        profile_list = [args.profile]

    for p in profile_list:
        output_dir = os.path.join(os.path.dirname(__file__), "output", args.project, p)

        if args.clean and os.path.exists(output_dir):
            shutil.rmtree(output_dir)

        os.makedirs(output_dir, exist_ok=True)

        build_cmd = ["python3", os.path.join(repo_dir, "gnb.py"), "build"]
        if args.verbose:
            build_cmd += ["-v"]
        if args.clean:
            build_cmd += ["-c"]
        build_cmd += ["-b", os.path.dirname(profile_path)]
        build_cmd += ["-p", os.path.join(profile_path, p + ".gn")]
        build_cmd += ["-o", output_dir]

        if extra_args:
            build_cmd += extra_args

        if args.verbose:
            print(build_cmd, output_dir)

        ret = subprocess.run(
            build_cmd,
            cwd=output_dir,
            stdout=sys.stdout,
            stderr=sys.stderr,
        )
        if ret.returncode != 0:
            exit(ret)


def xmake_build(repo_dir, args, extra_args):
    profile_list = []
    profile_path = os.path.join(
        os.path.dirname(__file__), "project", args.project, "profiles"
    )

    if args.profile == None:
        for pf in os.listdir(profile_path):
            if pf.endswith(".lua"):
                profile_list.append(pf[:-4])
    else:
        profile_list = [args.profile]

    for p in profile_list:
        output_dir = os.path.join(os.path.dirname(__file__), "output", args.project, p)

        if args.clean and os.path.exists(output_dir):
            shutil.rmtree(output_dir)

        os.makedirs(output_dir, exist_ok=True)

        build_cmd = ["xmake", "build", "-y"]
        build_cmd += ["-F", os.path.join(profile_path, p + ".lua")]

        if args.verbose:
            build_cmd += ["-v"]

        if extra_args:
            build_cmd += extra_args

        if args.verbose:
            print(build_cmd, output_dir)

        ret = subprocess.run(
            build_cmd,
            cwd=output_dir,
            stdout=sys.stdout,
            stderr=sys.stderr,
        )
        if ret.returncode != 0:
            exit(ret)


def main():
    argv = sys.argv[1:]

    if "--" in argv:
        idx = argv.index("--")
        main_args = argv[:idx]
        extra_args = argv[idx + 1 :]
    else:
        main_args = argv
        extra_args = []

    parser = argparse.ArgumentParser()

    parser.add_argument(
        "project",
        type=str,
        choices=os.listdir(os.path.join(os.getcwd(), "project")),
        help="the project to build",
    )
    parser.add_argument(
        "profile",
        type=str,
        nargs="?",
        default=None,
        help="the profile to build",
    )

    parser.add_argument("-v", "--verbose", action="store_true", default=False)
    parser.add_argument("-c", "--clean", action="store_true", default=False)
    parser.add_argument("-x", "--xmake", action="store_true", default=False)

    args = parser.parse_args(main_args)

    if args.verbose:
        print(args, extra_args)

    repo_dir = repo_check()
    if args.xmake:
        xmake_build(repo_dir, args, extra_args)
    else:
        gnb_build(repo_dir, args, extra_args)


if "__main__" == __name__:
    main()
