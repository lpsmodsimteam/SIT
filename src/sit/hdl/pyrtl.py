"""Implementation of the PyRTL class

This class inherits from the SIT base class and implements its own methods of parsing,
modifying and generating boilerplate code for its specific paradigms.
"""
import warnings

from ..sit import SIT


class PyRTL(SIT):
    def __init__(
        self, module_name, lib, ipc="sock", module_dir="", lib_dir="", desc=""
    ):
        """Constructor for PyRTL SIT.

        Parameters:
        -----------
        ipc : str (options: "sock", "zmq")
            method of IPC
        module : str
            SST element component and HDL module name
        lib : str
            SST element library name
        module_dir : str (default: "")
            directory of HDL module
        lib_dir : str (default: "")
            directory of SIT library
        desc : str (default: "")
            description of the SST model
        driver_template_path : str (default: "")
            path to the black box-driver boilerplate
        component_template_path : str (default: "")
            path to the black box-model boilerplate
        """
        super().__init__(
            ipc=ipc,
            module_name=module_name,
            lib=lib,
            module_dir=module_dir,
            lib_dir=lib_dir,
            desc=desc,
        )

        warnings.warn(
            "SIT-PyRTL has not been fully implemented yet", FutureWarning
        )
        if self.module_dir:
            self.module_dir = f'sys.path.append(str(Path(__file__).parent / "{self.module_dir}"))'

        if self.ipc == "sock":

            # driver attributes
            self.driver_ipc = "socket"
            self.driver_bind = (
                "_sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)"
            )
            self.send = "sendall"
            self.connect = "connect"

        elif self.ipc == "zmq":

            # driver attributes
            self.driver_ipc = "zmq"
            self.driver_bind = """context = zmq.Context()
_sock = context.socket(zmq.REQ)"""
            self.send = "send"
            self.connect = "bind"

        self.driver_path += "_driver.py"
        self.comp_path += "_comp.cpp"

    def _parse_signal_type(self, signal):
        """Parse the type and computes its width from the signal

        Parameters:
        -----------
        signal : str
            signal definition

        Returns:
        --------
        int
            signal width
        """
        return (
            1 if signal == "1" else self._get_num_digits(self._get_ints(signal))
        )

    def _get_driver_outputs(self):
        """Generate output bindings for both the components in the black box

        Returns:
        --------
        str
            snippet of code representing output bindings
        """
        return self._sig_fmt(
            "str({module}.sim.inspect({module}.{sig})).encode()",
            lambda x: {"module": self.module, "sig": x["name"]},
            self.ports["output"],
            " +\n" + " " * 8,
        )

    def _get_driver_inputs(self):
        """Generate input bindings for the driver.

        Returns:
        --------
        str
            snippet of code representing input bindings
        """
        fmt = '"{sig}": int(signal[{sp}:{sl}]),'
        start_pos = 0
        clock_fmt = '"{sig}": int(signal[{sp}:{sl}]) % 2,'
        driver_inputs = []

        for input_port in self.ports["input"]:
            driver_inputs.append(
                fmt.format(
                    sp=start_pos,
                    sl=str(input_port["len"] + start_pos),
                    sig=input_port["name"],
                )
            )
            start_pos += input_port["len"]

        if self.ports["clock"]:
            for clock_port in self.ports["clock"]:
                driver_inputs.append(
                    clock_fmt.format(
                        sp=start_pos,
                        sl=str(input_port["len"] + start_pos),
                        sig=clock_port["name"],
                    )
                )
                start_pos += int(clock_port["len"])

        self.driver_buf_size = start_pos + 1
        return ("\n" + " " * 8).join(driver_inputs)

    def _get_driver_defs(self):
        """Map definitions for the PyRTL driver format string

        Returns:
        --------
        dict(str:str)
            format mapping of template PyRTL driver string
        """
        return {
            "ipc": self.driver_ipc,
            "driver_bind": self.driver_bind,
            "connect": self.connect,
            "send": self.send,
            "module_dir": self.module_dir,
            "module": self.module,
            "buf_size": self.driver_buf_size,
        }
