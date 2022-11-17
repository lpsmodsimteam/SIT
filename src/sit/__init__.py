import argparse
import pkg_resources

from .pyrtl import PyRTL as SITPyRTL
from .systemc import SystemC as SITSystemC
from .verilog import Verilog as SITVerilog
from .util.libmgmt import LibraryManager


def run():

    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-v",
        "--version",
        action="version",
        version=pkg_resources.get_distribution("sst_it").version,
    )
    parser.add_argument(
        "-i",
        "--install",
        default=False,
        action="store_true",
        help="install the SIT library",
    )

    args = vars(parser.parse_args())

    if args["install"]:

        lib = LibraryManager()
        if lib.is_installed():
            print(f"SIT is already installed at '{lib.get_dest_dir()}'")
            if input("Reinstall library? [y/N] ").lower() == "y":
                lib.install()

        else:
            lib.install()
