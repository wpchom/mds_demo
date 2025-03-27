#!/usr/bin/python3

import os
import argparse


def main():

    MDS_BUILD_DIR = os.environ.get("MDS_BUILD_DIR") or os.path.join(
        os.path.expanduser("~"), ".mds_build"
    )

    if not os.path.exists(MDS_BUILD_DIR):
        try:
            os.system(
                "git clone https://github.com/wpchom/mds_build.git %s" % MDS_BUILD_DIR
            )
        except:
            print(
                "not exist `mds_build`, prepare with: `git clone https://github.com/wpchom/mds_build.git ~/.mds_build`",
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

    args, unkonw_args = parser.parse_known_args()

    profile_list = []
    profile_path = os.path.join(os.getcwd(), "project", args.project, "profile")

    if args.profile == None:
        for pf in os.listdir(profile_path):
            if pf.endswith(".gn"):
                profile_list.append(pf[:-3])
    else:
        profile_list = [args.profile]

    for p in profile_list:
        outdir = os.path.join(os.getcwd(), "outdir", args.project, p)

        build_cmd = ["python3", os.path.join(MDS_BUILD_DIR, "mds.py"), "build"]
        build_cmd += ["-b", os.path.dirname(profile_path)]
        build_cmd += ["-d", os.path.join(profile_path, p + ".gn")]
        build_cmd += ["-o", outdir]
        build_cmd += unkonw_args

        ret = os.system(" ".join(build_cmd))
        if ret != 0:
            exit(ret)


if "__main__" == __name__:
    main()
