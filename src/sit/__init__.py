import argparse
import pkg_resources

from .hdl import pyrtl, systemc, verilog
from .libmgmt import LibraryManager


class SSTInteroperability:
    def __init__(self, config_data):

        self.__config_data = config_data
        self.__instance: pyrtl.PyRTL | systemc.SystemC | verilog.Verilog
        self.__ports = []

    def ingest(self):

        self.set_ports(self.__config_data["ports"])

        self.set_hdl_str(self.__config_data["hdl"])
        self.__instance.set_ports(self.__ports)

        if extra_configs := self.__config_data.get("extra", None):

            if fixed_width_float_output := extra_configs.get(
                "fixed_width_float_output", None
            ):
                self.__instance.fixed_width_float_output(
                    fixed_width_float_output
                )

            if disable_runtime_warnings := extra_configs.get(
                "disable_runtime_warnings", None
            ):
                self.__instance.disable_runtime_warnings(
                    disable_runtime_warnings
                )

    def generate_black_boxes(self):

        self.__instance.generate_black_boxes()

    def set_hdl_str(self, hdl_str: str):

        if hdl_str.lower() == "pyrtl":
            self.__instance = pyrtl.PyRTL(**self.__config_data["config"])

        elif hdl_str.lower() == "systemc":
            self.__instance = systemc.SystemC(**self.__config_data["config"])

        elif hdl_str.lower() == "verilog":
            self.__instance = verilog.Verilog(**self.__config_data["config"])

    def set_ports(self, ports: dict[str, list[str]]):

        # PORT = PORT_DATA_TYPE, PORT_NAME, PORT_TYPE, PORT_LENGTH
        for k, v in ports.items():

            for port in v:
                self.__ports.append((k, port["name"], port["type"]))

    def set_config_data(self, config_data):

        self.__config_data = config_data


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
