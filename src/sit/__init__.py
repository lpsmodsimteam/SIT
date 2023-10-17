import argparse
import pkg_resources
from pprint import pprint

from .configfile import ConfigFile
from .exceptions import ConfigException
from .hdl import pyrtl, systemc, verilog
from .libmgmt import LibraryManager


class SIT:
    def __init__(self, config_data):

        self.__config_data = config_data
        self.__instance: pyrtl.PyRTL | systemc.SystemC | verilog.Verilog
        self.__config_file: ConfigFile

    def generate_boilerplate(self):

        self.__set_hdl_str(self.__config_data["hdl"])

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

            if width_macros := extra_configs.get("width_macros", None):
                self.__instance.set_width_macros(width_macros)

        self.__instance.set_ports(self.__config_data["ports"])
        self.__instance.generate_boilerplate()
        self.dump_summary()

    def dump_summary(self):

        summary = {
            "module name": self.__config_data["config"]["module_name"],
            "hdl": self.__config_data["hdl"],
            "generated filepaths": {
                "driver": self.__instance.paths.get_gen("driver"),
                "component": self.__instance.paths.get_gen("comp"),
            },
            "ports": {
                "input": [],
                "output": [],
                "inout": [],
            },
            "buffer sizes": {
                "driver": self.__instance.driver_buf_size,
                "component": self.__instance.comp_buf_size,
            },
        }

        for port_type in self.__config_data["ports"]:
            if self.__config_data["ports"][port_type]:
                for port in self.__config_data["ports"][port_type]:
                    summary["ports"][port_type].append(
                        {
                            "name": port["name"],
                            "data type": port["type"],
                            "buffer length": port["len"],
                        }
                    )

        pprint(summary, sort_dicts=False)

    def generate_config_file(self):

        self.__config_file = ConfigFile()
        print(
            self.__config_file.generate_linkwrapper_binding(
                self.__config_data["config"]["module_name"],
                self.__config_data["config"]["lib"],
                self.__instance._get_input_ports(),
            )
        )

    def set_config_data(self, config_data):

        self.__config_data = config_data

    def __set_hdl_str(self, hdl_str):

        match hdl_str.lower():

            case "pyrtl":
                self.__instance = pyrtl.PyRTL(**self.__config_data["config"])

            case "systemc":
                self.__instance = systemc.SystemC(
                    **self.__config_data["config"]
                )

            case "verilog":
                self.__instance = verilog.Verilog(
                    **self.__config_data["config"]
                )

            case _:
                raise ConfigException(
                    f'"{hdl_str}" is not supported as a hardware description language'
                )


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
