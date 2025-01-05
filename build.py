#!/usr/bin/python3

import os
import argparse


MDS_BUILD_DIR = os.path.join(os.path.expanduser("~"), ".mds_build")


def check_build():
    pass


def target_name(gnfile, targetname):
    val = None
    f = open(gnfile)
    for line in f.readlines():
        if line.strip().startswith(targetname):
            val = line.split('=')[1].strip().strip('"\'')
    f.close()

    return (val)


def main():

    if not os.path.exists(MDS_BUILD_DIR):
        print("not exist `mds_build`, prepare with: `git clone https://github.com/wpchom/mds_build.git ~/.mds_build`", flush=True)
        exit(1)

    parser = argparse.ArgumentParser(
        description="builds the MDS demo"
    )

    parser.add_argument("project", type=str, choices=os.listdir(os.path.join(os.getcwd(), 'project')),
                        help="the project to build")
    parser.add_argument("profile", type=str, nargs='?', const='', default=None,
                        help="the profile to build")

    args, unkonw_args = parser.parse_known_args()

    profile_list = []
    profile_path = os.path.join(
        os.getcwd(), 'project', args.project, 'profile')

    if args.profile == None:
        for pf in os.listdir(profile_path):
            if pf.endswith('.gn') and target_name(os.path.join(profile_path, pf), 'project_profile') == pf[:-3]:
                profile_list.append(pf[:-3])
    else:
        profile_list = [args.profile]

    for p in profile_list:
        outdir = os.path.join(os.getcwd(), 'outdir', target_name(
            os.path.join(profile_path, p + '.gn'), 'project_name'), p)

        build_cmd = ["python3", os.path.join(
            MDS_BUILD_DIR, "scripts", "build.py")]
        build_cmd += ["-b", os.path.dirname(profile_path)]
        build_cmd += ["-f", os.path.join(profile_path, p + '.gn')]
        build_cmd += ["-o", outdir]
        build_cmd += unkonw_args

        os.system(" ".join(build_cmd))


if '__main__' == __name__:
    main()
