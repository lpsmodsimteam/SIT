import argparse
import pkg_resources

from .pyrtl import PyRTL as SITPyRTL
from .systemc import SystemC as SITSystemC
from .verilog import Verilog as SITVerilog
from .util.libmgmt import LibraryManager


def run():

    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-i",
        "--install",
        default=False,
        action="store_true",
        help="install the SIT library",
    )
    parser.add_argument(
        "-v",
        "--version",
        action="version",
        version=pkg_resources.get_distribution("sst_it").version,
    )
    args = vars(parser.parse_args())

    if args["install"]:
        obj = LibraryManager()
        obj.install()
