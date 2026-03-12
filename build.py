#!/usr/bin/python3

import os
import argparse
import subprocess


def main():
    GNB_REPO_GIT = "https://github.com/wpchom/gnb.git"
    GNB_REPO_DIR = os.environ.get("MDS_BUILD_DIR") or os.path.join(
        os.path.expanduser("~"), ".gnb"
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

    parser = argparse.ArgumentParser(description="builds the MDS demo")

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
        const="",
        default=None,
        help="the profile to build",
    )

    parser.add_argument("-v", "--verbose", action="store_true", default=False)
    parser.add_argument("-c", "--clean", action="store_true", default=False)

    args, unkonw_args = parser.parse_known_args()

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
        build_cmd = ["python3", os.path.join(GNB_REPO_DIR, "gnb.py")]
        if args.verbose:
            build_cmd += ["-v"]
        build_cmd += ["build"]
        if args.clean:
            build_cmd += ["-c"]
        build_cmd += ["-b", os.path.dirname(profile_path)]
        build_cmd += ["-p", os.path.join(profile_path, p + ".gn")]
        build_cmd += ["-o", os.path.join(os.getcwd(), "outdir", args.project, p)]
        build_cmd += unkonw_args

        ret = subprocess.run(build_cmd)

        if ret.returncode != 0:
            exit(ret)


if "__main__" == __name__:
    main()
